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

#ifndef __CCM_UTIL_FORMATTER_H__
#define __CCM_UTIL_FORMATTER_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.IAppendable.h"
#include "ccm.core.IAutoCloseable.h"
#include "ccm.io.ICloseable.h"
#include "ccm.io.IFile.h"
#include "ccm.io.IFlushable.h"
#include "ccm.io.charset.ICharset.h"
#include "ccm.util.IFormatter.h"
#include "ccm.util.ILocale.h"
#include <ccmautoptr.h>

using ccm::core::IAppendable;
using ccm::core::IAutoCloseable;
using ccm::core::SyncObject;
using ccm::io::ICloseable;
using ccm::io::IFile;
using ccm::io::IFlushable;
using ccm::io::charset::ICharset;

namespace ccm {
namespace util {

class Formatter
    : public SyncObject
    , public IFormatter
    , public ICloseable
    , public IFlushable
    , public IAutoCloseable
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

private:
    static ECode ToCharset(
        /* [in] */ const String& csn,
        /* [out] */ ICharset** cs);

    static AutoPtr<IAppendable> NonNullAppendable(
        /* [in] */ IAppendable* a);

    ECode Constructor(
        /* [in] */ ILocale* l,
        /* [in] */ IAppendable* a);

    static Char GetZero(
        /* [in] */ ILocale* l);

    ECode Constructor(
        /* [in] */ ICharset* charset,
        /* [in] */ ILocale* l,
        /* [in] */ IFile* file);

private:
    AutoPtr<IAppendable> mA;
    AutoPtr<ILocale> mL;

    Char mZero;
};

}
}

#endif // __CCM_UTIL_FORMATTER_H__
