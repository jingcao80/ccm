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

add_custom_target(deploy_libcore_math_headers)
add_dependencies(deploy_libcore_math_headers ccmrt)

MKDIR(deploy_libcore_math_headers ${INC_DIR}/ccm/math)

COPY(deploy_libcore_math_headers ${LIBCORE_CCM_INC_DIR}/math/BigDecimalFactory.h ${INC_DIR}/ccm/math)
COPY(deploy_libcore_math_headers ${LIBCORE_CCM_INC_DIR}/math/BigIntegerFactory.h ${INC_DIR}/ccm/math)
