#ifndef DRAW_CLASS_HPP
#define DRAW_CLASS_HPP


#include <math.h>
#include "utils.hpp"

#define USE_MIDPOINT_CIRCLE_ALGHORITHM
//#define USE_BRESENHAM_CIRCLE_ALGHORITHM

class CDraw
{
    public:
        void update(unsigned int *memory_adress, unsigned int height, unsigned int width)
        {
            if (checkMemoryAdress(memory_adress) == true)
            {
                image_memory_adress = memory_adress;
            }

            max_height = height;
            max_width = width;
        }

        void fillScreen(unsigned int color)
        {
            if (checkMemoryAdress(image_memory_adress) == true)
            {
                unsigned int *pixel = image_memory_adress;
                for (int y = 0; y < max_height; y++)
                {
                    for (int x = 0; x < max_width; x++)
                    {
                        *pixel++ = color;
                    }
                }
            }
        }

        void drawPixel(int x, int y, unsigned int color)
        {
            if (checkMemoryAdress(image_memory_adress) == true)
            {
                if ((x > 0) && (x < max_width) && 
                    (y > 0) && (y < max_height))
                {
                    unsigned int *pixel = (unsigned int *)image_memory_adress + (y * max_width) + x;
                    *pixel = color;
                }
            }
        }

        void drawLine(int x_start, int y_start, int x_end, int y_end, unsigned int color)
        {
            //line formula:
            //y = mx + n
            //y = (dy/dx) * (x-x1) + y1

            int delta_x = x_end - x_start;
            int delta_y = y_end - y_start;

            if (abs(delta_y) <= abs(delta_x))
            {
                float m = static_cast<float>(delta_y) / static_cast<float>(delta_x);
                //swap start and end, so start is smaller
                if (x_start > x_end)
                {
                    int temp = x_start;
                    x_start = x_end;
                    x_end = temp;
                    temp = y_start;
                    y_start = y_end;
                    y_end = temp;
                }

                for (int x = x_start; x < x_end; x++)
                {
                    //calculate y part
                    int y = static_cast<int>(m * static_cast<float>(x - x_end)) + y_end;
                    //draw pixel
                    drawPixel(x, y, color);
                }
            }
            else
            {
                float m = static_cast<float>(delta_x) / static_cast<float>(delta_y);
                //swap start and end, so start is smaller
                if (y_start > y_end)
                {
                    int temp = y_start;
                    y_start = y_end;
                    y_end = temp;
                    temp = x_start;
                    x_start = x_end;
                    x_end = temp;
                }

                for (unsigned int y = y_start; y < y_end; y++)
                {
                    //calculate x part
                    int x = static_cast<int>(m * static_cast<float>(static_cast<signed int>(y) - static_cast<signed int>(y_end))) + x_end;
                    //draw pixel
                    drawPixel(x, y, color);
                }
            }
        }

        void drawCircle(int x_centre, int y_centre, int radius, unsigned int color, bool fill)
        #ifdef USE_MIDPOINT_CIRCLE_ALGHORITHM
        {
            int x = radius, y = 0;

            if (fill == false)
            {
                drawPixel(x + x_centre, y + y_centre, color);
                drawPixel(-x + x_centre, y + y_centre, color);
                drawPixel(y + x_centre, x + y_centre, color);
                drawPixel(y + x_centre, -x + y_centre, color);
            }
            else
            {
                drawLine(x + x_centre, y + y_centre, -x + x_centre, y + y_centre, color);
                drawPixel(y + x_centre, x + y_centre, color);
                drawPixel(y + x_centre, -x + y_centre, color);
            }

            static int P = 1 - radius;

            while (x > y)
            {
                y++;

                if (P <= 0)
                {
                    P = P + 2*y + 1;
                }
                else
                {
                    x--;
                    P = P + 2*y - 2*x + 1;
                }

                if (x < y)
                {
                    break;
                }

                if (fill == false)
                {
                    drawPixel(x + x_centre, y + y_centre, color);
                    drawPixel(-x + x_centre, y + y_centre, color);
                    drawPixel(x + x_centre, -y + y_centre, color);
                    drawPixel(-x + x_centre, -y + y_centre, color);

                    if (x != y)
                    {
                        drawPixel(y + x_centre, x + y_centre, color);
                        drawPixel(-y + x_centre, x + y_centre, color);
                        drawPixel(y + x_centre, -x + y_centre, color);
                        drawPixel(-y + x_centre, -x + y_centre, color);
                    }
                }
                else
                {
                    drawLine(x + x_centre, y + y_centre, -x + x_centre, y + y_centre, color);
                    drawLine(x + x_centre, -y + y_centre, -x + x_centre, -y + y_centre, color);

                    if (x != y)
                    {
                        drawLine(y + x_centre, x + y_centre, -y + x_centre, x + y_centre, color);
                        drawLine(y + x_centre, -x + y_centre, -y + x_centre, -x + y_centre, color);
                    }
                }
            }
        }
        #endif
        #ifdef USE_BRESENHAM_CIRCLE_ALGHORITHM
        {
            int x = 0, y = radius;
            int d = 3 - 2 * radius;

            drawPixel(x_centre + x, y_centre + y, color);
            drawPixel(x_centre - x, y_centre + y, color);
            drawPixel(x_centre + x, y_centre - y, color);
            drawPixel(x_centre - x, y_centre - y, color);
            drawPixel(x_centre + y, y_centre + x, color);
            drawPixel(x_centre - y, y_centre + x, color);
            drawPixel(x_centre + y, y_centre - x, color);
            drawPixel(x_centre - y, y_centre - x, color);

            while (y >= x)
            {
                x++;

                if (d > 0)
                {
                    y--;
                    d = d + 4 * (x - y) + 10;
                }
                else
                {
                    d = d + 4 * x + 6;
                }
                drawPixel(x_centre + x, y_centre + y, color);
                drawPixel(x_centre - x, y_centre + y, color);
                drawPixel(x_centre + x, y_centre - y, color);
                drawPixel(x_centre - x, y_centre - y, color);
                drawPixel(x_centre + y, y_centre + x, color);
                drawPixel(x_centre - y, y_centre + x, color);
                drawPixel(x_centre + y, y_centre - x, color);
                drawPixel(x_centre - y, y_centre - x, color);
            }
        }
        #endif

        bool checkMemoryAdress(unsigned int *memory_adress)
        {
            bool memoryAdress_valid{false};

            if (memory_adress != nullptr)
            {
                memoryAdress_valid = true;
            }
            else
            {
                std::cout << "Draw Error: invalid memory adress" << std::endl;
            }

            return memoryAdress_valid;
        }

    private:
        unsigned int *image_memory_adress {nullptr};
        unsigned int max_height {0};
        unsigned int max_width {0};
};

#endif /*DRAW_CLASS_HPP*/