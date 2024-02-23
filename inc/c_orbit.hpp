#ifndef ORBITALOBJECTCLASS_HPP
#define ORBITALOBJECTCLASS_HPP

#include <vector>
#include <iomanip>
#include <chrono>
#include "utils.hpp"

#define TRAIL_LENGTH                100

#define MASS_MIN                    1.0
#define MASS_MAX                    20.0
#define RADIUS_MIN                  5.0
#define RADIUS_MAX                  15.0

//#define USE_EULER_ALGHORITHM
#define USE_VERLET_ALGHORITHM

class COrbitalObject
{
public:
    struct coordinatesF {
        double x = 0.0;
        double y = 0.0;
    };

    struct coordinatesI {
        unsigned int x = 0;
        unsigned int y = 0;
    };

	                COrbitalObject          (double max_position_x, double max_position_y);
    void            updateForce             (std::vector<COrbitalObject>& orbitalObjects);
    void            updatePosition          (void);
    void            updateTrail             (void);
    void            updateOrbitalObject     (std::vector<COrbitalObject>& orbitalObjects);
    double          getDistanceToObject     (COrbitalObject &otherObject);
    double          getPosX                 (void);
    double          getPosY                 (void);
    double          getRadius               (void);
    unsigned int    getColor                (void);
    COrbitalObject::coordinatesI * getTrail (void);
    unsigned int    getTrailIterator        (void);
    void            setPos                  (double posX, double posY);
    void            setVel                  (double value_x, double value_y);
    void            setMass                 (double mass);
    void            setRadius               (double radius);
    void            setColor                (unsigned int color);
    void            setRandomColor          (void);
    void            outputInformation       (int instance);

private:
    enum class EnSimulationAlgorithm : uint8_t
    {
        kEuler   = 0U,
        kVerlet  = 1U
    };

    coordinatesF position_ {};
    coordinatesF position_prev_ {};
    coordinatesF accel_ {};
    double mass_{1.0};
    double radius_ {5.0};
    unsigned int color_ {0xff0000};
    coordinatesI trail_[TRAIL_LENGTH] = {0};
    unsigned int trailIterator_ {0};
    EnSimulationAlgorithm simulationAlgorithm_ {EnSimulationAlgorithm::kVerlet};
};

#endif /*ORBITALOBJECTCLASS_HPP*/