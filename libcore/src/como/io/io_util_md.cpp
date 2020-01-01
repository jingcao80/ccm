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

/*
 * Copyright (c) 2001, 2010, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#include "libcore.h"
#include <ccmtypes.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

using namespace como;

/*
 * Retry the operation if it is interrupted
 */
#define RESTARTABLE(_cmd, _result) do { \
    do { \
        _result = _cmd; \
    } while((_result == -1) && (errno == EINTR)); \
} while(0)

int handleOpen(const char* path, int oflag, int mode)
{
    int fd;
    RESTARTABLE(open64(path, oflag, mode), fd);
    if (fd != -1) {
        struct stat64 buf64;
        int result;
        RESTARTABLE(fstat64(fd, &buf64), result);
        if (result != -1) {
            if (S_ISDIR(buf64.st_mode)) {
                close(fd);
                errno = EISDIR;
                fd = -1;
            }
        }
        else {
            close(fd);
            fd = -1;
        }
    }
    return fd;
}

ECode fileOpen(const char* path, int flags, int* fd)
{
#if defined(__linux__) || defined(_ALLBSD_SOURCE)
    /* Remove trailing slashes, since the kernel won't */
    char *p = (char *)path + strlen(path) - 1;
    while ((p > path) && (*p == '/'))
        *p-- = '\0';
#endif
    *fd = handleOpen(path, flags, 0666);
    if (*fd != -1) {
        return NOERROR;
    }
    else {
        return como::io::E_FILE_NOT_FOUND_EXCEPTION;
    }
}
