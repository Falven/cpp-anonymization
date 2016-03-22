include_directories(${cpp-anonymization_SOURCE_DIR})

file(GLOB_RECURSE cpp-anonymization_HEADERS "${cpp-anonymization_SOURCE_DIR}/anonymization/*.hpp")

add_custom_target(cpp-anonymization SOURCES ${cpp-anonymization_HEADERS})

add_executable(awget ${cpp-anonymization_SOURCE_DIR}/lib/src/awget.cpp)

add_executable(ss ${cpp-anonymization_SOURCE_DIR}/lib/src/ss.cpp)