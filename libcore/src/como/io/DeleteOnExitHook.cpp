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

#include "como/core/AutoLock.h"
#include "como/core/CoreUtils.h"
#include "como/core/Thread.h"
#include "como/io/CFile.h"
#include "como/io/DeleteOnExitHook.h"
#include "como/util/CArrayList.h"
#include "como/util/Collections.h"
#include "como.core.ICharSequence.h"
#include "como.util.ICollection.h"
#include "como.util.IIterator.h"
#include "como.util.IList.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::CoreUtils;
using como::core::ICharSequence;
using como::core::IThread;
using como::core::Thread;
using como::util::CArrayList;
using como::util::Collections;
using como::util::IArrayList;
using como::util::ICollection;
using como::util::IID_IArrayList;
using como::util::IIterator;
using como::util::IList;

namespace como {
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
            return NOERROR;
        }
    };

    AutoPtr<IThread> hook = new _Thread();
    return NOERROR;
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
        return como::core::E_ILLEGAL_STATE_EXCEPTION;
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
    toBeDeleted->GetIterator(it);
    Boolean hasNext;
    while (it->HasNext(hasNext), hasNext) {
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
