#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <vector>
#include "config.hpp"
#include "window_handling.hpp"

HWND hwndComboBox_Presets;
HWND hwndListView_Objects;
HWND hwndText_CalculationTime;

bool simulationWindow_active{true};
int simulationWindow_width{0};
int simulationWindow_height{0};
bool controlWindow_active{true};

int presets_selected_index{0};

static WNDPROC OriginalEditCtrlProc = NULL;

POINT simulationWindwos_leftclickDownPos;
POINT simulationWindwos_leftclickUpPos;

struct buttonStruct {
    int     id;
    HWND    handle;
    bool    triggered;
};
std::vector<buttonStruct> buttonVector;

struct textFieldStruct {
    int     id;
    HWND    handle;
};
std::vector<textFieldStruct> textFieldVector;

int objectList_item_counter{0};
std::vector< std::vector<char*> >ObjectListItemVector;

//------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------static functions------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
void    fillControlWindow       (HWND parentWindowHandle);
HWND    createButton            (HWND parentWindowHandle, const char *name, int position_x, int position_y, int width, int height, int IDC, int addional_style);
HWND    createComboBox          (HWND parentWindowHandle, int position_x, int position_y, int width, int height, HMENU IDC, const char ** item_names, int number_of_columns);
HWND    createListView          (HWND parentWindowHandle, int position_x, int position_y, int width, int height, HMENU IDC, const char ** column_names, const int *column_sizes, int number_of_columns);
void    createListViewColumn    (HWND listviewHandle, int iCol, const char *text, int width);
void    addListViewItem         (HWND listviewHandle, std::vector<char*> &itemValues, int number_of_columns, int &item_counter);
HWND    createTextField         (HWND parentWindowHandle, const char *init_input, int position_x, int position_y, int width, int height, int id, int addional_style);
HWND    createInputField        (HWND parentWindowHandle, const char *init_input, int position_x, int position_y, int width, int height, int id, WNDPROC function_callback);
char *  convertIntToChar        (int input);
char *  convertDoubleToChar     (double input);
HWND    getButtonHandle         (int buttonId);
HWND    getTextFieldHandle      (int inputId);

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

        //left button was clicked inside simulation window
        case WM_LBUTTONDOWN:
        {
            if (GetCursorPos(&simulationWindwos_leftclickDownPos))
            {
                if (ScreenToClient(hwnd, &simulationWindwos_leftclickDownPos))
                {
                    //invert y coordinate
                    simulationWindwos_leftclickDownPos.y = simulationWindow_height - simulationWindwos_leftclickDownPos.y;
                    //write values to the text fields
                    setTextFieldValueAsDouble(IDC_TEXT_INPUT_POSX, double(simulationWindwos_leftclickDownPos.x));
                    setTextFieldValueAsDouble(IDC_TEXT_INPUT_POSY, double(simulationWindwos_leftclickDownPos.y));
                }
            }
            break;
        }

        //left button was clicked inside simulation window
        case WM_LBUTTONUP:
        {
            if (GetCursorPos(&simulationWindwos_leftclickUpPos))
            {
                if (ScreenToClient(hwnd, &simulationWindwos_leftclickUpPos))
                {
                    //invert y coordinate
                    simulationWindwos_leftclickUpPos.y = simulationWindow_height - simulationWindwos_leftclickUpPos.y;
                    double vel_x = static_cast<double>(simulationWindwos_leftclickUpPos.x - simulationWindwos_leftclickDownPos.x) / VELOCITY_SCALE_REDUCTION;
                    double vel_y = static_cast<double>(simulationWindwos_leftclickUpPos.y - simulationWindwos_leftclickDownPos.y) / VELOCITY_SCALE_REDUCTION;
                    //write values to the text fields
                    setTextFieldValueAsDouble(IDC_TEXT_INPUT_VELX, vel_x);
                    setTextFieldValueAsDouble(IDC_TEXT_INPUT_VELY, vel_y);
                }
            }
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
            //check if any button is triggered
            for (int i = 0; i < buttonVector.size(); ++i)
            {
                if (LOWORD(wParam) == buttonVector[i].id)
                {
                    buttonVector[i].triggered = true;
                }
            }

            //comboBox item was selected
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                //presets comboBox item was selected
                if ((HWND)lParam == hwndComboBox_Presets)
                {
                    presets_selected_index = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                }
            }

            //"randomize" button was pressed
            if (isButtonTriggered(IDC_BUTTON_RANDOMIZE))
            {
                //check which state the check box button has
                if (getCheckBoxState(IDC_BUTTON_RANDOMIZE) == true)
                {
                    //change checkbox state to unchecked
                    SendMessage(getButtonHandle(IDC_BUTTON_RANDOMIZE), BM_SETCHECK, BST_UNCHECKED, 0);

                    //update text box test
                    setTextFieldValueAsText(IDC_TEXT_STATIC_POSX, "Pos X");
                    setTextFieldValueAsText(IDC_TEXT_STATIC_POSY, "Pos Y");
                    setTextFieldValueAsText(IDC_TEXT_STATIC_VELX, "Vel X");
                    setTextFieldValueAsText(IDC_TEXT_STATIC_VELY, "Vel Y");

                    setTextFieldValueAsDouble(IDC_TEXT_INPUT_POSX, 0.0);
                    setTextFieldValueAsDouble(IDC_TEXT_INPUT_POSY, 0.0);
                    setTextFieldValueAsDouble(IDC_TEXT_INPUT_VELX, 0.0);
                    setTextFieldValueAsDouble(IDC_TEXT_INPUT_VELY, 0.0);
                    
                }
                else
                {
                    //change checkbox state to checked
                    SendMessage(getButtonHandle(IDC_BUTTON_RANDOMIZE), BM_SETCHECK, BST_CHECKED, 0);

                    //update text box test
                    setTextFieldValueAsText(IDC_TEXT_STATIC_POSX, "    Position - ");
                    setTextFieldValueAsText(IDC_TEXT_STATIC_POSY, "Range");
                    setTextFieldValueAsText(IDC_TEXT_STATIC_VELX, "    Velocity - ");
                    setTextFieldValueAsText(IDC_TEXT_STATIC_VELY, "Range");

                    int min_dim = ((simulationWindow_width < simulationWindow_height) ? simulationWindow_width : simulationWindow_height);
                    setTextFieldValueAsDouble(IDC_TEXT_INPUT_POSX, 0.0);
                    setTextFieldValueAsDouble(IDC_TEXT_INPUT_POSY, static_cast<double>(min_dim));
                    setTextFieldValueAsDouble(IDC_TEXT_INPUT_VELX, -2.0);
                    setTextFieldValueAsDouble(IDC_TEXT_INPUT_VELY, 2.0);
                }
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

LRESULT CALLBACK customDoubleInput_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_CHAR)
    {
        //only allow numbers and "."
        if ((wParam < '0' || wParam > '9') &&
            (wParam != '.') &&
            (wParam != VK_RETURN) &&
            (wParam != VK_DELETE) &&
            (wParam != VK_BACK) &&
            (wParam != '-'))
            {
                return 0;
            }
    }
    return CallWindowProc(OriginalEditCtrlProc, hwnd, uMsg, wParam, lParam);
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
		NULL,                                   //Parent window    
		NULL,                                   //Menu
		hInstance,                              //Instance handle
		NULL                                    //Additional application data
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
        INIT_CONTROL_POS_X, INIT_CONTROL_POS_Y, //position
        INIT_CONTROL_WIDTH, INIT_CONTROL_HEIGHT,//size
		NULL,                                   //Parent window    
		NULL,                                   //Menu
		hInstance,                              //Instance handle
		NULL                                    //Additional application data
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
    createButton(parentWindowHandle, "Stop", BUTTON_START_POSX, BUTTON_START_POSY, BUTTON_START_WIDTH, BUTTON_START_HEIGHT, IDC_BUTTON_STARTSTOP, BS_DEFPUSHBUTTON);

    //create button to reset the simulation to the initial state
    createButton(parentWindowHandle, "Reset", BUTTON_RESET_POSX, BUTTON_RESET_POSY, BUTTON_RESET_WIDTH, BUTTON_RESET_HEIGHT, IDC_BUTTON_RESET, BS_DEFPUSHBUTTON);

    //create button and comboBox for presets
    createButton(parentWindowHandle, "Load Preset", BUTTON_LOADPRESET_POSX, BUTTON_LOADPRESET_POSY, BUTTON_LOADPRESET_WIDTH, BUTTON_LOADPRESET_HEIGHT, IDC_BUTTON_LOADPRESET, BS_DEFPUSHBUTTON);
    hwndComboBox_Presets = createComboBox(parentWindowHandle, COMBOBOX_PRESETS_POSX, COMBOBOX_PRESETS_POSY, COMBOBOX_PRESETS_WIDTH, COMBOBOX_PRESETS_HEIGHT, (HMENU)IDC_COMBOBOX_PRESETS, presetNames, NUMBER_OF_PRESETS);
    
    //create list of active objects
    hwndListView_Objects = createListView(parentWindowHandle, LISTVIEW_OBJECTLIST_POSX, LISTVIEW_OBJECTLIST_POSY, LISTVIEW_OBJECTLIST_WIDTH, LISTVIEW_OBJECTLIST_HEIGHT, (HMENU)IDC_LISTVIEW_OBJECTS, objectListColumsText, objectListColumsSize, NUMBER_OF_OBJECTLIST_COLUMNS);
    createButton(parentWindowHandle, "Add", BUTTON_ADD_POSX, BUTTON_ADD_POSY, BUTTON_ADD_WIDTH, BUTTON_ADD_HEIGHT, IDC_BUTTON_ADD, BS_DEFPUSHBUTTON);
    createButton(parentWindowHandle, "Remove", BUTTON_REMOVE_POSX, BUTTON_REMOVE_POSY, BUTTON_REMOVE_WIDTH, BUTTON_REMOVE_HEIGHT, IDC_BUTTON_REMOVE, BS_DEFPUSHBUTTON);

    //create text box to show calculation time of last simulation loop
    hwndText_CalculationTime = createTextField(parentWindowHandle, "0", TEXT_CALCULATIONTIME_POSX, TEXT_CALCULATIONTIME_POSY, TEXT_CALCULATIONTIME_WIDTH, TEXT_CALCULATIONTIME_HEIGHT, IDC_TEXT_STATIC_CALCTIME, SS_SUNKEN);

    //create input text fields for object creation
    createInputField(parentWindowHandle, "1", TEXT_INPUT_LEFT_POSX,  TEXT_INPUT_MASSRADIUS_POSY, TEXT_INPUT_WIDTH, TEXT_INPUT_HEIGHT, IDC_TEXT_INPUT_MASS,  customDoubleInput_callback);
    createInputField(parentWindowHandle, "5", TEXT_INPUT_RIGHT_POSX, TEXT_INPUT_MASSRADIUS_POSY, TEXT_INPUT_WIDTH, TEXT_INPUT_HEIGHT, IDC_TEXT_INPUT_RADIUS,customDoubleInput_callback);
    createInputField(parentWindowHandle, "0", TEXT_INPUT_LEFT_POSX,  TEXT_INPUT_POSITION_POSY,   TEXT_INPUT_WIDTH, TEXT_INPUT_HEIGHT, IDC_TEXT_INPUT_POSX,  customDoubleInput_callback);
    createInputField(parentWindowHandle, "0", TEXT_INPUT_RIGHT_POSX, TEXT_INPUT_POSITION_POSY,   TEXT_INPUT_WIDTH, TEXT_INPUT_HEIGHT, IDC_TEXT_INPUT_POSY,  customDoubleInput_callback);
    createInputField(parentWindowHandle, "0", TEXT_INPUT_LEFT_POSX,  TEXT_INPUT_VELOCITY_POSY,   TEXT_INPUT_WIDTH, TEXT_INPUT_HEIGHT, IDC_TEXT_INPUT_VELX,  customDoubleInput_callback);
    createInputField(parentWindowHandle, "0", TEXT_INPUT_RIGHT_POSX, TEXT_INPUT_VELOCITY_POSY,   TEXT_INPUT_WIDTH, TEXT_INPUT_HEIGHT, IDC_TEXT_INPUT_VELY,  customDoubleInput_callback);

    //create static text fields for description of input text fields
    createTextField(parentWindowHandle, "Mass",     TEXT_INPUT_LEFT_POSX,   TEXT_INPUT_MASSRADIUS_POSY - 20,    TEXT_STATIC_WIDTH, TEXT_STATIC_HEIGHT, IDC_TEXT_STATIC_MASS, 0);
    createTextField(parentWindowHandle, "Radius",   TEXT_INPUT_RIGHT_POSX,  TEXT_INPUT_MASSRADIUS_POSY - 20,    TEXT_STATIC_WIDTH, TEXT_STATIC_HEIGHT, IDC_TEXT_STATIC_RADIUS, 0);
    createTextField(parentWindowHandle, "Pos X",    TEXT_INPUT_LEFT_POSX,   TEXT_INPUT_POSITION_POSY - 20,      TEXT_STATIC_WIDTH, TEXT_STATIC_HEIGHT, IDC_TEXT_STATIC_POSX, 0);
    createTextField(parentWindowHandle, "Pos Y",    TEXT_INPUT_RIGHT_POSX,  TEXT_INPUT_POSITION_POSY - 20,      TEXT_STATIC_WIDTH, TEXT_STATIC_HEIGHT, IDC_TEXT_STATIC_POSY, 0);
    createTextField(parentWindowHandle, "Vel X",    TEXT_INPUT_LEFT_POSX,   TEXT_INPUT_VELOCITY_POSY - 20,      TEXT_STATIC_WIDTH, TEXT_STATIC_HEIGHT, IDC_TEXT_STATIC_VELX, 0);
    createTextField(parentWindowHandle, "Vel Y",    TEXT_INPUT_RIGHT_POSX,  TEXT_INPUT_VELOCITY_POSY - 20,      TEXT_STATIC_WIDTH, TEXT_STATIC_HEIGHT, IDC_TEXT_STATIC_VELY, 0);

    //create checkbox button for randomization of position and velocity
    createButton(parentWindowHandle, "Randomize", BUTTON_RANDOMIZE_POSX, BUTTON_RANDOMIZE_POSY, BUTTON_RANDOMIZE_WIDTH, BUTTON_RANDOMIZE_HEIGHT, IDC_BUTTON_RANDOMIZE, BS_CHECKBOX);
}

