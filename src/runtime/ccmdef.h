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

#ifndef __CCM_CCMDEF_H__
#define __CCM_CCMDEF_H__

#include <stdint.h>

namespace ccm {

#define COCLASS_ID(x)
#define INTERFACE_ID(x)

#define interface       struct

#define Coclass(name)      class name : public _##name

#define COM_PUBLIC      __attribute__ ((visibility ("default")))
#define COM_LOCAL       __attribute__ ((visibility ("hidden")))

#define REFCOUNT_ADD(i)     if (i) { (i)->AddRef(); }
#define REFCOUNT_RELEASE(i) if (i) { (i)->Release(); }

#define VALIDATE_NOT_NULL(i) \
        if (i == nullptr) { return E_ILLEGAL_ARGUMENT_EXCEPTION; }

#ifndef MAX
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#endif

#ifndef CCM_INTERFACE_DECL
#define CCM_INTERFACE_DECL()                            \
    IInterface* Probe(                                  \
        /* [in] */ const InterfaceID& iid) override;    \
                                                        \
    Integer AddRef(                                     \
        /* [in] */ HANDLE id = 0) override;             \
                                                        \
    Integer Release(                                    \
        /* [in] */ HANDLE id = 0) override;             \
                                                        \
    ECode GetInterfaceID(                               \
        /* [in] */ IInterface* object,                  \
        /* [out] */ InterfaceID* iid) override;
#endif

} // namespace ccm

#endif // __CCM_CCMDEF_H__
