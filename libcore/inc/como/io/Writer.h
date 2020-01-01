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

#ifndef __COMO_IO_WRITER_H__
#define __COMO_IO_WRITER_H__

#include "como/core/SyncObject.h"
#include "como.core.IAppendable.h"
#include "como.core.IAutoCloseable.h"
#include "como.core.ICharSequence.h"
#include "como.core.ISynchronize.h"
#include "como.io.ICloseable.h"
#include "como.io.IFlushable.h"
#include "como.io.IWriter.h"

using como::core::IAppendable;
using como::core::IAutoCloseable;
using como::core::ICharSequence;
using como::core::ISynchronize;
using como::core::SyncObject;

namespace como {
namespace io {

class Writer
    : public SyncObject
    , public IWriter
    , public IAppendable
    , public ICloseable
    , public IFlushable
    , public IAutoCloseable
{
protected:
    virtual ~Writer();

    COMO_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ ISynchronize* lock);

    ECode Write(
        /* [in] */ Integer c) override;

    ECode Write(
        /* [in] */ const Array<Char>& buffer) override;

    ECode Write(
        /* [in] */ const String& str) override;

    ECode Write(
        /* [in] */ const String& str,
        /* [in] */ Integer off,
        /* [in] */ Integer len) override;

    ECode Append(
        /* [in] */ ICharSequence* csq) override;

    ECode Append(
        /* [in] */ ICharSequence* csq,
        /* [in] */ Integer start,
        /* [in] */ Integer end) override;

    ECode Append(
        /* [in] */ Char c) override;

    using IWriter::Write;

protected:
    ISynchronize* mLock;

private:
    Array<Char> mWriteBuffer;

    static constexpr Integer WRITE_BUFFER_SIZE = 1024;
};

}
}

#endif // __COMO_IO_WRITER_H__
