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

    inline void AddMode(
        /* [in] */ int mode);

    inline void ClearMode(
        /* [in] */ int mode);

    inline int GetMode();

private:
    Properties() {}

public:
    static const String INDENT;

    static constexpr int BUILD_MODE_MASK = 0x0f;
    static constexpr int BUILD_MODE_CLIENT = 0x01;
    static constexpr int BUILD_MODE_COMPONENT = 0x02;
    static constexpr int BUILD_MODE_RUNTIME = 0x04;

    static constexpr int CODEGEN_INTERFACE_SPLIT = 0x10;

private:
    std::set<String, StringCompareFunc> mSearchPaths;
    int mMode = 0;
};

const std::set<String, StringCompareFunc>& Properties::GetSearchPaths() const
{
    return mSearchPaths;
}

void Properties::AddMode(
    /* [in] */ int mode)
{
    mMode |= mode;
}

void Properties::ClearMode(
    /* [in] */ int mode)
{
    mMode &= ~mode;
}

int Properties::GetMode()
{
    return mMode;
}

}

#endif // __CDLC_PROPERTIES_H__
