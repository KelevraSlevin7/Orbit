#include <windows.h>
// #include <CommCtrl.h>
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

#define LOOP_TIME_MS 10

struct Render_State{
    int x_pos{100};
    int y_pos{100};
    int height{800};
    int width{800};
    void* memory;
    BITMAPINFO bitmapinfo;
};
Render_State render_state;

boolean running = true;
// number of created boids
unsigned int number_of_objects = 1;
// creat Boid Object Vector
std::vector<COrbitalObject> orbitalObjectVector;

CDraw drawLib;

//------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------static functions------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
        void        Simulation_Init             (void);
        void        Simulation_Loop             (HDC simulation_DC);
        void        Window_Loop                 (void);
        void        drawObject                  (unsigned int activeBoid);
        void        drawTrail                   (unsigned int activeBoid);
        void        setStartValues              (void);
        unsigned int getRandomColor             (void);
        void        waitUntilLoopEnd            (std::chrono::time_point<std::chrono::high_resolution_clock> loop_begin);

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------function declarations---------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

    std::cout << "Adress 0: " << &presets[0].objects[0].mass << std::endl;
    std::cout << "Adress 1: " << &presets[1].objects[0].mass << std::endl;
    //create all windows
    HWND simulationWindow_handle;
    HWND controlWindow_handle;
    createWindows(hInstance, nShowCmd, simulationWindow_handle, controlWindow_handle, render_state.x_pos, render_state.y_pos, render_state.width, render_state.height);
    HDC simulation_DC = GetDC(simulationWindow_handle);
    HDC control_DC = GetDC(controlWindow_handle);

    //intial message loop (so that the window size is updated)
    Window_Loop();

	//create objects to simulate
    Simulation_Init();

    //intial draw
    Simulation_Loop(simulation_DC);
    
    //main loop
    while (running)
    {
        //receive messages from window interaction and process them
        Window_Loop();
        
        //check if simulation needs to run
        if (get_startstopButton_state() == kStart)
        {
            //Simulate Orbital Objects
            Simulation_Loop(simulation_DC);
        }

        //check if reset needs to be performed
        if (is_resetButton_triggered() == true)
        {
            Simulation_Init();
            //draw one frame so the reset is visible
            Simulation_Loop(simulation_DC);
        }
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
    if ((get_simulationWindow_width() != render_state.width) ||
        (get_simulationWindow_height() != render_state.height))
    {
        render_state.width = get_simulationWindow_width();
        render_state.height = get_simulationWindow_height();

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

    //stop program is any window is closed
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
        unsigned int color      = getRandomColor();

        std::cout << "iter: " << iterator << std::endl;
        std::cout << "mass: " << mass << std::endl;
        std::cout << "radius: " << radius << std::endl;
        std::cout << "start_position_x: " << start_position_x << std::endl;
        std::cout << "start_position_y: " << start_position_y << std::endl;
        std::cout << "start_velocity_x: " << start_velocity_x << std::endl;
        std::cout << "start_velocity_y: " << start_velocity_y << std::endl;

		COrbitalObject orbitalObject(mass, radius, start_position_x, start_position_y, start_velocity_x, start_velocity_y, color);
		orbitalObjectVector.push_back(orbitalObject);
    }

    //set start values according to selected active set
    setStartValues();
}

