# Copyright 2015 libCellML Contributors
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
  set( CMAKE_INSTALL_PREFIX ${LIBCELLML_INSTALL_PREFIX} CACHE INTERNAL "Internalise CMAKE_INSTALL_PREFIX, manipulate via LIBCELLML_INSTALL_PREFIX" FORCE )
  set( CMAKE_BUILD_TYPE ${LIBCELLML_BUILD_TYPE} CACHE INTERNAL "Internalise CMAKE_BUILD_TYPE, manipulate via LIBCELLML_BUILD_TYPE" FORCE )
  set( BUILD_SHARED_LIBS ${LIBCELLML_BUILD_SHARED} CACHE INTERNAL "Internalise BUILD_SHARED_LIBS, manipulate via LIBCELLML_BUILD_SHARED" FORCE )
endfunction()

function(HIDE_DISTRACTING_VARIABLES)
  mark_as_advanced(CMAKE_CONFIGURATION_TYPES)
  if(APPLE)
    mark_as_advanced(CMAKE_OSX_ARCHITECTURES)
    mark_as_advanced(CMAKE_OSX_DEPLOYMENT_TARGET)
    mark_as_advanced(CMAKE_OSX_SYSROOT)
  endif()
endfunction()

function(GROUP_SOURCE_TO_DIR_STRUCTURE)
  if(MSVC)
    foreach(_F ${ARGN})
      string(REGEX MATCH "${CMAKE_CURRENT_SOURCE_DIR}" _M ${_F})
      # Basic test for determining if current file is a source file or header file.
      if(${_F} MATCHES ".+\.cpp$")
        set(_F_PREFIX "Source Files\\")
      else()
        set(_F_PREFIX "Header Files\\")
      endif()
      # Get relative path from current source directory or current binary directory.
      if(${_F} MATCHES "^${CMAKE_CURRENT_SOURCE_DIR}.+")
        file(RELATIVE_PATH _L "${CMAKE_CURRENT_SOURCE_DIR}" ${_F})
      else()
        file(RELATIVE_PATH _L "${CMAKE_CURRENT_BINARY_DIR}" ${_F})
        # Assume all files in the current binary directory are generated header files!
        set(_F_PREFIX "Generated Header Files\\")
      endif()
      get_filename_component(_G ${_L} DIRECTORY)
      if(_G)
        string(REPLACE "/" "\\" _G ${_G})
      endif()
      source_group("${_F_PREFIX}${_G}" FILES "${_F}")
    endforeach()
  endif()
endfunction()
