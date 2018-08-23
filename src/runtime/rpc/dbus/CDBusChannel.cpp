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

#include "ccmrpc.h"
#include "CDBusChannel.h"
#include "CDBusParcel.h"
#include "InterfacePack.h"
#include "util/ccmlogger.h"

namespace ccm {

CDBusChannel::ServiceRunnable::ServiceRunnable(
    /* [in] */ CDBusChannel* owner,
    /* [in] */ IStub* target)
    : mOwner(owner)
    , mTarget(target)
    , mRequestToQuit(false)
{}

ECode CDBusChannel::ServiceRunnable::Run()
{
    DBusError err;

    dbus_error_init(&err);

    DBusConnection* conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        Logger::E("CDBusChannel", "Connect to bus daemon failed, error is \"%s\".",
                err.message);
        dbus_error_free(&err);
        return E_RUNTIME_EXCEPTION;
    }

    const char* name = dbus_bus_get_unique_name(conn);
    if (name == nullptr) {
        Logger::E("CDBusChannel", "Get unique name failed.");
        if (conn != nullptr) {
            dbus_connection_close(conn);
            dbus_connection_unref(conn);
        }
        dbus_error_free(&err);
        return E_RUNTIME_EXCEPTION;
    }

    mOwner->mName = name;

    DBusObjectPathVTable opVTable;

    opVTable.unregister_function = nullptr;
    opVTable.message_function = CDBusChannel::ServiceRunnable::HandleMessage;

    dbus_connection_register_object_path(conn,
            STUB_OBJECT_PATH, &opVTable, static_cast<void*>(this));

    {
        Mutex::AutoLock lock(mOwner->mLock);
        mOwner->mStarted = true;
    }
    mOwner->mCond.Signal();

    while (true) {
        DBusDispatchStatus status;

        do {
            dbus_connection_read_write_dispatch(conn, -1);
        } while ((status = dbus_connection_get_dispatch_status(conn))
                == DBUS_DISPATCH_DATA_REMAINS && !mRequestToQuit);

        if (status == DBUS_DISPATCH_NEED_MEMORY) {
            Logger::E("CDBusChannel", "DBus dispatching needs more memory.");
            break;
        }
    }

    dbus_connection_close(conn);
    dbus_connection_unref(conn);

    return NOERROR;
}

DBusHandlerResult CDBusChannel::ServiceRunnable::HandleMessage(
    /* [in] */ DBusConnection* conn,
    /* [in] */ DBusMessage* msg,
    /* [in] */ void* arg)
{
    CDBusChannel::ServiceRunnable* thisObj = static_cast<CDBusChannel::ServiceRunnable*>(arg);

    if (dbus_message_is_method_call(msg,
            STUB_INTERFACE_PATH, "GetMetadata")) {
        if (CDBusChannel::DEBUG) {
            Logger::D("CDBusChannel", "Handle \"GetMetadata\" message.");
        }
    }
    else if (dbus_message_is_method_call(msg,
            STUB_INTERFACE_PATH, "Invoke")) {
        if (CDBusChannel::DEBUG) {
            Logger::D("CDBusChannel", "Handle \"Invoke\" message.");
        }

        DBusMessageIter args;
        DBusMessageIter subArg;
        void* data = nullptr;
        Integer size = 0;

        if (!dbus_message_iter_init(msg, &args)) {
            Logger::E("CDBusChannel", "\"Invoke\" message has no arguments.");
            return DBUS_HANDLER_RESULT_HANDLED;
        }
        if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&args)) {
            Logger::E("CDBusChannel", "\"Invoke\" message has no array arguments.");
            return DBUS_HANDLER_RESULT_HANDLED;
        }
        dbus_message_iter_recurse(&args, &subArg);
        dbus_message_iter_get_fixed_array(&subArg, &data, (int*)&size);

        AutoPtr<IParcel> argParcel = new CDBusParcel();
        argParcel->SetData(static_cast<Byte*>(data), size);
        AutoPtr<IParcel> resParcel;
        ECode ec = thisObj->mTarget->Invoke(argParcel, &resParcel);

        DBusMessage* reply = dbus_message_new_method_return(msg);

        dbus_message_iter_init_append(reply, &args);
        dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &ec);
        HANDLE resData;
        Long resSize;
        dbus_message_iter_open_container(&args,
                DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE_AS_STRING, &subArg);
        resParcel->GetData(&resData);
        resParcel->GetDataSize(&resSize);
        dbus_message_iter_append_fixed_array(&subArg,
                DBUS_TYPE_BYTE, &resData, resSize);
        dbus_message_iter_close_container(&args, &subArg);

        dbus_uint32_t serial = 0;
        if (!dbus_connection_send(conn, reply, &serial)) {
            Logger::E("CDBusChannel", "Send reply message failed.");
        }
        dbus_connection_flush(conn);

        dbus_message_unref(reply);
    }
    else if (dbus_message_is_method_call(msg,
            STUB_INTERFACE_PATH, "Release")) {
        if (CDBusChannel::DEBUG) {
            Logger::D("CDBusChannel", "Handle \"Release\" message.");
        }

        thisObj->mTarget->Release();
        thisObj->mRequestToQuit = true;
    }
    else {
        const char* name = dbus_message_get_member(msg);
        if (name != nullptr && CDBusChannel::DEBUG) {
            Logger::D("CDBusChannel",
                    "The message which name is \"%s\" does not be handled.", name);
        }
    }

    return DBUS_HANDLER_RESULT_HANDLED;
}

