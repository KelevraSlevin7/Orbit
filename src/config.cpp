#include "config.hpp"

const char * presetNames[NUMBER_OF_PRESETS]
{
    "2 Body stable orbit",
    "3 Body stable orbit",
    "Stable Orbit symmetric",
    "Stable Orbit asymmetric",
    "Sun Earth Moon"
};

double mid_x = static_cast<double>(INIT_SIMULATION_WIDTH / 2);
double mid_y = static_cast<double>(INIT_SIMULATION_HEIGHT / 2);

//"2 Body stable orbit"
const objectStruct preset0_objects[PRESET_0_NUMBER_OF_OBJECTS] {
    //  mass       radius       start_pos_x     start_pox_y     start_vel_x     start_vel_y
    {   10000.0,   40.0,        mid_x,          mid_y,          0.0,            0.0},
    {   1.0,       15.0,        mid_x + 200,    mid_y,          0.0,            0.0}
};

//"3 Body stable orbit"
const objectStruct preset1_objects[PRESET_1_NUMBER_OF_OBJECTS] {
    //  mass        radius      start_pos_x     start_pox_y     start_vel_x     start_vel_y
    {   200.0,      25.0,       mid_x,          mid_y,          0.0,            0.0},
    {   20.0,       10.0,       mid_x - 80.0,   mid_y,          0.0,           -1.9},
    {   20.0,       10.0,       mid_x + 80.0,   mid_y,          0.0,            1.9}
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

//"Stable Orbit asymmetric"
const objectStruct preset3_objects[PRESET_3_NUMBER_OF_OBJECTS] {
    //  mass        radius      start_pos_x     start_pox_y     start_vel_x     start_vel_y
    {   2000.0,     20.0,       mid_x,          mid_y,          0.0,            0.0},
    {   1.0,        4.0,        mid_x - 40.0,   mid_y,          0.0,           -8.8},
    {   1.0,        6.0,        mid_x + 180.0,  mid_y,          0.0,            2.5},
    {   1.0,        8.0,        mid_x,          mid_y + 300.0, -1.5,            0.0},
    {   1.0,        10.0,       mid_x,          mid_y - 300.0,  2.4,            0.0}
};

//"Sun Earth Moon"
const objectStruct preset4_objects[PRESET_4_NUMBER_OF_OBJECTS] {
    //  mass        radius      start_pos_x     start_pox_y     start_vel_x     start_vel_y
    {   2000.0,     20.0,       mid_x,          mid_y,          0.0,            0.0},
    {   20.0,       6.0,        mid_x + 300.0,  mid_y,          0.0,            2.5},
    {   1.0,        4.0,        mid_x + 320.0,  mid_y,          0.0,            2.7}
};

const presetStruct presets[NUMBER_OF_PRESETS] {
    {presetNames[0], PRESET_0_NUMBER_OF_OBJECTS, preset0_objects, true},
    {presetNames[1], PRESET_1_NUMBER_OF_OBJECTS, preset1_objects, false},
    {presetNames[2], PRESET_2_NUMBER_OF_OBJECTS, preset2_objects, false},
    {presetNames[3], PRESET_3_NUMBER_OF_OBJECTS, preset3_objects, false},
    {presetNames[4], PRESET_4_NUMBER_OF_OBJECTS, preset4_objects, false}
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