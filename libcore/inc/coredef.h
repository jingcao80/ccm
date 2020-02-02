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

#ifndef __COMO_COREDEF_H__
#define __COMO_COREDEF_H__

#define FOR_EACH(Type, element, TypeCastFunc, container) \
    { \
        AutoPtr<IIterator> it; \
        container->GetIterator(it); \
        Boolean hasNext; \
        while (it->HasNext(&hasNext), hasNext) { \
            AutoPtr<IInterface> obj; \
            it->Next(&obj); \
            Type element = (Type)TypeCastFunc(obj);

#define FOR_EACH_2(Type, element, TypeCastFuncInner, TypeCastFuncOuter, container) \
    { \
        AutoPtr<IIterator> it; \
        container->GetIterator(it); \
        Boolean hasNext; \
        while (it->HasNext(&hasNext), hasNext) { \
            AutoPtr<IInterface> obj; \
            it->Next(&obj); \
            Type element = (Type)TypeCastFuncOuter(TypeCastFuncInner(obj));

#define END_FOR_EACH() }}

#endif // __COMO_COREDEF_H__
