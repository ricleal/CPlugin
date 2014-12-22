cmake_minimum_required (VERSION 2.8)
project (CPlugin)


add_subdirectory (SimpleModel)
add_subdirectory (SampleModel)
add_subdirectory (SphereModel)

file(GLOB PYTHON_FILES "*.py")
file(COPY ${PYTHON_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
