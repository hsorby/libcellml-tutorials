# The content of this file was generated using the Python profile of libCellML 0.2.0.

from enum import Enum
from math import *


__version__ = "0.1.0"
LIBCELLML_VERSION = "0.2.0"

STATE_COUNT = 2
VARIABLE_COUNT = 4


class VariableType(Enum):
    CONSTANT = 1
    COMPUTED_CONSTANT = 2
    ALGEBRAIC = 3


VOI_INFO = {"name": "time", "units": "day", "component": "predator_prey_component"}

STATE_INFO = [
    {"name": "y_s", "units": "number_of_sharks", "component": "predator_prey_component"},
    {"name": "y_f", "units": "thousands_of_fish", "component": "predator_prey_component"}
]

VARIABLE_INFO = [
    {"name": "a", "units": "per_day", "component": "predator_prey_component", "type": VariableType.CONSTANT},
    {"name": "b", "units": "per_shark_day", "component": "predator_prey_component", "type": VariableType.CONSTANT},
    {"name": "d", "units": "per_1000fish_day", "component": "predator_prey_component", "type": VariableType.CONSTANT},
    {"name": "c", "units": "per_day", "component": "predator_prey_component", "type": VariableType.COMPUTED_CONSTANT}
]


def create_states_array():
    return [nan]*2


def create_variables_array():
    return [nan]*4


def initialize_states_and_constants(states, variables):
    states[0] = 2.0
    states[1] = 1.0
    variables[0] = 1.2
    variables[1] = -0.6
    variables[2] = 0.3


def compute_computed_constants(variables):
    variables[3] = variables[0]-2.0


def compute_rates(voi, states, rates, variables):
    rates[0] = variables[0]*states[0]+variables[1]*states[0]*states[1]
    rates[1] = variables[3]*states[1]+variables[2]*states[0]*states[1]


def compute_variables(voi, states, rates, variables):
    pass
