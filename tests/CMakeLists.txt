# ===========================================================================
# tests/CMakeLists.txt
# ----------------------
#
# Module for creating the main SCTools test target.
# ===========================================================================

include(${CMAKE_SOURCE_DIR}/cmake/FetchContent.cmake)

# ---------------------------------------------------------------------------
# Retrieve the external projects SCTools tests depend on.
# ---------------------------------------------------------------------------

# Register Google test dependency.
fetchcontent_declare(sctools_googletest
                     GIT_REPOSITORY
                     https://github.com/google/googletest.git
                     GIT_TAG
                     release-1.8.1)
fetchcontent_getproperties(sctools_googletest)
if (NOT sctools_googletest_POPULATED)
	fetchcontent_populate(sctools_googletest)
	set(BUILD_GMOCK
	    CACHE BOOL
	    OFF)
	set(INSTALL_GTEST
	    CACHE BOOL
	    OFF)
	add_subdirectory(${sctools_googletest_SOURCE_DIR}
	                 ${sctools_googletest_BINARY_DIR})
endif ()

# ---------------------------------------------------------------------------
# Configure the library top test target SCTools::Test.
# ---------------------------------------------------------------------------

add_library(SCTools_Test
            INTERFACE)
target_include_directories(SCTools_Test
                           INTERFACE
                           ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(SCTools_Test
                      INTERFACE
                      SCTools
                      gtest_main
                      gtest)

# ---------------------------------------------------------------------------
# Configure unit test targets.
# ---------------------------------------------------------------------------

add_subdirectory(units)
