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

    dbus_bus_request_name(conn, ServiceManager::DBUS_NAME,
            DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err)) {
        Logger::E("servicemanager", "Request servicemanager dbus name failed, error is \"%s\".",
                err.message);
        dbus_error_free(&err);
        dbus_connection_close(conn);
        dbus_connection_unref(conn);
        return -1;
    }

    DBusObjectPathVTable opVTable;

    opVTable.unregister_function = nullptr;
    opVTable.message_function = ServiceManager::HandleMessage;

    dbus_connection_register_object_path(conn,
            ServiceManager::OBJECT_PATH, &opVTable, nullptr);

    while (true) {
        DBusDispatchStatus status;

        do {
            dbus_connection_read_write_dispatch(conn, -1);
        } while ((status = dbus_connection_get_dispatch_status(conn))
                == DBUS_DISPATCH_DATA_REMAINS);

        if (status == DBUS_DISPATCH_NEED_MEMORY) {
            Logger::E("CDBusChannel", "DBus dispatching needs more memory.");
            break;
        }
    }

    dbus_connection_close(conn);
    dbus_connection_unref(conn);

    return 0;
}
