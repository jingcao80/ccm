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

#ifndef __COMO_IO_BYTEARRAYOUTPUTSTREAM_H__
#define __COMO_IO_BYTEARRAYOUTPUTSTREAM_H__

#include "como/io/OutputStream.h"
#include "como.core.IInteger.h"
#include "como.io.IByteArrayOutputStream.h"

using como::core::IInteger;

namespace como {
namespace io {

class ByteArrayOutputStream
    : public OutputStream
    , public IByteArrayOutputStream
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ Integer size);

    ECode Write(
        /* [in] */ Integer byte) override;

    ECode Write(
        /* [in] */ const Array<Byte>& buffer,
        /* [in] */ Integer offset,
        /* [in] */ Integer size) override;

    ECode WriteTo(
        /* [in] */ IOutputStream* outs) override;

    ECode Reset() override;

    ECode ToByteArray(
        /* [out, callee] */ Array<Byte>* array) override;

    ECode GetSize(
        /* [out] */ Integer* size) override;

    ECode ToString(
        /* [out] */ String* desc) override;

    ECode ToString(
        /* [in] */ const String& charsetName,
        /* [out] */ String* desc) override;

    ECode Close() override;

private:
    void EnsureCapacity(
        /* [in] */ Integer minCapacity);

    void Grow(
        /* [in] */ Integer minCapacity);

    static Integer HugeCapacity(
        /* [in] */ Integer minCapacity);

protected:
    Array<Byte> mBuf;
    Integer mCount;

    static constexpr Integer MAX_ARRAY_SIZE = IInteger::MAX_VALUE - 8;
};

}
}

#endif // __COMO_IO_BYTEARRAYOUTPUTSTREAM_H__
