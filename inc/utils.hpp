#ifndef UTILS_HPP
#define UTILS_HPP

#include <math.h>
#include <iostream>

class CWorld
{
public:
    CWorld(const CWorld &) = delete;

    static CWorld &getInstance()
    {
        static CWorld instance;
        return instance;
    }

    static int height() { return getInstance().world_height; }
    static int width() { return getInstance().world_width; }

private:
    CWorld() {}
    int world_height{400};
    int world_width{400};
};

template <typename T>
T clamp(T minimum, T maximum, T value)
{
    T return_value = minimum;

    if (value < minimum)
    {
        return_value = minimum;
    }
    else if (value > maximum)
    {
        return_value = maximum;
    }
    else
    {
        return_value = value;
    }

    return return_value;
}
template <typename T>
T getRandomNumber(T minimum, T maximum)
{
    // return minimum + (static_cast<T>(rand()) / (static_cast<T>(RAND_MAX/(maximum - minimum))));
    return minimum + (static_cast<T>(rand() * (maximum - minimum)) / static_cast<T>(RAND_MAX));
}

class  CVector2
{
public:
    float x{0.0f};
    float y{0.0f};

    CVector2()
        : x(0.0f), y(0.0f) {}

    CVector2(float x, float y)
        : x(x), y(y) {}

    //operation with other CVector2's
    CVector2 operator+(const CVector2& other)
    {
        return CVector2(x + other.x, y + other.y);
    }

    CVector2 operator-(const CVector2& other)
    {
        return CVector2(x - other.x, y - other.y);
    }

    CVector2 operator*(const CVector2& other)
    {
        return CVector2(x * other.x, y * other.y);
    }

    CVector2 operator/(const CVector2& other)
    {
        return CVector2(x / other.x, y / other.y);
    }

    CVector2& operator+=(const CVector2& other)
    {
        x += other.x;
        y += other.y;
        return*this;
    }

    //Scalars
    CVector2 operator+(const float& scalar)
    {
        return CVector2(x + scalar, y + scalar);
    }

    CVector2 operator-(const float& scalar)
    {
        return CVector2(x - scalar, y - scalar);
    }

    CVector2 operator*(const float& scalar)
    {
        return CVector2(x * scalar, y * scalar);
    }

    CVector2 operator/(const float& scalar)
    {
        return CVector2(x / scalar, y / scalar);
    }

    //Comparison
    //ToDo
};

//color transformation
class CColor
{
public:
    typedef struct {
        double r;       // a fraction between 0 and 1
        double g;       // a fraction between 0 and 1
        double b;       // a fraction between 0 and 1
    } rgb_struct;

    typedef struct {
        double h;       // angle in degrees
        double s;       // a fraction between 0 and 1
        double v;       // a fraction between 0 and 1
    } hsv_struct;

    rgb_struct rgb;
    hsv_struct hsv;

    CColor(){};

    hsv_struct rgb2hsv(rgb_struct in)
    {
        double      min, max, delta;
        hsv_struct  out;

        min = in.r < in.g ? in.r : in.g;
        min = min  < in.b ? min  : in.b;

        max = in.r > in.g ? in.r : in.g;
        max = max  > in.b ? max  : in.b;

        out.v = max;                                // v
        delta = max - min;
        if (delta < 0.00001)
        {
            out.s = 0;
            out.h = 0; // undefined, maybe nan?
            return out;
        }
        if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
            out.s = (delta / max);                  // s
        } else {
            // if max is 0, then r = g = b = 0              
            // s = 0, h is undefined
            out.s = 0.0;
            out.h = NAN;                            // its now undefined
            return out;
        }
        if( in.r >= max )                           // > is bogus, just keeps compilor happy
            out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
        else
        if( in.g >= max )
            out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
        else
            out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

        out.h *= 60.0;                              // degrees

        if( out.h < 0.0 )
            out.h += 360.0;

        return out;
    }

    rgb_struct hsv2rgb(hsv_struct in)
    {
        double      hh, p, q, t, ff;
        long        i;
        rgb_struct  out;

        if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
            out.r = in.v;
            out.g = in.v;
            out.b = in.v;
            return out;
        }
        hh = in.h;
        if(hh >= 360.0) hh = 0.0;
        hh /= 60.0;
        i = (long)hh;
        ff = hh - i;
        p = in.v * (1.0 - in.s);
        q = in.v * (1.0 - (in.s * ff));
        t = in.v * (1.0 - (in.s * (1.0 - ff)));

        switch(i) {
        case 0:
            out.r = in.v;
            out.g = t;
            out.b = p;
            break;
        case 1:
            out.r = q;
            out.g = in.v;
            out.b = p;
            break;
        case 2:
            out.r = p;
            out.g = in.v;
            out.b = t;
            break;

        case 3:
            out.r = p;
            out.g = q;
            out.b = in.v;
            break;
        case 4:
            out.r = t;
            out.g = p;
            out.b = in.v;
            break;
        case 5:
        default:
            out.r = in.v;
            out.g = p;
            out.b = q;
            break;
        }
        return out;
    }
};


#endif /* UTILS_HPP */