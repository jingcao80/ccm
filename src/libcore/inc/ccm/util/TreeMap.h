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

#ifndef __CCM_UTIL_TREEMAP_H__
#define __CCM_UTIL_TREEMAP_H__

#include "ccm.util.IComparator.h"
#include "ccm.util.ISortedSet.h"
#include "ccm.util.ITreeMap.h"

namespace ccm {
namespace util {

class TreeMap
    : public ITreeMap
{
public:
    ECode Comparator(
        /* [out] */ IComparator** comparator)
    {
        return NOERROR;
    }

    ECode AddAllForTreeSet(
        /* [in] */ ISortedSet* set,
        /* [in] */ IInterface* defaultVal)
    {
        return NOERROR;
    }
};

}
}

#endif // __CCM_UTIL_TREEMAP_H__