HWND createButton(HWND parentWindowHandle, const char *name, int position_x, int position_y, int width, int height, int IDC, int addional_style)
{
    HWND buttonHandle = CreateWindowEx( 
        0,                                                      //extended window styles
        "BUTTON",                                               // Predefined class; Unicode assumed
        name,                                                   // window name
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | addional_style,    // window style
        position_x,                                             // x position
        position_y,                                             // y position
        width,                                                  // window width
        height,                                                 // nutton height
        parentWindowHandle,                                     // parent window handle
        reinterpret_cast<HMENU>(IDC),                           // control ID
        (HINSTANCE)GetWindowLongPtr(parentWindowHandle, GWLP_HINSTANCE),  //handle to the instance
        NULL                                                    // pointer to a value (not needed)
    );

    buttonStruct buttonBuffer {IDC, buttonHandle, false};
    buttonVector.push_back(buttonBuffer);

    return buttonHandle;
}

HWND createComboBox(HWND parentWindowHandle, int position_x, int position_y, int width, int height, HMENU IDC, const char ** item_names, int number_of_columns)
{
    HWND comboBox_handle =  CreateWindowEx( 
        0,                                                      //extended window styles
        "COMBOBOX",                                             // Predefined class; Unicode assumed
        "",                                                     // window name
        WS_VISIBLE | WS_CHILD | WS_OVERLAPPED | CBS_DROPDOWN | CBS_HASSTRINGS,  // window style
        position_x,                                             // x position
        position_y,                                             // y position
        width,                                                  // window width
        height,                                                 // nutton height
        parentWindowHandle,                                     // parent window handle
        IDC,                                                    // control ID
        (HINSTANCE)GetWindowLongPtr(parentWindowHandle, GWLP_HINSTANCE),  //handle to the instance
        NULL                                                    // pointer to a value (not needed)
    );

    //add drop down items
    for (int i = 0; i < number_of_columns; i++)
    {
        SendMessage(comboBox_handle, CB_ADDSTRING, (WPARAM)0, (LPARAM)item_names[i]);
    }
    //select first one as initial value
    SendMessage (comboBox_handle, CB_SETCURSEL, (WPARAM) 0, (LPARAM) 0);

    return comboBox_handle;
}

