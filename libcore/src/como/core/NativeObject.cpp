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

#include "como/core/NativeObject.h"
#include "como/core/SyncObject.h"
#include <ccmautoptr.h>

namespace como {
namespace core {

NativeObject::NativeObject(
    /* [in] */ HANDLE comoObject)
    : mCcmObject(comoObject)
{}

NativeLockWord NativeObject::GetLockWord(
    /* [in] */ Boolean asVolatile)
{
    uint32_t val = asVolatile ? mMonitor.LoadSequentiallyConsistent() :
            mMonitor.LoadRelaxed();
    return NativeLockWord(val);
}

void NativeObject::SetLockWord(
    /* [in] */ NativeLockWord newVal,
    /* [in] */ Boolean asVolatile)
{
    // Force use of non-transactional mode and do not check.
    if (asVolatile) {
        mMonitor.StoreSequentiallyConsistent(newVal.GetValue());
    }
    else {
        mMonitor.StoreRelaxed(newVal.GetValue());
    }
}

String NativeObject::PrettyTypeOf(
    /* [in] */ NativeObject* obj)
{
    if (obj == nullptr) {
        return String("null");
    }
    return obj->PrettyTypeOf();
}

String NativeObject::PrettyTypeOf()
{
    SyncObject* obj = reinterpret_cast<SyncObject*>(mCcmObject);
    AutoPtr<IMetaCoclass> mc;
    obj->GetCoclass(&mc);
    String ns, name;
    mc->GetNamespace(&ns);
    mc->GetName(&name);
    return ns + name;
}

}
}
