import numpy as np
import time

n = 30000
dt = 0.01
steps = 10
SOFTENING = 1e-9

class Body:
    def __init__(self):
        self.pos = np.random.rand(3) * 2 - 1
        self.vel = np.zeros(3, dtype=np.float32)

def initialize_bodies():
    return [Body() for _ in range(n)]

def compute_force(bodies):
    forces = np.zeros((n, 3), dtype=np.float32)
    for i in range(n):
        for j in range(n):
            if i != j:
                r = bodies[j].pos - bodies[i].pos
                dist_sqr = np.sum(r**2) + SOFTENING
                inv_dist = 1.0 / np.sqrt(dist_sqr)
                inv_dist3 = inv_dist**3
                forces[i] += r * inv_dist3
    return forces

def update(bodies, forces):
    for i in range(n):
        bodies[i].vel += dt * forces[i]
        bodies[i].pos += dt * bodies[i].vel

def main():
    bodies = initialize_bodies()
    totalTime = 0.0
    for step in range(1, steps + 1):
        start = time.time()
        forces = compute_force(bodies)
        update(bodies, forces)
        end = time.time()
        elapsedSeconds = end - start
        if step > 1:
            totalTime += elapsedSeconds
        print(f"Iteration {step}: {elapsedSeconds:.3f} seconds")
    averageTime = totalTime / (steps - 1)
    print(f"Average rate: {averageTime:.3f} seconds per iteration")

if __name__ == "__main__":
    main()
