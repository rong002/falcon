####################################################################
# @MAIN_PRJ@
#
# CMake configuration file for @PROJECT_NAME@
####################################################################

cmake_minimum_required(VERSION 2.6)
project(conio)

# find the falcon installation.  Set CMAKE_INSTALL_PREFIX to the falcon root 
# directory to make it work.
find_package(Falcon REQUIRED)

## process source directories
# add other subdirectories that contain libraries on which the final module depends here..
#add_subdirectory(awesome_library)
# ..before the module's source dir:
add_subdirectory(src)
add_subdirectory(conbox)
