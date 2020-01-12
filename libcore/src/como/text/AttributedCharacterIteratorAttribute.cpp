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

#include "como/core/CoreUtils.h"
#include "como/text/AttributedCharacterIteratorAttribute.h"
#include "como/text/CAttributedCharacterIteratorAttribute.h"
#include "como/util/CHashMap.h"
#include <comolog.h>

using como::core::CoreUtils;
using como::io::E_INVALID_OBJECT_EXCEPTION;
using como::util::CHashMap;
using como::util::IID_IMap;

namespace como {
namespace text {

COMO_INTERFACE_REFCOUNT(AttributedCharacterIteratorAttribute)

COMO_INTERFACE_PROBE_BEGIN(AttributedCharacterIteratorAttribute)
COMO_INTERFACE_PROBE_NESTEDINTERFACE(IAttributedCharacterIterator, IAttribute)
COMO_INTERFACE_PROBE_END(SyncObject)

COMO_INTERFACE_GETINTERFACEID_BEGIN(AttributedCharacterIteratorAttribute)
COMO_INTERFACE_GETINTERFACEID_NESTEDINTERFACE(IAttributedCharacterIterator, IAttribute)
COMO_INTERFACE_GETINTERFACEID_END(SyncObject)

static AutoPtr<IMap> CreateHashMap()
{
    AutoPtr<IMap> map;
    CHashMap::New(7, IID_IMap, (IInterface**)&map);
    return map;
}

AutoPtr<IMap> AttributedCharacterIteratorAttribute::GetInstanceMap()
{
    static const AutoPtr<IMap> sInstanceMap = CreateHashMap();
    return sInstanceMap;
}

ECode AttributedCharacterIteratorAttribute::Constructor(
    /* [in] */ const String& name)
{
    mName = name;
    CoclassID cid;
    GetCoclassID(cid);
    if (cid == CID_CAttributedCharacterIteratorAttribute) {
        GetInstanceMap()->Put(CoreUtils::Box(name), (IAttributedCharacterIterator::IAttribute*)this);
    }
    return NOERROR;
}

ECode AttributedCharacterIteratorAttribute::ToString(
    /* [out] */ String& str)
{
    str = String::Format("%s(%s)",
            Object::GetCoclassName((IAttributedCharacterIterator::IAttribute*)this).string(), mName.string());
    return NOERROR;
}

String AttributedCharacterIteratorAttribute::GetName()
{
    return mName;
}

ECode AttributedCharacterIteratorAttribute::ReadResolve(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    CoclassID cid;
    GetCoclassID(cid);
    if (cid != CID_CAttributedCharacterIteratorAttribute) {
        Logger::E("AttributedCharacterIteratorAttribute", "subclass didn't correctly implement readResolve");
        return E_INVALID_OBJECT_EXCEPTION;
    }

    GetInstanceMap()->Get(CoreUtils::Box(GetName()), obj);
    if (*obj == nullptr) {
        Logger::E("AttributedCharacterIteratorAttribute", "unknown attribute name");
        return E_INVALID_OBJECT_EXCEPTION;
    }
    return NOERROR;
}

static AutoPtr<IAttributedCharacterIterator::IAttribute> CreateAttributedCharacterIteratorAttribute(
    /* [in] */ const String& name)
{
    AutoPtr<IAttributedCharacterIterator::IAttribute> attribute;
    CAttributedCharacterIteratorAttribute::New(name, IAttributedCharacterIterator::IID_IAttribute, (IInterface**)&attribute);
    return attribute;
}

ECode AttributedCharacterIteratorAttribute::GetLANGUAGE(
    /* [out] */ IAttributedCharacterIterator::IAttribute** attribute)
{
    VALIDATE_NOT_NULL(attribute);

    static const AutoPtr<IAttributedCharacterIterator::IAttribute> sLANGUAGE =
            CreateAttributedCharacterIteratorAttribute(String("language"));

    *attribute = sLANGUAGE;
    REFCOUNT_ADD(*attribute);
    return NOERROR;
}

ECode AttributedCharacterIteratorAttribute::GetREADING(
    /* [out] */ IAttributedCharacterIterator::IAttribute** attribute)
{
    VALIDATE_NOT_NULL(attribute);

    static const AutoPtr<IAttributedCharacterIterator::IAttribute> sREADING =
            CreateAttributedCharacterIteratorAttribute(String("reading"));

    *attribute = sREADING;
    REFCOUNT_ADD(*attribute);
    return NOERROR;
}

ECode AttributedCharacterIteratorAttribute::GetINPUT_METHOD_SEGMENT(
    /* [out] */ IAttributedCharacterIterator::IAttribute** attribute)
{
    VALIDATE_NOT_NULL(attribute);

    static const AutoPtr<IAttributedCharacterIterator::IAttribute> sINPUT_METHOD_SEGMENT =
            CreateAttributedCharacterIteratorAttribute(String("input_method_segment"));

    *attribute = sINPUT_METHOD_SEGMENT;
    REFCOUNT_ADD(*attribute);
    return NOERROR;
}

}
}
