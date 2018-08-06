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
#include "ccm.io.IOutputStream.h"
#include "ccm.io.IPrintStream.h"
#include "ccm.io.charset.ICharset.h"
#include "ccm.util.IFormatter.h"
#include "ccm.util.ILocale.h"
#include <ccmautoptr.h>
#include <ccmrefbase.h>

using ccm::core::IAppendable;
using ccm::core::IAutoCloseable;
using ccm::core::SyncObject;
using ccm::io::ICloseable;
using ccm::io::IFile;
using ccm::io::IFlushable;
using ccm::io::IOutputStream;
using ccm::io::IPrintStream;
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
private:
    static const InterfaceID IID_IFormatString;
    INTERFACE_ID(d6dc3cb6-3ca2-4066-a773-e89e69b00a6d)
    interface IFormatString
        : public IInterface
    {
        inline static IFormatString* Probe(
            /* [in] */ IInterface* object)
        {
            if (object == nullptr) return nullptr;
            return (IFormatString*)object->Probe(IID_IFormatString);
        }

        virtual ECode GetIndex(
            /* [out] */ Integer* idx) = 0;

        virtual ECode Print(
            /* [in] */ IInterface* arg,
            /* [in] */ ILocale* l) = 0;

        virtual ECode ToString(
            /* [out] */ String* str) = 0;
    };

    class FixedString
        : public LightRefBase
        , public IFormatString
    {
    public:
        CCM_INTERFACE_DECL();

        FixedString(
            /* [in] */ Formatter* owner,
            /* [in] */ const String& s)
            : mOwner(owner)
            , mS(s)
        {}

        ECode GetIndex(
            /* [out] */ Integer* idx) override;

        ECode Print(
            /* [in] */ IInterface* arg,
            /* [in] */ ILocale* l) override;

        ECode ToString(
            /* [out] */ String* str) override;

    private:
        Formatter* mOwner;
        String mS;
    };

    class FormatSpecifier
        : public LightRefBase
        , public IFormatString
    {
    public:
        CCM_INTERFACE_DECL();

        ECode Constructor(
            /* [in] */ const String& indexStr,
            /* [in] */ const String& flagsStr,
            /* [in] */ const String& widthStr,
            /* [in] */ const String& precisionStr,
            /* [in] */ const String& tTStr,
            /* [in] */ const String& convStr);

        ECode GetIndex(
            /* [out] */ Integer* idx) override;

        ECode Print(
            /* [in] */ IInterface* arg,
            /* [in] */ ILocale* l) override;

        ECode ToString(
            /* [out] */ String* str) override;
    };

    class FormatSpecifierParser
        : public LightRefBase
    {
    public:
        ECode Constructor(
            /* [in] */ const String& format,
            /* [in] */ Integer startIdx);

        String NextInt();

        Boolean NextIsInt();

        AutoPtr<FormatSpecifier> GetFormatSpecifier();

        Integer GetEndIdx();

    private:
        static String GetFLAGS();

        ECode Peek(
            /* [out] */ Char* c);

        ECode Advance(
            /* [out] */ Char* c = nullptr);

        void Back(
            /* [in] */ Integer len);

    private:
        String mFormat;
        Integer mCursor;
        AutoPtr<FormatSpecifier> mFs;

        String mIndex;
        String mFlags;
        String mWidth;
        String mPrecision;
        String mTT;
        String mConv;
    };

public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ IAppendable* a);

    ECode Constructor(
        /* [in] */ ILocale* l);

    ECode Constructor(
        /* [in] */ IAppendable* a,
        /* [in] */ ILocale* l);

    ECode Constructor(
        /* [in] */  const String& fileName);

    ECode Constructor(
        /* [in] */ const String& fileName,
        /* [in] */ const String& csn);

    ECode Constructor(
        /* [in] */ const String& fileName,
        /* [in] */ const String& csn,
        /* [in] */ ILocale* l);

    ECode Constructor(
        /* [in] */ IFile* file);

    ECode Constructor(
        /* [in] */ IFile* file,
        /* [in] */ const String& csn);

    ECode Constructor(
        /* [in] */ IFile* file,
        /* [in] */ const String& csn,
        /* [in] */ ILocale* l);

    ECode Constructor(
        /* [in] */ IPrintStream* ps);

    ECode Constructor(
        /* [in] */ IOutputStream* os);

    ECode Constructor(
        /* [in] */ IOutputStream* os,
        /* [in] */ const String& csn);

    ECode Constructor(
        /* [in] */ IOutputStream* os,
        /* [in] */ const String& csn,
        /* [in] */ ILocale* l);

    ECode GetLocale(
        /* [out] */ ILocale** locale) override;

    ECode GetOut(
        /* [out] */ IAppendable** output) override;

    ECode ToString(
        /* [out] */ String* str) override;

    ECode Flush() override;

    ECode Close() override;

    ECode GetIoException(
        /* [out] */ ECode* ec) override;

    ECode Format(
        /* [in] */ const String& format,
        /* [in] */ const Array<IInterface*>* args) override;

    ECode Format(
        /* [in] */ ILocale* l,
        /* [in] */ const String& format,
        /* [in] */ const Array<IInterface*>* args) override;

private:
    static ECode ToCharset(
        /* [in] */ const String& csn,
        /* [out] */ ICharset** cs);

    static AutoPtr<IAppendable> NonNullAppendable(
        /* [in] */ IAppendable* a);

    ECode Constructor(
        /* [in] */ ILocale* l,
        /* [in] */ IAppendable* a);

    ECode Constructor(
        /* [in] */ ICharset* charset,
        /* [in] */ ILocale* l,
        /* [in] */ IFile* file);

    static Char GetZero(
        /* [in] */ ILocale* l);

    ECode EnsureOpen();

    ECode Parse(
        /* [in] */ const String& s,
        /* [out, callee] */ Array<IFormatString*>* formats);

private:
    AutoPtr<IAppendable> mA;
    AutoPtr<ILocale> mL;

    ECode mLastException;

    Char mZero;
};

}
}

#endif // __CCM_UTIL_FORMATTER_H__
