cmake_minimum_required (VERSION 2.6)

# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their
# definition and dump it in the cache along with proper documentation, otherwise
# set MAKE_BUILD_TYPE to Debug prior to calling PROJECT()
if (DEFINED CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
else()
    set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
endif()

project ("lightship" C)

# Make macs happy
set (CMAKE_MACOSX_RPATH OFF)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (PLATFORM "LINUX")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (PLATFORM "MAC")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (PLATFORM "WINDOWS")
endif ()

# create vcproj.user file for Visual Studio to set debug working directory
function(create_vcproj_userfile TARGETNAME)
  if(MSVC)
    configure_file(
      ${CMAKE_SOURCE_DIR}/cmake/templates/VisualStudioUserFile.vcproj.user.in
      ${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcproj.user
      @ONLY)
    configure_file(
      ${CMAKE_SOURCE_DIR}/cmake/templates/VisualStudioUserFile.vcxproj.user.in
      ${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcxproj.user
      @ONLY)
  endif()
endfunction()

###############################################################################
# set output locations
###############################################################################

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
if (WIN32)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
else ()
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif ()

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

###############################################################################
# compiler definitions and flags
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
    add_definitions (/D "_CRT_SECURE_NO_WARNINGS")
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions (/MTd)
    else ()
        add_definitions (/MT)
    endif ()
endif ()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(
        -D_DEBUG
        -DDEBUG
    )
endif ()

# library is being built (so dllexport gets defined on windows)
add_definitions (-DLIGHTSHIP_BUILDING_LIBRARY)

###############################################################################
# D compiler (experimental)
###############################################################################

option (ENABLE_DLANG "Enable D-language based plugins" OFF)

if(ENABLE_DLANG)
    set( D_PLUGIN_IMPORT_DIRECTORY "${CMAKE_SOURCE_DIR}/util/include/util/d_api/" )

    if (NOT D_COMPILER)
        find_program (D_COMPILER "ldc2")
    endif ()
    if (NOT D_COMPILER)
        find_program (D_COMPILER "dmd")
    endif ()
    if (NOT D_COMPILER)
        find_program (D_COMPILER "gdc")
    endif ()

    if (NOT D_COMPILER)
        set (HAVE_D_COMPILER OFF)
        message (WARNING "Could NOT find a D compiler. Maybe define -DD_COMPILER=<path_to_compiler>")
        message (WARNING "D plugins will not be built.")
    else ()
        set (HAVE_D_COMPILER ON)
    endif ()

    if (D_COMPILER MATCHES "ldc2")
        set (D_GLOBAL_COMPILER_FLAGS -shared -relocation-model=pic)
    endif ()
    if (D_COMPILER MATCHES "dmd")
        set (D_GLOBAL_COMPILER_FLAGS -shared -fPIC)
    endif ()
    if (D_COMPILER MATCHES "gdc")
        set (D_GLOBAL_COMPILER_FLAGS -shared -fPIC)
    endif ()
    
else ()
    set (ENABLE_DLANG OFF)
endif ()

###############################################################################
# generate missing folders and files
###############################################################################

file (MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins")
# ignore auto generated file config.h
file (WRITE "${CMAKE_SOURCE_DIR}/util/include/util/.gitignore" "config.h")

###############################################################################
# Dependency settings
###############################################################################

include_directories ("lightship/include")
include_directories ("util/include")

add_subdirectory ("lightship")
add_subdirectory ("plugins")
add_subdirectory ("util")
add_subdirectory ("cross-compile")
