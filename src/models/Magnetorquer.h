/*
Metadata for MS GUI:
imdata = {"displayname" : "Magnetorquer Model",
          "exclude" : False,
          "category" : "Custom"
}
aliases = {"m" : "Magnetic dipole vector",
           "B" : "Magnetic field vector",
           "torque" : "Torque"
}
*/

#ifndef MODELS_MAGNETORQUER_H
#define MODELS_MAGNETORQUER_H

#include "simulation/Model.h"

#include "modelspace/core/Matrix.hpp" 
#include "modelspace/core/matrixmath.hpp"
#include "modelspace/core/vectormath.hpp"
#include "modelspace/core/mathmacros.h"
#include "modelspace/core/CartesianVector.hpp"
#include "modelspace/core/safemath.h"

namespace modelspace {

    /**
     * @brief   Simple implementation of a magnetorquer.
    */
    MODEL(Magnetorquer)
    public:
        // Model params
        //         NAME                     TYPE                    DEFAULT VALUE
        START_PARAMS
            /** Magnetic dipole vector. Currently a parameter, will later be a function of : # of wire turns (n), current (I), vector area of the coil (A). */
            SIGNAL(m,                       CartesianVector3,                 0)
        END_PARAMS

        // Model inputs
        //         NAME                     TYPE                    DEFAULT VALUE
        START_INPUTS
            /** Magnetic field vector input. Typically earths magnetic field. */
            SIGNAL(B,                       CartesianVector3,                 0)

            /** Amount of time that the magnetorquer will apply torque. TODO: determine what the default time step is, and determine units for this parameter. */
            SIGNAL(duration,                 int,                 0)

        END_INPUTS

        // Model outputs
        //         NAME                     TYPE                    DEFAULT VALUE
        START_OUTPUTS
            /** The torque vector from the cross product of m x B */
            SIGNAL(torque,                       CartesianVector3,                 0.0)
        END_OUTPUTS

        int16 activate() override;
        int16 deactivate() override;

    protected:
        int16 start() override;
        int16 execute() override;
        
        CartesianVector3 torque_internal;
    };

}

#endif