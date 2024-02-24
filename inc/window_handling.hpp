#ifndef WINDOWHANDLING_HPP
#define WINDOWHANDLING_HPP

#include <windows.h>
#include <chrono>

LRESULT CALLBACK    simulationWindow_callback   (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK    controlWindow_callback      (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        void        createWindows               (HINSTANCE hInstance, int nShowCmd, HWND &simulationWindow_handle, HWND &controlWindow_handle, int x_pos, int y_pos, int width, int height);
        void        updateCalculationTimeText   (long long int calculationDuration);
        bool        get_simulationWindow_status (void);
        bool        get_controlWindow_status    (void);
        int         get_simulationWindow_width  (void);
        int         get_simulationWindow_height (void);
        bool        get_simulation_running      (void);
        bool        get_simulation_reset_trigger(void);

#endif /*WINDOWHANDLING_HPP*/