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

#include "como/core/CoreUtils.h"
#include "como/core/SyncObject.h"
#include "como/text/AttributedCharacterIteratorAttributeFactory.h"
#include "como.core.CStringBuffer.h"
#include "como.core.ICloneable.h"
#include "como.core.IStringBuffer.h"
#include "como.text.CAttributedString.h"
#include "como.text.IAttributedString.h"
#include "como.text.IAttributedCharacterIterator.h"
#include "como.text.ICharacterIterator.h"
#include "como.util.IMap.h"
#include "como.util.ISet.h"
#include <comosp.h>
#include <gtest/gtest.h>

using namespace como;
using como::core::CoreUtils;
using como::core::CStringBuffer;
using como::core::IID_ICloneable;
using como::core::IID_IStringBuffer;
using como::core::ICloneable;
using como::core::IStringBuffer;
using como::core::SyncObject;
using como::text::AttributedCharacterIteratorAttributeFactory;
using como::text::CAttributedString;
using como::text::IAttributedCharacterIterator;
using como::text::IAttributedString;
using como::text::ICharacterIterator;
using como::text::IID_IAttributedCharacterIterator;
using como::text::IID_IAttributedString;
using como::text::IID_ICharacterIterator;
using como::util::IMap;
using como::util::ISet;

class TestAttributedCharacterIterator
    : public SyncObject
    , public IAttributedCharacterIterator
    , public ICharacterIterator
    , public ICloneable
{
public:
    COMO_INTERFACE_DECL();

    ECode GetAllAttributeKeys(
        /* [out] */ AutoPtr<ISet>& keys) override;

    ECode GetAttribute(
        /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
        /* [out] */ AutoPtr<IInterface>& value) override;

    ECode GetAttributes(
        /* [out] */ AutoPtr<IMap>& attributes) override;

    ECode GetRunLimit(
        /* [out] */ Integer& index) override;

    ECode GetRunLimit(
        /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
        /* [out] */ Integer& index) override;

    ECode GetRunLimit(
        /* [in] */ ISet* attributes,
        /* [out] */ Integer& index) override;

    ECode GetRunStart(
        /* [out] */ Integer& index) override;

    ECode GetRunStart(
        /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
        /* [out] */ Integer& index) override;

    ECode GetRunStart(
        /* [in] */ ISet* attributes,
        /* [out] */ Integer& index) override;

    ECode Clone(
        /* [in] */ const InterfaceID& iid,
        /* [out] */ IInterface** obj) override;

    ECode Current(
        /* [out] */ Char& currChar) override;

    ECode First(
        /* [out] */ Char& firstChar) override;

    ECode GetBeginIndex(
        /* [out] */ Integer& beginIndex) override;

    ECode GetEndIndex(
        /* [out] */ Integer& endIndex) override;

    ECode GetIndex(
        /* [out] */ Integer& currIndex) override;

    ECode Last(
        /* [out] */ Char& lastChar) override;

    ECode Next(
        /* [out] */ Char& nextChar) override;

    ECode Previous(
        /* [out] */ Char& prevChar) override;

    ECode SetIndex(
        /* [in] */ Integer position,
        /* [out] */ Char* currChar = nullptr) override;
};

COMO_INTERFACE_IMPL_3(TestAttributedCharacterIterator, SyncObject, IAttributedCharacterIterator, ICharacterIterator, ICloneable);

ECode TestAttributedCharacterIterator::GetAllAttributeKeys(
    /* [out] */ AutoPtr<ISet>& keys)
{
    keys = nullptr;
    return NOERROR;
}

ECode TestAttributedCharacterIterator::GetAttribute(
    /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
    /* [out] */ AutoPtr<IInterface>& value)
{
    value = nullptr;
    return NOERROR;
}

ECode TestAttributedCharacterIterator::GetAttributes(
    /* [out] */ AutoPtr<IMap>& attributes)
{
    attributes = nullptr;
    return NOERROR;
}

ECode TestAttributedCharacterIterator::GetRunLimit(
    /* [out] */ Integer& index)
{
    index = 0;
    return NOERROR;
}

ECode TestAttributedCharacterIterator::GetRunLimit(
    /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
    /* [out] */ Integer& index)
{
    index = 0;
    return NOERROR;
}

ECode TestAttributedCharacterIterator::GetRunLimit(
    /* [in] */ ISet* attributes,
    /* [out] */ Integer& index)
{
    index = 0;
    return NOERROR;
}

ECode TestAttributedCharacterIterator::GetRunStart(
    /* [out] */ Integer& index)
{
    index = 0;
    return NOERROR;
}

