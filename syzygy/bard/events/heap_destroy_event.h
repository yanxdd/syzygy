// Copyright 2015 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Declares an event to represent a HeapDestroy function call.
#ifndef SYZYGY_BARD_EVENTS_HEAP_DESTROY_EVENT_H_
#define SYZYGY_BARD_EVENTS_HEAP_DESTROY_EVENT_H_

#include "syzygy/bard/events/linked_event.h"

namespace bard {
namespace events {

// An event that wraps a call to HeapDestroy, to be played against a
// HeapBackdrop.
class HeapDestroyEvent : public LinkedEvent {
 public:
  HeapDestroyEvent(HANDLE trace_heap, BOOL trace_succeeded);

  // Event implementation.
  const char* name() const override;

  // @name Accessors.
  // @{
  HANDLE trace_heap() const { return trace_heap_; }
  BOOL trace_succeeded() const { return trace_succeeded_; }
  // @}

 private:
  // LinkedEvent implementation.
  bool PlayImpl(void* backdrop) override;

  // Argument to HeapDestory.
  HANDLE trace_heap_;

  // Recorded return value.
  BOOL trace_succeeded_;

  DISALLOW_COPY_AND_ASSIGN(HeapDestroyEvent);
};

}  // namespace events
}  // namespace bard

#endif  // SYZYGY_BARD_EVENTS_HEAP_DESTROY_EVENT_H_
