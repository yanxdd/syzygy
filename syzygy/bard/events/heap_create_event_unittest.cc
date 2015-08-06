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

#include "syzygy/bard/events/heap_create_event.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "syzygy/bard/unittest_util.h"
#include "syzygy/bard/backdrops/heap_backdrop.h"

namespace bard {
namespace events {

namespace {

using bard::backdrops::HeapBackdrop;
using bard::events::HeapCreateEvent;
using testing::Return;

const HANDLE kLiveHeap = reinterpret_cast<HANDLE>(0x4197FC83);
const HANDLE kTraceHeap = reinterpret_cast<HANDLE>(0xAB12CD34);
const DWORD kOptions = 0;
const SIZE_T kInitialSize = 1;
const SIZE_T kMaximumSize = 1000;

class HeapCreateEventTest : public testing::Test {
 public:
  HeapCreateEventTest() : heap_create_event_(kOptions,
                                             kInitialSize,
                                             kMaximumSize,
                                             kTraceHeap) {}

  MOCK_METHOD3(FakeCall, HANDLE(DWORD options,
                                SIZE_T initial_size,
                                SIZE_T maximum_size));

 protected:
  HeapCreateEvent heap_create_event_;
  HeapBackdrop backdrop_;
};

}  // namespace

TEST_F(HeapCreateEventTest, TestSuccessCall) {
  backdrop_.set_heap_create(base::Bind(&HeapCreateEventTest::FakeCall,
                                       base::Unretained(this)));

  EXPECT_CALL(*this, FakeCall(kOptions, kInitialSize, kMaximumSize))
      .WillOnce(Return(kLiveHeap));

  EXPECT_TRUE(heap_create_event_.Play(reinterpret_cast<void*>(&backdrop_)));

  testing::CheckTraceLiveMapContains(backdrop_.heap_map(),
                                     kTraceHeap,
                                     kLiveHeap);
}

TEST_F(HeapCreateEventTest, TestFailCall) {
  backdrop_.set_heap_create(base::Bind(&HeapCreateEventTest::FakeCall,
                                       base::Unretained(this)));

  EXPECT_CALL(*this, FakeCall(kOptions, kInitialSize, kMaximumSize))
      .WillOnce(Return(nullptr));

  EXPECT_FALSE(heap_create_event_.Play(reinterpret_cast<void*>(&backdrop_)));

  testing::CheckTraceLiveMapNotContain(backdrop_.heap_map(),
                                       kTraceHeap,
                                       kLiveHeap);
}

}  // namespace events
}  // namespace bard
