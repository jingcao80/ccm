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

#ifndef __CCDL_AST_METHOD_H__
#define __CCDL_AST_METHOD_H__

#include "Node.h"
#include "Parameter.h"
#include "../util/ArrayList.h"
#include "../util/String.h"

namespace ccdl {
namespace ast {

class Method : public Node
{
public:
    inline String GetName()
    { return mName; }

    inline void SetName(
        /* [in] */ const String& name)
    { mName = name; }

    inline String GetSignature()
    { return mSignature; }

    bool AddParameter(
        /* [in] */ Parameter* param);

    inline int GetParameterNumber()
    { return mParameters.GetSize(); }

    inline Parameter* GetParameter(
        /* [in] */ int index)
    { return mParameters.Get(index); }

    void BuildSignature();

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    String mName;
    String mSignature;
    ArrayList<Parameter*> mParameters;
};

}
}

#endif // __CCDL_AST_METHOD_H__
