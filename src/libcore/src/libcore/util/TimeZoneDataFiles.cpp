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

#include "ccm/core/CStringBuilder.h"
#include "ccm/core/System.h"
#include "libcore/util/TimeZoneDataFiles.h"

using ccm::core::CStringBuilder;
using ccm::core::IID_IStringBuilder;
using ccm::core::IStringBuilder;
using ccm::core::System;

namespace libcore {
namespace util {

const String TimeZoneDataFiles::CCM_ROOT_ENV("CCM_ROOT");
const String TimeZoneDataFiles::CCM_DATA_ENV("CCM_DATA");

Array<String> TimeZoneDataFiles::GetTimeZoneFilePaths(
    /* [in] */ const String& fileName)
{
    return Array<String>{
            GetDataTimeZoneFile(fileName),
            GetSystemTimeZoneFile(fileName) };
}

String TimeZoneDataFiles::GetDataTimeZoneFile(
    /* [in] */ const String& fileName)
{
    String value;
    System::GetEnv(CCM_DATA_ENV, &value);
    return value + "/misc/zoneinfo/current/" + fileName;
}

String TimeZoneDataFiles::GetSystemTimeZoneFile(
    /* [in] */ const String& fileName)
{
    String value;
    System::GetEnv(CCM_ROOT_ENV, &value);
    return value + "/usr/share/zoneinfo/" + fileName;
}

String TimeZoneDataFiles::GenerateIcuDataPath()
{
    AutoPtr<IStringBuilder> icuDataPathBuilder;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&icuDataPathBuilder);

    String dataIcuDataPath = GetEnvironmentPath(CCM_DATA_ENV, String("/misc/zoneinfo/current/icu"));
    if (!dataIcuDataPath.IsNull()) {
        icuDataPathBuilder->Append(dataIcuDataPath);
    }

    String systemIcuDataPath = GetEnvironmentPath(CCM_ROOT_ENV, String("/usr/icu"));
    if (!systemIcuDataPath.IsNull()) {
        Integer length;
        if (icuDataPathBuilder->GetLength(&length), length > 0) {
            icuDataPathBuilder->Append(String(":"));
        }
        icuDataPathBuilder->Append(systemIcuDataPath);
    }
    String path;
    icuDataPathBuilder->ToString(&path);
    return path;
}

String TimeZoneDataFiles::GetEnvironmentPath(
    /* [in] */ const String& environmentVariable,
    /* [in] */ const String& path)
{
    String value;
    System::GetEnv(environmentVariable, &value);
    if (value.IsNull()) {
        return String(nullptr);
    }
    return value + path;
}

}
}
