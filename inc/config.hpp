#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>

#define NUMBER_OF_OBJECTLIST_COLUMNS 5
inline const char* objectListColums[NUMBER_OF_OBJECTLIST_COLUMNS] = 
{
    "Number",
    "Mass",
    "Radius",
    "Start Pos X",
    "Start Pos Y"
};

#define NUMBER_OF_PRESETS   3
inline const char * presetNames[NUMBER_OF_PRESETS] =
{
    "2 Body stable orbit",
    "3 Body stable orbit",
    "i dont know yet"
};

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
    objectStruct objects[];
};

// inline presetStruct preset0 = {
//     presetNames[0],
//     2,
//     (10000.0,   30.0,   200.0,    200.0,    0.0,    0.0),
//     (1.0,       10.0,   500.0,    200.0,    0.0,    0.0)
// };

// inline presetStruct preset1 = {
//     presetNames[1],
//     3,
//     (200.0,     10.0,   200.0,  200.0,  0.0,    0.0),
//     (20.0,      5.0,    120.0,  200.0,  0.0,    -1.9),
//     (20.0,      5.0,    280.0,  200.0,  0.0,    1.9)
// };

// inline presetStruct preset2 = {
//     presetNames[2],
//     1,
//     (10000.0,    30.0,  0.0,    0.0,    0.0,    0.0)
// };




#endif /*CONFIG_HPP*/