import sys
import parser
import itertools
import subprocess

def calculate(formula, base):
    code = parser.expr(formula).compile()
    return eval(code)%base

def pdsToTable(pds, variable_names, number_of_states):
    tables = []
    for i, equation in enumerate(pds):
        table = "STATE TRANSITION TABLE for " + variable_names[i] + ":\n"
        if equation == '1':
            table += variable_names[i] + ' ' + variable_names[i] + '\n1\n1\n'
        elif equation == '0':
            table += variable_names[i] + ' ' + variable_names[i] + '\n0\n0\n'
        else:
            vars_in_equation = []
            for v_name in variable_names:
                if v_name in equation:
                    vars_in_equation.append(v_name)
                    table += v_name + " "
            table += variable_names[i] + "\n"

            formula = equation.replace("^", "**")
            permutations = list(itertools.product(range(number_of_states), repeat=len(vars_in_equation)))
            for p in permutations:
                # substite each variable in vars_in_equation with its corresponding value from p
                compute = formula[:]
                for i in range(len(vars_in_equation)):
                    compute = compute.replace(vars_in_equation[i], str(p[i]))
                table += str(calculate(compute, number_of_states)) + "\n"

        table += "\n\n"
        tables.append(table)

    return tables


with open(sys.argv[1], 'r') as f_open:
    model_name = f_open.readline().split(":")[1].strip()
    simulation_name = f_open.readline().split(":")[1].strip()
    number_of_variables = int(f_open.readline().split(":")[1].strip())
    variable_names = f_open.readline().split(":")[1].strip().split(" ")
    number_of_states = map(lambda x: int(x), f_open.readline().split(":")[1].strip().split(" "))
    speed_of_variables = map(lambda x: int(x), f_open.readline().split(":")[1].strip().split(" "))
    pds = []

    equation = f_open.readline()
    while equation == "\n":
        equation = f_open.readline()
    while equation:
        pds.append(equation.split("=")[1].strip())
        equation = f_open.readline()

    with open("cyclone_input.txt", 'w') as f_write:
        f_write.write("MODEL NAME: " + model_name + "\n")
        f_write.write("SIMULATION NAME: " + simulation_name + "\n")
        f_write.write("NUMBER OF VARIABLES: " + `number_of_variables` + "\n")
        f_write.write("VARIABLE NAMES: " + " ".join(variable_names) + "\n")
        f_write.write("NUMBER OF STATES: " + " ".join(map(str, number_of_states)) + "\n")
        f_write.write("SPEED OF VARIABLES: " + " ".join(map(str, speed_of_variables)) + "\n\n")

        f_write.writelines(pdsToTable(pds, variable_names, number_of_states[0]))
        
    subprocess.call(["cyclone", "cyclone_input.txt", "-table", "-edges", "-f", "cyclone_output.txt"])
    subprocess.call(["dot", "-Tpng", "cyclone_output.txt", "-o", "cyclone_output.png"])
    
