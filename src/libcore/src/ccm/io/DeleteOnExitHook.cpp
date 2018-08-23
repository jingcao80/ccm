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

#include "ccm/core/AutoLock.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/Thread.h"
#include "ccm/io/CFile.h"
#include "ccm/io/DeleteOnExitHook.h"
#include "ccm/util/CArrayList.h"
#include "ccm/util/Collections.h"
#include "ccm.core.ICharSequence.h"
#include "ccm.util.ICollection.h"
#include "ccm.util.IIterator.h"
#include "ccm.util.IList.h"
#include <ccmlogger.h>

using ccm::core::AutoLock;
using ccm::core::CoreUtils;
using ccm::core::ICharSequence;
using ccm::core::IThread;
using ccm::core::Thread;
using ccm::util::CArrayList;
using ccm::util::Collections;
using ccm::util::IArrayList;
using ccm::util::ICollection;
using ccm::util::IID_IArrayList;
using ccm::util::IIterator;
using ccm::util::IList;

namespace ccm {
namespace io {

AutoPtr<IHashSet> DeleteOnExitHook::FILES;

ECode DeleteOnExitHook::StaticInitialize()
{
    class _Thread
        : public Thread
    {
    public:
        ECode Run() override
        {
            RunHooks();
        }
    };

    AutoPtr<IThread> hook = new _Thread();
}

SyncObject& DeleteOnExitHook::GetClassLock()
{
    static SyncObject sLock;
    return sLock;
}

ECode DeleteOnExitHook::Add(
    /* [in] */ const String& file)
{
    AutoLock lock(GetClassLock());

    if (FILES == nullptr) {
        Logger::E("DeleteOnExitHook", "Shutdown in progress");
        return ccm::core::E_ILLEGAL_STATE_EXCEPTION;
    }

    FILES->Add(CoreUtils::Box(file));
    return NOERROR;
}

void DeleteOnExitHook::RunHooks()
{
    AutoPtr<IHashSet> theFiles;

    {
        AutoLock lock(GetClassLock());
        theFiles = FILES;
        FILES = nullptr;
    }

    AutoPtr<IArrayList> toBeDeleted;
    CArrayList::New(ICollection::Probe(theFiles), IID_IArrayList, (IInterface**)&toBeDeleted);

    Collections::Reverse(IList::Probe(toBeDeleted));
    AutoPtr<IIterator> it;
    toBeDeleted->GetIterator(&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->Next(&obj);
        String filename = CoreUtils::Unbox(ICharSequence::Probe(obj));
        AutoPtr<IFile> f;
        CFile::New(filename, IID_IFile, (IInterface**)&f);
        f->Delete();
    }
}

}
}