HWND createListView(HWND parentWindowHandle, int position_x, int position_y, int width, int height, HMENU IDC, const char ** column_names, const int *column_sizes, int number_of_columns)
{
    HWND listView_handle = CreateWindowEx( 
        0,                                                      //extended window styles
        WC_LISTVIEW,                                            // Predefined class; Unicode assumed
        "",                                                     // window name
        WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_EDITLABELS | WS_EX_CLIENTEDGE, // window style (no sorting)
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
    for (int i = 0; i < number_of_columns; i++)
    {
        createListViewColumn(listView_handle, i, column_names[i], column_sizes[i]);
    }

    return listView_handle;
}

HWND createTextField(HWND parentWindowHandle, const char *init_input, int position_x, int position_y, int width, int height, int id, int addional_style)
{
    HWND textFieldHandle = CreateWindowEx( 
        0,                                                      //extended window styles
        "STATIC",                                               // Predefined class; Unicode assumed
        init_input,                                             // initial text
        WS_VISIBLE | WS_CHILD | SS_LEFT | addional_style,       // window style
        // WS_VISIBLE | WS_CHILD | SS_LEFT | SS_SUNKEN,            // window style
        position_x,                                             // x position
        position_y,                                             // y position
        width,                                                  // window width
        height,                                                 // nutton height
        parentWindowHandle,                                     // parent window handle
        reinterpret_cast<HMENU>(id),                            // control ID
        (HINSTANCE)GetWindowLongPtr(parentWindowHandle, GWLP_HINSTANCE),  //handle to the instance
        NULL                                                    // pointer to a value (not needed)
    );

    //store input data inside vector
    textFieldStruct textFieldBuffer {id, textFieldHandle};
    textFieldVector.push_back(textFieldBuffer);

    return textFieldHandle;
}

HWND createInputField(HWND parentWindowHandle, const char *init_input, int position_x, int position_y, int width, int height, int id, WNDPROC function_callback)
{
    HWND inputHandle = CreateWindowEx( 
        0,                                                      //extended window styles
        "EDIT",                                                 // Predefined class; Unicode assumed
        init_input,                                             // initial text
        WS_VISIBLE | WS_CHILD | ES_LEFT | WS_BORDER,            // window style
        position_x,                                             // x position
        position_y,                                             // y position
        width,                                                  // window width
        height,                                                 // nutton height
        parentWindowHandle,                                     // parent window handle
        reinterpret_cast<HMENU>(id),                            // control ID
        (HINSTANCE)GetWindowLongPtr(parentWindowHandle, GWLP_HINSTANCE),  //handle to the instance
        NULL                                                    // pointer to a value (not needed)
    );

    if (inputHandle != NULL)
    {
        //replace callback function
        WNDPROC new_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(inputHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(function_callback)));
        if (OriginalEditCtrlProc == NULL)
        {
            OriginalEditCtrlProc = new_proc;
        }
    }

    //store input data inside vector
    textFieldStruct inputBuffer {id, inputHandle};
    textFieldVector.push_back(inputBuffer);

    return inputHandle;
}

