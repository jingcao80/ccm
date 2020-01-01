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

add_custom_target(deploy_libcore_util_headers)
add_dependencies(deploy_libcore_util_headers comort)

MKDIR(deploy_libcore_util_headers ${INC_DIR}/como/util)
MKDIR(deploy_libcore_util_headers ${INC_DIR}/como/util/calendar)
MKDIR(deploy_libcore_util_headers ${INC_DIR}/como/util/regex)

COPY(deploy_libcore_util_headers ${LIBCORE_COMO_INC_DIR}/util/Arrays.h ${INC_DIR}/como/util)
COPY(deploy_libcore_util_headers ${LIBCORE_COMO_INC_DIR}/util/CalendarFactory.h ${INC_DIR}/como/util)
COPY(deploy_libcore_util_headers ${LIBCORE_COMO_INC_DIR}/util/EmptyArray.h ${INC_DIR}/como/util)
COPY(deploy_libcore_util_headers ${LIBCORE_COMO_INC_DIR}/util/LocaleFactory.h ${INC_DIR}/como/util)
COPY(deploy_libcore_util_headers ${LIBCORE_COMO_INC_DIR}/util/TimeZoneFactory.h ${INC_DIR}/como/util)
COPY(deploy_libcore_util_headers ${LIBCORE_COMO_INC_DIR}/util/calendar/CalendarSystemFactory.h ${INC_DIR}/como/util/calendar)
COPY(deploy_libcore_util_headers ${LIBCORE_COMO_INC_DIR}/util/calendar/CalendarUtils.h ${INC_DIR}/como/util/calendar)
COPY(deploy_libcore_util_headers ${LIBCORE_COMO_INC_DIR}/util/regex/PatternFactory.h ${INC_DIR}/como/util/regex)
