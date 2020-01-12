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

#ifndef __LIBCORE_UTIL_TIMEZONEDATAFILES_H__
#define __LIBCORE_UTIL_TIMEZONEDATAFILES_H__

#include <comotypes.h>

using namespace como;

namespace libcore {
namespace util {

class TimeZoneDataFiles
{
public:
    static Array<String> GetTimeZoneFilePaths(
        /* [in] */ const String& fileName);

    static String GenerateIcuDataPath();

private:
    TimeZoneDataFiles();

    static String GetDataTimeZoneFile(
        /* [in] */ const String& fileName);

    static String GetSystemTimeZoneFile(
        /* [in] */ const String& fileName);

    static String GetEnvironmentPath(
        /* [in] */ const String& environmentVariable,
        /* [in] */ const String& path);

private:
    static const String COMO_ROOT_ENV;
    static const String COMO_DATA_ENV;
};

}
}

#endif //__LIBCORE_UTIL_TIMEZONEDATAFILES_H__
