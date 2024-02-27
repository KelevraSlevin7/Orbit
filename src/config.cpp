#include "config.hpp"

const char* objectListColums[NUMBER_OF_OBJECTLIST_COLUMNS] = 
{
    "Number",
    "Mass",
    "Radius",
    "Start Pos X",
    "Start Pos Y"
};

const char * presetNames[NUMBER_OF_PRESETS] =
{
    "2 Body stable orbit",
    "3 Body stable orbit",
    "i dont know yet"
};


static const objectStruct preset0_objects[PRESET_0_NUMBER_OF_OBJECTS] = {
    (10000.0,   30.0,   200.0,  200.0,  0.0,    0.0),
    (1.0,       10.0,   500.0,  200.0,  0.0,    0.0)
};

static const objectStruct preset1_objects[PRESET_1_NUMBER_OF_OBJECTS] = {
    (200.0,     10.0,   200.0,  200.0,  0.0,    0.0),
    (20.0,      5.0,    120.0,  200.0,  0.0,   -1.9),
    (20.0,      5.0,    280.0,  200.0,  0.0,    1.9)
};

static const objectStruct preset2_objects[PRESET_2_NUMBER_OF_OBJECTS] = {
    (10000.0,    30.0,  0.0,    0.0,    0.0,    0.0)
};

const presetStruct presets[NUMBER_OF_PRESETS] = {
    "2 Body stable orbit",  PRESET_0_NUMBER_OF_OBJECTS, preset0_objects,
    "3 Body stable orbit",  PRESET_1_NUMBER_OF_OBJECTS, preset1_objects,
    "i dont know yet",      PRESET_2_NUMBER_OF_OBJECTS, preset2_objects
};

int testVar = 5;