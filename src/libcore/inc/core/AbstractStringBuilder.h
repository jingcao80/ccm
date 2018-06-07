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

#ifndef __CCM_CORE_ABSTRACTSTRINGBUILDER_H__
#define __CCM_CORE_ABSTRACTSTRINGBUILDER_H__

#include "ccm.core.IAppendable.h"
#include "ccm.core.ICharSequence.h"
#include "core/SyncObject.h"

namespace ccm {
namespace core {

class AbstractStringBuilder
    : public SyncObject
    , public IAppendable
    , public ICharSequence
{
public:
    CCM_INTERFACE_DECL();

    ECode constructor(
        /* [in] */ Integer capacity);

    ECode GetLength(
        /* [out] */ Integer* number);

    ECode GetCharAt(
        /* [in] */ Integer index,
        /* [out] */ Char* c);

    virtual ECode Append(
        /* [in] */ const String& str);

    virtual ECode Append(
        /* [in] */ IStringBuffer* sb);

    virtual ECode Append(
        /* [in] */ ICharSequence* s);

    virtual ECode Append(
        /* [in] */ ICharSequence* s,
        /* [in] */ Integer start,
        /* [in] */ Integer end);

    virtual ECode Append(
        /* [in] */ const Array<Char>& str);

    virtual ECode Append(
        /* [in] */ const Array<Char>& str,
        /* [in] */ Integer offset,
        /* [in] */ Integer len);

    virtual ECode Append(
        /* [in] */ Boolean b);

    virtual ECode Append(
        /* [in] */ Char c);

    virtual ECode Append(
        /* [in] */ Integer i);

    virtual ECode Append(
        /* [in] */ Long l);

    virtual ECode Append(
        /* [in] */ Float f);

    virtual ECode Append(
        /* [in] */ Double d);

    virtual ECode Delete(
        /* [in] */ Integer start,
        /* [in] */ Integer end);

    virtual ECode DeleteCharAt(
        /* [in] */ Integer index);

    virtual ECode Replace(
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [in] */ const String& str);

    virtual ECode Insert(
        /* [in] */ Integer index,
        /* [in] */ const Array<Char>& str,
        /* [in] */ Integer offset,
        /* [in] */ Integer len);

    virtual ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ IInterface* obj);

    virtual ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ const String& str);

    virtual ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ const Array<Char>& str);

    virtual ECode Insert(
        /* [in] */ Integer dstOffset,
        /* [in] */ ICharSequence* s);

    virtual ECode Insert(
        /* [in] */ Integer dstOffset,
        /* [in] */ ICharSequence* s,
        /* [in] */ Integer start,
        /* [in] */ Integer end);

    virtual ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ Boolean b);

    virtual ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ Char c);

    virtual ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ Integer i);

    virtual ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ Long l);

    virtual ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ Float f);

    virtual ECode Insert(
        /* [in] */ Integer offset,
        /* [in] */ Double d);

    virtual ECode IndexOf(
        /* [in] */ const String& str,
        /* [out] */ Integer* idx);

    virtual ECode IndexOf(
        /* [in] */ const String& str,
        /* [in] */ Integer fromIndex,
        /* [out] */ Integer* idx);

    virtual ECode LastIndexOf(
        /* [in] */ const String& str,
        /* [out] */ Integer* idx);

    virtual ECode LastIndexOf(
        /* [in] */ const String& str,
        /* [in] */ Integer fromIndex,
        /* [out] */ Integer* idx);

    virtual ECode Reverse();

    ECode SubSequence(
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [out] */ ICharSequence** subcsq);

    ECode ToString(
        /* [out] */ String* str);
};

}
}

#endif // __CCM_CORE_ABSTRACTSTRINGBUILDER_H__