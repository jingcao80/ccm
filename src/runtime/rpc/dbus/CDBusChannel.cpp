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

#include "CDBusChannel.h"
#include "CDBusParcel.h"
#include "util/ccmlogger.h"
#include <dbus/dbus.h>

namespace ccm {

const CoclassID CID_CDBusChannel =
        {{0x8efc6167,0xe82e,0x4c7d,0x89aa,{0x6,0x6,0x8,0xf,0x3,0x9,0x7,0xb,0x2,0x3,0xc,0xc}}, &CID_CCMRuntime};

CCM_INTERFACE_IMPL_1(CDBusChannel, Object, IRPCChannel);

CCM_OBJECT_IMPL(CDBusChannel);

ECode CDBusChannel::Initialize(
    /* [in] */ RPCType type,
    /* [in] */ RPCPeer peer)
{
    ECode ec = NOERROR;
    mType = type;
    mPeer = peer;
    if (mPeer == RPCPeer::Stub) {
        DBusError err;
        DBusConnection* conn = nullptr;
        const char* name = nullptr;
        dbus_error_init(&err);
        conn = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);
        if (dbus_error_is_set(&err)) {
            Logger::E("CDBusChannel", "Connect to bus daemon failed, error is \"%s\".",
                    err.message);
            ec = E_RUNTIME_EXCEPTION;
            goto Exit;
        }

        name = dbus_bus_get_unique_name(conn);
        if (name == nullptr) {
            Logger::E("CDBusChannel", "Get unique name failed.");
            ec = E_RUNTIME_EXCEPTION;
            goto Exit;
        }

        mName = name;

Exit:
        if (conn != nullptr) {
            dbus_connection_close(conn);
            dbus_connection_unref(conn);
        }
        dbus_error_free(&err);
    }
    return ec;
}

ECode CDBusChannel::GetRPCType(
    /* [out] */ RPCType* type)
{
    VALIDATE_NOT_NULL(type);

    *type = mType;
    return NOERROR;
}

ECode CDBusChannel::CreateArgumentParcel(
    /* [out] */ IParcel** parcel)
{
    VALIDATE_NOT_NULL(parcel)

    *parcel = new CDBusParcel();
    REFCOUNT_ADD(*parcel);
    return NOERROR;
}

ECode CDBusChannel::IsPeerAlive(
    /* [out] */ Boolean* alive)
{
    return NOERROR;
}

ECode CDBusChannel::LinkToDeath(
    /* [in] */ IDeathRecipient* recipient,
    /* [in] */ HANDLE cookie,
    /* [in] */ Integer flags)
{
    return NOERROR;
}

ECode CDBusChannel::UnlinkToDeath(
    /* [in] */ IDeathRecipient* recipient,
    /* [in] */ HANDLE cookie,
    /* [in] */ Integer flags,
    /* [out] */ IDeathRecipient** outRecipient)
{
    return NOERROR;
}

ECode CDBusChannel::UnmarshalArguments(
    /* [in] */ void* data,
    /* [in] */ Long size,
    /* [in] */ IMetaMethod* method,
    /* [in] */ IParcel* argParcel)
{
    return NOERROR;
}

ECode CDBusChannel::Invoke(
    /* [in] */ IProxy* proxy,
    /* [in] */ IMetaMethod* method,
    /* [in] */ IParcel* argParcel,
    /* [out] */ IParcel** resParcel)
{
    ECode ec = NOERROR;
    DBusError err;
    DBusConnection* conn = nullptr;
    DBusMessage* msg = nullptr;
    DBusMessage* reply = nullptr;
    DBusMessageIter args, subArg;
    HANDLE data;
    Long size;

    dbus_error_init(&err);

    conn = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);

    if (dbus_error_is_set(&err)) {
        Logger::E("CDBusChannel", "Connect to bus daemon failed, error is \"%s\".",
                err.message);
        ec = E_RUNTIME_EXCEPTION;
        goto Exit;
    }

    msg = dbus_message_new_method_call(
            mName, mPath, nullptr, "Invoke");
    if (msg == nullptr) {
        Logger::E("CDBusChannel", "Fail to create dbus message.");
        ec = E_RUNTIME_EXCEPTION;
        goto Exit;
    }

    dbus_message_iter_init_append(msg, &args);
    dbus_message_iter_open_container(&args,
            DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE_AS_STRING, &subArg);
    argParcel->GetDataPayload(&data);
    argParcel->GetDataSize(&size);
    dbus_message_iter_append_fixed_array(&subArg,
            DBUS_TYPE_BYTE, &data, size);
    dbus_message_iter_close_container(&args, &subArg);

    if (DEBUG) {
        Logger::D("CDBusChannel", "Send message.");
    }

    reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
    if (dbus_error_is_set(&err)) {
        Logger::E("CDBusChannel", "Fail to send message, error is \"%s\"", err.message);
        ec = E_REMOTE_EXCEPTION;
        goto Exit;
    }

    if (!dbus_message_iter_init(reply, &args)) {
        Logger::E("CDBusChannel", "Reply has no results.");
        ec = E_REMOTE_EXCEPTION;
        goto Exit;
    }

    if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args)) {
        Logger::E("CDBusChannel", "The first result is not Integer.");
        ec = E_REMOTE_EXCEPTION;
        goto Exit;
    }

    dbus_message_iter_get_basic(&args, &ec);

    if (SUCCEEDED(ec)) {
        Boolean hasOutArgs;
        method->HasOutArguments(&hasOutArgs);
        if (hasOutArgs) {
            if (!dbus_message_iter_next(&args)) {
                Logger::E("CDBusChannel", "Reply has no out arguments.");
                ec = E_REMOTE_EXCEPTION;
                goto Exit;
            }
            if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&args)) {
                Logger::E("CDBusChannel", "Reply arguments is not array.");
                ec = E_REMOTE_EXCEPTION;
                goto Exit;
            }

            void* replyData = nullptr;
            Integer replySize;

            dbus_message_iter_recurse(&args, &subArg);
            dbus_message_iter_get_fixed_array(&subArg,
                    &replyData, &replySize);
            if (replyData != nullptr) {
                *resParcel = new CDBusParcel();
                REFCOUNT_ADD(*resParcel);
                ec = UnmarshalArguments(replyData, replySize, method, *resParcel);
            }
        }
    }
    else {
        if (DEBUG) {
            Logger::D("CDBusChannel", "Remote call failed with ec = 0x%x.", ec);
        }
    }

Exit:
    if (msg != nullptr) {
        dbus_message_unref(msg);
    }
    if (reply != nullptr) {
        dbus_message_unref(reply);
    }
    if (conn != nullptr) {
        dbus_connection_close(conn);
        dbus_connection_unref(conn);
    }

    dbus_error_free(&err);

    return ec;
}

ECode CDBusChannel::StartListening()
{
    if (mPeer == RPCPeer::Stub) {

    }
    return NOERROR;
}

}
