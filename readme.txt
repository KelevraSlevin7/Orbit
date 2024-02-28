This is a program that simulates gravity on 2 dimensional objects.
It uses simplified physic laws , e.g the gravitational constant is 1 and time is also 1 so that the formulas are easier.

The program uses the Win32 API to create the windows for showing the objects and to control the simulation.
So it will probably only run on windows machines.
The code is build in a way that the executable can be run without any required libraries.

If you want to build it yourself, update the "MINGW64_PATH" in the "mingw64.cmake" file so that it targets your folder with gcc.

How to build fresh:
	1. delete build folder
	2. cmake --preset mingw64
	3. cmake --build --preset mingw64

How to rebuild existing project:
	1. cmake --build --preset mingw64
