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

set(MATH_DIR ${LIBCORE_CCM_SRC_DIR}/math)

include_directories(
    ./
    ../
    ../../../inc
    ${EXTERNAL_DIR}/boringssl/inc
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    BigInt.cpp
    BigInteger.cpp
    BitLevel.cpp
    Conversion.cpp
    Division.cpp
    Logical.cpp
    Primality.cpp
    math.cpp)

add_library(ccm_math STATIC
    ${SOURCES})
add_dependencies(ccm_math build_libcore)