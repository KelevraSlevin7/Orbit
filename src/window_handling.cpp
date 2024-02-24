#include <iostream>
#include "window_handling.hpp"

HWND hwndText_CalculationTime;
HWND hwndButton_Start;
#define IDC_BUTTON_START    100
HWND hwndButton_Stop;
#define IDC_BUTTON_STOP     101
HWND hwndButton_Reset;
#define IDC_BUTTON_RESET    102

#define BUTTON_WIDTH    150
#define BUTTON_HEIGHT   70
#define BUTTON_SPACING  20

bool simulation_running{false};
bool simulation_reset_trigger{false};

bool simulationWindow_active{true};
int simulationWindow_width{0};
int simulationWindow_height{0};

bool controlWindow_active{true};
int controlWindow_width{0};
int controlWindow_height{0};

LRESULT CALLBACK simulationWindow_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (uMsg)
    {
		//stop the programm when the window is closed
        case WM_CLOSE:
        case WM_DESTROY:
        {
            simulationWindow_active = false;
			break;
        }

		//when the window is resized
        case WM_SIZE:
        {
            //calculate new window size
            RECT rect;
            GetClientRect(hwnd, &rect);
            simulationWindow_width = rect.right - rect.left;
            simulationWindow_height = rect.bottom - rect.top;
			break;
        }

        default:
        {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    return result;
}

LRESULT CALLBACK controlWindow_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_DESTROY:
        {
            controlWindow_active = false;
            break;
        }
        case WM_COMMAND:
        {
            if (LOWORD(wParam) == IDC_BUTTON_START)
            {
                simulation_running = true;
            }
            else if (LOWORD(wParam) == IDC_BUTTON_STOP)
            {
                simulation_running = false;
            }
            else if (LOWORD(wParam) == IDC_BUTTON_RESET)
            {
                simulation_reset_trigger = true;
            }
            break;
        }
        break;

        default:
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    return 0;
}

