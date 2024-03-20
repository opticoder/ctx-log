import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.build import can_run


class ctx_logTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    # generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("ctx-log/0.1.0")
        # self.requires(self.tested_reference_str)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.user_presets_path = False
        if self.dependencies["ctx-log"].options.context_engine == "userver":
            tc.variables["CONTEXT_ENGINE_USERVER"] = True
        tc.generate()

    def layout(self):
        cmake_layout(self)

    def test(self):
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindir, "demo")
            # cmd = os.path.join(self.cpp.build.bindir, "context")
            self.run(cmd, env="conanrun")