ECode TestAttributedCharacterIterator::GetRunStart(
    /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
    /* [out] */ Integer& index)
{
    index = 0;
    return NOERROR;
}

ECode TestAttributedCharacterIterator::GetRunStart(
    /* [in] */ ISet* attributes,
    /* [out] */ Integer& index)
{
    index = 0;
    return NOERROR;
}

ECode TestAttributedCharacterIterator::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    *obj = nullptr;
    return NOERROR;
}

ECode TestAttributedCharacterIterator::Current(
    /* [out] */ Char& currChar)
{
    currChar = U'a';
    return NOERROR;
}

ECode TestAttributedCharacterIterator::First(
    /* [out] */ Char& firstChar)
{
    firstChar = U'a';
    return NOERROR;
}

ECode TestAttributedCharacterIterator::GetBeginIndex(
    /* [out] */ Integer& beginIndex)
{
    beginIndex = 0;
    return NOERROR;
}

ECode TestAttributedCharacterIterator::GetEndIndex(
    /* [out] */ Integer& endIndex)
{
    endIndex = 0;
    return NOERROR;
}

ECode TestAttributedCharacterIterator::GetIndex(
    /* [out] */ Integer& currIndex)
{
    currIndex = 0;
    return NOERROR;
}

ECode TestAttributedCharacterIterator::Last(
    /* [out] */ Char& lastChar)
{
    lastChar = U'a';
    return NOERROR;
}

ECode TestAttributedCharacterIterator::Next(
    /* [out] */ Char& nextChar)
{
    nextChar = U'a';
    return NOERROR;
}

ECode TestAttributedCharacterIterator::Previous(
    /* [out] */ Char& prevChar)
{
    prevChar = U'a';
    return NOERROR;
}

ECode TestAttributedCharacterIterator::SetIndex(
    /* [in] */ Integer position,
    /* [out] */ Char* currChar)
{
    if (currChar != nullptr) {
        *currChar = U'a';
    }
    return NOERROR;
}

TEST(AttributedStringTest, TestConstructorWithString)
{
    String test("Test string");
    AutoPtr<IAttributedString> as;
    CAttributedString::New(test, IID_IAttributedString, (IInterface**)&as);
    AutoPtr<IAttributedCharacterIterator> it;
    as->GetIterator(it);
    AutoPtr<ICharacterIterator> cit = ICharacterIterator::Probe(it);
    AutoPtr<IStringBuffer> buf;
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&buf);
    Char ch;
    cit->First(ch);
    buf->Append(ch);
    while (cit->Next(ch), ch != ICharacterIterator::DONE) {
        buf->Append(ch);
    }
    String bufStr;
    buf->ToString(bufStr);
    EXPECT_STREQ(test.string(), bufStr.string());
}

TEST(AttributedStringTest, TestConstructorWithIAttributedCharacterIterator)
{
    AutoPtr<IAttributedString> as;
    CAttributedString::New(new TestAttributedCharacterIterator(), IID_IAttributedString, (IInterface**)&as);
    EXPECT_TRUE(as != nullptr);
}

TEST(AttributedStringTest, TestConstructorWithIAttributedCharacterIteratorII)
{
    AutoPtr<IAttributedString> as;
    CAttributedString::New(new TestAttributedCharacterIterator(), 0, 0, IID_IAttributedString, (IInterface**)&as);
    EXPECT_TRUE(as != nullptr);
}

TEST(AttributedStringTest, TestAddAttributeWithIAttributedCharacterIteratorIInterfaceII)
{
    AutoPtr<IAttributedCharacterIterator::IAttribute> language;
    AttributedCharacterIteratorAttributeFactory::GetLANGUAGE(&language);
    AutoPtr<IAttributedString> as;
    CAttributedString::New(String("test"), IID_IAttributedString, (IInterface**)&as);
    as->AddAttribute(language, CoreUtils::Box(String("a")), 2, 3);
    AutoPtr<IAttributedCharacterIterator> it;
    as->GetIterator(it);
    Integer limit;
    it->GetRunLimit(language, limit);
    EXPECT_EQ(limit, 2);

    as = nullptr;
    CAttributedString::New(String("test"), IID_IAttributedString, (IInterface**)&as);
    as->AddAttribute(language, nullptr, 2, 3);
    as->GetIterator(it);
    it->GetRunLimit(language, limit);
    EXPECT_EQ(limit, 4);

    as = nullptr;
    CAttributedString::New(String("test"), IID_IAttributedString, (IInterface**)&as);
    ECode ec = as->AddAttribute(language, nullptr, -1, 3);
    EXPECT_TRUE(ec != NOERROR);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
