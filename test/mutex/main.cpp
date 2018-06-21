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

#include <Mutex.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

using namespace ccm;

typedef void *(*ThreadEntryFunc)(void *);

Mutex gLock;
Condition gCond(gLock);

void* ThreadEntryForMutex(void* arg)
{
    const char* name = (const char*)arg;
    printf("==== %s start ====\n", name);
    {
        Mutex::AutoLock lock(gLock);

        printf("==== %s get lock ====\n", name);
        sleep(2);
    }
    printf("==== %s release lock ====\n", name);
}

void* ThreadEntryForConditionWait(void* arg)
{
    const char* name = (const char*)arg;
    printf("==== %s start ====\n", name);
    {
        Mutex::AutoLock lock(gLock);

        printf("==== %s wait for gCond ====\n", name);
        gCond.Wait();
    }
    printf("==== %s wakeup ====\n", name);
}

void* ThreadEntryForConditionSignal(void* arg)
{
    const char* name = (const char*)arg;
    printf("==== %s start ====\n", name);
    {
        Mutex::AutoLock lock(gLock);

        printf("==== %s gCond signal begin ====\n", name);
        gCond.Signal();
    }
    printf("==== %s gCond signal end ====\n", name);
}

void* ThreadEntryForConditionSignalAll(void* arg)
{
    const char* name = (const char*)arg;
    printf("==== %s start ====\n", name);
    {
        Mutex::AutoLock lock(gLock);

        printf("==== %s gCond signal begin ====\n", name);
        gCond.SignalAll();
    }
    printf("==== %s gCond signal end ====\n", name);
}

void CreateThread(const char* name, ThreadEntryFunc func)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_t newPthread;
    pthread_create(&newPthread, &attr, func, const_cast<char*>(name));
    pthread_attr_destroy(&attr);
}

int main(int argv, char** argc)
{
    CreateThread("thread m1", ThreadEntryForMutex);
    sleep(0.2);
    CreateThread("thread m2", ThreadEntryForMutex);
    sleep(0.2);
    CreateThread("thread m3", ThreadEntryForMutex);
    sleep(6);

    CreateThread("thread w1", ThreadEntryForConditionWait);
    sleep(0.2);
    CreateThread("thread w2", ThreadEntryForConditionWait);
    sleep(0.2);
    CreateThread("thread w3", ThreadEntryForConditionWait);
    sleep(0.2);
    CreateThread("thread w4", ThreadEntryForConditionWait);
    sleep(0.2);
    CreateThread("thread s1", ThreadEntryForConditionSignal);
    sleep(2);
    CreateThread("thread sa1", ThreadEntryForConditionSignalAll);
    sleep(6);
    return 0;
}
