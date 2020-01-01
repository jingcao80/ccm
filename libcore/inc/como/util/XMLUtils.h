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

#ifndef __COMO_UTIL_XMLUTILS_H__
#define __COMO_UTIL_XMLUTILS_H__

#include "como.io.IInputStream.h"
#include "como.io.IOutputStream.h"
#include "como.util.IProperties.h"

using como::io::IInputStream;
using como::io::IOutputStream;

namespace como {
namespace util {

class XMLUtils
{
public:
    static ECode Load(
        /* [in] */ IProperties* props,
        /* [in] */ IInputStream* in);

    static ECode Save(
        /* [in] */ IProperties* props,
        /* [in] */ IOutputStream* os,
        /* [in] */ const String& comment,
        /* [in] */ const String& encoding);
};

}
}

#endif // __COMO_UTIL_XMLUTILS_H__
