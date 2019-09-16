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

#include "ccmapi.h"
#include "util/ccmautoptr.h"

namespace ccm {

extern void Init_EMPTY_STRING();
extern void Uninit_EMPTY_STRING();
extern void Init_Proxy_Entry();
extern void Uninit_Proxy_Entry();

static CONS_PROI_1
void RTInitialize()
{
    Init_EMPTY_STRING();
    Init_Proxy_Entry();
}

static DEST_PROI_10
void RTUninitialize()
{
    Uninit_EMPTY_STRING();
    Uninit_Proxy_Entry();
}

}