void createWindows(HINSTANCE hInstance, int nShowCmd, HWND &simulationWindow_handle, HWND &controlWindow_handle, int x_pos, int y_pos, int width, int height)
{
    //------------------------------------------------------------------------------------------------------------------------------------------
	//-------------------------------------------simulation window------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------------------------
    WNDCLASSEX simulationWindow_class = {};
    ZeroMemory(&simulationWindow_class,sizeof(WNDCLASSEX));
    simulationWindow_class.cbSize           = sizeof(WNDCLASSEX);                   //The size, in bytes, of this structure
    simulationWindow_class.style            = CS_HREDRAW | CS_VREDRAW;              //The class style(s) (window will be redrawn if the size in x or y is changed)
	simulationWindow_class.lpfnWndProc      = (WNDPROC)simulationWindow_callback;   //A pointer to the window procedure
    simulationWindow_class.cbClsExtra       = 0;                                    //The number of extra bytes to allocate following the window-class structure
    simulationWindow_class.cbWndExtra       = 0;                                    //The number of extra bytes to allocate following the window instance
	simulationWindow_class.hInstance        = hInstance;                            //A handle to the instance that contains the window procedure for the class
    simulationWindow_class.hIcon            = NULL;                                 //A handle to the class icon
    simulationWindow_class.hCursor          = LoadCursor(NULL,IDC_ARROW);           //A handle to the class cursor
    simulationWindow_class.hbrBackground    = (HBRUSH)COLOR_WINDOW;                 //A handle to the class background brus
    simulationWindow_class.lpszMenuName     = NULL;                                 //Pointer to a null-terminated character string that specifies the resource name of the class menu, as the name appears in the resource file
    simulationWindow_class.lpszClassName    = "Simulation Window Class";            //A pointer to a null-terminated string or is an atom
    simulationWindow_class.hIconSm          = NULL;                                 //A handle to a small icon that is associated with the window class
    //register the window to the operating system
    RegisterClassEx(&simulationWindow_class);

    //ToDo: why in the world does this function here change render_state.height and width??????
	simulationWindow_handle = CreateWindowEx(
		0,                                      //Optional window styles
		simulationWindow_class.lpszClassName,   //Window class
		"Orbital Object Simulation!",           //Window text
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,	    //Window style
		x_pos, y_pos,                           //position
		width, height,                          //size
		NULL,                                   // Parent window    
		NULL,                                   // Menu
		hInstance,                              // Instance handle
		NULL                                    // Additional application data
		);

	if (simulationWindow_handle == NULL)
	{
		std::cout << "Issue with the simulation window class. Aborting." << std::endl;
	}
	ShowWindow(simulationWindow_handle, nShowCmd);


    //------------------------------------------------------------------------------------------------------------------------------------------
	//-------------------------------------------control window---------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------------------------
    WNDCLASSEX controlWindow_class = {};
    ZeroMemory(&controlWindow_class,sizeof(WNDCLASSEX));
    controlWindow_class.cbSize          = sizeof(WNDCLASSEX);                   //The size, in bytes, of this structure
    controlWindow_class.style           = CS_HREDRAW | CS_VREDRAW;              //The class style(s) (window will be redrawn if the size in x or y is changed)
	controlWindow_class.lpfnWndProc     = (WNDPROC)controlWindow_callback;      //A pointer to the window procedure
    controlWindow_class.cbClsExtra      = 0;                                    //The number of extra bytes to allocate following the window-class structure
    controlWindow_class.cbWndExtra      = 0;                                    //The number of extra bytes to allocate following the window instance
	controlWindow_class.hInstance       = hInstance;                            //A handle to the instance that contains the window procedure for the class
    controlWindow_class.hIcon           = NULL;                                 //A handle to the class icon
    controlWindow_class.hCursor         = LoadCursor(NULL,IDC_ARROW);           //A handle to the class cursor
    controlWindow_class.hbrBackground   = (HBRUSH)COLOR_WINDOW;                 //A handle to the class background brus
    controlWindow_class.lpszMenuName    = NULL;                                 //Pointer to a null-terminated character string that specifies the resource name of the class menu, as the name appears in the resource file
    controlWindow_class.lpszClassName   = "Control Window Class";               //A pointer to a null-terminated string or is an atom
    controlWindow_class.hIconSm         = NULL;                                 //A handle to a small icon that is associated with the window class
    //register the window to the operating system
    RegisterClassEx(&controlWindow_class);

	controlWindow_handle = CreateWindowEx(
		0,                                      //Optional window styles
		controlWindow_class.lpszClassName,      //Window class
		"Orbital Object Control!",              //Window text
		WS_SYSMENU | WS_VISIBLE,	            //Window style
		x_pos + width + 100, y_pos,             //position
		600, 600,                               //size
		NULL,                                   // Parent window    
		NULL,                                   // Menu
		hInstance,                              // Instance handle
		NULL                                    // Additional application data
		);

	if (controlWindow_handle == NULL)
	{
		std::cout << "Issue with the control window class. Aborting." << std::endl;
	}
	ShowWindow(controlWindow_handle, nShowCmd);

    //------------------------------------------------------------------------------------------------------------------------------------------
	//-------------------------------------------control window content-------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------------------------
    hwndButton_Start = CreateWindow( 
        "BUTTON",                                               // Predefined class; Unicode assumed
        "Start",                                                // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
        BUTTON_SPACING,                                         // x position
        20,                                                     // y position
        BUTTON_WIDTH,                                           // Button width
        BUTTON_HEIGHT,                                          // Button height
        controlWindow_handle,                                   // Parent window
        (HMENU)IDC_BUTTON_START,                                // control ID
        (HINSTANCE)GetWindowLongPtr(controlWindow_handle, GWLP_HINSTANCE), 
        NULL                                                    // Pointer not needed.
    );

    hwndButton_Stop = CreateWindow( 
        "BUTTON",                                               // Predefined class; Unicode assumed
        "Stop",                                                 // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
        BUTTON_SPACING,                                         // x position
        2 * BUTTON_SPACING + BUTTON_HEIGHT,                     // y position
        BUTTON_WIDTH,                                           // Button width
        BUTTON_HEIGHT,                                          // Button height
        controlWindow_handle,                                   // Parent window
        (HMENU)IDC_BUTTON_STOP,                                 // control ID
        (HINSTANCE)GetWindowLongPtr(controlWindow_handle, GWLP_HINSTANCE), 
        NULL                                                    // Pointer not needed.
    );

    hwndButton_Reset = CreateWindow( 
        "BUTTON",                                               // Predefined class; Unicode assumed
        "Reset",                                                // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
        BUTTON_SPACING,                                         // x position
        3 * BUTTON_SPACING + 2 * BUTTON_HEIGHT,                 // y position
        BUTTON_WIDTH,                                           // Button width
        BUTTON_HEIGHT,                                          // Button height
        controlWindow_handle,                                   // Parent window
        (HMENU)IDC_BUTTON_RESET,                                // control ID
        (HINSTANCE)GetWindowLongPtr(controlWindow_handle, GWLP_HINSTANCE), 
        NULL                                                    // Pointer not needed.
    );

    hwndText_CalculationTime = CreateWindow("STATIC", "0", WS_VISIBLE | WS_CHILD | SS_LEFT | SS_SUNKEN, 200, 10, 160, 20, controlWindow_handle, NULL, hInstance, NULL);
}

void updateCalculationTimeText(long long int calculationDuration)
{
    char text[30];
    sprintf(text, "Loop Time: %d us", calculationDuration);
    SetWindowText(hwndText_CalculationTime, TEXT(text));
}

bool get_simulationWindow_status(void)
{
    return simulationWindow_active;
}

bool get_controlWindow_status(void)
{
    return controlWindow_active;
}

int get_simulationWindow_width(void)
{
    return simulationWindow_width;
}

int get_simulationWindow_height(void)
{
    return simulationWindow_height;
}

bool get_simulation_running(void)
{
    return simulation_running;
}

bool get_simulation_reset_trigger(void)
{
    bool return_val = simulation_reset_trigger;
    simulation_reset_trigger = false;
    return return_val;
}