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

#include "comotypes.h"

namespace como {

const CoclassID CoclassID::Null =
    {{0x00000000,0x0000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00}}, nullptr};

const InterfaceID InterfaceID::Null =
    {{0x00000000,0x0000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00}}, nullptr};

COM_PUBLIC extern const ComponentID CID_COMORuntime =
    {{0xffffffff,0xffff,0xffff,0xffff,{0xff,0xff,0xff,0xff,0xff,0xff}},
    "http://como.org/comort.so"};

COM_PUBLIC extern const InterfaceID IID_IInterface =
    {{0x00000000,0x0000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x01}}, &CID_COMORuntime};

} // namespace como
