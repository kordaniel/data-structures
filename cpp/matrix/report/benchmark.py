#!/usr/bin/env python3

import numpy as np
import time

t_B0 = time.time()
B = np.matrix(np.random.rand(10**3, 10**6).astype(np.single))
t_B1 = time.time()
print("DONE: Generated matrix B in:", t_B1 - t_B0, "seconds.")


t_C0 = time.time()
C = np.matrix(np.random.rand(10**6, 1).astype(np.single))
t_C1 = time.time()
print("DONE: Generated matric C in:", t_C1 - t_C0, "seconds.")

t_BC0 = time.time()
BC = B * C
t_BC1 = time.time()
print("DONE: Computed matrix BC in:", t_BC1 - t_BC0, "seconds.")

del B
del C

t_A0 = time.time()
A = np.matrix(np.random.rand(10**6, 10**3).astype(np.single))
t_A1 = time.time()
print("DONE: Generated matrix A in:", t_A1 - t_A0, "seconds.")

t_ABC0 = time.time()
ABC = A * BC
t_ABC1 = time.time()

print("DONE: Computed matrix A*(BC) in", t_ABC1 - t_ABC0, "seconds.")

