cmake_minimum_required(VERSION 3.19)
project(ctx-log CXX)

add_library(ctx-log
        ctx-log.cc
        coloring.cc
        context_threads.cc
)

target_compile_features(ctx-log PUBLIC cxx_std_20)

target_include_directories(ctx-log PUBLIC include)
set_target_properties(ctx-log PROPERTIES PUBLIC_HEADER
        "include/ctx-log.h;include/context.h;include/coloring.h;"
)

find_package(quill REQUIRED)
find_package(fmt COMPONENTS REQUIRED)
find_package(fkyaml REQUIRED)

target_link_libraries(ctx-log quill::quill)
target_link_libraries(ctx-log fmt::fmt)
#target_link_libraries(ctx-log PRIVATE fmt::fmt-header-only)
target_link_libraries(ctx-log fkyaml::fkyaml)

if (CONTEXT_ENGINE_USERVER)
    find_package(userver COMPONENTS REQUIRED)
    target_link_libraries(ctx-log userver::core)
    target_link_libraries(ctx-log userver::universal)
    target_sources(ctx-log PRIVATE context_userver.cc)
#    target_compile_definitions(ctx-log PUBLIC CONTEXT_ENGINE_USERVER=1)
endif()

# ORIGIN: https://stackoverflow.com/questions/410980/include-a-text-file-in-a-c-program-as-a-char/47801116#47801116
function(make_includable input_file output_file)
    file(READ ${input_file} content)
    set(delim "for_c++_include")
    set(content "R\"${delim}(\n${content})${delim}\"")
    file(WRITE ${output_file} "${content}")
endfunction(make_includable)

make_includable(colors.yaml generated/colors.yaml.inc)


install(TARGETS ctx-log)

#if(DEFINED CMAKE_PROJECT_TOP_LEVEL_INCLUDES)
#        add_subdirectory("${PROJECT_SOURCE_DIR}/tests" "${CMAKE_CURRENT_BINARY_DIR}-tests" EXCLUDE_FROM_ALL)
##    include_directories("${PROJECT_SOURCE_DIR}/tests") #"${CMAKE_CURRENT_BINARY_DIR}-tests" EXCLUDE_FROM_ALL)
#        add_dependencies(example
#                ctx-log)
#    add_test(NAME example
#            COMMAND example)
#endif()
