#=========================================================================
# Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

set(UNIT_TEST_SRC_DIR ${TEST_DIR}/libcore/como/util)
set(UNIT_TEST_OBJ_DIR ${CMAKE_BINARY_DIR}/test/libcore/como/util)

add_subdirectory(arraylist)
add_subdirectory(calendar)
add_subdirectory(date)
add_subdirectory(formatter)
add_subdirectory(hashmap)
add_subdirectory(hashtable)
add_subdirectory(locale)
add_subdirectory(random)
add_subdirectory(stringtokenizer)
add_subdirectory(treemap)
add_subdirectory(vector)

add_subdirectory(concurrent)
