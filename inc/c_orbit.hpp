#ifndef ORBITALOBJECTCLASS_HPP
#define ORBITALOBJECTCLASS_HPP

#include <vector>
#include <iomanip>
#include "config.hpp"
#include "utils.hpp"

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

	                COrbitalObject          (double mass, double radius, double start_position_x, double start_position_y, double start_velocity_x, double start_velocity_y, unsigned int color);
    void            updateForce             (std::vector<COrbitalObject>& orbitalObjects);
    void            updatePosition          (void);
    void            updateTrail             (void);
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
    void            setRadius               (double radius, bool scaled);
    void            setColor                (unsigned int color);
    void            setInitValues           (void);
    void            restoreInitValues       (void);
    void            outputInformation       (int instance);

private:
    enum class EnSimulationAlgorithm : int
    {
        kEuler   = 0U,
        kVerlet  = 1U
    };

    double mass_{1.0};
    double radius_ {5.0};
    coordinatesF position_ {};
    coordinatesF position_prev_ {};
    unsigned int color_ {0xff0000};
    coordinatesF accel_ {};
    coordinatesF init_position_ {};
    coordinatesF init_position_prev_ {};
    coordinatesI trail_[TRAIL_LENGTH] = {0};
    unsigned int trailIterator_ {0};
    EnSimulationAlgorithm simulationAlgorithm_ {EnSimulationAlgorithm::kVerlet};
};

#endif /*ORBITALOBJECTCLASS_HPP*/