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

add_custom_target(deploy_libcore_core_headers)
add_dependencies(deploy_libcore_core_headers ccmrt)

if (NOT EXISTS "${INC_DIR}/ccm/core")
MKDIR(deploy_libcore_core_headers ${INC_DIR}/ccm/core)

COPY(deploy_libcore_core_headers ${LIBCORE_CCM_INC_DIR}/core/AutoLock.h ${INC_DIR}/ccm/core)
COPY(deploy_libcore_core_headers ${LIBCORE_CCM_INC_DIR}/core/Character.h ${INC_DIR}/ccm/core)
COPY(deploy_libcore_core_headers ${LIBCORE_CCM_INC_DIR}/core/ClassLoader.h ${INC_DIR}/ccm/core)
COPY(deploy_libcore_core_headers ${LIBCORE_CCM_INC_DIR}/core/CoreUtils.h ${INC_DIR}/ccm/core)
COPY(deploy_libcore_core_headers ${LIBCORE_CCM_INC_DIR}/core/Math.h ${INC_DIR}/ccm/core)
COPY(deploy_libcore_core_headers ${LIBCORE_CCM_INC_DIR}/core/NativeAtomic.h ${INC_DIR}/ccm/core)
COPY(deploy_libcore_core_headers ${LIBCORE_CCM_INC_DIR}/core/Runnable.h ${INC_DIR}/ccm/core)
COPY(deploy_libcore_core_headers ${LIBCORE_CCM_INC_DIR}/core/StringUtils.h ${INC_DIR}/ccm/core)
COPY(deploy_libcore_core_headers ${LIBCORE_CCM_INC_DIR}/core/SyncObject.h ${INC_DIR}/ccm/core)
COPY(deploy_libcore_core_headers ${LIBCORE_CCM_INC_DIR}/core/volatile.h ${INC_DIR}/ccm/core)
COPY(deploy_libcore_core_headers ${LIBCORE_CCM_INC_DIR}/core/Thread.h ${INC_DIR}/ccm/core)
COPY(deploy_libcore_core_headers ${LIBCORE_CCM_INC_DIR}/core/ThreadLocal.h ${INC_DIR}/ccm/core)
endif()