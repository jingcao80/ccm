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

#ifndef __COMO_TEXT_DONTCAREFIELDPOSITION_H__
#define __COMO_TEXT_DONTCAREFIELDPOSITION_H__

#include "como/text/FieldPosition.h"

namespace como {
namespace text {

static const AutoPtr<IFieldPosition> CreateDontCareFieldPosition();

class DontCareFieldPosition
    : public FieldPosition
{
public:
    static const AutoPtr<IFieldPosition> GetInstance();

    ECode GetFieldDelegate(
        /* [out] */ IFormatFieldDelegate** delegate) override;

    ECode ToString(
        /* [out] */ String* desc) override;

private:
    ECode Constructor();

private:
    AutoPtr<IFormatFieldDelegate> mNoDelegate;

    friend const AutoPtr<IFieldPosition> CreateDontCareFieldPosition();
};

}
}

#endif // __COMO_TEXT_DONTCAREFIELDPOSITION_H__
