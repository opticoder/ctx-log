cmake_minimum_required(VERSION 3.19)
project(tests CXX)

find_package(ctx-log REQUIRED)

add_executable(demo demo.cc)
#target_compile_features(demo PUBLIC cxx_std_20)
target_link_libraries(demo ctx-log::ctx-log)

add_executable(context context.cc)
#target_compile_features(context PUBLIC cxx_std_20)
target_link_libraries(context PUBLIC ctx-log::ctx-log)
if (CONTEXT_ENGINE_USERVER)
    add_compile_definitions(CONTEXT_ENGINE_USERVER=1)
endif()
