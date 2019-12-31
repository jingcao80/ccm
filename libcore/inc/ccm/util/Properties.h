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

#ifndef __CCM_UTIL_PROPERTIES_H__
#define __CCM_UTIL_PROPERTIES_H__

#include "ccm/util/Hashtable.h"
#include "ccm.io.IBufferedWriter.h"
#include "ccm.io.IInputStream.h"
#include "ccm.io.IOutputStream.h"
#include "ccm.io.IPrintStream.h"
#include "ccm.io.IPrintWriter.h"
#include "ccm.io.IReader.h"
#include "ccm.io.IWriter.h"
#include "ccm.util.IProperties.h"
#include <ccmrefbase.h>

using ccm::io::IBufferedWriter;
using ccm::io::IInputStream;
using ccm::io::IOutputStream;
using ccm::io::IPrintStream;
using ccm::io::IPrintWriter;
using ccm::io::IReader;
using ccm::io::IWriter;

namespace ccm {
namespace util {

class Properties
    : public Hashtable
    , public IProperties
{
private:
    class LineReader
        : public LightRefBase
    {
    public:
        LineReader(
            /* [in] */ IInputStream* inStream);

        LineReader(
            /* [in] */ IReader* reader);

        ECode ReadLine(
            /* [in] */ Integer* number);

    public:
        Array<Byte> mInByteBuf;
        Array<Char> mInCharBuf;
        Array<Char> mLineBuf = Array<Char>(1024);
        Integer mInLimit = 0;
        Integer mInOff = 0;
        AutoPtr<IInputStream> mInStream;
        AutoPtr<IReader> mReader;
    };

public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ IProperties* defaults);

    ECode SetProperty(
        /* [in] */ const String& key,
        /* [in] */ const String& value,
        /* [out] */ String* prevValue = nullptr) override;

    ECode Load(
        /* [in] */ IReader* reader) override;

    ECode Load(
        /* [in] */ IInputStream* instream) override;

    ECode Save(
        /* [in] */ IOutputStream* outstream,
        /* [in] */ const String& comments) override;

    ECode Store(
        /* [in] */ IWriter* writer,
        /* [in] */ const String& comments) override;

    ECode Store(
        /* [in] */ IOutputStream* outstream,
        /* [in] */ const String& comments) override;

    ECode LoadFromXML(
        /* [in] */ IInputStream* instream) override;

    ECode StoreToXML(
        /* [in] */ IOutputStream* os,
        /* [in] */ const String& comment) override;

    ECode StoreToXML(
        /* [in] */ IOutputStream* os,
        /* [in] */ const String& comment,
        /* [in] */ const String& encoding) override;

    ECode GetProperty(
        /* [in] */ const String& key,
        /* [out] */ String* value) override;

    ECode GetProperty(
        /* [in] */ const String& key,
        /* [in] */ const String& defaultValue,
        /* [out] */ String* value) override;

    ECode PropertyNames(
        /* [out] */ IEnumeration** names) override;

    ECode StringPropertyNames(
        /* [out] */ ISet** names) override;

    ECode List(
        /* [in] */ IPrintStream* outstream) override;

    ECode List(
        /* [in] */ IPrintWriter* outwriter) override;

private:
    ECode Load0 (
        /* [in] */ AutoPtr<LineReader> lr);

    ECode LoadConvert(
        /* [in] */ const Array<Char>& in,
        /* [in] */ Integer off,
        /* [in] */ Integer len,
        /* [in] */ Array<Char>& convtBuf,
        /* [out] */ String* str);

    String SaveConvert(
        /* [in] */ const String& theString,
        /* [in] */ Boolean escapeSpace,
        /* [in] */ Boolean escapeUnicode);

    static ECode WriteComments(
        /* [in] */ IBufferedWriter* bw,
        /* [in] */ const String& comments);

    ECode Store0(
        /* [in] */ IBufferedWriter* bw,
        /* [in] */ const String& comments,
        /* [in] */ Boolean escUnicode);

    void Enumerate(
        /* [in] */ IHashtable* h);

    void EnumerateStringProperties(
        /* [in] */ IHashtable* h);

    static Char ToHex(
        /* [in] */ Integer nibble);

protected:
    AutoPtr<IProperties> mDefaults;
};

}
}

#endif // __CCM_UTIL_PROPERTIES_H__
