import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.build import check_max_cppstd, check_min_cppstd
from conan.tools.files import copy


class ctx_logRecipe(ConanFile):
    name = "ctx-log"
    version = "0.1.0"
    package_type = "library"

    license = "<Put the package license here>"
    description = "<Description of ctx-log package here>"

    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "context_engine": [
            "threads",
            "userver",
        ],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "context_engine": "threads",
    }

    exports_sources = "CMakeLists.txt", "*.cc", "include/*"

    def export_sources(self):
        copy(self, "colors.yaml", os.path.join(self.recipe_folder, ".."), self.export_sources_folder)

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        # self.options["userver/*"].shared = True
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def validate(self):
        check_min_cppstd(self, "20")

    def build_requirements(self):
        current_directory = os.getcwd()
        from_project_root = False
        if self.recipe_folder in current_directory:
            from_project_root = True

        # TODO: https://github.com/conan-io/conan/pull/13930
        # recipes_path = "3rdparty-index"
        # if from_project_root:
        #     recipes_path = os.path.join(self.recipe_folder, recipes_path)
        # self.run("conan remote add --force 3rdparty file://"+recipes_path)

        requirements = self.conan_data.get('3rdparty', [])
        for name in requirements:
            recipes_path = os.path.join("3rdparty-index", "recipes", name, "all")
            if from_project_root:
                recipes_path = os.path.join(self.recipe_folder, recipes_path)
            self.run("conan export .", cwd=recipes_path)

        if self.options.context_engine == "userver":
            recipes_path = os.path.join("3rdparty-index", "recipes", "userver", "all")
            if from_project_root:
                recipes_path = os.path.join(self.recipe_folder, recipes_path)
            self.run("conan export --version cci.20240219 .", cwd=recipes_path)

    def requirements(self):
        requirements = self.conan_data.get('requirements', [])
        transitive_headers = self.conan_data.get('transitive_headers', [])
        for requirement in requirements:
            if requirement in transitive_headers:
                self.requires(requirement, transitive_headers=True)
            else:
                self.requires(requirement)

        if self.options.context_engine == "userver":
            self.requires('userver/cci.20240219', transitive_headers=True)
        # self.requires("quill/3.6.0", transitive_headers=True)
        self.requires("fmt/10.2.1", transitive_headers=True, override=True)
#        self.requires("fkyaml/0.3.1")

    def layout(self):
        cmake_layout(self)
        # cmake_layout(self, src_folder="src")
        # self.cpp.package.includedirs = ["myinclude"]

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.user_presets_path = False
        if self.options.context_engine == "userver":
            tc.variables["CONTEXT_ENGINE_USERVER"] = True
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["ctx-log"]
