
# --------------- END_TO_END TESTING -------------------------


# Set the root for searching for files to test.
set(ORIG_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tutorials")

# Make a list of the files that need to be built and run.
set(SRC_CPP 
    tutorial1/tutorial1_complete.cpp
    tutorial2/tutorial2_complete.cpp
    tutorial3/tutorial3_complete.cpp
    tutorial5/tutorial5_complete.cpp
)

# Make a list of extra source files which need to be included in the 
# build.  These should be common for all files to be tested in the list
# above.
set(EXTRA_CPP 
    "${CMAKE_CURRENT_SOURCE_DIR}/tutorials/tutorial_utilities.cpp"
)

# Set the path(s) to any header files that are needed.
set(EXTRA_H 
    "${CMAKE_CURRENT_SOURCE_DIR}/tutorials/tutorial_utilities.h"
)

# For each of the files in the SRC_CPP list you must supply a corresponding
# set of resources to test against.  These will be in the same directory structure
# from the ORIG_SRC_DIR downwards, under the EXPECTED_OUTPUT_PATH root.  
set(EXPECTED_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tests/tutorial_tests")

# Make working directory locally.  This is where the source code and the 
# resources will be temporarily copied to.  It will be deleted by the 'make cleanup' command.
set(TEMP_WORKING_DIR "tutorials")

# ------------------ Launch the build ---------------------------
include(tests/make_all_tests.cmake)