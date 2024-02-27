#include "c_orbit.hpp"

#include <math.h>
#include <iostream>

COrbitalObject::COrbitalObject(double mass, double radius, double start_position_x, double start_position_y, double start_velocity_x, double start_velocity_y, unsigned int color)
{
    setMass(mass);
    setRadius(radius, false);
    setPos(start_position_x, start_position_y);
    setVel(start_velocity_x, start_velocity_y);
    setColor(color);
}

void COrbitalObject::updateForce(std::vector<COrbitalObject>& orbitalObjects)
{
    coordinatesF distance {0.0, 0.0};
    double angleToObject = 0.0;
    double distanceTotal = 0.0;

    accel_.x = 0.0;
    accel_.y = 0.0;

    //iterate over all other Objects
    for (int iter = 0U; iter < static_cast<int>(orbitalObjects.size()); iter++)
    {
        distanceTotal = getDistanceToObject(orbitalObjects[iter]);

        //ignore yourself and Objects that are inside your radius
        if ((&orbitalObjects[iter] != this) &&
            (distanceTotal > radius_ + orbitalObjects[iter].radius_))
        {
            distance.x = orbitalObjects[iter].position_.x - position_.x;
            distance.y = orbitalObjects[iter].position_.y - position_.y;
            //calculate angle to other object
            if (distance.x > 0.0)
            {
                angleToObject = atan((distance.y) / (distance.x));
            }
            else if (distance.x < 0.0)
            {
                
                angleToObject = atan((distance.y) / (distance.x)) + M_PI;
            }
            else
            {
                if (distance.y >= 0)
                {
                    angleToObject = M_PI / 2;
                }
                else
                {
                    angleToObject = (M_PI / 2) + M_PI;
                }
            }
            //Calculate gravitational acceleration (force) to other object
            //              m1 * m2
            //    F = G * -----------
            //                r^2
            //
            //    F = m * a --> a = F / m1
            //
            //    a = G * m2 / r^2
            //
            //    G is changed to 1 (attraction force can be adapted by changes the mass) 
            accel_.x += cos(angleToObject) * orbitalObjects[iter].mass_ / (pow(distanceTotal, 2));
            accel_.y += sin(angleToObject) * orbitalObjects[iter].mass_ / (pow(distanceTotal, 2));
        }
    }
}

void COrbitalObject::updatePosition(void)
#ifdef USE_EULER_ALGHORITHM
{
    position_prev_.x = position_.x;
    position_prev_.y = position_.y;

    position_.x += accel_.x;
    position_.y += accel_.y;
}
#endif
#ifdef USE_VERLET_ALGHORITHM
{
    coordinatesF temp_pos_prev;
    temp_pos_prev.x = position_.x;
    temp_pos_prev.y = position_.y;

    //Verlet Integration
    //pos(t+dt) = 2*pos(t) - pos(t-dt) + a*dt^2
    position_.x = (2 * position_.x) - position_prev_.x + accel_.x;  // normally this last part would be multiplied by the delta_time^2
    position_.y = (2 * position_.y) - position_prev_.y + accel_.y;  // but this time is constant so it can be included in the "GRAVITATIONAL_CONSTANT"

    position_prev_.x = temp_pos_prev.x;
    position_prev_.y = temp_pos_prev.y;
}
#endif

void COrbitalObject::updateTrail(void)
{
    //save last position for trail but only if it is different then the last saved point
    if ((static_cast<int>(position_prev_.x) != static_cast<int>(position_.x)) ||
        (static_cast<int>(position_prev_.y) != static_cast<int>(position_.y)))
    {
        trail_[trailIterator_].x = static_cast<unsigned int>(position_.x);
        trail_[trailIterator_].y = static_cast<unsigned int>(position_.y);
        trailIterator_ = (trailIterator_ + 1) % (TRAIL_LENGTH); //iterate the iterator
    }
}

double COrbitalObject::getDistanceToObject(COrbitalObject &otherObject)
{
    //Pythagoras
    return sqrt(pow(position_.x - otherObject.position_.x, 2) + pow(position_.y - otherObject.position_.y, 2));
}

double COrbitalObject::getPosX(void)
{
    return position_.x;
}

double COrbitalObject::getPosY(void)
{
    return position_.y;
}

double COrbitalObject::getRadius(void)
{
    return radius_;
}

unsigned int COrbitalObject::getColor(void)
{
    return color_;
}

COrbitalObject::coordinatesI * COrbitalObject::getTrail(void)
{
    return &trail_[0];
}

unsigned int COrbitalObject::getTrailIterator(void)
{
    return trailIterator_;
}

void COrbitalObject::setPos(double posX, double posY)
{
    position_.x = posX;
    position_.y = posY;
}

void COrbitalObject::setVel(double value_x, double value_y)
{
    if (simulationAlgorithm_ == EnSimulationAlgorithm::kEuler)
    {
        //this if for Euler
        accel_.x = value_x;
        accel_.y = value_y;
    }
    else
    {
        //this is for Verlet
        position_prev_.x = position_.x - value_x;
        position_prev_.y = position_.y - value_y;
    }
}

void COrbitalObject::setMass(double mass)
{
    mass_ = mass;
}

void COrbitalObject::setRadius(double radius, bool scaled)
{
    if (scaled == false)
    {
        //overwriting radius calculated when mass was set
        radius_ = radius;
    }
    else
    {
        //scale radius proportional to mass
        radius_ = ((mass_ - MASS_MIN) / (MASS_MAX - MASS_MIN)) * (RADIUS_MAX - RADIUS_MIN) + RADIUS_MIN;
        // radius_ = clamp(RADIUS_MIN, RADIUS_MAX, radius_);
    }
}

void COrbitalObject::setColor(unsigned int color)
{
    if (color <= 0xffffff)
    {
        color_ = color;
    }
    else
    {
        color_ = 0xffffff;
    }
}

void COrbitalObject::outputInformation(int instance)
{
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "_______________________________" << std::endl;
    std::cout << "Orbital Object " << instance << std::endl;
    std::cout << "            X           Y" << std::endl;
    std::cout << "Position:     ";
    std::cout << std::left << std::setw(12) << std::setfill(' ') << position_.x;
    std::cout << std::left << std::setw(12) << std::setfill(' ') << position_.y << std::endl;
    std::cout << "Acceleration: ";
    std::cout << std::left << std::setw(12) << std::setfill(' ') << accel_.x;
    std::cout << std::left << std::setw(12) << std::setfill(' ') << accel_.y << std::endl;
    std::cout << std::endl;
}