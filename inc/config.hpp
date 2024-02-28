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

//number of presets and the correpsonding number of objects per preset
#define NUMBER_OF_PRESETS   3
#define PRESET_0_NUMBER_OF_OBJECTS  2
#define PRESET_1_NUMBER_OF_OBJECTS  3
#define PRESET_2_NUMBER_OF_OBJECTS  5
extern const char * presetNames[NUMBER_OF_PRESETS];

//list of column names for the listView
#define NUMBER_OF_OBJECTLIST_COLUMNS 5

//button sizes
#define BUTTON_WIDTH    150
#define BUTTON_HEIGHT   70
#define BUTTON_SPACING  20

//length of object trail
#define TRAIL_LENGTH    100

//config of limits of objects (only used when radius is scaled according to mass)
#define MASS_MIN        1.0
#define MASS_MAX        20.0
#define RADIUS_MIN      5.0
#define RADIUS_MAX      15.0



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