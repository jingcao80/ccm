//=========================================================================
// Copyright (C) 2018 The C++ Component Model(CCM) Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "ccm/core/CArrayHolder.h"
#include "ccm/core/CRuntimePermission.h"
#include "ccm/core/CStackTrace.h"
#include "ccm/core/CStackTraceElement.h"
#include "ccm/core/CThread.h"
#include "ccm/core/CThreadGroup.h"
#include "ccm/core/CThreadLocal.h"

namespace ccm {
namespace core {

CCM_OBJECT_IMPL(CArrayHolder);
CCM_OBJECT_IMPL(CRuntimePermission);
CCM_OBJECT_IMPL(CStackTrace);
CCM_OBJECT_IMPL(CStackTraceElement);
CCM_OBJECT_IMPL(CThread);
CCM_OBJECT_IMPL(CThreadGroup);
CCM_OBJECT_IMPL(CThreadLocal);

}
}
