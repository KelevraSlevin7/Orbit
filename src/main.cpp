#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <limits>
#include <fstream>
#include <algorithm>
#include "config.hpp"
#include "window_handling.hpp"
#include "c_draw.hpp"
#include "c_orbit.hpp"

struct Render_State{
    int x_pos{INIT_SIMULATION_POS_X};
    int y_pos{INIT_SIMULATION_POS_Y};
    int height{INIT_SIMULATION_HEIGHT};
    int width{INIT_SIMULATION_WIDTH};
    void* memory;
    BITMAPINFO bitmapinfo;
};
Render_State render_state;

boolean running = true;

enum simulation_State
{
        kStart  = 0U,
        kStop   = 1U
};
simulation_State simulationStatus {kStart};

//create Object Vector
std::vector<COrbitalObject> orbitalObjectVector;
//create Object to draw
CDraw drawLib;

//------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------static functions------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
void            Window_Loop                         (void);
void            Simulation_Init                     (void);
void            Simulation_Loop                     (HDC simulationHandle);
void            checkForButtonTrigger               (HDC simulationHandle);
void            drawAllObjects                      (HDC simulationHandle);
void            drawObject                          (unsigned int activeBoid);
void            drawTrail                           (unsigned int activeBoid);
void            calculateStableOrbit_startVelocity  (double bigObject_mass, double bigObject_posX, double bigObject_posY, double smallObject_posX, double smallObject_posY, double &smallObject_velX, double &smallObject_velY);
unsigned int    getRandomColor                      (unsigned int iteratio);
void            updateWindowSize                    (int width, int height);
void            waitUntilLoopEnd                    (std::chrono::time_point<std::chrono::high_resolution_clock> loop_begin);

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------function declarations---------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    //create all windows
    HWND simulationWindow_handle;
    HWND controlWindow_handle;
    createWindows(hInstance, nShowCmd, simulationWindow_handle, controlWindow_handle, render_state.x_pos, render_state.y_pos, render_state.width, render_state.height);
    HDC simulationHandle = GetDC(simulationWindow_handle);

    //intial message loop (so that the window size is updated)
    Window_Loop();

	//create objects to simulate
    Simulation_Init();

    //intial draw
    Simulation_Loop(simulationHandle);

    //main loop
    while (running)
    {
        //receive messages from window interaction and process them
        Window_Loop();
        
        //check if simulation needs to run
        if (simulationStatus == kStart)
        {
            //Simulate Orbital Objects
            Simulation_Loop(simulationHandle);
        }

        //check if any button in the control window was pressed and act accordingly
        checkForButtonTrigger(simulationHandle);
    }
    return 0;
}

void Window_Loop(void)
{
    //Check if some input happened on the window or the size changed
    MSG message;
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    //check if the size of the simulation window changed
    if ((get_simulationWindow_width()  != render_state.width) ||
        (get_simulationWindow_height() != render_state.height))
    {
        updateWindowSize(get_simulationWindow_width(), get_simulationWindow_height());
    }

    //check if start stop button was pressed and change the simulation state accordingly
    if (isButtonTriggered(IDC_BUTTON_STARTSTOP) == true)
    {
        if (simulationStatus == kStart)
        {
            simulationStatus = kStop;
            writeStartStopButtonText("Start");
        }
        else
        {
            simulationStatus = kStart;
            writeStartStopButtonText("Stop");
        }
    }

    //stop program if any window is closed
    //ToDo: only stop when both are closed
    //ToDo: add possibility to open closed windows again
    if ((get_simulationWindow_status() == false) ||
        (get_controlWindow_status() == false))
    {
        running = false;
    }
}

void Simulation_Init(void)
{
    //delete vector contents
    orbitalObjectVector.clear();

    //empty Object List
    clearObjectList();

    //get currently selected preset
    int selected_preset = get_presets_selection();
    
	//create all objects
    for (unsigned int iterator = 0; iterator < presets[selected_preset].number_of_objects; iterator++)
    {
        double mass             = presets[selected_preset].objects[iterator].mass;
        double radius           = presets[selected_preset].objects[iterator].radius;
        double start_position_x = presets[selected_preset].objects[iterator].start_position_x;
        double start_position_y = presets[selected_preset].objects[iterator].start_position_y;
        double start_velocity_x = presets[selected_preset].objects[iterator].start_velocity_x;
        double start_velocity_y = presets[selected_preset].objects[iterator].start_velocity_y;
        unsigned int color      = getRandomColor(iterator);

        //check if stable orbit needs to be calculated and ignore the first object
        if ((presets[selected_preset].calculate_stable == true) &&
            (iterator != 0))
        {
            //assume first object is the massive one
            calculateStableOrbit_startVelocity(
                presets[selected_preset].objects[0].mass, 
                presets[selected_preset].objects[0].start_position_x, 
                presets[selected_preset].objects[0].start_position_y,
                presets[selected_preset].objects[iterator].start_position_x, 
                presets[selected_preset].objects[iterator].start_position_y,
                start_velocity_x,
                start_velocity_y
            );
        }

        //construct object
		COrbitalObject orbitalObject(mass, radius, start_position_x, start_position_y, start_velocity_x, start_velocity_y, color);
        //add it to the object vector
		orbitalObjectVector.push_back(orbitalObject);

        //add object data to ListView item
        addObjectListItem(iterator, mass, radius, start_position_x, start_position_y, start_velocity_x, start_velocity_y);
    }
}

