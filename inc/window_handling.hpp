#ifndef WINDOWHANDLING_HPP
#define WINDOWHANDLING_HPP

#include <windows.h>
#include <CommCtrl.h>

#define IDC_BUTTON_STARTSTOP            100
#define IDC_BUTTON_RESET                101
#define IDC_BUTTON_LOADPRESET           102
#define IDC_COMBOBOX_PRESETS            103
#define IDC_LISTVIEW_OBJECTS            104
#define IDC_BUTTON_ADD                  105
#define IDC_BUTTON_REMOVE               106
#define IDC_TEXT_STATIC_CALCTIME        107
#define IDC_TEXT_INPUT_MASS             108
#define IDC_TEXT_INPUT_RADIUS           109
#define IDC_TEXT_INPUT_POSX             110
#define IDC_TEXT_INPUT_POSY             111
#define IDC_TEXT_INPUT_VELX             112
#define IDC_TEXT_INPUT_VELY             113
#define IDC_TEXT_STATIC_MASS            114
#define IDC_TEXT_STATIC_RADIUS          115
#define IDC_TEXT_STATIC_POSX            116
#define IDC_TEXT_STATIC_POSY            117
#define IDC_TEXT_STATIC_VELX            118
#define IDC_TEXT_STATIC_VELY            119
#define IDC_BUTTON_RANDOMIZE            120

LRESULT CALLBACK                simulationWindow_callback       (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK                controlWindow_callback          (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK                customDoubleInput_callback              (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        void                    createWindows                   (HINSTANCE hInstance, int nShowCmd, HWND &simulationWindow_handle, HWND &controlWindow_handle, int x_pos, int y_pos, int width, int height);

        void                    addObjectListItem               (unsigned int i_number, double d_mass, double d_radius, double d_position_x, double d_position_y, double d_vel_x, double d_vel_y);
        void                    removeObjectListItem            (int index);
        void                    clearObjectList                 (void);
        int                     getObjectListSelectedIndex      (void);
        void                    removeObjectListSelectedItem    (void);

        void                    updateCalculationTimeText       (long long int calculationDuration);

        double                  getTextFieldValueAsDouble       (int inputId);
        void                    setTextFieldValueAsDouble       (int inputId, double input_value);
        void                    setTextFieldValueAsText         (int inputId, const char* text);

        bool                    getCheckBoxState                (int buttonId);

        bool                    getSimulationWindowStatus       (void);
        bool                    getControlWindowStatus          (void);
        int                     getSimulationWindowWidth        (void);
        int                     getSimulationWindowHeight       (void);
        void                    writeStartStopButtonText        (const char* text);
        int                     get_presets_selection           (void);
        bool                    isButtonTriggered               (int buttonId);

#endif /*WINDOWHANDLING_HPP*/