# ===========================================================================
# include/CMakeLists.txt
# ----------------------
#
# Module for creating the SCTools library core target.
# ===========================================================================

include(${CMAKE_SOURCE_DIR}/cmake/FetchContent.cmake)

# ---------------------------------------------------------------------------
# Retrieve the external projects SCTools depends on.
# ---------------------------------------------------------------------------

# Register SeqAn dependency.
fetchcontent_declare(sctools_seqan
                     GIT_REPOSITORY
                     https://github.com/seqan/seqan.git
                     GIT_TAG
                     seqan-v2.4.0)
fetchcontent_getproperties(sctools_seqan)
if (NOT sctools_seqan_POPULATED)
	fetchcontent_populate(sctools_seqan)
	list(APPEND
	     CMAKE_INCLUDE_PATH
	     ${CMAKE_INCLUDE_PATH}
	     ${sctools_seqan_SOURCE_DIR}/include)
	set(SEQAN_FIND_DEPENDENCIES
	    "DEFAULT")
	find_package(SEQAN
	             REQUIRED
	             HINTS ${sctools_seqan_SOURCE_DIR}/util/cmake)
endif ()
add_library(SCTools_SeqAn
            INTERFACE)
target_include_directories(SCTools_SeqAn
                           INTERFACE
                           ${SEQAN_INCLUDE_DIRS})
target_link_libraries(SCTools_SeqAn
                      INTERFACE
                      ${SEQAN_LIBRARIES})
target_compile_definitions(SCTools_SeqAn
                           INTERFACE
                           ${SEQAN_DEFINITIONS}
                           -DSEQAN_DISABLE_VERSION_CHECK)
separate_arguments(SEQAN_SEPARATED_CXX_FLAGS
                   UNIX_COMMAND
                   ${SEQAN_CXX_FLAGS})
target_compile_options(SCTools_SeqAn
                       INTERFACE
                       ${SEQAN_SEPARATED_CXX_FLAGS})

# ---------------------------------------------------------------------------
# Configure the library global target SCTools::SCTools.
# ---------------------------------------------------------------------------

add_library(SCTools
            INTERFACE)
target_include_directories(SCTools
                           INTERFACE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(SCTools
                      INTERFACE
                      SCTools_SeqAn
                      pthread
                      stdc++fs
                      -fopenmp)
target_compile_definitions(SCTools
                           INTERFACE
                           -DSCTools_VERSION="${SCTools_VERSION}")
