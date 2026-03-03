"""
MagnetorquerPayload Model Test
----------------------------------
Tests the MagnetorquerPayload model by applying different currents to the payload over a few time intervals, 
and displays these maneuvers with VizKitPlanetRelative.

Author: Elias Dahl
"""
import sys
import site
import time
from pathlib import Path

##################################################################################
# Build path configuration. This tells the script where our custom models are.
# This must be included at the start of any script that uses custom models.

#This finds the folder where the script is
script_directory = Path(__file__).parent

# This looks up one folder into the main project folder, then looks down into the build folder, where the custom models are.
build_path_object = (script_directory / ".." / "build").resolve()

# This turns the Path object into a string for the lines below.
build_path = str(build_path_object)

# Force it to the FRONT of the search path
if build_path not in sys.path:
    sys.path.insert(0, build_path)

# Force Python to re-scan for namespace packages (Crucial for ModelSpace)
site.addsitedir(build_path)
##################################################################################

# Modelspace Imports
from modelspace.Spacecraft import Spacecraft
from modelspace.WorldMagneticFieldModel import WorldMagneticFieldModel
from modelspace.FrameStateSensorModel import FrameStateSensorModel
from modelspace.ModelSpacePy import SimulationExecutive, DEGREES_TO_RADIANS, connectSignals, CartesianVector3, Node, Time, DERIVATIVE
from modelspace.SpicePlanet import SpicePlanet
from modelspaceutils.vizkit.VizKitPlanetRelative import VizKitPlanetRelative

from modelspace.Magnetorquer import Magnetorquer
from modelspace.MagnetorquerPayload import MagnetorquerPayload

#### Constants ####
# The simulation rate / time step size, e.g. 100Hz -> 0.1 sec time steps.
SIMULATION_RATE_HZ = 100

# The physical properties of the payload's magnetorquers that provide a magnetic dipole when multiplied by an input current.
# Currently using a realistic example value provided by Prince of the Blood, A.F. Moriarty.
# For SCALAR, two of these will be ~identical to represent our manufactured torquers, 
# while one will be different and smaller, representing the embedded torquer.
# These represent vector areas, but since the payload torquers are in a fixed direction, one along each axis, scalar values are used for simplicity and correctness.
M_PRIME_X = 0.36674
M_PRIME_Y = 0.36674
M_PRIME_Z = 0.36674

#### Script ####

exc = SimulationExecutive()     # Create our executive -- by convention named exc
exc.parseArgs(sys.argv)         # this interperets command-line inputs
exc.setRateHz(SIMULATION_RATE_HZ)                # Sets the simulation rate from our constant.

# Creates our planet model
earth = SpicePlanet(exc, "earth")

# Creates our spacecraft model and set it to orbit earth
sc = Spacecraft(exc, "sc")
sc.params.planet_ptr(earth)


# Determine where the spacecraft is so we know the field at that pos.
pos_sensor = FrameStateSensorModel(exc, DERIVATIVE, "pos_sensor")

pos_sensor.params.target_frame_ptr(sc.body())                 
pos_sensor.params.reference_frame_ptr(earth.outputs.rotating_frame())

# Set up magnetic field
mag_field = WorldMagneticFieldModel(exc, DERIVATIVE, "mag_field")

connectSignals(pos_sensor.outputs.pos_tgt_ref__out, mag_field.inputs.pos_body_planet__pcr)

# Create a payload model 
payload = MagnetorquerPayload(exc, "payload")

# Set the payload m_primes from the constants. Parameters in general should be set before simulation start and shouldn't be changed.
# However, this is a requirement for the MagnetorquerPayload m_prime parameters.
payload.params.m_prime_x(M_PRIME_X)
payload.params.m_prime_y(M_PRIME_Y)
payload.params.m_prime_z(M_PRIME_Z)

# Connect magnetic field to the payload.
connectSignals(mag_field.outputs.mag_field__NED, payload.inputs.B)

# Creates a node that the payload model will output torque to, and connect it to the spacecraft.
magnetorquer_node = Node("magnetorquer_node", sc.body())

# Connect the payload model output to the torque node input
connectSignals(payload.outputs.torque, magnetorquer_node.moment)

# Set up planet vizkit, which displays our spacecraft and earth.
vk = VizKitPlanetRelative(exc)
# Adds earth to the vizkit
vk.planet(earth.outputs.inertial_frame())

# Adds the spacecraft to the vizkit. Both are necessary.
vk.target(sc.body())
vk.addSpacecraft(sc.body())

# Adds the vizkit to the simulation exc and sets its display rate.
exc.logManager().addLog(vk, SIMULATION_RATE_HZ)

### Simulation Initialization ###
exc.startup()

# Sets the inital values for the spacecraft's parameters.
sc.initializeFromOrbitalElements(7278137.0, 0.1, DEGREES_TO_RADIANS*40.0, DEGREES_TO_RADIANS*10.0, DEGREES_TO_RADIANS*10.0, DEGREES_TO_RADIANS*30.0)


## Simulation Loop ###
print("Sim start!")
while not exc.isTerminated():

    current_time = exc.time().base_time().asFloatingPoint()
    if current_time < 5.0:
        # Turn on the x and y torquers
        payload.inputs.I_x(0.00005)
        payload.inputs.I_y(0.00005)

    elif current_time < 10.0:
        # Turn off x & y torquers
        payload.inputs.I_x(0.0)
        payload.inputs.I_y(0.0)

        # Turn on the z torquer
        payload.inputs.I_z(-0.0001)

    elif current_time < 15.0:
        # Turn on x & y torquers, in the opposite direction
        payload.inputs.I_x(-0.00005)
        payload.inputs.I_y(-0.00005)

        # Turn off the z torquer
        payload.inputs.I_z(0)

    elif current_time < 20.0:
        # Turn off x & y torquers
        payload.inputs.I_x(0.0)
        payload.inputs.I_y(0.0)

        # Turn on the z torquer
        payload.inputs.I_z(0.0001)
    elif current_time < 25.0:
        # turn on the y torquer
        payload.inputs.I_y(0.0001)

        # Turn off the z torquer
        payload.inputs.I_z(0.0)
    else:
        # turn off the payload / all torquers.
        payload.deactivate()

    exc.step()

    # This slows down the loop to run in ~real time by stalling for a time step. 
    # Good for this short sim, but you'd want to speed this up for longer sims.
    time.sleep(1/float(SIMULATION_RATE_HZ))

    if current_time > 30.0:
        break
print("Sim end!")