void Simulation_Loop(HDC simulationHandle)
{
    //measure time at start of loop
    auto loop_begin = std::chrono::high_resolution_clock::now();

    //clear Screen
    drawLib.fillScreen(0x000000);

    for (unsigned int iter = 0; iter < orbitalObjectVector.size(); iter++)
    {
        //update forces applied to every object
        orbitalObjectVector[iter].updateForce(orbitalObjectVector);
    }

    //draw all objects to the screen
    drawAllObjects(simulationHandle);

    waitUntilLoopEnd(loop_begin);
}

void checkForButtonTrigger(HDC simulationHandle)
{
    bool redraw_needed {false};

    //"Reset" button was pressed
    if (isButtonTriggered(IDC_BUTTON_RESET) == true)
    {
        //restore init values of all objects
        for (unsigned int iter = 0; iter < orbitalObjectVector.size(); iter++)
        {
            orbitalObjectVector[iter].restoreInitValues();
        }
        redraw_needed = true;
    }

    //"Load Preset" button was pressed
    if (isButtonTriggered(IDC_BUTTON_LOADPRESET) == true)
    {
        //start init again
        Simulation_Init();
        redraw_needed = true;
    }

    //"Add" button was pressed
    if (isButtonTriggered(IDC_BUTTON_ADD) == true)
    {
        //get all the values from the input fields
        double mass = getInputFieldValue(IDC_TEXT_INPUT_MASS);
        double radius = getInputFieldValue(IDC_TEXT_INPUT_RADIUS);
        double posX = getInputFieldValue(IDC_TEXT_INPUT_POSX);
        double posY = getInputFieldValue(IDC_TEXT_INPUT_POSY);
        double velX = getInputFieldValue(IDC_TEXT_INPUT_VELX);
        double velY = getInputFieldValue(IDC_TEXT_INPUT_VELY);

        //construct new object
        COrbitalObject orbitalObject(mass, radius, posX, posY, velX, velY, getRandomColor(orbitalObjectVector.size()));
        //append it to the object vector
        orbitalObjectVector.push_back(orbitalObject);
        //add item to the ListView
        addObjectListItem(orbitalObjectVector.size() - 1, mass, radius, posX, posY, velX, velY);
        redraw_needed = true;
    }

    //"Remove" button was pressed
    if (isButtonTriggered(IDC_BUTTON_REMOVE) == true)
    {
        //convert from listView index to vector index (list view newest items are at the top)
        int selectedItem = orbitalObjectVector.size() - getObjectListSelectedIndex() - 1;

        // remove from vector but only if something was selected
        if (selectedItem < orbitalObjectVector.size())
        {
            //remove object from Vector
            orbitalObjectVector.erase(orbitalObjectVector.begin() + selectedItem);
        }
        //remove item from ListView
        removeObjectListSelectedItem();
        redraw_needed = true;
    }
    
    if (redraw_needed == true)
    {
        //if simulation is stopped, redraw the screen with the new item
        if (simulationStatus == kStop)
        {
            //Simulate Orbital Objects
            drawLib.fillScreen(0x000000);
            drawAllObjects(simulationHandle);
        }
    }
}

void drawAllObjects(HDC simulationHandle)
{
    //go through all objects
    for (unsigned int iter = 0; iter < orbitalObjectVector.size(); iter++)
    {
        //update object positions and trail
        orbitalObjectVector[iter].updatePosition();
        orbitalObjectVector[iter].updateTrail();
    
        //draw Object and Trail
        drawObject(iter);
        drawTrail(iter);
        //orbitalObjectVector[iter].outputInformation(iter);
    }

    //Render
    StretchDIBits(simulationHandle, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmapinfo, DIB_RGB_COLORS, SRCCOPY);
}

void drawObject(unsigned int activeBoid)
{
    int objectPositionX = static_cast<int>(orbitalObjectVector[activeBoid].getPosX());
    int objectPositionY = static_cast<int>(orbitalObjectVector[activeBoid].getPosY());
    int objectRadius = static_cast<int>(orbitalObjectVector[activeBoid].getRadius());

    //dont draw objects that are not visible anymore
    if ((objectPositionX < render_state.width + objectRadius) && (objectPositionX > 0 - objectRadius) &&
        (objectPositionY < render_state.height + objectRadius) && (objectPositionY > 0 - objectRadius))
    {
        drawLib.drawCircle(objectPositionX, objectPositionY, objectRadius, orbitalObjectVector[activeBoid].getColor(), true);
    }
}

