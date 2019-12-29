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

#include "ServiceManager.h"
#include <comolog.h>
#include <dbus/dbus.h>

namespace jing {

AutoPtr<ServiceManager> ServiceManager::sInstance = new ServiceManager();

AutoPtr<ServiceManager> ServiceManager::GetInstance()
{
    return sInstance;
}

ECode ServiceManager::AddService(
    /* [in] */ const String& name,
    /* [in] */ IInterface* object)
{
    if (name.IsEmpty() || object == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInterfacePack> ipack;
    ECode ec = CoMarshalInterface(object, RPCType::Local, ipack);
    if (FAILED(ec)) {
        Logger::E("ServiceManager", "Marshal the interface which named \"%s\" failed.",
                name.string());
        return ec;
    }

    AutoPtr<IParcel> parcel;
    CoCreateParcel(RPCType::Local, parcel);
    IParcelable::Probe(ipack)->WriteToParcel(parcel);
    HANDLE buffer;
    parcel->GetData(buffer);
    Long size;
    parcel->GetDataSize(size);

    DBusError err;
    DBusConnection* conn = nullptr;
    DBusMessage* msg = nullptr;
    DBusMessage* reply = nullptr;
    DBusMessageIter args, subArg;
    const char* str = nullptr;

    dbus_error_init(&err);

    conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        Logger::E("ServiceManager", "Connect to bus daemon failed, error is \"%s\".",
                err.message);
        ec = E_RUNTIME_EXCEPTION;
        goto Exit;
    }

    msg = dbus_message_new_method_call(
            DBUS_NAME, OBJECT_PATH, INTERFACE_PATH,
            "AddService");
    if (msg == nullptr) {
        Logger::E("ServiceManager", "Fail to create dbus message.");
        ec = E_RUNTIME_EXCEPTION;
        goto Exit;
    }

    dbus_message_iter_init_append(msg, &args);
    str = name.string();
    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &str);
    dbus_message_iter_open_container(&args,
            DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE_AS_STRING, &subArg);
    dbus_message_iter_append_fixed_array(&subArg,
            DBUS_TYPE_BYTE, (void*)&buffer, size);
    dbus_message_iter_close_container(&args, &subArg);

    reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
    if (dbus_error_is_set(&err)) {
        Logger::E("ServiceManager", "Fail to send message, error is \"%s\"", err.message);
        ec = E_REMOTE_EXCEPTION;
        goto Exit;
    }

    if (!dbus_message_iter_init(reply, &args)) {
        Logger::E("ServiceManager", "Reply has no results.");
        ec = E_REMOTE_EXCEPTION;
        goto Exit;
    }

