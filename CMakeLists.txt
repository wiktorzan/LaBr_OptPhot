#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(LABr3_V2)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

find_package(ROOT REQUIRED)
message(STATUS "ROOT version: ${ROOT_VERSION}")
if(ROOT_FOUND AND ROOT_VERSION VERSION_LESS "6.0")
  message(FATAL_ERROR "ROOT 6.0 is required ")
  # tested version above 6.10 can not display graphics while /run/beamOn due to llvm problem
endif()

include(${ROOT_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(SYSTEM ${Geant4_INCLUDE_DIR} ${ROOT_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${Geant4_INCLUDE_DIR})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(LaBr3_V2 LaBr3_V2.cc ${sources} ${headers})
target_link_libraries(LaBr3_V2 ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

add_dependencies(LaBr3_V2 ICRPdata)
#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build TestEm5. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(LaBr3_V2_SCRIPTS
    run.mac vis.mac ColourMap.dat
  )

foreach(_script ${LaBr3_V2_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

include(ExternalProject)
ExternalProject_Add(ICRPdata
  SOURCE_DIR ${PROJECT_BINARY_DIR}/ICRPdata
  URL https://cern.ch/geant4-data/datasets/examples/advanced/ICRP110Phantoms/ICRPdata.tar.gz
  URL_MD5 2aba5409d4e7b7a2f0328e7d3402f187
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  )
#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS LaBr3_V2 DESTINATION bin)

