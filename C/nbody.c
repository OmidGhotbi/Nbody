#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> // Include time.h for timing

#define SOFTENING 1e-9f

typedef struct {
    float x, y, z, vx, vy, vz;
} Body;

void initializeBodies(Body *bodies, int numBodies) {
    for (int i = 0; i < numBodies; i++) {
        bodies[i].x = 2.0f * (rand() / (float)RAND_MAX) - 1.0f;
        bodies[i].y = 2.0f * (rand() / (float)RAND_MAX) - 1.0f;
        bodies[i].z = 2.0f * (rand() / (float)RAND_MAX) - 1.0f;
    }
}

void computeForces(Body *bodies, float dt, int numBodies) {
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < numBodies; i++) {
        float Fx = 0.0f, Fy = 0.0f, Fz = 0.0f;

        for (int j = 0; j < numBodies; j++) {
            float dx = bodies[j].x - bodies[i].x;
            float dy = bodies[j].y - bodies[i].y;
            float dz = bodies[j].z - bodies[i].z;
            float distSqr = dx * dx + dy * dy + dz * dz + SOFTENING;
            float invDist = 1.0f / sqrtf(distSqr);
            float invDist3 = invDist * invDist * invDist;

            Fx += dx * invDist3;
            Fy += dy * invDist3;
            Fz += dz * invDist3;
        }

        bodies[i].vx += dt * Fx;
        bodies[i].vy += dt * Fy;
        bodies[i].vz += dt * Fz;
    }
}

int main(int argc, char **argv) {
    int numBodies = 30000;
    if (argc > 1) {
        numBodies = atoi(argv[1]);
    }

    const float timeStep = 0.01f;
    const int numIterations = 10;

    Body *bodies = (Body *)malloc(numBodies * sizeof(Body));
    initializeBodies(bodies, numBodies);

    double totalTime = 0.0;

    for (int iter = 1; iter <= numIterations; iter++) {
        clock_t start = clock(); // Start timer

        computeForces(bodies, timeStep, numBodies);

        for (int i = 0; i < numBodies; i++) {
            bodies[i].x += bodies[i].vx * timeStep;
            bodies[i].y += bodies[i].vy * timeStep;
            bodies[i].z += bodies[i].vz * timeStep;
        }

        clock_t end = clock(); // Stop timer
        double elapsedSeconds = (double)(end - start) / CLOCKS_PER_SEC;

        if (iter > 1) {
            totalTime += elapsedSeconds;
        }

#ifndef SHMOO
        printf("Iteration %d: %.3f seconds\n", iter, elapsedSeconds);
#endif
    }

    double averageTime = totalTime / (double)(numIterations - 1);

#ifdef SHMOO
    printf("%d, %0.3f\n", numBodies, 1e-9 * numBodies * numBodies / averageTime);
#else
    printf("Average rate: %.3f seconds per iteration\n", averageTime);
#endif

    free(bodies);
    return 0;
}
