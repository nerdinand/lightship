project ("lightship" C)

###############################################################################
# compiler flags for this project
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions (-W -Wall -Wextra -pedantic -Wno-unused-parameter)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
endif ()

###############################################################################
# source files and runtime definition
###############################################################################

file (GLOB lightship_HEADERS "include/lightship/*.h")
file (GLOB lightship_SOURCES "src/*.c")

# dynamic linker
if (${PLATFORM} MATCHES "LINUX")
	set (DYNAMIC_LINKER_LIBRARY "dl")
elseif (${PLATFORM} MATCHES "MAC")
    set (DYNAMIC_LINKER_LIBRARY "dl")
endif ()

add_executable (lightship
    ${lightship_HEADERS}
    ${lightship_SOURCES}
)

target_link_libraries (lightship
    lightship_util
    ${DYNAMIC_LINKER_LIBRARY}
)
create_vcproj_userfile (lightship)

include_directories ("${CMAKE_SOURCE_DIR}/plugins/core/yaml/include")

###############################################################################
# install targets
###############################################################################

install (
    TARGETS
        lightship
    DESTINATION
        "bin"
)
install (
    DIRECTORY
        "${CMAKE_CURRENT_LIST_DIR}/cfg"
    DESTINATION
        "share"
)