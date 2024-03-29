#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>

//targeted loop time in milliseconds
#define LOOP_TIME_MS 10

//initial simulation window size
#define INIT_SIMULATION_WIDTH   800
#define INIT_SIMULATION_HEIGHT  800
#define INIT_SIMULATION_POS_X   100
#define INIT_SIMULATION_POS_Y   100

//initial control window size
#define INIT_CONTROL_WIDTH   710
#define INIT_CONTROL_HEIGHT  470
#define INIT_CONTROL_POS_X   (INIT_SIMULATION_POS_X + INIT_SIMULATION_WIDTH + 100)
#define INIT_CONTROL_POS_Y   (INIT_SIMULATION_POS_Y)

//"start stop" button
//size
    #define BUTTON_START_WIDTH          150
    #define BUTTON_START_HEIGHT         70
//pos
    #define BUTTON_START_POSX           20
    #define BUTTON_START_POSY           20

//"reset" buton
//size
    #define BUTTON_RESET_WIDTH          150
    #define BUTTON_RESET_HEIGHT         70
//pos
    #define BUTTON_RESET_POSX           (BUTTON_START_POSX)
    #define BUTTON_RESET_POSY           (BUTTON_START_POSX + BUTTON_START_HEIGHT + 20)

//"load preset" button
//size
    #define BUTTON_LOADPRESET_WIDTH     180
    #define BUTTON_LOADPRESET_HEIGHT    40
//pos
    #define BUTTON_LOADPRESET_POSX      (BUTTON_START_POSX + BUTTON_START_WIDTH + 30)
    #define BUTTON_LOADPRESET_POSY      (BUTTON_START_POSY)

//"preset" comboBox
//size
    #define COMBOBOX_PRESETS_WIDTH      (BUTTON_LOADPRESET_WIDTH)
    #define COMBOBOX_PRESETS_HEIGHT     500     //height needs to be this big, so the drop down menu can be seen
//pos
    #define COMBOBOX_PRESETS_POSX       (BUTTON_LOADPRESET_POSX)
    #define COMBOBOX_PRESETS_POSY       (BUTTON_LOADPRESET_POSY + BUTTON_LOADPRESET_HEIGHT + 10)

//"object list" listView
//size
    #define LISTVIEW_OBJECTLIST_WIDTH   480
    #define LISTVIEW_OBJECTLIST_HEIGHT  310
//pos
    #define LISTVIEW_OBJECTLIST_POSX    (COMBOBOX_PRESETS_POSX)
    #define LISTVIEW_OBJECTLIST_POSY    (COMBOBOX_PRESETS_POSY + 40)

//"add" button
//size
    #define BUTTON_ADD_WIDTH            (BUTTON_RESET_WIDTH)
    #define BUTTON_ADD_HEIGHT           30
//pos
    #define BUTTON_ADD_POSX             (BUTTON_RESET_POSX)
    #define BUTTON_ADD_POSY             (BUTTON_RESET_POSY + BUTTON_RESET_HEIGHT + 20)

//"remove" button
//size
    #define BUTTON_REMOVE_WIDTH         (BUTTON_ADD_WIDTH)
    #define BUTTON_REMOVE_HEIGHT        30
//pos
    #define BUTTON_REMOVE_POSX          (BUTTON_ADD_POSX)
    #define BUTTON_REMOVE_POSY          (BUTTON_ADD_POSY + BUTTON_ADD_HEIGHT + 160)

//"calculation time" text
//size
    #define TEXT_CALCULATIONTIME_WIDTH  160
    #define TEXT_CALCULATIONTIME_HEIGHT 20
//pos
    #define TEXT_CALCULATIONTIME_POSX   LISTVIEW_OBJECTLIST_POSX + LISTVIEW_OBJECTLIST_WIDTH - TEXT_CALCULATIONTIME_WIDTH
    #define TEXT_CALCULATIONTIME_POSY   (BUTTON_START_POSY)

//input fields
//size
    #define TEXT_INPUT_WIDTH            65
    #define TEXT_INPUT_HEIGHT           20
//pos
    #define TEXT_INPUT_LEFT_POSX        20
    #define TEXT_INPUT_RIGHT_POSX       (TEXT_INPUT_LEFT_POSX + TEXT_INPUT_WIDTH + 20)
    #define TEXT_INPUT_MASSRADIUS_POSY  (BUTTON_ADD_POSY + BUTTON_ADD_HEIGHT + 20)
    #define TEXT_INPUT_POSITION_POSY    (TEXT_INPUT_MASSRADIUS_POSY + TEXT_INPUT_HEIGHT + 20)
    #define TEXT_INPUT_VELOCITY_POSY    (TEXT_INPUT_POSITION_POSY + TEXT_INPUT_HEIGHT + 20)

//static fields
//size
    #define TEXT_STATIC_WIDTH           80
    #define TEXT_STATIC_HEIGHT          20

//"randomize" checkbox
//size
    #define BUTTON_RANDOMIZE_WIDTH      100
    #define BUTTON_RANDOMIZE_HEIGHT     20
//pos
    #define BUTTON_RANDOMIZE_POSX       (TEXT_INPUT_LEFT_POSX)
    #define BUTTON_RANDOMIZE_POSY       (TEXT_INPUT_VELOCITY_POSY + TEXT_INPUT_HEIGHT + 10)

//number of presets and the correpsonding number of objects per preset
#define NUMBER_OF_PRESETS   5
#define PRESET_0_NUMBER_OF_OBJECTS  2
#define PRESET_1_NUMBER_OF_OBJECTS  3
#define PRESET_2_NUMBER_OF_OBJECTS  5
#define PRESET_3_NUMBER_OF_OBJECTS  5
#define PRESET_4_NUMBER_OF_OBJECTS  3
extern const char * presetNames[NUMBER_OF_PRESETS];

//list of column names for the listView
#define NUMBER_OF_OBJECTLIST_COLUMNS 7

//length of object trail
#define TRAIL_LENGTH    100

//config of limits of objects (only used when radius is scaled according to mass)
#define MASS_MIN        1.0
#define MASS_MAX        20.0
#define RADIUS_MIN      5.0
#define RADIUS_MAX      15.0

//velocity drag input scaler
#define VELOCITY_SCALE_REDUCTION    40

struct objectStruct {
    double mass;
    double radius;
    double start_position_x;
    double start_position_y;
    double start_velocity_x;
    double start_velocity_y;
};

struct presetStruct {
    const char* presetName;         //name of the preset
    int number_of_objects;          //number of objects in this preset
    const objectStruct *objects;    //pointer to object array
    bool calculate_stable;          //flag to calculate start velocities so that the orbit is stable (will ignore first object)
};

extern const presetStruct presets[NUMBER_OF_PRESETS];

extern const char* objectListColumsText[NUMBER_OF_OBJECTLIST_COLUMNS];
extern const int objectListColumsSize[NUMBER_OF_OBJECTLIST_COLUMNS];


#endif /*CONFIG_HPP*/