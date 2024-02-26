#include <iostream>
#include <string>
#include <sstream>
#include "window_handling.hpp"

HWND hwndButton_StartStop;
#define IDC_BUTTON_STARTSTOP    100
HWND hwndButton_Reset;
#define IDC_BUTTON_RESET        101
HWND hwndListView_Objects;
#define IDC_LISTVIEW_OBJECTS    102
HWND hwndText_CalculationTime;

#define BUTTON_WIDTH    150
#define BUTTON_HEIGHT   70
#define BUTTON_SPACING  20

StartStopButton_State startstopButton_state{kStart};
bool resetButton_trigger{false};

bool simulationWindow_active{true};
int simulationWindow_width{0};
int simulationWindow_height{0};

bool controlWindow_active{true};

const char* objectListColums[NUMBER_OF_OBJECTLIST_COLUMNS] = 
{
    "Number",
    "Mass",
    "Radius",
    "Start Pos X",
    "Start Pos Y"
};

//------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------static functions------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
void    fillControlWindow       (HWND parentWindowHandle);
HWND    createButton            (HWND parentWindowHandle, const char *name, int position_x, int position_y, int width, int height, HMENU IDC);
HWND    createListView          (HWND parentWindowHandle, int position_x, int position_y, int width, int height, HMENU IDC, const char ** column_names, int numberOfColumns);
HWND    createTextField         (HWND parentWindowHandle, const char *name, int position_x, int position_y, int width, int height);
void    createListView_Column   (HWND listviewHandle, int iCol, const char *text, int width);
void    createListView_Item     (HWND listviewHandle, const char ** itemValues, int numberOfColumns);

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------function declarations---------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
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

        case WM_CREATE:
        {
            fillControlWindow(hwnd);
            break;
        }

        case WM_COMMAND:
        {
            if (LOWORD(wParam) == IDC_BUTTON_STARTSTOP)
            {
                //toggle Start Stop button when pressed
                if (startstopButton_state == kStop)
                {
                    startstopButton_state = kStart;
                    SendMessage(hwndButton_StartStop, WM_SETTEXT, 0, (LPARAM)"Stop");
                }
                else
                {
                    startstopButton_state = kStop;
                    SendMessage(hwndButton_StartStop, WM_SETTEXT, 0, (LPARAM)"Start");
                }
            }
            else if (LOWORD(wParam) == IDC_BUTTON_RESET)
            {
                resetButton_trigger = true;
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
    //initalize common controls (used to integrate the lib somehow)
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

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
		800, 800,                               //size
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
}

void fillControlWindow(HWND parentWindowHandle)
{
    //create button for starting and stopping the simulation
    hwndButton_StartStop = createButton(parentWindowHandle, "Stop", BUTTON_SPACING, BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT, (HMENU)IDC_BUTTON_STARTSTOP);

    //create button to reset the simulation to the initial state
    hwndButton_Reset = createButton(parentWindowHandle, "Reset", BUTTON_SPACING, 2 * BUTTON_SPACING + 1 * BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, (HMENU)IDC_BUTTON_RESET);

    // create list of active objects
    hwndListView_Objects = createListView(parentWindowHandle, 200, 200, 500, 200, (HMENU)IDC_LISTVIEW_OBJECTS, objectListColums, NUMBER_OF_OBJECTLIST_COLUMNS);
    
    // const char* objectListItem[NUMBER_OF_OBJECTLIST_COLUMNS] = 
    // {
    //     "c_number",
    //     "c_mass",
    //     "Radius",
    //     "Start Pos X",
    //     "Start Pos Y"
    // };

    // createListView_Item(hwndListView_Objects, objectListItem, NUMBER_OF_OBJECTLIST_COLUMNS);
    // createListView_Item(hwndListView_Objects, objectListItem, NUMBER_OF_OBJECTLIST_COLUMNS);

    //create text box to show calculation time of last simulation loop
    hwndText_CalculationTime = createTextField(parentWindowHandle, "0", 200, 10, 160, 20);
}

HWND createButton(HWND parentWindowHandle, const char *name, int position_x, int position_y, int width, int height, HMENU IDC)
{
    return CreateWindowEx( 
        0,                                                      //extended window styles
        "BUTTON",                                               // Predefined class; Unicode assumed
        name,                                                   // window name
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON,  // window style
        position_x,                                             // x position
        position_y,                                             // y position
        width,                                                  // window width
        height,                                                 // nutton height
        parentWindowHandle,                                     // parent window handle
        IDC,                                                    // control ID
        (HINSTANCE)GetWindowLongPtr(parentWindowHandle, GWLP_HINSTANCE),  //handle to the instance
        NULL                                                    // pointer to a value (not needed)
    );
}

HWND createListView(HWND parentWindowHandle, int position_x, int position_y, int width, int height, HMENU IDC, const char ** column_names, int numberOfColumns)
{
    HWND listView_handle = CreateWindowEx( 
        0,                                                      //extended window styles
        WC_LISTVIEW,                                            // Predefined class; Unicode assumed
        "",                                                     // window name
        WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_EDITLABELS | WS_EX_CLIENTEDGE | LVS_SORTASCENDING,  // window style
        position_x,                                             // x position
        position_y,                                             // y position
        width,                                                  // window width
        height,                                                 // nutton height
        parentWindowHandle,                                     // parent window handle
        IDC,                                                    // control ID
        (HINSTANCE)GetWindowLongPtr(parentWindowHandle, GWLP_HINSTANCE),  //handle to the instance
        NULL                                                    // pointer to a value (not needed)
    );

    //change style so that a line is fully selected and not only the first element
    SendMessage(listView_handle, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //create columns
    for (int i = 0; i < numberOfColumns; i++)
    {
        createListView_Column(listView_handle, i, column_names[i], 100);
    }

    return listView_handle;
}

void createListView_Column(HWND listviewHandle, int iCol, const char *text, int width)
{
    LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = width;
	lvc.pszText = (LPSTR)text;
	lvc.iSubItem = iCol;
	ListView_InsertColumn(listviewHandle, iCol, &lvc);
}

int counter = 0;
void createListView_Item(HWND listviewHandle, const char ** itemValues, int numberOfColumns)
{
    LVITEM lvi = {0};
    lvi.mask = LVIF_TEXT;
    lvi.pszText = (LPSTR)itemValues[0];
    ListView_InsertItem(listviewHandle, &lvi);

    for (int i = 1; i < numberOfColumns; i++)
    {
        ListView_SetItemText(listviewHandle, counter, i, (LPSTR)itemValues[i]);
    }
    counter++;
}

void add_ObjectList_Item(int i_number, double d_mass, double d_radius, double d_position_x, double d_position_y)
{
    const char* c_number = std::to_string(i_number).c_str();
    // std::stringstream s_mass;
    // s_mass << d_mass;
    // const char* c_mass = s_mass.str().c_str();

    
    std::cout << c_number << std::endl;

    const char* objectListItem[NUMBER_OF_OBJECTLIST_COLUMNS] = 
    {
        c_number,
        c_number,
        "Radius",
        "Start Pos X",
        "Start Pos Y"
    };

    // const char* objectListItem[NUMBER_OF_OBJECTLIST_COLUMNS] = 
    // {
    //     "c_number",
    //     "c_mass",
    //     "Radius",
    //     "Start Pos X",
    //     "Start Pos Y"
    // };

    createListView_Item(hwndListView_Objects, objectListItem, NUMBER_OF_OBJECTLIST_COLUMNS);
}

void remove_ObjectList_Item(void)
{
    ListView_DeleteItem(hwndListView_Objects, 1);
    counter--;
}

HWND createTextField(HWND parentWindowHandle, const char *name, int position_x, int position_y, int width, int height)
{
    return CreateWindowEx( 
        0,                                                      //extended window styles
        "STATIC",                                               // Predefined class; Unicode assumed
        name,                                                   // window name
        WS_VISIBLE | WS_CHILD | SS_LEFT | SS_SUNKEN,            // window style
        position_x,                                             // x position
        position_y,                                             // y position
        width,                                                  // window width
        height,                                                 // nutton height
        parentWindowHandle,                                     // parent window handle
        NULL,                                                   // control ID
        (HINSTANCE)GetWindowLongPtr(parentWindowHandle, GWLP_HINSTANCE),  //handle to the instance
        NULL                                                    // pointer to a value (not needed)
    );
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

StartStopButton_State get_startstopButton_state(void)
{
    return startstopButton_state;
}

bool is_resetButton_triggered(void)
{
    bool return_val = resetButton_trigger;
    resetButton_trigger = false;
    return return_val;
}


    // create list of active objects
    // hwndListBox_Objects = CreateWindowEx(
    //     WS_EX_CLIENTEDGE, 
    //     "LISTBOX",                                          // Predefined class; Unicode assumed
    //     "test ListBox",                                                // Button text
    //     WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL,    // Styles
    //     400,                                         // x position
    //     20,                 // y position
    //     100,                                           // Button width
    //     100,                                          // Button height
    //     hwnd,                                   // Parent window
    //     (HMENU)IDC_LISTBOX_OBJECTS,                                // control ID
    //     (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
    //     NULL                                                    // Pointer not needed.
    // );
    // SendMessage(hwndListBox_Objects, LB_ADDSTRING, 0, (LPARAM)"first");
    // SendMessage(hwndListBox_Objects, LB_ADDSTRING, 0, (LPARAM)"second");
    // SendMessage(hwndListBox_Objects, LB_ADDSTRING, 0, (LPARAM)"last");