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

#ifndef __COMO_TEST_SUPPORT_UNMODIFIABLECOLLECTIONTEST_H__
#define __COMO_TEST_SUPPORT_UNMODIFIABLECOLLECTIONTEST_H__

#include "como.util.ICollection.h"
#include <comoobj.h>

using como::util::ICollection;

namespace como {
namespace test {

class Support_UnmodifiableCollectionTest
    : public Object
{
public:
    Support_UnmodifiableCollectionTest()
    {}

    Support_UnmodifiableCollectionTest(
        /* [in] */ ICollection* c)
        : mCol(c)
    {}

    ECode RunTest();

public:
    AutoPtr<ICollection> mCol;
};

}
}

#endif // __COMO_TEST_SUPPORT_UNMODIFIABLECOLLECTIONTEST_H__
