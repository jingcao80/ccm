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

#ifndef __CCM_CORE_NATIVETHREADSTATE_H__
#define __CCM_CORE_NATIVETHREADSTATE_H__

namespace ccm {
namespace core {

enum NativeThreadState {
    kTerminated = 66,                 // TERMINATED     TS_ZOMBIE    Thread.run has returned, but Thread* still around
    kRunnable,                        // RUNNABLE       TS_RUNNING   runnable
    kTimedWaiting,                    // TIMED_WAITING  TS_WAIT      in Object.wait() with a timeout
    kSleeping,                        // TIMED_WAITING  TS_SLEEPING  in Thread.sleep()
    kBlocked,                         // BLOCKED        TS_MONITOR   blocked on a monitor
    kWaiting,                         // WAITING        TS_WAIT      in Object.wait()
    kWaitingForSignalCatcherOutput,   // WAITING        TS_WAIT      waiting for signal catcher IO to complete
    kWaitingInMainSignalCatcherLoop,  // WAITING        TS_WAIT      blocking/reading/processing signals
    kNative,                          // RUNNABLE       TS_RUNNING   running in a native method
};

}
}

#endif // __CCM_CORE_NATIVETHREADSTATE_H__
