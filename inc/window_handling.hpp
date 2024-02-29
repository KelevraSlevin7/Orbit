#ifndef WINDOWHANDLING_HPP
#define WINDOWHANDLING_HPP

#include <windows.h>
#include <CommCtrl.h>

#define IDC_BUTTON_STARTSTOP    100
#define IDC_BUTTON_RESET        101
#define IDC_BUTTON_LOADPRESET   102
#define IDC_COMBOBOX_PRESETS    103
#define IDC_LISTVIEW_OBJECTS    104
#define IDC_BUTTON_ADD          105
#define IDC_BUTTON_REMOVE       106
#define IDC_TEXT_CALCTIME       107

LRESULT CALLBACK                simulationWindow_callback       (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK                controlWindow_callback          (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        void                    createWindows                   (HINSTANCE hInstance, int nShowCmd, HWND &simulationWindow_handle, HWND &controlWindow_handle, int x_pos, int y_pos, int width, int height);

        void                    addObjectListItem               (unsigned int i_number, double d_mass, double d_radius, double d_position_x, double d_position_y, double d_vel_x, double d_vel_y);
        void                    removeObjectListItem            (int index);
        void                    clearObjectList                 (void);
        void                    updateCalculationTimeText       (long long int calculationDuration);

        bool                    get_simulationWindow_status     (void);
        bool                    get_controlWindow_status        (void);
        int                     get_simulationWindow_width      (void);
        int                     get_simulationWindow_height     (void);
        void writeStartStopButtonText(const char* text);
        int                     get_presets_selection           (void);
        bool                    isButtonTriggered               (int buttonId);

#endif /*WINDOWHANDLING_HPP*/