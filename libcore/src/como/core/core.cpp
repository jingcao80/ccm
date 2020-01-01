//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

#include "como/core/CArrayHolder.h"
#include "como/core/CRuntimePermission.h"
#include "como/core/CStackTrace.h"
#include "como/core/CStackTraceElement.h"
#include "como/core/CThread.h"
#include "como/core/CThreadGroup.h"
#include "como/core/CThreadLocal.h"

namespace como {
namespace core {

COMO_OBJECT_IMPL(CArrayHolder);
COMO_OBJECT_IMPL(CRuntimePermission);
COMO_OBJECT_IMPL(CStackTrace);
COMO_OBJECT_IMPL(CStackTraceElement);
COMO_OBJECT_IMPL(CThread);
COMO_OBJECT_IMPL(CThreadGroup);
COMO_OBJECT_IMPL(CThreadLocal);

}
}