void Simulation_Loop(HDC simulation_DC)
{
    //measure time at start of loop
    auto loop_begin = std::chrono::high_resolution_clock::now();

    //clear Screen
    drawLib.fillScreen(0x000000);

    for (unsigned int iter = 0; iter < number_of_objects; iter++)
    {
        //update forces applied to every object
        orbitalObjectVector[iter].updateForce(orbitalObjectVector);
    }

    for (unsigned int iter = 0; iter < number_of_objects; iter++)
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
    StretchDIBits(simulation_DC, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmapinfo, DIB_RGB_COLORS, SRCCOPY);

    waitUntilLoopEnd(loop_begin);
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



void setStartValues(void)
{
    // double distance = 300.0;
    // double MidPosX = static_cast<double>(render_state.width / 2);
    // double MidPosY = static_cast<double>(render_state.height / 2);
    // double m2 = 10000.0;

    // // Fg = Ff  (Gravitation = Fliehkraft)
    // // m1 * m2 / r^2 = m1 * w^2 * r
    // // w = sqrt(m2/r^3)
    // // vx = cos(w) * r
    // // vy = sin(w) * r
    // double vel_x = distance - cos(sqrt(m2/(distance*distance*distance))) * distance;
    // double vel_y = sin(sqrt(m2/(distance*distance*distance))) * distance;

    // orbitalObjectVector[0].setMass(m2);
    // orbitalObjectVector[0].setRadius(30.0);
    // orbitalObjectVector[0].setColor(0xff0000);
    // orbitalObjectVector[0].setPos(MidPosX, MidPosY);
    // orbitalObjectVector[0].setVel(0.0, 0.0);
    // // const char* objectItems[NUMBER_OF_OBJECTLIST_COLUMNS] = {1, m2, 30.0, MidPosX, MidPosY };
    // add_ObjectList_Item(1, m2, 30.0, MidPosX, MidPosY);

    // orbitalObjectVector[1].setMass(1.0);
    // orbitalObjectVector[1].setRadius(10.0);
    // orbitalObjectVector[1].setColor(0x00ff00);
    // orbitalObjectVector[1].setPos(MidPosX + distance, MidPosY);
    // orbitalObjectVector[1].setVel(vel_x, vel_y);
    // // const char* objectItems[NUMBER_OF_OBJECTLIST_COLUMNS] = {1, 1.0, 10.0, MidPosX + distance, MidPosY };
    // add_ObjectList_Item(2, 1.0, 10.0, MidPosX + distance, MidPosY);

    // add_ObjectList_Item(3, 1.0, 10.0, MidPosX + distance, MidPosY);
    // add_ObjectList_Item(4, 1.0, 10.0, MidPosX + distance, MidPosY);
    // add_ObjectList_Item(5, 1.0, 10.0, MidPosX + distance, MidPosY);
    // add_ObjectList_Item(6, 1.0, 10.0, MidPosX + distance, MidPosY);

    // remove_ObjectList_Item();

    // add_ObjectList_Item(7, 1.0, 10.0, MidPosX + distance, MidPosY);
    // add_ObjectList_Item(8, 1.0, 10.0, MidPosX + distance, MidPosY);
}

void calculateStableOrbit_startVelocity(double bigObject_mass, double bigObject_posX, double bigObject_posY, double smallObject_posX, double smallObject_posY, double &smallObject_velX, double &smallObject_velY)
{
    // Fg = Ff  (Gravitation = Fliehkraft)
    // m1 * m2 / r^2 = m1 * w^2 * r
    // w = sqrt(m2/r^3)
    // vx = cos(w) * r
    // vy = sin(w) * r
    double distance = sqrt(pow((bigObject_posX - smallObject_posX), 2) + pow((bigObject_posY - smallObject_posY), 2));
    smallObject_velX = distance - cos(sqrt(bigObject_mass / (distance*distance*distance))) * distance;
    smallObject_velY = sin(sqrt(bigObject_mass / (distance*distance*distance))) * distance;
}

unsigned int getRandomColor(void)
{
    static CColor color;
    color.hsv.h = (static_cast<int>(color.hsv.h) % 360) + 50; //hue (color)
    color.hsv.s = 1.0;                          //saturation
    color.hsv.v = 1.0;                          //value
    color.rgb = color.hsv2rgb(color.hsv);

    unsigned int color_r, color_g, color_b;

    color_r = static_cast<unsigned int>(static_cast<double>(0xFFu) * color.rgb.r);
    color_g = static_cast<unsigned int>(static_cast<double>(0xFFu) * color.rgb.g);
    color_b = static_cast<unsigned int>(static_cast<double>(0xFFu) * color.rgb.b);

    return ((color_r << 16) | (color_g << 8) | (color_b));
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
    //output message if loop takes too long
    if (loop_duration > 2 * LOOP_TIME_MS * 1000)
    {
        std::cout << "WARNING!!!! Loop time exceeded. Too many operations." << std::endl;
        std::cout << "    Target Loop Time: " << LOOP_TIME_MS << std::endl;
        std::cout << "    Current Loop Time: " << loop_duration << std::endl;
    }
}