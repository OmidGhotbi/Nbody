import numpy as np
cimport numpy as np
from libc.stdlib cimport malloc, free
from libc.math cimport sqrt
from cython.parallel import prange
import time

cdef struct Body:
    float x, y, z, vx, vy, vz

cdef void initializeBodies(Body *bodies, int numBodies):
    for i in range(numBodies):
        bodies[i].x = 2.0 * (np.random.rand() - 1.0)
        bodies[i].y = 2.0 * (np.random.rand() - 1.0)
        bodies[i].z = 2.0 * (np.random.rand() - 1.0)

cdef void computeForces(Body *bodies, float dt, int numBodies):
    cdef:
        int i, j
        float dx, dy, dz, distSqr, invDist, invDist3
        float Fx, Fy, Fz
    for i in prange(numBodies, nogil=True):
        Fx = 0.0
        Fy = 0.0
        Fz = 0.0
        for j in range(numBodies):
            dx = bodies[j].x - bodies[i].x
            dy = bodies[j].y - bodies[i].y
            dz = bodies[j].z - bodies[i].z
            distSqr = dx * dx + dy * dy + dz * dz + 1e-9
            invDist = 1.0 / sqrt(distSqr)
            invDist3 = invDist * invDist * invDist
            Fx += dx * invDist3
            Fy += dy * invDist3
            Fz += dz * invDist3
        bodies[i].vx += dt * Fx
        bodies[i].vy += dt * Fy
        bodies[i].vz += dt * Fz

def main():
    cdef:
        int numBodies = 30000
        float timeStep = 0.01
        int numIterations = 10
        double totalTime = 0.0
        double averageTime
        double start, end, elapsedSeconds
    cdef Body *bodies = <Body *> malloc(numBodies * sizeof(Body))
    initializeBodies(bodies, numBodies)
    for iter in range(1, numIterations + 1):
        start = time.time()
        computeForces(bodies, timeStep, numBodies)
        for i in range(numBodies):
            bodies[i].x += bodies[i].vx * timeStep
            bodies[i].y += bodies[i].vy * timeStep
            bodies[i].z += bodies[i].vz * timeStep
            print(f"Body {i}: x = {bodies[i].x}, y = {bodies[i].y}, z = {bodies[i].z}")
        end = time.time()
        elapsedSeconds = end - start
        if iter > 1:
            totalTime += elapsedSeconds
    averageTime = totalTime / (numIterations - 1)
    print(f"Average rate: {averageTime:.3f} seconds per iteration")
    free(bodies)
