cmake_minimum_required(VERSION 3.10)
project(eddie_pmu_control)

find_package(soem)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")

add_executable(eddie_pmu_control
    eddie_pmu_control.c
)
target_link_libraries(eddie_pmu_control
    soem
)

set_target_properties(eddie_pmu_control PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

install(TARGETS eddie_pmu_control
    RUNTIME DESTINATION bin
)
