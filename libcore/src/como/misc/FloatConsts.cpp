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

#include "como/misc/FloatConsts.h"
#include "como.core.IFloat.h"

using como::core::IFloat;

namespace como {
namespace misc {

const Float FloatConsts::POSITIVE_INFINITY = IFloat::POSITIVE_INFINITY;
const Float FloatConsts::NEGATIVE_INFINITY = IFloat::NEGATIVE_INFINITY;
const Float FloatConsts::NaN = IFloat::NaN;
const Float FloatConsts::MAX_VALUE = IFloat::MAX_VALUE;
const Float FloatConsts::MIN_VALUE = IFloat::MIN_VALUE;

}
}
