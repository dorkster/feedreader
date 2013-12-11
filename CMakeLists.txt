Project (feedreader)
cmake_minimum_required (VERSION 2.6)

Set (PACKAGE "feedreader")
Set (VERSION "1.0")


# Default definitions
if (NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wunused -Wshadow -Woverloaded-virtual -Wunreachable-code")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-math-errno -fno-exceptions")
  if (NOT CMAKE_CXX_COMPILER MATCHES "clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
  endif (NOT CMAKE_CXX_COMPILER MATCHES "clang")
  set(CMAKE_LD_FLAGS "-fwhole-program")
endif (NOT MSVC)

if (CMAKE_CXX_COMPILER MATCHES "clang")
  # clang needs lstdc++ linked explicitly:
  set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -lstdc++")
elseif ()
  # assume gcc:
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-enforce-eh-specs")
endif ()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g0")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -g0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -pg")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-pg")
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "-pg")
  set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "-pg")
endif()

# directories
set(CMAKE_INSTALL_PREFIX  "/usr")
set(BINDIR  "bin"             CACHE STRING "Directory from CMAKE_INSTALL_PREFIX where game executable will be installed.")

If(NOT IS_ABSOLUTE "${BINDIR}")
    set(FEEDREADER_EXECUTABLE_PATH ${CMAKE_INSTALL_PREFIX}/${BINDIR}/feedreader)
Else(NOT IS_ABSOLUTE "${BINDIR}")
    set(FEEDREADER_EXECUTABLE_PATH ${BINDIR}/feedreader)
EndIf(NOT IS_ABSOLUTE "${BINDIR}")
# Detect missing dependencies


# libraries
Find_Package (GTK2 REQUIRED)
If (NOT GTK2_FOUND)
  Message (FATAL_ERROR "Couldn't find GTK development files.")
Else (NOT GTK2_FOUND)
    Include_Directories (${GTK2_INCLUDE_DIRS})
EndIf (NOT GTK2_FOUND)

Find_Package (LibXml2 REQUIRED)
If (NOT LIBXML2_FOUND)
  Message (FATAL_ERROR "Couldn't find libxml2 development files.")
Else (NOT LIBXML2_FOUND)
    Include_Directories (${LIBXML2_INCLUDE_DIR})
EndIf (NOT LIBXML2_FOUND)

Find_Package (CURL REQUIRED)
If (NOT CURL_FOUND)
    Message (FATAL_ERROR "Couldn't find CURL development files.")
Else (NOT CURL_FOUND)
    Include_Directories (${CURL_INCLUDE_DIRS})
EndIf (NOT CURL_FOUND)

# Sources

Set (FEEDREADER_SOURCES
    ./src/download.c
    ./src/feedlist.c
    ./src/fileparser.c
    ./src/gui.c
    ./src/parse.c
    ./src/util.c
    ./src/main.c
)

Add_Executable (feedreader ${FEEDREADER_SOURCES})

Target_Link_Libraries (feedreader ${CMAKE_LD_FLAGS} ${GTK2_LIBRARIES} ${LIBXML2_LIBRARIES} ${CURL_LIBRARIES})


# installing to the proper places
install(PROGRAMS
  ${CMAKE_CURRENT_BINARY_DIR}/feedreader
  DESTINATION ${BINDIR})
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/feedreader.png"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps)

