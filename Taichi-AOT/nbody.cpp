#include <taichi/cpp/taichi.hpp>

struct App {
    static const uint32_t NPARTICLE = 8192 * 2;
    static const uint32_t N_ITER = 50;

    ti::Runtime runtime_;
    ti::AotModule module_;
    ti::Kernel k_init_;
    ti::Kernel k_compute_forces_;
    ti::Kernel k_update_;
    ti::NdArray<float> x_;

    App() {
        runtime_ = ti::Runtime(TI_ARCH_VULKAN); //TI_ARCH_GPU for cuda backend, CPU for cpu etc.
        module_ = runtime_.load_aot_module("nbody");
        k_init_ = module_.get_kernel("initialize");
        k_compute_forces_ = module_.get_kernel("compute_forces");
        k_update_ = module_.get_kernel("update");
        x_ = runtime_.allocate_ndarray<float>({NPARTICLE}, {}, true);
        std::cout << "Initialized!" << std::endl;
    }

    bool run() {
        float base = 0.2;

        k_init_.push_arg(x_);
        k_init_.launch();
        k_compute_forces_.push_arg(x_);
        k_compute_forces_.launch();
        for (int i = 0; i < N_ITER; i++) {
            k_update_.launch();
        }
        runtime_.wait();
        return true;
    }
};

int main(int argc, const char** argv) {
    App app;
    app.run();
    return 0;
}
