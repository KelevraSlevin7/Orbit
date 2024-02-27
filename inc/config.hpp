#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>

#define NUMBER_OF_OBJECTLIST_COLUMNS 5
extern const char* objectListColums[NUMBER_OF_OBJECTLIST_COLUMNS];

#define NUMBER_OF_PRESETS   3
#define PRESET_0_NUMBER_OF_OBJECTS  2
#define PRESET_1_NUMBER_OF_OBJECTS  3
#define PRESET_2_NUMBER_OF_OBJECTS  1
extern const char * presetNames[NUMBER_OF_PRESETS];

struct objectStruct {
    double mass;
    double radius;
    double start_position_x;
    double start_position_y;
    double start_velocity_x;
    double start_velocity_y;
};

struct presetStruct {
    const char* presetName;
    int number_of_objects;
    const objectStruct *objects;
};

extern const presetStruct presets[NUMBER_OF_PRESETS];

extern int testVar;

#endif /*CONFIG_HPP*/