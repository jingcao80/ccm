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

#include "pisces/system/OsConstants.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>

namespace pisces {
namespace system {

const Integer OsConstants::AF_INET_ = AF_INET;
const Integer OsConstants::AF_INET6_ = AF_INET6;
const Integer OsConstants::MS_SYNC_ = MS_SYNC;
const Integer OsConstants::O_ACCMODE_ = O_ACCMODE;
const Integer OsConstants::O_RDONLY_ = O_RDONLY;
const Integer OsConstants::SOL_SOCKET_ = SOL_SOCKET;
const Integer OsConstants::SO_DOMAIN_ = SO_DOMAIN;
const Integer OsConstants::SO_LINGER_ = SO_LINGER;

}
}
