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

#ifndef __COMO_IO_FLOATBUFFER_H__
#define __COMO_IO_FLOATBUFFER_H__

#include "como/io/Buffer.h"
#include "como.core.IComparable.h"
#include "como.io.IFloatBuffer.h"

using como::core::IComparable;

namespace como {
namespace io {

class FloatBuffer
    : public Buffer
    , public IFloatBuffer
    , public IComparable
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap,
        /* [in] */ const Array<Float>& hb,
        /* [in] */ Integer offset);

    ECode Constructor(
        /* [in] */ Integer mark,
        /* [in] */ Integer pos,
        /* [in] */ Integer lim,
        /* [in] */ Integer cap);

    static ECode Allocate(
        /* [in] */ Integer capacity,
        /* [out] */ IFloatBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Float>& array,
        /* [in] */ Integer offset,
        /* [in] */ Integer length,
        /* [out] */ IFloatBuffer** buffer);

    static ECode Wrap(
        /* [in] */ const Array<Float>& array,
        /* [out] */ IFloatBuffer** buffer);

    ECode Get(
        /* [out] */ Array<Float>& dst,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Get(
        /* [out] */ Array<Float>& dst) override;

    using IFloatBuffer::Get;

    ECode Put(
        /* [in] */ IFloatBuffer* src) override;

    ECode Put(
        /* [in] */ const Array<Float>& src,
        /* [in] */ Integer offset,
        /* [in] */ Integer length) override;

    ECode Put(
        /* [in] */ const Array<Float>& src) override final;

    ECode HasArray(
        /* [out] */ Boolean* result) override final;

    ECode GetArray(
        /* [out] */ IInterface** array) override final;

    ECode GetArrayOffset(
        /* [out] */ Integer* offset) override final;

    ECode ToString(
        /* [out] */ String& desc) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& same) override;

    ECode CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Integer& result) override;

private:
    static Boolean Equals(
        /* [in] */ Float x,
        /* [in] */ Float y);

    static Integer Compare(
        /* [in] */ Float x,
        /* [in] */ Float y);

protected:
    Array<Float> mHb;
    Integer mOffset = 0;
    Boolean mIsReadOnly = false;
};

}
}

#endif // __COMO_IO_FLOATBUFFER_H__
