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
#include <ccmlogger.h>
#include <dbus/dbus.h>
#include <stdio.h>

using ccm::Logger;
using xos::ServiceManager;

static const char* SERVICE_MANAGER_DBUS_NAME = "servicemanager";
static const char* SERVICE_MANAGER_INTERFACE_PATH = "xos.servicemanager";

int main(int argv, char** argc)
{
    DBusError err;

    dbus_error_init(&err);

    DBusConnection* conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        Logger::E("servicemanager", "Connect to bus daemon failed, error is \"%s\".",
                err.message);
        dbus_error_free(&err);
        return -1;
    }

    dbus_bus_request_name(conn, SERVICE_MANAGER_DBUS_NAME,
            DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err)) {
        Logger::E("servicemanager", "Request servicemanager dbus name failed, error is \"%s\".",
                err.message);
        dbus_error_free(&err);
        dbus_connection_close(conn);
        dbus_connection_unref(conn);
        return -1;
    }

    while (true) {
        dbus_bool_t res = dbus_connection_read_write(conn, -1);
        if (!res) {
            Logger::E("servicemanager", "Disconnect to bus daemon.");
            return -1;
        }
        DBusMessage* msg = dbus_connection_pop_message(conn);
        if (dbus_message_is_method_call(msg,
                SERVICE_MANAGER_INTERFACE_PATH, "AddService")) {
            DBusMessageIter args;
            DBusMessageIter subArg;
            void* data = nullptr;
            Integer size = 0;

            if (!dbus_message_iter_init(msg, &args)) {
                Logger::E("ServiceManager", "\"AddService\" message has no arguments.");
                continue;
            }
            if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) {
                Logger::E("ServiceManager", "\"AddService\" message has no string arguments.");
                continue;
            }
            const char* str;
            dbus_message_iter_get_basic(&args, &str);
            dbus_message_iter_next(&args);
            if (DBUS_TYPE_ARRAY != dbus_message_iter_get_arg_type(&args)) {
                Logger::E("ServiceManager", "\"AddService\" message has no array arguments.");
                continue;
            }
            dbus_message_iter_recurse(&args, &subArg);
            dbus_message_iter_get_fixed_array(&subArg, &data, (int*)&size);

            AutoPtr<IParcel> parcel;
            CoCreateParcel(RPCType::Local, (IParcel**)&parcel);
            parcel->SetData(static_cast<Byte*>(data), size);
            ServiceManager::InterfacePack ipack;
            parcel->ReadString(&ipack.mDBusName);
            parcel->ReadCoclassID(&ipack.mCid);
            parcel->ReadInterfaceID(&ipack.mIid);
            ECode ec = ServiceManager::GetInstance()->AddService(String(str), ipack);

            DBusMessage* reply = dbus_message_new_method_return(msg);
            dbus_message_iter_init_append(reply, &args);
            dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &ec);
            dbus_uint32_t serial = 0;
            if (!dbus_connection_send(conn, reply, &serial)) {
                Logger::E("ServiceManager", "Send reply message failed.");
            }
            dbus_connection_flush(conn);
            dbus_message_unref(reply);
        }
        else if (dbus_message_is_method_call(msg,
                SERVICE_MANAGER_INTERFACE_PATH, "GetService")) {
            DBusMessageIter args;
            DBusMessageIter subArg;

            if (!dbus_message_iter_init(msg, &args)) {
                Logger::E("ServiceManager", "\"AddService\" message has no arguments.");
                continue;
            }
            if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) {
                Logger::E("ServiceManager", "\"AddService\" message has no string arguments.");
                continue;
            }
            const char* str;
            dbus_message_iter_get_basic(&args, &str);

            ServiceManager::InterfacePack* ipack;
            ECode ec = ServiceManager::GetInstance()->GetService(String(str), &ipack);

            DBusMessage* reply = dbus_message_new_method_return(msg);
            dbus_message_iter_init_append(reply, &args);
            dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &ec);
            HANDLE resData = 0;
            Long resSize = 0;
            if (ipack != nullptr) {
                AutoPtr<IParcel> parcel;
                CoCreateParcel(RPCType::Local, (IParcel**)&parcel);
                parcel->WriteString(ipack->mDBusName);
                parcel->WriteCoclassID(ipack->mCid);
                parcel->WriteInterfaceID(ipack->mIid);
                parcel->GetData(&resData);
                parcel->GetDataSize(&resSize);
            }
            dbus_message_iter_open_container(&args,
                    DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE_AS_STRING, &subArg);
            dbus_message_iter_append_fixed_array(&subArg,
                    DBUS_TYPE_BYTE, &resData, resSize);
            dbus_message_iter_close_container(&args, &subArg);

            dbus_uint32_t serial = 0;
            if (!dbus_connection_send(conn, reply, &serial)) {
                Logger::E("ServiceManager", "Send reply message failed.");
            }
            dbus_connection_flush(conn);
            dbus_message_unref(reply);
        }
        else if (dbus_message_is_method_call(msg,
                SERVICE_MANAGER_INTERFACE_PATH, "RemoveService")) {
            DBusMessageIter args;
            DBusMessageIter subArg;

            if (!dbus_message_iter_init(msg, &args)) {
                Logger::E("ServiceManager", "\"AddService\" message has no arguments.");
                continue;
            }
            if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) {
                Logger::E("ServiceManager", "\"AddService\" message has no string arguments.");
                continue;
            }
            const char* str;
            dbus_message_iter_get_basic(&args, &str);

            ECode ec = ServiceManager::GetInstance()->RemoveService(String(str));

            DBusMessage* reply = dbus_message_new_method_return(msg);
            dbus_message_iter_init_append(reply, &args);
            dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &ec);
            dbus_uint32_t serial = 0;
            if (!dbus_connection_send(conn, reply, &serial)) {
                Logger::E("ServiceManager", "Send reply message failed.");
            }
            dbus_connection_flush(conn);
            dbus_message_unref(reply);
        }
        else {
            const char* sign = dbus_message_get_signature(msg);
            if (sign != nullptr) {
                Logger::D("servicemanager",
                        "The message which signature is \"%\" does not be handled.", sign);
            }
        }
    }

    return 0;
}
