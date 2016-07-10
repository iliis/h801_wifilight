#!/bin/env python3

MAX_OUT = 1023
MAX_IN = 255

gamma = 2.8


def gamma_correction(val):
    return int(pow(val / MAX_IN, gamma) * MAX_OUT + 0.5)



values = map(gamma_correction, range(0, MAX_IN+1))

print("uint8_t gamma_table[] = {")

for i, v in enumerate(values):
    if i % 16 == 0:
        print("    ", end="")

    print("{:4d}, ".format(v), end="")

    if i % 16 == 15:
        print("")

print("};")
