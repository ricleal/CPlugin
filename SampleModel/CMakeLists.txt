# To avoid warning: warning: deprecated conversion from string constant to ‘char*’ [-Wwrite-strings]
set(CMAKE_VERBOSE_MAKEFILE true)

FILE(GLOB SOURCE_CPP "*.cpp")

add_library (SampleModel SHARED ${SOURCE_CPP})

target_include_directories (SampleModel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
