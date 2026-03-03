/*
Metadata for MS GUI:
imdata = {"displayname" : "Magnetorquer Payload Model",
          "exclude" : False,
          "category" : "Custom"
}
aliases = {"m_prime_x" : "m prime for the x-axis magnetorquer, # of wire turns (n) * area of the coil (A).",
            "m_prime_y" : "m prime for the y-axis magnetorquer, # of wire turns (n) * area of the coil (A).",
            "m_prime_z" : "m prime for the z-axis magnetorquer, # of wire turns (n) * area of the coil (A).",
            "B" : "Magnetic field vector",
            "I_x" : "Current through the x-axis magnetorquer",
            "I_y" : "Current through the y-axis magnetorquer",
            "I_z" : "Current through the z-axis magnetorquer",
            "torque" : "Output torque of the payload"
}
*/

#ifndef MODELS_MAGNETORQUER_PAYLOAD_H
#define MODELS_MAGNETORQUER_PAYLOAD_H

#include "simulation/Model.h"

#include "modelspace/core/Matrix.hpp" 
#include "modelspace/core/matrixmath.hpp"
#include "modelspace/core/vectormath.hpp"
#include "modelspace/core/mathmacros.h"
#include "modelspace/core/CartesianVector.hpp"
#include "modelspace/core/safemath.h"

#include "Magnetorquer.h"

namespace modelspace {

    /**
     * @brief Composite magnetorquer payload.
    */
    MODEL(MagnetorquerPayload)
    public:
        // Model parameters
        //         NAME                     TYPE                    DEFAULT VALUE
        START_PARAMS
            /** For the magnetorquer along the x-axis: Number of wire turns (n) * area of the coil (A). */
            SIGNAL(m_prime_x,                 double,       0)

            /** For the magnetorquer along the y-axis: Number of wire turns (n) * area of the coil (A). */
            SIGNAL(m_prime_y,                 double,       0)

            /** For the magnetorquer along the z-axis: Number of wire turns (n) * area of the coil (A). */
            SIGNAL(m_prime_z,                 double,       0)
        END_PARAMS

        // Model inputs
        //         NAME                     TYPE                    DEFAULT VALUE
        START_INPUTS
            /** Magnetic field vector. Usually Earth's for our purposes.*/
            SIGNAL(B,                       CartesianVector3,        0)

            /** Current through the x-axis magnetorquer*/
            SIGNAL(I_x,                       double,                   0)

            /** Current through the y-axis magnetorquer*/
            SIGNAL(I_y,                       double,                   0)

            /** Current through the z-axis magnetorquer*/
            SIGNAL(I_z,                       double,                   0)

        END_INPUTS

        // Model outputs
        //         NAME                     TYPE                    DEFAULT VALUE
        START_OUTPUTS
            /** Output torque of the payload: m_x X B + m_y X B + m_z X B */
            SIGNAL(torque,                  CartesianVector3,       0.0)
        END_OUTPUTS

        int16 activate() override;
        int16 deactivate() override;

    protected:
        int16 start() override;
        int16 execute() override;

        void _configureInternal();
        
        // Internal torque varible
        // Calculations are stored to this, and the output torquer follows its value.
        CartesianVector3 _torque;

        // The 3 magnetorquers that make up the payload.
        // One along each axis.
        Magnetorquer _magnetorquer_x;
        Magnetorquer _magnetorquer_y;
        Magnetorquer _magnetorquer_z;
        
        // Magnetic dipole vectors of each magnetorquer.
        CartesianVector3 _m_x;
        CartesianVector3 _m_y;
        CartesianVector3 _m_z;
    };

}

#endif