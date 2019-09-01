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

#include "CFooBar.h"
#include "ccm.test.IFoo.h"

using ccm::test::CFooBar;
using ccm::test::IFoo;
using ccm::test::IID_IFoo;

int main(int argc, char** argv)
{
    AutoPtr<ccm::IInterface> foo;
    CFooBar::New(1, IID_IFoo, foo);

    return 0;
}