void drawTrail(unsigned int activeBoid)
{
    unsigned int trailOldestValue = orbitalObjectVector[activeBoid].getTrailIterator();
    unsigned int trailIterator = trailOldestValue;
    unsigned int trailIteratorNext = (trailIterator + 1) % (TRAIL_LENGTH);

    //get Trail
    COrbitalObject::coordinatesI * orbitalObjectTrail = orbitalObjectVector[activeBoid].getTrail();

    //iterate trail length times (k is not used)
    for (unsigned int k = 0; k < TRAIL_LENGTH - 1; k++)
    {
        //draw every single point of the trail
        //draw.drawPixel(orbitalObjectTrail[k].x, orbitalObjectTrail[k].y, orbitalObjectVector[activeBoid].getColor());

        //draw a line between 2 points of the Trail (ignore not yet written points in the trail array)
        if ((orbitalObjectTrail[trailIterator].x != 0) && (orbitalObjectTrail[trailIterator].y != 0))
        {
            drawLib.drawLine(orbitalObjectTrail[trailIterator].x, orbitalObjectTrail[trailIterator].y, orbitalObjectTrail[trailIteratorNext].x, orbitalObjectTrail[trailIteratorNext].y, orbitalObjectVector[activeBoid].getColor());
        }

        //update iterator
        trailIterator = trailIteratorNext;
        trailIteratorNext = (trailIterator + 1) % (TRAIL_LENGTH);
    }
}

void calculateStableOrbit_startVelocity(double bigObject_mass, double bigObject_posX, double bigObject_posY, double smallObject_posX, double smallObject_posY, double &smallObject_velX, double &smallObject_velY)
{
    // Fg = Fc  (gravitation = centrifugal force)
    // m1 * m2 / r^2 = m1 * w^2 * r
    // w = sqrt(m2/r^3)
    // vx = cos(w) * r
    // vy = sin(w) * r
    double distance = sqrt(pow((bigObject_posX - smallObject_posX), 2) + pow((bigObject_posY - smallObject_posY), 2));
    smallObject_velX = distance - cos(sqrt(bigObject_mass / (distance*distance*distance))) * distance;
    smallObject_velY = sin(sqrt(bigObject_mass / (distance*distance*distance))) * distance;
}

unsigned int getRandomColor(unsigned int iteration)
{
    CColor color;
    color.hsv.h = (static_cast<int>(color.hsv.h + (50 * iteration)) % 360); //hue (color)
    color.hsv.s = 1.0;                          //saturation
    color.hsv.v = 1.0;                          //value
    color.rgb = color.hsv2rgb(color.hsv);

    unsigned int color_r, color_g, color_b;

    color_r = static_cast<unsigned int>(static_cast<double>(0xFFu) * color.rgb.r);
    color_g = static_cast<unsigned int>(static_cast<double>(0xFFu) * color.rgb.g);
    color_b = static_cast<unsigned int>(static_cast<double>(0xFFu) * color.rgb.b);

    return ((color_r << 16) | (color_g << 8) | (color_b));
}

void updateWindowSize(int width, int height)
{
        render_state.width = width;
        render_state.height = height;

        //update buffer with new window size
        if (render_state.memory)
        {
            VirtualFree(render_state.memory, 0, MEM_RELEASE);
        }
        int buffer_size = render_state.width * render_state.height * sizeof(unsigned int);
        render_state.memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        render_state.bitmapinfo.bmiHeader.biSize = sizeof(render_state.bitmapinfo.bmiHeader);
        render_state.bitmapinfo.bmiHeader.biWidth = render_state.width;
        render_state.bitmapinfo.bmiHeader.biHeight = render_state.height;
        render_state.bitmapinfo.bmiHeader.biPlanes = 1;
        render_state.bitmapinfo.bmiHeader.biBitCount = 32;
        render_state.bitmapinfo.bmiHeader.biCompression = BI_RGB;

        //update drawLib memory adress and height and width
        drawLib.update(static_cast<unsigned int*>(render_state.memory), render_state.height, render_state.width);
}

void waitUntilLoopEnd(std::chrono::time_point<std::chrono::high_resolution_clock> loop_begin)
{
    //time measurement stuff
    auto loop_end = std::chrono::high_resolution_clock::now();
    auto loop_duration = std::chrono::duration_cast<std::chrono::microseconds>(loop_end - loop_begin).count();

    //output loop duration on window
    updateCalculationTimeText(loop_duration);

    //repeat loop until fixed time frame is reached
    while (loop_duration < LOOP_TIME_MS * 1000)
    {
        loop_end = std::chrono::high_resolution_clock::now();
        loop_duration = std::chrono::duration_cast<std::chrono::microseconds>(loop_end - loop_begin).count();
    }
    //output message if calculation took too long
    if (loop_duration > 2 * LOOP_TIME_MS * 1000)
    {
        std::cout << "WARNING!!!! Loop time exceeded. Too many operations." << std::endl;
        std::cout << "    Target Loop Time: " << LOOP_TIME_MS << std::endl;
        std::cout << "    Current Loop Time: " << loop_duration << std::endl;
    }
}