
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

#include <ccmlogger.h>
#include <dbus/dbus.h>
#include <stdio.h>

using ccm::Logger;

static const char* SERVICE_MANAGER_DBUS_NAME = "servicemanager";
static const char* SERVICE_MANAGER_OBJECT_PATH = "/xos/servicemanager";
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

        }
        else if (dbus_message_is_method_call(msg,
                SERVICE_MANAGER_INTERFACE_PATH, "GetService")) {

        }
        else if (dbus_message_is_method_call(msg,
                SERVICE_MANAGER_INTERFACE_PATH, "RemoveService")) {

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
