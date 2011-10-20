// Copyright 2011 Google Inc.
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
// Call trace event parsing classes.

#ifndef SYZYGY_CALL_TRACE_PARSE_ENGINE_H_
#define SYZYGY_CALL_TRACE_PARSE_ENGINE_H_

#include <map>
#include <set>
#include <string>

#include "base/file_path.h"
#include "sawbuck/sym_util/types.h"
#include "syzygy/common/align.h"
#include "syzygy/core/address.h"
#include "syzygy/core/address_space.h"
#include "syzygy/call_trace/call_trace_defs.h"
#include "syzygy/pe/pe_file.h"

typedef struct _EVENT_TRACE EVENT_TRACE;

namespace call_trace {
namespace parser {

typedef sym_util::ModuleInformation ModuleInformation;
typedef uint64 AbsoluteAddress64;
typedef uint64 Size64;
typedef core::AddressSpace<AbsoluteAddress64, Size64, ModuleInformation>
    ModuleSpace;

// Forward declaration.
class ParseEventHandler;

// This base class defines the implements the common event dispatching and
// module tracking for all supported parse engines. It also declares the
// abstact interface a parse engine exposes to its clients.
class ParseEngine {
 public:
  virtual ~ParseEngine();

  // Returns a short human readable name by which this parse engine can be
  // recognized.
  const char* name() const;

  // Returns true if an error occurred while parsing the trace files.
  bool error_occurred() const;

  // Registers an event handler with this trace-file parse engine.
  void set_event_handler(ParseEventHandler* event_handler);

  // Returns true if the file given by @p trace_file_path is parseable by this
  // parse engine.
  virtual bool IsRecognizedTraceFile(const FilePath& trace_file_path) = 0;

  // Opens the trace log given by @p trace_file_path and prepares it for
  // consumption. It is an error to call this method given a file that
  // will not be recognized by the parse engine.
  virtual bool OpenTraceFile(const FilePath& trace_file_path) = 0;

  // Consume all events across all currently open trace files and for each
  // event call the dispatcher to notify the event handler.
  virtual bool ConsumeAllEvents() = 0;

  // Close all currently open trace files.
  virtual bool CloseAllTraceFiles() = 0;

  // Given an address and a process id, returns the module in memory at that
  // address. Returns NULL if no such module exists.
  const ModuleInformation* GetModuleInformation(uint32 process_id,
                                                AbsoluteAddress64 addr) const;

 protected:
  typedef std::map<uint32, ModuleSpace> ProcessMap;
  typedef std::set<uint32> ProcessSet;

  explicit ParseEngine(const char* const name);

  // Set or reset the error flag.
  void set_error_occurred(bool value);

  // Registers a module in the address space of the process denoted by
  // @p process_id.
  bool AddModuleInformation(DWORD process_id,
                            const ModuleInformation& module_info);

  // Unregisters a module from the address space of the process denoted by
  // @p process_id.
  bool RemoveModuleInformation(DWORD process_id,
                               const ModuleInformation& module_info);

  // The main entry point by which trace events are dispatched to the
  // event handler.
  virtual bool DispatchEvent(EVENT_TRACE* event);

  // Parses and dispatches function entry and exit events.
  bool DispatchEntryExitEvent(EVENT_TRACE* event, TraceEventType type);

  // Parses and dispatches batch function entry events.
  bool DispatchBatchEnterEvent(EVENT_TRACE* event);

  // Parses and dispatches dynamic library events (i.e., process and thread
  // attach/detach events).
  bool DispatchModuleEvent(EVENT_TRACE* event, TraceEventType type);

  // The name by which this parse engine is known.
  std::string name_;

  // The event handler to be notified on trace events.
  ParseEventHandler* event_handler_;

  // For each process, we store its point of view of the world.
  ProcessMap processes_;

  // The set of processes of interest. That is, those that have had code
  // run in the instrumented module. These are the only processes for which
  // we are interested in OnProcessEnded events.
  ProcessSet matching_process_ids_;

  // Flag indicating whether or not an error has occured in parsing the trace
  // event stream.
  bool error_occurred_;

 private:
  DISALLOW_COPY_AND_ASSIGN(ParseEngine);
};

}  // namespace call_trace::parser
}  // namespace call_trace

#endif  // SYZYGY_CALL_TRACE_PARSE_ENGINE_H_
