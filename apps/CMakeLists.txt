# ===========================================================================
# apps/CMakeLists.txt
# ----------------------
#
# Module for configuring applications part of the SCTools application
# suite.
# ===========================================================================

include(GNUInstallDirs)

# ---------------------------------------------------------------------------
# Configure an target for every application of the SCTools suite.
# ---------------------------------------------------------------------------

add_executable(sctools_demultiplex
               demultiplex/demultiplex.cpp)
target_link_libraries(sctools_demultiplex
                      PUBLIC
                      SCTools)
target_compile_options(sctools_demultiplex
                       INTERFACE
                       -march=native)

# ---------------------------------------------------------------------------
# Configure application suite installation
# ---------------------------------------------------------------------------

install(TARGETS
        sctools_demultiplex
        DESTINATION
        ${CMAKE_INSTALL_BINDIR})
