#include "config.hpp"

const char * presetNames[NUMBER_OF_PRESETS]
{
    "2 Body stable orbit",
    "3 Body stable orbit",
    "Stable Orbit symmetric"
};

double mid_x = static_cast<double>(INIT_SIMULATION_WIDTH / 2);
double mid_y = static_cast<double>(INIT_SIMULATION_HEIGHT / 2);

//"2 Body stable orbit"
const objectStruct preset0_objects[PRESET_0_NUMBER_OF_OBJECTS] {
    //  mass       radius       start_pos_x     start_pox_y     start_vel_x     start_vel_y
    {   10000.0,   30.0,        mid_x,          mid_y,          0.0,            0.0},
    {   1.0,       10.0,        mid_x + 200,    mid_y,          0.0,            0.0}
};

//"3 Body stable orbit"
const objectStruct preset1_objects[PRESET_1_NUMBER_OF_OBJECTS] {
    //  mass        radius      start_pos_x     start_pox_y     start_vel_x     start_vel_y
    {   200.0,      10.0,       mid_x,          mid_y,          0.0,            0.0},
    {   20.0,       5.0,        mid_x - 80.0,   mid_y,          0.0,           -1.9},
    {   20.0,       5.0,        mid_x + 80.0,   mid_y,          0.0,            1.9}
};

//"Stable Orbit symmetric"
const objectStruct preset2_objects[PRESET_2_NUMBER_OF_OBJECTS] {
    //  mass        radius      start_pos_x     start_pox_y     start_vel_x     start_vel_y
    {   200.0,      20.0,       mid_x,          mid_y,          0.0,            0.0},
    {   20.0,       5.0,        mid_x - 80.0,   mid_y,          0.0,           -1.9},
    {   20.0,       5.0,        mid_x + 80.0,   mid_y,          0.0,            1.9},
    {   20.0,       5.0,        mid_x,          mid_y - 80.0,   1.9,            0.0},
    {   20.0,       5.0,        mid_x,          mid_y + 80.0,  -1.9,            0.0}
};

const presetStruct presets[NUMBER_OF_PRESETS] {
    {presetNames[0], PRESET_0_NUMBER_OF_OBJECTS, preset0_objects, true},
    {presetNames[1], PRESET_1_NUMBER_OF_OBJECTS, preset1_objects, false},
    {presetNames[2], PRESET_2_NUMBER_OF_OBJECTS, preset2_objects, false}
};

const char* objectListColumsText[NUMBER_OF_OBJECTLIST_COLUMNS]
{
    "Number",
    "Mass",
    "Radius",
    "Start Pos X",
    "Start Pos Y",
    "Start Vel X",
    "Start Vel Y"
};

const int objectListColumsSize[NUMBER_OF_OBJECTLIST_COLUMNS]
{
    60,
    50,
    50,
    80,
    80,
    80,
    80
};