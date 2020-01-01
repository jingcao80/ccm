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

#ifndef __COMO_SECURITY_CCA_INSTANCEFACTORY_H__
#define __COMO_SECURITY_CCA_INSTANCEFACTORY_H__

#include "como.security.IProvider.h"
#include "como.security.cca.IInstance.h"

namespace como {
namespace security {
namespace cca {

class InstanceFactory
{
public:
    static ECode GetInstance(
        /* [in] */ const String& type,
        /* [in] */ const InterfaceID& iid,
        /* [in] */ const String& algorithm,
        /* [out] */ IInstance** instance)
    {
        return NOERROR;
    }

    static ECode GetInstance(
        /* [in] */ const String& type,
        /* [in] */ const InterfaceID& iid,
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IInstance** instance)
    {
        return NOERROR;
    }

    static ECode GetInstance(
        /* [in] */ const String& type,
        /* [in] */ const InterfaceID& iid,
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ IInstance** instance)
    {
        return NOERROR;
    }
};

}
}
}

#endif // __COMO_SECURITY_CCA_INSTANCEFACTORY_H__
