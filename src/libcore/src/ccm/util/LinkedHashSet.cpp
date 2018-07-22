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

#include "ccm/core/Math.h"
#include "ccm/util/LinkedHashSet.h"

using ccm::core::Math;

namespace ccm {
namespace util {

ECode LinkedHashSet::Constructor(
    /* [in] */ Integer initialCapacity,
    /* [in] */ Float loadFactor)
{
    return HashSet::Constructor(initialCapacity, loadFactor, true);
}

ECode LinkedHashSet::Constructor(
    /* [in] */ Integer initialCapacity)
{
    return HashSet::Constructor(initialCapacity, .75f, true);
}

ECode LinkedHashSet::Constructor()
{
    return HashSet::Constructor(16, .75f, true);
}

ECode LinkedHashSet::Constructor(
    /* [in] */ ICollection* c)
{
    Integer size;
    c->GetSize(&size);
    HashSet::Constructor(Math::Max(2 * size, 11), .75f, true);
    AddAll(c);
    return NOERROR;
}

}
}