//general function to add ListView Column
void createListViewColumn(HWND listviewHandle, int iCol, const char *text, int width)
{
    LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = width;
	lvc.pszText = (LPSTR)text;
	lvc.iSubItem = iCol;
	ListView_InsertColumn(listviewHandle, iCol, &lvc);
}

//general function to add ListView Row
void addListViewItem(HWND listviewHandle, std::vector<char*> &itemValues, int number_of_columns, int &item_counter)
{
    LVITEM lvi = {0};
    lvi.mask = LVIF_TEXT;
    lvi.pszText = (LPSTR)itemValues[0];
    int itemPos = ListView_InsertItem(listviewHandle, &lvi); //this line will already sort the item

    for (int i = 1; i < number_of_columns; i++)
    {
        ListView_SetItemText(listviewHandle, itemPos, i, (LPSTR)itemValues[i]); //the item texts need to put to the position of the first item
    }
    item_counter++;
}

//general function to remove ListView Row
void removeListViewItem(HWND listviewHandle, int index, int &item_counter)
{
    ListView_DeleteItem(listviewHandle, index);
    item_counter--;
}

//specific function to add Item to Object ListView
void addObjectListItem(unsigned int i_number, double d_mass, double d_radius, double d_position_x, double d_position_y, double d_vel_x, double d_vel_y)
{
    char* c_number      = convertIntToChar(i_number);
    char* c_mass        = convertDoubleToChar(d_mass);
    char* c_radius      = convertDoubleToChar(d_radius);
    char* c_position_x  = convertDoubleToChar(d_position_x);
    char* c_position_y  = convertDoubleToChar(d_position_y);
    char* c_vel_x       = convertDoubleToChar(d_vel_x);
    char* c_vel_y       = convertDoubleToChar(d_vel_y);

    std::vector<char*> bufferVector {c_number, c_mass, c_radius, c_position_x, c_position_y, c_vel_x, c_vel_y};
    ObjectListItemVector.push_back(bufferVector);

    addListViewItem(hwndListView_Objects, ObjectListItemVector.back(), ObjectListItemVector.back().size(), objectList_item_counter);
}

