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

#ifndef __COMO_CORE_SYNCOBJECT_H__
#define __COMO_CORE_SYNCOBJECT_H__

#include "como.core.ISynchronize.h"
#include <ccmobject.h>

namespace como {
namespace core {

class COM_PUBLIC SyncObject
    : public Object
    , public ISynchronize
{
public:
    COMO_INTERFACE_DECL();

    SyncObject();

    virtual ~SyncObject();

    ECode Lock() override;

    ECode Unlock() override;

    ECode Notify() override;

    ECode NotifyAll() override;

    ECode Wait() override;

    ECode Wait(
        /* [in] */ Long millis) override;

    ECode Wait(
        /* [in] */ Long millis,
        /* [in] */ Integer nanos) override;

    static SyncObject* From(
        /* [in] */ IInterface* obj);

    using Object::Equals;

protected:
    friend class Thread;

    HANDLE mNativeObject = 0;
};

inline SyncObject* SyncObject::From(
    /* [in] */ IInterface* obj)
{
    return (SyncObject*)ISynchronize::Probe(obj);
}

} // namespace core
} // namespace como

#endif // __COMO_CORE_SYNCOBJECT_H__
