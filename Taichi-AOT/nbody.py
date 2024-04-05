import taichi as ti

ti.init(arch=ti.vulkan) # gpu for cuda backend, cpu for CPU etc.

n = 30000
dt = 0.01
steps = 10
SOFTENING = 1e-9

pos = ti.Vector.field(3, dtype=ti.f32, shape=n)
vel = ti.Vector.field(3, dtype=ti.f32, shape=n)

@ti.kernel
def initialize():
    for i in pos:
        pos[i] = ti.Vector([2 * ti.random() - 1 for _ in range(3)])

@ti.kernel
def compute_forces():
    for i in pos:
        force = ti.Vector([0.0, 0.0, 0.0])
        for j in range(n):
            if i != j:
                r = pos[j] - pos[i]
                dist_sqr = r.norm_sqr() + SOFTENING
                inv_dist = 1.0 / ti.sqrt(dist_sqr)
                inv_dist3 = inv_dist * inv_dist * inv_dist
                force += r * inv_dist3
        vel[i] += dt * force

@ti.kernel
def update():
    for i in pos:
        pos[i] += dt * vel[i]

if __name__ == "__main__":
    mod = ti.aot.Module(ti.vulkan) # gpu for cuda backend, cpu for CPU etc.
    mod.add_kernel(initialize)
    mod.add_kernel(compute_forces)
    mod.add_kernel(update)
    mod.save("build")
