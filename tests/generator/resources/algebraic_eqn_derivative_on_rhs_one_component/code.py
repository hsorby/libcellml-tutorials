def initializeConstants(states, variables):
    states[0] = 1.0
    variables[0] = 1.0

def computeComputedConstants(variables):
    pass

def computeRates(voi, states, rates, variables):
    rates[0] = variables[0]

def computeVariables(voi, states, rates, variables):
    variables[1] = rates[0]
