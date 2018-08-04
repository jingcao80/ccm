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

set(LOCALE_DIR ${UTIL_DIR}/locale)

include_directories(
    ./
    ../
    ../../../../inc
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    BaseLocale.cpp
    LocaleObjectCache.cpp
    LocaleUtils.cpp)

add_library(ccm_util_locale STATIC
    ${SOURCES})
add_dependencies(ccm_util_locale build_libcore)
