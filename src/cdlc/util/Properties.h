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

#ifndef __CDLC_PROPERTIES_H__
#define __CDLC_PROPERTIES_H__

#include "util/String.h"
#include <set>

namespace cdlc {

class Properties
{
public:
    static Properties& Get();

    inline const std::set<String, StringCompareFunc>& GetSearchPaths() const;

    void AddSearchPath(
        /* [in] */ const String& path);

private:
    Properties() {}

private:
    std::set<String, StringCompareFunc> mSearchPaths;
};

const std::set<String, StringCompareFunc>& Properties::GetSearchPaths() const
{
    return mSearchPaths;
}

}

#endif // __CDLC_PROPERTIES_H__
