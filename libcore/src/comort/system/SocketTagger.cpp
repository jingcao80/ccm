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

#include "comort/system/SocketTagger.h"
#include "como.io.IFileDescriptor.h"
#include <comolog.h>

using como::io::IFileDescriptor;

namespace comort {
namespace system {

COMO_INTERFACE_IMPL_1(SocketTagger, SyncObject, ISocketTagger);

AutoPtr<ISocketTagger> SocketTagger::GetOrSet(
    /* [in] */ ISocketTagger* tagger)
{
    class _SocketTagger
        : public SocketTagger
    {
    public:
        ECode Tag(
            /* [in] */ IFileDescriptor* socketDescriptor) override
        {
            return NOERROR;
        }

        ECode Untag(
            /* [in] */ IFileDescriptor* socketDescriptor) override
        {
            return NOERROR;
        }
    };

    static AutoPtr<ISocketTagger> sTagger = new _SocketTagger();
    if (tagger != nullptr) {
        sTagger = tagger;
    }
    return sTagger;
}

ECode SocketTagger::Tag(
    /* [in] */ ISocket* socket)
{
    Boolean closed;
    if (socket->IsClosed(closed), !closed) {
        AutoPtr<IFileDescriptor> fd;
        socket->GetFileDescriptor(fd);
        return Tag(fd);
    }
    return NOERROR;
}

ECode SocketTagger::Untag(
    /* [in] */ ISocket* socket)
{
    Boolean closed;
    if (socket->IsClosed(closed), !closed) {
        AutoPtr<IFileDescriptor> fd;
        socket->GetFileDescriptor(fd);
        return Untag(fd);
    }
    return NOERROR;
}

ECode SocketTagger::Tag(
    /* [in] */ IDatagramSocket* socket)
{
    Boolean closed;
    if (socket->IsClosed(closed), !closed) {
        AutoPtr<IFileDescriptor> fd;
        socket->GetFileDescriptor(fd);
        return Tag(fd);
    }
    return NOERROR;
}

ECode SocketTagger::Untag(
    /* [in] */ IDatagramSocket* socket)
{
    Boolean closed;
    if (socket->IsClosed(closed), !closed) {
        AutoPtr<IFileDescriptor> fd;
        socket->GetFileDescriptor(fd);
        return Untag(fd);
    }
    return NOERROR;
}

ECode SocketTagger::Set(
    /* [in] */ ISocketTagger* tagger)
{
    if (tagger == nullptr) {
        Logger::E("SocketTagger", "tagger == null");
        return como::core::E_NULL_POINTER_EXCEPTION;
    }
    GetOrSet(tagger);
    return NOERROR;
}

}
}
