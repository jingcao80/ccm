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

#ifndef __CDLC_NODE_H__
#define __CDLC_NODE_H__

#include "util/LightRefBase.h"
#include "util/String.h"

namespace cdlc {

class Node
    : public LightRefBase
{
public:
    virtual String ToString();

    virtual String Dump(
        /* [in] */ const String& prefix);
};

}

#endif // __CDLC_NODE_H__
