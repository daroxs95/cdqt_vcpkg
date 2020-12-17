cmake_minimum_required (VERSION 3.5)

SET(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake")

### Project Name
project (CheckDoseQt LANGUAGES CXX)


### General required config
set (CMAKE_AUTOUIC ON)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)

if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set (CMAKE_INCLUDE_CURRENT_DIR ON)
endif ()

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package (Qt5 COMPONENTS Widgets Core REQUIRED)
find_package (Qt5 COMPONENTS Widgets Core REQUIRED)
find_package (jsoncpp CONFIG REQUIRED)
find_package (Eigen3 CONFIG REQUIRED)


### Add needed files for compilation
add_executable (CheckDoseQt
    main.cpp
    checkdoseqt.cpp
    checkdoseqt.h
    checkdoseqt.ui
    tuberosum_tools/peel_wrappers.h
    tuberosum_tools/puree_math.h
    tuberosum_tools/puree_math.cpp
    tuberosum_tools/potato_linac.h
    tuberosum_tools/potato_linac.cpp
)


### Add some util files to project build folder
set (TestFiles
  sample.dcm
  Machines.rar
  )

foreach (_files ${TestFiles})
  configure_file (
    ${PROJECT_SOURCE_DIR}/${_files}
    ${PROJECT_BINARY_DIR}/${_files}
    COPYONLY
    )
endforeach()


### Unpack the Machine folder in the executable folder
execute_process ( COMMAND cmake -E tar -x Machines.rar)

execute_process(COMMAND cmake -E echo "-- Machines.rar extracted to ${PROJECT_BINARY_DIR}")



### Target include dirs and libs
target_include_directories (CheckDoseQt PRIVATE ${EIGEN3_DIR} ${JSON_DIR})

### Declare some vars according to the host OS
if (UNIX)  # All Unix relative configs
  target_link_libraries (CheckDoseQt PRIVATE Qt5::Widgets Qt5::Core jsoncpp_object jsoncpp_static Eigen3::Eigen)
endif (UNIX)

if (WIN32)       # All Windows relative configs
  #target_link_libraries (CheckDoseQt PRIVATE Qt5::Widgets Qt5::Core jsoncpp_lib jsoncpp_object Eigen3::Eigen)
  target_link_libraries (CheckDoseQt PRIVATE Qt5::Widgets Qt5::Core jsoncpp_object jsoncpp_static Eigen3::Eigen)
endif (WIN32)