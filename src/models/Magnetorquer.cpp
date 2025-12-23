#include "Magnetorquer.h"
#include "simulation/SimulationExecutive.h"

using namespace clockwerk;
using namespace cfspp;

namespace modelspace
{
    // These constructor functions can be largely ignored. These are boilerplate defaults
    // that only need to be modified when including a model within a model, or setting
    // variable values on construction. The only item worth noting is the START_STEP
    // in the first two instances of the constructor. This sets the default step for
    // the model to run in (see User's Guide for more info)
    Magnetorquer::Magnetorquer(Model &pnt, const std::string &m_name)
        : Model(pnt, START_STEP, m_name) {}
    Magnetorquer::Magnetorquer(SimulationExecutive &e, const std::string &m_name)
        : Model(e, START_STEP, m_name) {}
    Magnetorquer::Magnetorquer(Model &pnt, int schedule_slot, const std::string &m_name)
        : Model(pnt, schedule_slot, m_name) {}
    Magnetorquer::Magnetorquer(SimulationExecutive &e, int schedule_slot, const std::string &m_name)
        : Model(e, schedule_slot, m_name) {}
    Magnetorquer::~Magnetorquer() {}

    int16 Magnetorquer::activate()
    {
        // Define behavior to occur when the model is activated after deactivation
        // Here we just run execute, so outputs reflect the inputs once more.
        active(true);
        execute();

        // Model functions always return an error code. NO_ERROR means everything
        // went fine, and is an alias for 0. Anything nonzero is intepreted by
        // the sim as an error
        return NO_ERROR;
    }

    int16 Magnetorquer::deactivate()
    {
        // Here behavior to deactivate the model is written. At a minimum it should
        // set the active flag to false, but we can also set up the model to show
        // it is deactivated. In slope intercept, we will set the outputs to 0.
        active(false);
        outputs.torque(0.0);

        // Model functions always return an error code. NO_ERROR means everything
        // went fine, and is an alias for 0. Anything nonzero is intepreted by
        // the sim as an error
        return NO_ERROR;
    }

    int16 Magnetorquer::start()
    {
        // Code used to configure the model goes here. This function runs once
        // at simulation start... Slope Intercept model doesn't have anything for
        // this, but this is where files should be loaded and items should be
        // pre-calculated.

        // Model functions always return an error code. NO_ERROR means everything
        // went fine, and is an alias for 0. Anything nonzero is intepreted by
        // the sim as an error

        // TODO: pre-calculate n*A as it won't change.

        return NO_ERROR;
    }

    int16 Magnetorquer::execute()
    {
        // Code which runs every model step goes here. Per the above and the User's
        // Guide, valid steps are START_STEP, DERIVATIVE, and END_STEP. In general
        // here is where you should write values to all outputs on the basis of
        // inputs and params.

        if (inputs.duration() > 0)
        {
            // Set torque = m x B.
            cross(params.m(), inputs.B(), torque_internal);
            outputs.torque(torque_internal);

            // Decrement from remaining active duration.
            inputs.duration(inputs.duration() - 1);
        }

        // Model functions always return an error code. NO_ERROR means everything
        // went fine, and is an alias for 0. Anything nonzero is intepreted by
        // the sim as an error
        return NO_ERROR;
    }

}