    if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args)) {
        Logger::E("ServiceManager", "The first result is not Integer.");
        ec = E_REMOTE_EXCEPTION;
        goto Exit;
    }

    dbus_message_iter_get_basic(&args, &ec);

    if (FAILED(ec)) {
        Logger::E("ServiceManager", "Remote call failed with ec = 0x%x.", ec);
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

ECode ServiceManager::GetService(
    /* [in] */ const String& name,
    /* [out] */ AutoPtr<IInterface>& object)
{
    if (name.IsEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    object = nullptr;

    ECode ec = NOERROR;
    DBusError err;
    DBusConnection* conn = nullptr;
    DBusMessage* msg = nullptr;
    DBusMessage* reply = nullptr;
    DBusMessageIter args, subArg;
    const char* str = nullptr;

    dbus_error_init(&err);

    conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        Logger::E("ServiceManager", "Connect to bus daemon failed, error is \"%s\".",
                err.message);
        ec = E_RUNTIME_EXCEPTION;
        goto Exit;
    }

    msg = dbus_message_new_method_call(
            DBUS_NAME, OBJECT_PATH, INTERFACE_PATH, "GetService");
    if (msg == nullptr) {
        Logger::E("ServiceManager", "Fail to create dbus message.");
        ec = E_RUNTIME_EXCEPTION;
        goto Exit;
    }

    dbus_message_iter_init_append(msg, &args);
    str = name.string();
    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &str);

    reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
    if (dbus_error_is_set(&err)) {
        Logger::E("ServiceManager", "Fail to send message, error is \"%s\"", err.message);
        ec = E_REMOTE_EXCEPTION;
        goto Exit;
    }

    if (!dbus_message_iter_init(reply, &args)) {
        Logger::E("ServiceManager", "Reply has no results.");
        ec = E_REMOTE_EXCEPTION;
        goto Exit;
    }

    if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args)) {
        Logger::E("ServiceManager", "The first result is not Integer.");
        ec = E_REMOTE_EXCEPTION;
        goto Exit;
    }

    dbus_message_iter_get_basic(&args, &ec);

    if (SUCCEEDED(ec)) {
        if (!dbus_message_iter_next(&args)) {
            Logger::E("ServiceManager", "Reply has no out arguments.");
            ec = E_REMOTE_EXCEPTION;
            goto Exit;
        }
        if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&args)) {
            Logger::E("ServiceManager", "Reply arguments is not array.");
            ec = E_REMOTE_EXCEPTION;
            goto Exit;
        }

        void* replyData = nullptr;
        Integer replySize;
        dbus_message_iter_recurse(&args, &subArg);
        dbus_message_iter_get_fixed_array(&subArg,
                &replyData, &replySize);
        if (replyData != nullptr) {
            AutoPtr<IParcel> parcel;
            CoCreateParcel(RPCType::Local, parcel);
            parcel->SetData(reinterpret_cast<HANDLE>(replyData), replySize);

            AutoPtr<IInterfacePack> ipack;
            CoCreateInterfacePack(RPCType::Local, ipack);
            IParcelable::Probe(ipack)->ReadFromParcel(parcel);
            ec = CoUnmarshalInterface(ipack, RPCType::Local, object);
        }
    }
    else {
        Logger::E("ServiceManager", "Remote call failed with ec = 0x%x.", ec);
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

ECode ServiceManager::RemoveService(
    /* [in] */ const String& name)
{
    if (name.IsEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    ECode ec = NOERROR;
    DBusError err;
    DBusConnection* conn = nullptr;
    DBusMessage* msg = nullptr;
    DBusMessage* reply = nullptr;
    DBusMessageIter args;
    const char* str = nullptr;

    dbus_error_init(&err);

    conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        Logger::E("ServiceManager", "Connect to bus daemon failed, error is \"%s\".",
                err.message);
        ec = E_RUNTIME_EXCEPTION;
        goto Exit;
    }

    msg = dbus_message_new_method_call(
            DBUS_NAME, OBJECT_PATH, INTERFACE_PATH, "RemoveService");
    if (msg == nullptr) {
        Logger::E("ServiceManager", "Fail to create dbus message.");
        ec = E_RUNTIME_EXCEPTION;
        goto Exit;
    }

    dbus_message_iter_init_append(msg, &args);
    str = name.string();
    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &str);

    reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
    if (dbus_error_is_set(&err)) {
        Logger::E("ServiceManager", "Fail to send message, error is \"%s\"", err.message);
        ec = E_REMOTE_EXCEPTION;
        goto Exit;
    }

    if (!dbus_message_iter_init(reply, &args)) {
        Logger::E("ServiceManager", "Reply has no results.");
        ec = E_REMOTE_EXCEPTION;
        goto Exit;
    }

    if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args)) {
        Logger::E("ServiceManager", "The first result is not Integer.");
        ec = E_REMOTE_EXCEPTION;
        goto Exit;
    }

    dbus_message_iter_get_basic(&args, &ec);

    if (FAILED(ec)) {
        Logger::E("ServiceManager", "Remote call failed with ec = 0x%x.", ec);
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

}
