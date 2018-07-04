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

namespace ccm {
namespace util {
namespace calendar {

Integer CalendarUtils::FloorDivide(
    /* [in] */ Integer n,
    /* [in] */ Integer d,
    /* [out] */ Array<Integer>& r)
{
    if (n >= 0) {
        r[0] = n % d;
        return n / d;
    }
    Integer q = ((n + 1) / d) - 1;
    r[0] = n - (q * d);
    return q;
}

Integer CalendarUtils::FloorDivide(
    /* [in] */ Long n,
    /* [in] */ Integer d,
    /* [out] */ Array<Integer>& r)
{
    if (n >= 0) {
        r[0] = (Integer)(n % d);
        return (Integer)(n / d);
    }
    Integer q = (Integer)(((n + 1) / d) - 1);
    r[0] = (Integer)(n - (q * d));
    return q;
}

ECode CalendarUtils::Sprintf0d(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Integer value,
    /* [in] */ Integer width)
{
    Long d = value;
    if (d < 0) {
        sb->AppendChar('-');
        d = -d;
        --width;
    }
    Integer n = 10;
    for (Integer i = 2; i < width; i++) {
        n *= 10;
    }
    for (Integer i = 1; i < width && d < n; i++) {
        sb->AppendChar('0');
        n /= 10;
    }
    sb->Append(d);
    return NOERROR;
}

ECode CalendarUtils::Sprintf0d(
    /* [in] */ IStringBuffer* sb,
    /* [in] */ Integer value,
    /* [in] */ Integer width)
{
    Long d = value;
    if (d < 0) {
        sb->AppendChar('-');
        d = -d;
        --width;
    }
    Integer n = 10;
    for (Integer i = 2; i < width; i++) {
        n *= 10;
    }
    for (Integer i = 1; i < width && d < n; i++) {
        sb->AppendChar('0');
        n /= 10;
    }
    sb->Append(d);
    return NOERROR;
}

}
}
}
