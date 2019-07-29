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

#ifndef __CCM_TEST_SUPPORT_COLLECTIONTEST_H__
#define __CCM_TEST_SUPPORT_COLLECTIONTEST_H__

#include "ccm.util.ICollection.h"
#include <ccmobject.h>

using ccm::util::ICollection;

namespace ccm {
namespace test {

class Support_CollectionTest
    : public Object
{
public:
    Support_CollectionTest()
    {}

    Support_CollectionTest(
        /* [in] */ ICollection* c)
        : mCol(c)
    {}

    ECode RunTest();

public:
    AutoPtr<ICollection> mCol;
};

}
}

#endif // __CCM_TEST_SUPPORT_COLLECTIONTEST_H__
