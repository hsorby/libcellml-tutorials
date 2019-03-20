# Copyright libCellML Contributors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.cmake_minimum_required (VERSION 3.1)

function(TARGET_WARNINGS_AS_ERRORS _TARGET)
  set(_COMPILER_WAE)

  set(_GNU_FLAGS_COMPILER_COMPAT "GNU" "AppleClang" "Clang")
  list(FIND _GNU_FLAGS_COMPILER_COMPAT "${CMAKE_CXX_COMPILER_ID}" _INDEX)
  if(${_INDEX} GREATER -1)
    set(_COMPILER_WAE -Wall -W -Werror)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(_COMPILER_WAE /W3)
  endif()

  if(_COMPILER_WAE)
    target_compile_options(${_TARGET} PRIVATE ${_COMPILER_WAE})
  endif()
  unset(_COMPILER_WAE)
endfunction()

function(INTERNALISE_CMAKE_VARIABLES)
  # internalise some CMake variables
  set(CMAKE_INSTALL_PREFIX ${LIBCELLML_INSTALL_PREFIX} CACHE INTERNAL "Internalise CMAKE_INSTALL_PREFIX, manipulate via LIBCELLML_INSTALL_PREFIX" FORCE)
  set(CMAKE_BUILD_TYPE ${LIBCELLML_BUILD_TYPE} CACHE INTERNAL "Internalise CMAKE_BUILD_TYPE, manipulate via LIBCELLML_BUILD_TYPE" FORCE)
  set(BUILD_SHARED_LIBS ${LIBCELLML_BUILD_SHARED} CACHE INTERNAL "Internalise BUILD_SHARED_LIBS, manipulate via LIBCELLML_BUILD_SHARED" FORCE)
endfunction()

function(HIDE_DISTRACTING_VARIABLES)
  mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
  mark_as_advanced(CMAKE_CODEBLOCKS_EXECUTABLE)
  mark_as_advanced(QT_QMAKE_EXECUTABLE)
  if(APPLE)
    mark_as_advanced(CMAKE_OSX_ARCHITECTURES)
    mark_as_advanced(CMAKE_OSX_DEPLOYMENT_TARGET)
    mark_as_advanced(CMAKE_OSX_SYSROOT)
  endif()
endfunction()

function(GROUP_SOURCE_TO_DIR_STRUCTURE)
  if(MSVC)
    foreach(_FILE ${ARGN})
      # Basic test for determining if current file is a source file or header file.
      if(${_FILE} MATCHES ".+\.cpp$")
        set(_FILE_PREFIX "Source Files\\")
      else()
        set(_FILE_PREFIX "Header Files\\")
      endif()
      # Get relative path from current source directory or current binary directory.
      if(${_FILE} MATCHES "^${CMAKE_CURRENT_SOURCE_DIR}.+")
        file(RELATIVE_PATH _FILE_RELATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${_FILE})
      else()
        file(RELATIVE_PATH _FILE_RELATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}" ${_FILE})
        # Assume all files in the current binary directory are generated header files!
        set(_FILE_PREFIX "Generated Header Files\\")
      endif()
      get_filename_component(_FILE_GROUP ${_FILE_RELATIVE_PATH} DIRECTORY)
      if(_FILE_GROUP)
        string(REPLACE "/" "\\" _FILE_GROUP ${_FILE_GROUP})
      endif()
      source_group("${_FILE_PREFIX}${_FILE_GROUP}" FILES "${_FILE}")
    endforeach()
  endif()
endfunction()

