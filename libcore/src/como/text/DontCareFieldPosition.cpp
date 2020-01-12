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

#include "como/text/DontCareFieldPosition.h"

namespace como {
namespace text {

static const AutoPtr<IFieldPosition> CreateDontCareFieldPosition()
{
    const AutoPtr<DontCareFieldPosition> fp = new DontCareFieldPosition();
    fp->Constructor();
    return (IFieldPosition*)fp.Get();
}

const AutoPtr<IFieldPosition> DontCareFieldPosition::GetInstance()
{
    static const AutoPtr<IFieldPosition> INSTANCE = CreateDontCareFieldPosition();
    return INSTANCE;
}

ECode DontCareFieldPosition::Constructor()
{
    class _FormatFieldDelegate
        : public LightRefBase
        , public IFormatFieldDelegate
    {
    public:
        Integer AddRef(
            /* [in] */ HANDLE id = 0) override
        {
            return LightRefBase::AddRef(id);
        }

        Integer Release(
            /* [in] */ HANDLE id = 0) override
        {
            return LightRefBase::Release(id);
        }

        IInterface* Probe(
            /* [in] */ const InterfaceID& iid) override
        {
            if (iid == IID_IInterface) {
                return (IInterface*)(IFormatFieldDelegate*)this;
            }
            else if (iid == IID_IFormatFieldDelegate) {
                return (IFormatFieldDelegate*)this;
            }
            return nullptr;
        }

        ECode GetInterfaceID(
            /* [in] */ IInterface* object,
            /* [out] */ InterfaceID& iid) override
        {
            if (object == (IInterface*)(IFormatFieldDelegate*)this) {
                iid = IID_IFormatFieldDelegate;
                return NOERROR;
            }
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        ECode Formatted(
            /* [in] */ IFormatField* attr,
            /* [in] */ IInterface* value,
            /* [in] */ Integer start,
            /* [in] */ Integer end,
            /* [in] */ IStringBuffer* buffer) override
        {
            return NOERROR;
        }

        ECode Formatted(
            /* [in] */ Integer fieldID,
            /* [in] */ IFormatField* attr,
            /* [in] */ IInterface* value,
            /* [in] */ Integer start,
            /* [in] */ Integer end,
            /* [in] */ IStringBuffer* buffer) override
        {
            return NOERROR;
        }
    };

    mNoDelegate = new _FormatFieldDelegate();
    return FieldPosition::Constructor(0);
}

ECode DontCareFieldPosition::GetFieldDelegate(
    /* [out] */ IFormatFieldDelegate** delegate)
{
    VALIDATE_NOT_NULL(delegate);

    *delegate = mNoDelegate;
    REFCOUNT_ADD(*delegate);
    return NOERROR;
}

ECode DontCareFieldPosition::ToString(
    /* [out] */ String& desc)
{
    AutoPtr<IFormatField> attribute;
    GetFieldAttribute(&attribute);
    desc = String::Format("DontCareFieldPosition[mField=%d,mAttribute=%s,mBeginIndex=%d,mEndIndex=%d]",
            mField, Object::ToString(attribute).string(), mBeginIndex, mEndIndex);
    return NOERROR;
}

}
}