//-------------------------------------------------------------------------------

const CoclassID CID_CDBusChannel =
        {{0x8efc6167,0xe82e,0x4c7d,0x89aa,{0x6,0x6,0x8,0xf,0x3,0x9,0x7,0xb,0x2,0x3,0xc,0xc}}, &CID_CCMRuntime};

CCM_INTERFACE_IMPL_1(CDBusChannel, Object, IRPCChannel);

CCM_OBJECT_IMPL(CDBusChannel);

CDBusChannel::CDBusChannel(
    /* [in] */ RPCType type,
    /* [in] */ RPCPeer peer)
    : mType(type)
    , mPeer(peer)
    , mStarted(false)
    , mCond(mLock)
{}

ECode CDBusChannel::GetRPCType(
    /* [out] */ RPCType* type)
{
    VALIDATE_NOT_NULL(type);

    *type = mType;
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

    conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);

    if (dbus_error_is_set(&err)) {
        Logger::E("CDBusChannel", "Connect to bus daemon failed, error is \"%s\".",
                err.message);
        ec = E_RUNTIME_EXCEPTION;
        goto Exit;
    }

    msg = dbus_message_new_method_call(
            mName, STUB_OBJECT_PATH, STUB_INTERFACE_PATH, "Invoke");
    if (msg == nullptr) {
        Logger::E("CDBusChannel", "Fail to create dbus message.");
        ec = E_RUNTIME_EXCEPTION;
        goto Exit;
    }

    dbus_message_iter_init_append(msg, &args);
    dbus_message_iter_open_container(&args,
            DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE_AS_STRING, &subArg);
    argParcel->GetData(&data);
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

ECode CDBusChannel::StartListening(
    /* [in] */ IStub* stub)
{
    ECode ec = NOERROR;
    if (mPeer == RPCPeer::Stub) {
        AutoPtr<ThreadPoolExecutor::Runnable> r = new ServiceRunnable(this, stub);
        ec = ThreadPoolExecutor::GetInstance()->RunTask(r);
    }

    {
        Mutex::AutoLock lock(mLock);
        while (!mStarted) {
            mCond.Wait();
        }
    }
    return ec;
}

ECode CDBusChannel::Match(
    /* [in] */ IInterfacePack* ipack,
    /* [out] */ Boolean* matched)
{
    VALIDATE_NOT_NULL(matched);

    IDBusInterfacePack* idpack = IDBusInterfacePack::Probe(ipack);
    if (idpack != nullptr) {
        InterfacePack* pack = (InterfacePack*)idpack;
        if (pack->GetDBusName().Equals(mName)) {
            *matched = true;
            return NOERROR;
        }
    }
    *matched = false;
    return NOERROR;
}

}
