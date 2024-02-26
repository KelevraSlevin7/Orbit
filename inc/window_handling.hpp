#ifndef WINDOWHANDLING_HPP
#define WINDOWHANDLING_HPP

#include <windows.h>
#include <CommCtrl.h>
#include <chrono>

#define NUMBER_OF_OBJECTLIST_COLUMNS 5

enum StartStopButton_State
{
        kStart = 0U,
        kStop = 1U
};

LRESULT CALLBACK                simulationWindow_callback   (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK                controlWindow_callback      (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        void                    createWindows               (HINSTANCE hInstance, int nShowCmd, HWND &simulationWindow_handle, HWND &controlWindow_handle, int x_pos, int y_pos, int width, int height);

        void                    updateCalculationTimeText   (long long int calculationDuration);
        bool                    get_simulationWindow_status (void);
        bool                    get_controlWindow_status    (void);
        int                     get_simulationWindow_width  (void);
        int                     get_simulationWindow_height (void);
        StartStopButton_State   get_startstopButton_state   (void);
        bool                    is_resetButton_triggered    (void);

        void add_ObjectList_Item(int i_number, double d_mass, double d_radius, double d_position_x, double d_position_y);
        void remove_ObjectList_Item(void);

#endif /*WINDOWHANDLING_HPP*/