//specific function to remove Item to Object ListView
void removeObjectListItem(int index)
{
    removeListViewItem(hwndListView_Objects, index, objectList_item_counter);
}

//specific function to remove all Item from Object ListView
void clearObjectList(void)
{
    ObjectListItemVector.clear();

    //necessary so that the for loop is not affected by a changed counter
    int rowsToDelete = objectList_item_counter;
    for (int i = 0; i < rowsToDelete; i++)
    {
        removeListViewItem(hwndListView_Objects, 0, objectList_item_counter);
    }
}

//specific function to get selected item index
int getObjectListSelectedIndex(void)
{
    return SendMessage(hwndListView_Objects, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
}

//specific function to remove selected item
void removeObjectListSelectedItem(void)
{
    int selectedItem = getObjectListSelectedIndex();
    removeObjectListItem(selectedItem);
}

void updateCalculationTimeText(long long int calculationDuration)
{
    char text[30];
    sprintf(text, "Loop Time: %d us", calculationDuration);
    SetWindowText(hwndText_CalculationTime, TEXT(text));
}

double getTextFieldValueAsDouble(int inputId)
{
    for (int i = 0; i < textFieldVector.size(); ++i)
    {
        if (textFieldVector[i].id == inputId)
        {
            TCHAR input_string[1024];
            GetWindowText(textFieldVector[i].handle, input_string, sizeof(input_string));
            return atof(input_string);
        }
    }
    return 0.0;
}

void setTextFieldValueAsDouble(int inputId, double input_value)
{
    SetWindowText(getTextFieldHandle(inputId), TEXT(convertDoubleToChar(input_value)));
}

void setTextFieldValueAsText(int inputId, const char* text)
{
    SetWindowText(getTextFieldHandle(inputId), TEXT(text));
}




HWND getButtonHandle(int buttonId)
{
    for (int i = 0; i < buttonVector.size(); ++i)
    {
        if (buttonVector[i].id == buttonId)
        {
            return buttonVector[i].handle;
        }
    }
    return NULL;
}

HWND getTextFieldHandle(int inputId)
{
    for (int i = 0; i < textFieldVector.size(); ++i)
    {
        if (textFieldVector[i].id == inputId)
        {
            return textFieldVector[i].handle;
        }
    }
    return NULL;
}

bool getCheckBoxState(int buttonId)
{
    return SendMessage(getButtonHandle(buttonId), BM_GETCHECK, 0, 0);
}

bool getSimulationWindowStatus(void)
{
    return simulationWindow_active;
}

bool getControlWindowStatus(void)
{
    return controlWindow_active;
}

int getSimulationWindowWidth(void)
{
    return simulationWindow_width;
}

int getSimulationWindowHeight(void)
{
    return simulationWindow_height;
}

void writeStartStopButtonText(const char* text)
{
    SendMessage(getButtonHandle(IDC_BUTTON_STARTSTOP), WM_SETTEXT, 0, (LPARAM)text);
}

int get_presets_selection(void)
{
    return presets_selected_index;
}

bool isButtonTriggered(int buttonId)
{
    bool triggered{false};
    for (int i = 0; i < buttonVector.size(); ++i)
    {
        if (buttonVector[i].id == buttonId)
        {
            triggered = buttonVector[i].triggered;
            buttonVector[i].triggered = false;
        }
    }
    return triggered;
}

char * convertDoubleToChar(double input)
{
    std::string input_str = std::to_string(input);
    //remove trailing zeroes
    input_str.erase (input_str.find_last_not_of('0') + 1, std::string::npos);
    input_str.erase (input_str.find_last_not_of('.') + 1, std::string::npos);

    const std::string::size_type size = input_str.size();
    char *output = new char[size + 1];
    memcpy(output, input_str.c_str(), size + 1);
    return output;
}

char * convertIntToChar(int input)
{
    std::string input_str = std::to_string(input);
    const std::string::size_type size = input_str.size();
    char *output = new char[size + 1];
    memcpy(output, input_str.c_str(), size + 1);
    return output;
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