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

#include "como/core/SyncObject.h"
#include "como/core/nativeapi.h"

namespace como {
namespace core {

COMO_INTERFACE_IMPL_1(SyncObject, Object, ISynchronize);

SyncObject::SyncObject()
{
    mNativeObject = CreateNativeObject(reinterpret_cast<HANDLE>(this));
}

SyncObject::~SyncObject()
{
    DestroyNativeObject(mNativeObject);
}

ECode SyncObject::Lock()
{
    return NativeObjectLock(mNativeObject);
}

ECode SyncObject::Unlock()
{
    return NativeObjectUnlock(mNativeObject);
}

ECode SyncObject::Notify()
{
    return NativeObjectNotify(mNativeObject);
}

ECode SyncObject::NotifyAll()
{
    return NativeObjectNotifyAll(mNativeObject);
}

ECode SyncObject::Wait()
{
    return NativeObjectWait(mNativeObject);
}

ECode SyncObject::Wait(
    /* [in] */ Long millis)
{
    return Wait(millis, 0);
}

ECode SyncObject::Wait(
    /* [in] */ Long millis,
    /* [in] */ Integer nanos)
{
    return NativeObjectWait(mNativeObject, millis, nanos);
}

}
}
