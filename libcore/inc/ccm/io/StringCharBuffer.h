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

#ifndef __CCM_IO_STRINGCHARBUFFER_H__
#define __CCM_IO_STRINGCHARBUFFER_H__

#include "ccm/io/CharBuffer.h"

namespace ccm {
namespace io {

class StringCharBuffer
    : public CharBuffer
{
public:
    ECode Constructor(
        /* [in] */ ICharSequence* s,
        /* [in] */ Integer start,
        /* [in] */ Integer end);

    ECode Slice(
        /* [out] */ ICharBuffer** buffer) override;

    ECode Duplicate(
        /* [out] */ ICharBuffer** buffer) override;

    ECode AsReadOnlyBuffer(
        /* [out] */ ICharBuffer** buffer) override;

    ECode Get(
        /* [out] */ Char* c) override final;

    ECode Get(
        /* [in] */ Integer index,
        /* [out] */ Char* c) override final;

    ECode GetUnchecked(
        /* [in] */ Integer index,
        /* [out] */ Char* c) override;

    ECode Put(
        /* [in] */ Char c) override final;

    ECode Put(
        /* [in] */ Integer index,
        /* [in] */ Char c) override final;

    ECode Compact() override final;

    ECode IsReadOnly(
        /* [out] */ Boolean* readOnly) override final;

    ECode ToString(
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [out] */ String* desc) override final;

    ECode SubSequence(
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [out] */ ICharSequence** subcsq) override final;

    ECode IsDirect(
        /* [out] */ Boolean* direct) override;

    ECode GetOrder(
        /* [out] */ IByteOrder** bo) override;

private:
    ECode Constructor(
        /* [in] */ ICharSequence* s,
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer limit,
        /* [in] */ Integer cap,
        /* [in] */ Integer offset);

protected:
    AutoPtr<ICharSequence> mStr;
};

}
}

#endif // __CCM_IO_STRINGCHARBUFFER_H__
