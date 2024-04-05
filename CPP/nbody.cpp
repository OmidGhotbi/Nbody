#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>

#define SOFTENING 1e-9f

struct Body {
    float x, y, z, vx, vy, vz;
};

void initializeBodies(std::vector<Body>& bodies, int numBodies) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

    for (int i = 0; i < numBodies; i++) {
        bodies[i].x = 2.0f * distribution(generator);
        bodies[i].y = 2.0f * distribution(generator);
        bodies[i].z = 2.0f * distribution(generator);
    }
}

void computeForces(std::vector<Body>& bodies, float dt) {
    const int numBodies = bodies.size();

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < numBodies; i++) {
        float Fx = 0.0f, Fy = 0.0f, Fz = 0.0f;

        for (int j = 0; j < numBodies; j++) {
            float dx = bodies[j].x - bodies[i].x;
            float dy = bodies[j].y - bodies[i].y;a
            float dz = bodies[j].z - bodies[i].z;
            float distSqr = dx * dx + dy * dy + dz * dz + SOFTENING;
            float invDist = 1.0f / std::sqrt(distSqr);
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

int main(int argc, char** argv) {
    int numBodies = 30000;
    if (argc > 1) {
        numBodies = std::atoi(argv[1]);
    }

    const float timeStep = 0.01f;
    const int numIterations = 10;

    std::vector<Body> bodies(numBodies);
    initializeBodies(bodies, numBodies);

    double totalTime = 0.0;

    // Enable multithreading
    omp_set_num_threads(omp_get_max_threads());

    for (int iter = 1; iter <= numIterations; iter++) {
        auto start = std::chrono::high_resolution_clock::now();

        computeForces(bodies, timeStep);

        for (auto& body : bodies) {
            body.x += body.vx * timeStep;
            body.y += body.vy * timeStep;
            body.z += body.vz * timeStep;
        }

        auto end = std::chrono::high_resolution_clock::now();
        double elapsedSeconds = std::chrono::duration<double>(end - start).count();

        if (iter > 1) {
            totalTime += elapsedSeconds;
        }

#ifndef SHMOO
        std::cout << "Iteration " << iter << ": " << elapsedSeconds << " seconds\n";
#endif
    }

    double averageTime = totalTime / static_cast<double>(numIterations - 1);

#ifdef SHMOO
    std::cout << numBodies << ", " << 1e-9 * numBodies * numBodies / averageTime << "\n";
#else
    std::cout << "Average rate: " << averageTime << " seconds per iteration\n";
#endif

    return 0;
}
