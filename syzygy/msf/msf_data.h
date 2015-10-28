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

#ifndef SYZYGY_MSF_MSF_DATA_H_
#define SYZYGY_MSF_MSF_DATA_H_

#include <windows.h>

#include "base/basictypes.h"
#include "syzygy/common/assertions.h"
#include "syzygy/msf/msf_constants.h"

namespace msf {

// Multi-Stream Format (MSF) Header
// See http://code.google.com/p/pdbparser/wiki/MSF_Format
struct MsfHeader {
  uint8 magic_string[kMsfHeaderMagicStringSize];
  uint32 page_size;
  uint32 free_page_map;
  uint32 num_pages;
  uint32 directory_size;
  uint32 reserved;
  uint32 root_pages[kMsfMaxDirPages];
};
COMPILE_ASSERT_IS_POD_OF_SIZE(MsfHeader, 344);

}  // namespace msf

#endif  // SYZYGY_MSF_MSF_DATA_H_