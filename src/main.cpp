#include <windows.h>
#include <CommCtrl.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <limits>
#include <fstream>
#include <algorithm>
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

unsigned int activeSet = 1;

CDraw drawLib;

//------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------static functions------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
        void        Simulation_Init             (void);
        void        Simulation_Loop             (HDC simulation_DC);
        void        Window_Loop                 (void);
        void        drawObject                  (unsigned int activeBoid);
        void        drawTrail                   (unsigned int activeBoid);
        int         getCommandLineArguments     (void);
        void        setStartValues              (void);
        void        readConfigFile              (void);
        void        waitUntilLoopEnd            (std::chrono::time_point<std::chrono::high_resolution_clock> loop_begin);

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------function declarations---------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    if (getCommandLineArguments() == 1)
    {
        return 1;
    }

    //create all windows
    HWND simulationWindow_handle;
    HWND controlWindow_handle;
    createWindows(hInstance, nShowCmd, simulationWindow_handle, controlWindow_handle, render_state.x_pos, render_state.y_pos, render_state.width, render_state.height);
    HDC simulation_DC = GetDC(simulationWindow_handle);
    HDC control_DC = GetDC(controlWindow_handle);

    //initialize draw Object
    drawLib.update(static_cast<unsigned int*>(render_state.memory), render_state.height, render_state.width);

	//Create Boids
    Simulation_Init();
    
    while (running)
    {
        //receive messages from window interaction and process them
        Window_Loop();
            
        //Simulate Orbital Obejcts
        Simulation_Loop(simulation_DC);
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
	//create all objects
    for (unsigned int iterator = 0; iterator < number_of_objects; iterator++)
    {
		COrbitalObject orbitalObject(static_cast<float>(render_state.width), static_cast<float>(render_state.height));
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

int getCommandLineArguments(void)
{
	if (__argc != 3)
	{
		std::cout << "Expected Arguments are: '...exe objectNumber setSelection'" << std::endl;
        std::cout << "        objectNumber: number of Objects that will be simulated." << std::endl;
        std::cout << "        setSelection:" << std::endl;
        std::cout << "            0 -> Using configFile" << std::endl;
        std::cout << "            1 -> 3 Body System with 'stable' orbit" << std::endl;
        std::cout << "            2 -> i dont know yet" << std::endl;
        std::cout << "            X -> random" << std::endl;
        std::cout << std::endl;
		return 1;
	}
	else
	{
        if (atoi(__argv[1]) == 0)
        {
            std::cout << "Number of Objects cannot be 0" << std::endl;
            std::cout << std::endl;
            return 1;
        }
        //set values
		number_of_objects = atoi(__argv[1]);
		std::cout << "Number of Orbital Objects that will be simulated: " << number_of_objects << std::endl;
        activeSet = atoi(__argv[2]);
        std::cout << "Active Set: " << activeSet << std::endl;
        std::cout << std::endl;
        return 0;
	}
}

void setStartValues(void)
{
    switch (activeSet)
    {
        case 0:
            readConfigFile();
            break;
        case 1:
            if (number_of_objects >= 1){
                orbitalObjectVector[0].setMass(200.0);
                orbitalObjectVector[0].setRadius(10.0);
                orbitalObjectVector[0].setColor(0x0000ff);
                orbitalObjectVector[0].setPos(static_cast<double>(render_state.width / 2), static_cast<double>(render_state.height / 2));
                orbitalObjectVector[0].setVel(0.0, 0.0);

            }
            if (number_of_objects >= 2){
                orbitalObjectVector[1].setMass(20.0);
                orbitalObjectVector[1].setRadius(5.0);
                orbitalObjectVector[1].setColor(0x00ff00);
                orbitalObjectVector[1].setPos(static_cast<double>(render_state.width / 2) - 80.0, static_cast<double>(render_state.height / 2));
                orbitalObjectVector[1].setVel(0.0, -1.9);
            }
            if (number_of_objects >= 3){
                orbitalObjectVector[2].setMass(20.0);
                orbitalObjectVector[2].setRadius(5.0);
                orbitalObjectVector[2].setColor(0xff0000);
                orbitalObjectVector[2].setPos(static_cast<double>(render_state.width / 2) + 80.0, static_cast<double>(render_state.height / 2));
                orbitalObjectVector[2].setVel(0.0, 1.9);
            }
            break;

        case 2:
        {
            double distance = 300.0;
            double MidPosX = static_cast<double>(render_state.width / 2);
            double MidPosY = static_cast<double>(render_state.height / 2);
            double m2 = 10000.0;

            // Fg = Ff  (Gravitation = Fliehkraft)
            // m1 * m2 / r^2 = m1 * w^2 * r
            // w = sqrt(m2/r^3)
            // vx = cos(w) * r
            // vy = sin(w) * r
            double vel_x = distance - cos(sqrt(m2/(distance*distance*distance))) * distance;
            double vel_y = sin(sqrt(m2/(distance*distance*distance))) * distance;

            if (number_of_objects >= 1){
                orbitalObjectVector[0].setMass(m2);
                orbitalObjectVector[0].setRadius(60.0);
                orbitalObjectVector[0].setColor(0xff0000);
                orbitalObjectVector[0].setPos(MidPosX, MidPosY);
                orbitalObjectVector[0].setVel(0.0, 0.0);
            }
            if (number_of_objects >= 2){
                orbitalObjectVector[1].setMass(1.0);
                orbitalObjectVector[1].setRadius(30.0);
                orbitalObjectVector[1].setColor(0x00ff00);
                orbitalObjectVector[1].setPos(MidPosX + distance, MidPosY);
                orbitalObjectVector[1].setVel(vel_x, vel_y);
            }
            break;
        }

        default:
            //default constructor sets everything to random
            break;
    }
}

void readConfigFile(void)
{
    unsigned int objectNumber = 0;
    double mass = 1.0;
    double radius = 1.0;
    double positionX = 0.0;
    double positionY = 0.0; 
    double velocityX = 0.0;
    double velocityY = 0.0;

    //try to open config file
    std::ifstream configFile ("../../config/configFile.txt");
    if (configFile.is_open())
    {
        std::cout << "Using object values from config File." << std::endl;

        std::string line;
        //get 1 line from the file
        while(getline(configFile, line))
        {
            //remove all whitespaces
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
            //if the line is empty or starts with a comment -> get a new line
            if(line[0] == '#' || line.empty())
            {
                continue;
            }

            //find the position of the "=" sign
            auto delimiterPos = line.find("=");
            auto objectStart = line.find("{");
            auto objectEnd = line.find("}");

            //stop searching if no "=" AND no "{" AND no "}" was in the string, break the loop
            if ((delimiterPos == std::string::npos) && (objectStart == std::string::npos) && (objectEnd == std::string::npos))
            {
                std::cout << "Wrong format used for config file. Stopping..." << std::endl;
                break;
            }

            //new object --> reset all values
            if (objectStart != std::string::npos)
            {
                objectNumber = 0;
                mass = 1.0;
                radius = 1.0;
                positionX = 0.0;
                positionY = 0.0;
                velocityX = 0.0;
                velocityY = 0.0;
                continue;
            }

            //object finished --> write all values
            if (objectEnd != std::string::npos)
            {
                if (objectNumber < number_of_objects)
                {
                    orbitalObjectVector[objectNumber].setMass(mass);
                    orbitalObjectVector[objectNumber].setRadius(radius);
                    orbitalObjectVector[objectNumber].setPos(positionX, positionY);
                    orbitalObjectVector[objectNumber].setVel(velocityX, velocityY);
                }
                continue;
            }

            if ((line.substr(0, delimiterPos)).compare("ObjectNumber") == 0U)
                    objectNumber = atoi((line.substr(delimiterPos + 1)).c_str());

            if ((line.substr(0, delimiterPos)).compare("mass") == 0U)
                mass = atof((line.substr(delimiterPos + 1)).c_str());
            
            if ((line.substr(0, delimiterPos)).compare("radius") == 0U)
                radius = atof((line.substr(delimiterPos + 1)).c_str());

            if ((line.substr(0, delimiterPos)).compare("positionX") == 0U)
                positionX = atof((line.substr(delimiterPos + 1)).c_str());

            if ((line.substr(0, delimiterPos)).compare("positionY") == 0U)
                positionY = atof((line.substr(delimiterPos + 1)).c_str());

            if ((line.substr(0, delimiterPos)).compare("velocityX") == 0U)
                velocityX = atof((line.substr(delimiterPos + 1)).c_str());

            if ((line.substr(0, delimiterPos)).compare("velocityY") == 0U)
                velocityY = atof((line.substr(delimiterPos + 1)).c_str());
        }
    }
    else
    {
        std::cout << "Could not open config file." << std::endl;
        TCHAR executablePath[MAX_PATH] = {0};
        GetModuleFileNameA(NULL, executablePath, MAX_PATH);
        std::cout << "Searched in same Folder as:" << std::endl;
        std::cout << "    " << executablePath << std::endl;
        std::cout << "Using default Parameter values." << std::endl;
    }
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