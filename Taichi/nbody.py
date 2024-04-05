import taichi as ti
import numpy as np
import time

ti.init(arch=ti.vulkan) #gpu for cuda backend and cpu for CPU

n = 30000
dt = 0.01
steps = 10
SOFTENING = 1e-9

pos = ti.Vector.field(3, dtype=ti.f32, shape=n)
vel = ti.Vector.field(3, dtype=ti.f32, shape=n)
force = ti.Vector.field(3, dtype=ti.f32, shape=n)

@ti.kernel
def initialize():
    for i in pos:
        pos[i] = ti.Vector([2 * ti.random() - 1 for _ in range(3)])

@ti.kernel
def compute_force():
    for i in pos:
        force[i] = ti.Vector([0.0, 0.0, 0.0])
        for j in range(n):
            if i != j:
                r = pos[j] - pos[i]
                distSqr = r.norm_sqr() + SOFTENING
                invDist = 1.0 / ti.sqrt(distSqr)
                invDist3 = invDist * invDist * invDist
                force[i] += r * invDist3

@ti.kernel
def update():
    for i in pos:
        vel[i] += dt * force[i]
        pos[i] += dt * vel[i]

initialize()
totalTime = 0.0
for step in range(1, steps + 1):
    start = time.time()
    compute_force()
    update()
    end = time.time()
    elapsedSeconds = end - start
    if step > 1:
        totalTime += elapsedSeconds
    print(f"Iteration {step}: {elapsedSeconds:.3f} seconds")
averageTime = totalTime / (steps - 1)
print(f"Average rate: {averageTime:.3f} seconds per iteration")
