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

//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
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

namespace ccm {

extern const InterfaceID IID_IInterface;
extern const InterfaceID IID_IClassObject;

INTERFACE_ID(00000000-0000-0000-0000-000000000000)
interface IInterface
{
    inline static IInterface* Probe(
        /* [in] */ IInterface* object)
    {
        return object->Probe(IID_IInterface);
    }

    virtual IInterface* Probe(
        /* [in] */ const InterfaceID& iid) = 0;

    virtual Integer AddRef(
        /* [in] */ HANDLE id = 0) = 0;

    virtual Integer Release(
        /* [in] */ HANDLE id = 0) = 0;

    virtual ECode GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid) = 0;
};

INTERFACE_ID(00000000-0000-0000-0000-000000000001)
interface IClassObject : public IInterface
{
    inline static IClassObject* Probe(
        /* [in] */ IInterface* object)
    {
        return (IClassObject*)object->Probe(IID_IClassObject);
    }

    virtual ECode CreateObject(
        /* [in] */ const InterfaceID& iid,
        /* [out] */ IInterface** object) = 0;
};

}
