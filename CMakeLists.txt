cmake_minimum_required(VERSION 3.10)

# Set the project name
project(FreddyControl)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Include directories (equivalent to -I"../install/include/")
include_directories("install/include/soem/")

# Link directories (equivalent to -L"../install/lib/")
link_directories("install/lib/")

# Specify the source file
set(SOURCES freddy_power_control/pmu_control.c)

# Specify the executable to be built from the source file
add_executable(pmu_control ${SOURCES})

# Link libraries (equivalent to -lsoem -lpthread -lgsl -ldl -lgslcblas -lm)
target_link_libraries(pmu_control
    soem
)