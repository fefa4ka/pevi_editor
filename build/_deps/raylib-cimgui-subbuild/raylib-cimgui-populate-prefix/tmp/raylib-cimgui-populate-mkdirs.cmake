# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/fefa4ka/Documents/my/pevi_editor/build/_deps/raylib-cimgui-src")
  file(MAKE_DIRECTORY "/Users/fefa4ka/Documents/my/pevi_editor/build/_deps/raylib-cimgui-src")
endif()
file(MAKE_DIRECTORY
  "/Users/fefa4ka/Documents/my/pevi_editor/build/_deps/raylib-cimgui-build"
  "/Users/fefa4ka/Documents/my/pevi_editor/build/_deps/raylib-cimgui-subbuild/raylib-cimgui-populate-prefix"
  "/Users/fefa4ka/Documents/my/pevi_editor/build/_deps/raylib-cimgui-subbuild/raylib-cimgui-populate-prefix/tmp"
  "/Users/fefa4ka/Documents/my/pevi_editor/build/_deps/raylib-cimgui-subbuild/raylib-cimgui-populate-prefix/src/raylib-cimgui-populate-stamp"
  "/Users/fefa4ka/Documents/my/pevi_editor/build/_deps/raylib-cimgui-subbuild/raylib-cimgui-populate-prefix/src"
  "/Users/fefa4ka/Documents/my/pevi_editor/build/_deps/raylib-cimgui-subbuild/raylib-cimgui-populate-prefix/src/raylib-cimgui-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/fefa4ka/Documents/my/pevi_editor/build/_deps/raylib-cimgui-subbuild/raylib-cimgui-populate-prefix/src/raylib-cimgui-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/fefa4ka/Documents/my/pevi_editor/build/_deps/raylib-cimgui-subbuild/raylib-cimgui-populate-prefix/src/raylib-cimgui-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