function(CONFIGURE_CLANG_AND_CLANG_TIDY_SETTINGS TARGET)
  if(   "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
     OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(COMPILE_OPTIONS
      -Weverything
      -Wno-c++98-compat
      -Wno-c++98-compat-pedantic
      -Wno-documentation
      -Wno-documentation-unknown-command
      -Wno-exit-time-destructors
      -Wno-global-constructors
      -Wno-gnu-zero-variadic-macro-arguments
      -Wno-missing-prototypes
      -Wno-newline-eof
      -Wno-padded
      -Wno-reserved-id-macro
      -Wno-shadow
      -Wno-switch-enum
      -Wno-unused-macros
      -Wno-used-but-marked-unused
    )

  if(NOT "${TARGET}" STREQUAL "cellml")
    list(APPEND COMPILE_OPTIONS
      --system-header-prefix=gtest/
    )
  endif()

    set_target_properties(${TARGET} PROPERTIES
      COMPILE_OPTIONS "${COMPILE_OPTIONS}"
    )
  endif()

  if(CLANG_TIDY_EXE)
    if("${TARGET}" STREQUAL "cellml")
      set(CPPCOREGUIDELINES_PRO_TYPE_VARARG cppcoreguidelines-pro-type-vararg)
      set(CPPCOREGUIDELINES_SPECIAL_MEMBER_FUNCTIONS cppcoreguidelines-special-member-functions)
      set(HICPP_SPECIAL_MEMBER_FUNCTIONS hicpp-special-member-functions)
      set(HICPP_VARARG hicpp-vararg)
    endif()

    set(CLANG_TIDY_WARNINGS
      -*
      bugprone-*
      cppcoreguidelines-avoid-goto
      cppcoreguidelines-c-copy-assignment-signature
      cppcoreguidelines-interfaces-global-init
      cppcoreguidelines-narrowing-conversions
      cppcoreguidelines-no-malloc
      cppcoreguidelines-pro-bounds-*
      cppcoreguidelines-pro-type-const-cast
      cppcoreguidelines-pro-type-cstyle-cast
      cppcoreguidelines-pro-type-member-init
      cppcoreguidelines-pro-type-static-cast-downcast
      cppcoreguidelines-pro-type-union-access
      ${CPPCOREGUIDELINES_PRO_TYPE_VARARG}
      ${CPPCOREGUIDELINES_SPECIAL_MEMBER_FUNCTIONS}
      fuchsia-header-anon-namespaces
      fuchsia-multiple-inheritance
      fuchsia-overloaded-operator
      fuchsia-restrict-system-includes
      fuchsia-trailing-return
      fuchsia-virtual-inheritance
      google-build-*
      google-default-arguments
      google-explicit-constructor
      google-global-names-in-headers
      google-objc-*
      google-readability-braces-around-statements
      google-readability-casting
      google-readability-function-size
      google-readability-namespace-comments
      google-runtime-int
      google-runtime-operator
      hicpp-avoid-goto
      hicpp-braces-around-statements
      hicpp-deprecated-headers
      hicpp-exception-baseclass
      hicpp-explicit-conversions
      hicpp-function-size
      hicpp-invalid-access-moved
      hicpp-member-init
      hicpp-move-const-arg
      hicpp-multiway-paths-covered
      hicpp-named-parameter
      hicpp-new-delete-operators
      hicpp-no-*
      hicpp-noexcept-move
      hicpp-signed-bitwise
      ${HICPP_SPECIAL_MEMBER_FUNCTIONS}
      hicpp-static-assert
      hicpp-undelegated-constructor
      hicpp-use-*
      ${HICPP_VARARG}
      llvm-*
      misc-*
      modernize-*
      performance-*
      readability-*
    )
    string(REPLACE ";" ","
           CLANG_TIDY_WARNINGS "${CLANG_TIDY_WARNINGS}")
    if(LIBCELLML_TREAT_WARNINGS_AS_ERRORS)
      set(CLANG_TIDY_WARNINGS_AS_ERRORS ";-warnings-as-errors=${CLANG_TIDY_WARNINGS}")
    endif()
    set_target_properties(${TARGET} PROPERTIES
      CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=${CLANG_TIDY_WARNINGS}${CLANG_TIDY_WARNINGS_AS_ERRORS}"
    )
  endif()
endfunction()
