# ===========================================================================
# tests/units/CMakeLists.txt
# --------------------------
#
# CMakeLists.txt in charge of creating the actual unit test targets. A
# single target for each library component is configured.
# ===========================================================================

# ---------------------------------------------------------------------------
# Enable testing.
# ---------------------------------------------------------------------------

enable_testing()

# ---------------------------------------------------------------------------
# Configure unit test targets.
# ---------------------------------------------------------------------------

add_executable(sctools_units_sctools
               main.cpp)
target_link_libraries(sctools_units_sctools
                      PUBLIC
                      SCTools_Test)
