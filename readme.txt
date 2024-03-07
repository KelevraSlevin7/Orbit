This is a program that simulates gravity on objects on a 2 dimensional plane.
It uses simplified physic laws , e.g the gravitational constant is 1 and time is also 1 so that the formulas are easier.

The program uses heavily the Win32 API to create the windows for showing the objects and the control of the simulation.
So it will probably only run on windows machines.
The code is build in a way that the executable can be run without any required libraries.


Used tools:
	cmake (version 3.28.1)
	ninja (version 1.11.1.git.kitware.jobserver-1)
	gcc (version 13.2.0)


If you want to build it yourself, update the "MINGW64_PATH" in the "mingw64.cmake" file so that it targets your folder with gcc.

Commandline for CMake:
	1. navigate to build folder
	2. mkdir build
	3. cmake --preset mingw64
	4. cmake --build --preset mingw64

Commandline for gcc:
	g++ -static -L. -L C:\Windows\System32 -L C:\tools\mingw64\bin -l:comctl32.dll -l:gdi32.dll -l:libwinpthread-1.dll -o orbit_simulationtest_v1.0.0.exe src/main.cpp src/config.cpp src/c_orbit.cpp src/window_handling.cpp
	OR
	g++ -static -static-libgcc -static-libstdc++ -L. -L C:\Windows\System32 -L C:\tools\mingw64\bin -l:comctl32.dll -l:gdi32.dll -l:libwinpthread-1.dll -o orbit_simulationtest_v1.0.0.exe src/main.cpp src/config.cpp src/c_orbit.cpp src/window_handling.cpp
