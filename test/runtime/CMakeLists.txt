#=========================================================================
# Copyright (C) 2018 The C++ Component Model(CCM) Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#=========================================================================

set(UNIT_TEST_SRC_DIR ${TEST_DIR}/runtime)
set(UNIT_TEST_OBJ_DIR ${CMAKE_BINARY_DIR}/test/runtime)

add_subdirectory(array)
add_subdirectory(autoptr)
add_subdirectory(char)
add_subdirectory(localstaticvariable)
add_subdirectory(macro)
# testMutex tests mutex in ccmrt, so running testMutex needs export
# mutex.h and related methods from ccmrt.so
#add_subdirectory(mutex)
add_subdirectory(nestedinterface)
add_subdirectory(outreferencetype)
add_subdirectory(reflection)
add_subdirectory(rpc)
