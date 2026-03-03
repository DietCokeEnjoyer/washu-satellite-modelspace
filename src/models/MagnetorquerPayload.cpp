#include "MagnetorquerPayload.h"

#include "simulation/SimulationExecutive.h"
#include "modelspace/core/Matrix.hpp"
#include "modelspace/core/matrixmath.hpp"
#include "modelspace/core/vectormath.hpp"
#include "modelspace/core/mathmacros.h"
#include "modelspace/core/CartesianVector.hpp"
#include "modelspace/core/safemath.h"
#include "architecture/signalutils.h" //For connectSignals()

#include "Magnetorquer.h"

using namespace clockwerk;
using namespace cfspp;

namespace modelspace
{
    /**
     * Constructors for the payload.
     *
     */
    MagnetorquerPayload::MagnetorquerPayload(Model& pnt, const std::string& m_name)
        : Model(pnt, DERIVATIVE, m_name), _magnetorquer_x(*this, NOT_SCHEDULED), _magnetorquer_y(*this, NOT_SCHEDULED), _magnetorquer_z(*this, NOT_SCHEDULED)
    {
        _configureInternal();
    }
    MagnetorquerPayload::MagnetorquerPayload(SimulationExecutive& e, const std::string& m_name)
        : Model(e, DERIVATIVE, m_name), _magnetorquer_x(*this, NOT_SCHEDULED), _magnetorquer_y(*this, NOT_SCHEDULED), _magnetorquer_z(*this, NOT_SCHEDULED)
    {
        _configureInternal();
    }
    MagnetorquerPayload::MagnetorquerPayload(Model& pnt, int schedule_slot, const std::string& m_name)
        : Model(pnt, schedule_slot, m_name), _magnetorquer_x(*this, NOT_SCHEDULED), _magnetorquer_y(*this, NOT_SCHEDULED), _magnetorquer_z(*this, NOT_SCHEDULED)
    {
        _configureInternal();
    }
    MagnetorquerPayload::MagnetorquerPayload(SimulationExecutive& e, int schedule_slot, const std::string& m_name)
        : Model(e, schedule_slot, m_name), _magnetorquer_x(*this, NOT_SCHEDULED), _magnetorquer_y(*this, NOT_SCHEDULED), _magnetorquer_z(*this, NOT_SCHEDULED)
    {
        _configureInternal();
    }

    /**
     * Default destuctor.
     */
    MagnetorquerPayload::~MagnetorquerPayload() {}

    /**
     * Sets up the connections between the payload inputs and the internal magnetorquers.
     * Because the m_prime parameters need to be vectorized, and are assigned after construction,
     * they are set in start() rather than here.
     */
    void MagnetorquerPayload::_configureInternal()
    {
        // Pass the Magnetic field to each magnetorquer
        connectSignals(inputs.B, _magnetorquer_x.inputs.B);
        connectSignals(inputs.B, _magnetorquer_y.inputs.B);
        connectSignals(inputs.B, _magnetorquer_z.inputs.B);

        // X-axis current
        connectSignals(inputs.I_x, _magnetorquer_x.inputs.I);

        // Y-axis current
        connectSignals(inputs.I_y, _magnetorquer_y.inputs.I);

        // Z-axis current
        connectSignals(inputs.I_z, _magnetorquer_z.inputs.I);
    }

    /**
     * Turns on the payload. Activates each component magnetorquer, then calls execute to resume outputting torque.
     */
    int16 MagnetorquerPayload::activate()
    {
        // Exit if already active.
        if (active())
        {
            return NO_ERROR;
        }

        // Activate each magnetorquer
        // Check for errors upon activation.
        _error = _magnetorquer_x.activate();
        if (_error)
        {
            return _error;
        }

        _error = _magnetorquer_y.activate();
        if (_error)
        {
            return _error;
        }

        _error = _magnetorquer_z.activate();
        if (_error)
        {
            return _error;
        }

        active(true);

        // Calculates and sets the output torque.
        execute();

        return NO_ERROR;
    }

    /**
     * Turns off the payload. Deactivates the internal torquers, and sets the output torque to 0.
     */
    int16 MagnetorquerPayload::deactivate()
    {
        // Exit if already inactive.
        if (not active())
        {
            return NO_ERROR;
        }

        // Deactivate each magnetorquer
        // Check for errors upon deactivation.
        _error = _magnetorquer_x.deactivate();
        if (_error)
        {
            return _error;
        }

        _error = _magnetorquer_y.deactivate();
        if (_error)
        {
            return _error;
        }

        _error = _magnetorquer_z.deactivate();
        if (_error)
        {
            return _error;
        }

        // Set the payload to inactive
        outputs.torque(0.0);
        active(false);

        return NO_ERROR;
    }

    /**
     * Configures the payload at simulation start. Sets the m_prime for each magnetorquer, assuming they are set before simulation start.
     */
    int16 MagnetorquerPayload::start()
    {
        // Set the m_prime of each magnetorquer.
        _magnetorquer_x.params.m_prime(CartesianVector3({ params.m_prime_x(), 0.0, 0.0 }));
        _magnetorquer_y.params.m_prime(CartesianVector3({ 0.0, params.m_prime_y(), 0.0 }));
        _magnetorquer_z.params.m_prime(CartesianVector3({ 0.0, 0.0, params.m_prime_z() }));

        // Start each magnetorquer
        _magnetorquer_x.startup();
        _magnetorquer_y.startup();
        _magnetorquer_z.startup();

        return NO_ERROR;
    }

    /**
     *
     */
    int16 MagnetorquerPayload::execute()
    {
        // Step the internal torquers to update their output torques
        _magnetorquer_x.step();
        _magnetorquer_y.step();
        _magnetorquer_z.step();


        // Net torque is the sum of the component torques
        _torque = _magnetorquer_x.outputs.torque() + _magnetorquer_y.outputs.torque() + _magnetorquer_z.outputs.torque();
        
        outputs.torque(_torque);
        return NO_ERROR;
    }

}