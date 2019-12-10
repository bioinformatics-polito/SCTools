# ===========================================================================
# CMakeLists.txt
# --------------
#
# Top-level CMake build configuration file. It defines project meta-data
# and decides what sub directories have to be considered for the
# configuration process.
# ===========================================================================

cmake_minimum_required(VERSION
                       3.7.2)

# ---------------------------------------------------------------------------
# Initialize project configuration.
# ---------------------------------------------------------------------------

project(SCTools
        LANGUAGES CXX
        VERSION 0.1.0.0)
set(SCTools_DESCRIPTION
    "A set of tools for dealing with 10X technology files.")
set(SCTools_HOMEPAGE_URL
    "https://philae.polito.it/gitlab/biotools/sctools")
set(CMAKE_CXX_STANDARD
    14)
set(CMAKE_CXX_STANDARD_REQUIRED
    YES)

# ---------------------------------------------------------------------------
# Select the proper default build type.
# ---------------------------------------------------------------------------

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE
	    "Release"
	    CACHE STRING
	    "The default project build type is 'Release'")
endif ()
message(STATUS
        "CMake build type is ${CMAKE_BUILD_TYPE}")

# ---------------------------------------------------------------------------
# Configure SCTools project configuration.
# ---------------------------------------------------------------------------

option(SCTools_BUILD_SUITE
       "Choose if SCTools suite applications have to be configured" ON)
option(SCTools_BUILD_TESTS
       "Choose if SCTools suite tests have to be configured" OFF)
option(SCTools_BUILD_DOCS
       "Choose if SCTools suite documentation has to be configured" OFF)

# ---------------------------------------------------------------------------
# Configure SCTools library targets.
# ---------------------------------------------------------------------------

add_subdirectory(include)

# ---------------------------------------------------------------------------
# Check what of the optional SCTools project targets have to be
# configured.
# ---------------------------------------------------------------------------

if (${SCTools_BUILD_SUITE})
	add_subdirectory(apps)
else ()
	message(STATUS
	        "User chose not to build SCTools suite applications")
endif ()

if (${SCTools_BUILD_TESTS})
	add_subdirectory(tests)
else ()
	message(STATUS
	        "User chose not to build SCTools tests")
endif ()

if (${SCTools_BUILD_DOCS})
	add_subdirectory(docs)
else ()
	message(STATUS
	        "User chose not to build SCTools documentation")
endif ()
