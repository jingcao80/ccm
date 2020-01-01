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

#ifndef __COMO_MISC_ASCIICASEINSENSITIVECOMPARATOR_H__
#define __COMO_MISC_ASCIICASEINSENSITIVECOMPARATOR_H__

#include "como.util.IComparator.h"

using como::util::IComparator;

namespace como {
namespace misc {

class ASCIICaseInsensitiveComparator
{
public:
    static AutoPtr<IComparator> GetCASE_INSENSITIVE_ORDER()
    {
        return nullptr;
    }
};

}
}

#endif // __COMO_MISC_ASCIICASEINSENSITIVECOMPARATOR_H__
