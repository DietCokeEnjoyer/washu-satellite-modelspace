/*
Metadata for MS GUI:
imdata = {"displayname" : "Magnetorquer Model",
          "exclude" : False,
          "category" : "Custom"
}
aliases = {"m_prime" : "m prime, # of wire turns (n) * vector area of the coil (A).",
           "B" : "Magnetic field vector",
           "I" : "Current",
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
            /** Number of wire turns (n) * vector area of the coil (A). */
            SIGNAL(m_prime,                 CartesianVector3,       0)
        END_PARAMS

        // Model inputs
        //         NAME                     TYPE                    DEFAULT VALUE
        START_INPUTS
            /** Magnetic field vector. Usually Earth's for our purposes.*/
            SIGNAL(B,                       CartesianVector3,        0)

            /** Current */
            SIGNAL(I,                       double,                   0)

        END_INPUTS

        // Model outputs
        //         NAME                     TYPE                    DEFAULT VALUE
        START_OUTPUTS
            /** Torque from the cross product of m x B */
            SIGNAL(torque,                  CartesianVector3,       0.0)
        END_OUTPUTS

        int16 activate() override;
        int16 deactivate() override;

    protected:
        int16 start() override;
        int16 execute() override;
        
        CartesianVector3 _torque;
        CartesianVector3 _m;
    };

}

#endif