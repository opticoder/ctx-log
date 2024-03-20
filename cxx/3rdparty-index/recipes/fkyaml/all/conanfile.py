from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.scm import Git
from conan.tools.build import check_max_cppstd, check_min_cppstd


class fkYAMLRecipe(ConanFile):
    name = "fkyaml"
    version = "0.3.1"
    package_type = "header-library"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # generators = "CMakeDeps"

    def validate(self):
        check_min_cppstd(self, "11")
        # check_max_cppstd(self, "20")

    def source(self):
        # https://github.com/conan-io/conan-center-index/blob/master/recipes/gtest/all/conanfile.py#L106C9-L106C79
        git = Git(self)
        git.clone(url="https://github.com/fktn-k/fkYAML.git", target=".", args=("--branch v"+self.version, "--depth=1"))

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_id(self):
        self.info.clear()

    # FIXME: headers only
    # def package_info(self):
    #     self.cpp_info.bindirs = []
    #     self.cpp_info.libdirs = []
        # self.cpp_info.libs = ["fkYAML"]
