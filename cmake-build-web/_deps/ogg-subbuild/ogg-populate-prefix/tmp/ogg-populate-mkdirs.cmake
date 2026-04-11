# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/mihailo-sb/CLionProjects/Troll/cmake-build-web/_deps/ogg-src"
  "/home/mihailo-sb/CLionProjects/Troll/cmake-build-web/_deps/ogg-build"
  "/home/mihailo-sb/CLionProjects/Troll/cmake-build-web/_deps/ogg-subbuild/ogg-populate-prefix"
  "/home/mihailo-sb/CLionProjects/Troll/cmake-build-web/_deps/ogg-subbuild/ogg-populate-prefix/tmp"
  "/home/mihailo-sb/CLionProjects/Troll/cmake-build-web/_deps/ogg-subbuild/ogg-populate-prefix/src/ogg-populate-stamp"
  "/home/mihailo-sb/CLionProjects/Troll/cmake-build-web/_deps/ogg-subbuild/ogg-populate-prefix/src"
  "/home/mihailo-sb/CLionProjects/Troll/cmake-build-web/_deps/ogg-subbuild/ogg-populate-prefix/src/ogg-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/mihailo-sb/CLionProjects/Troll/cmake-build-web/_deps/ogg-subbuild/ogg-populate-prefix/src/ogg-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/mihailo-sb/CLionProjects/Troll/cmake-build-web/_deps/ogg-subbuild/ogg-populate-prefix/src/ogg-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
