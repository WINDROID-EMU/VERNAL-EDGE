#ifndef RUNNER_BUILTINS_DISPATCHER_H
#define RUNNER_BUILTINS_DISPATCHER_H

#include "rvalue.h"
#include "ds.h"
#include "instance.h"
#include "room.h"
#include "gml_globals.h"
#include "input.h"
#include "files.h"

#ifdef __cplusplus
extern "C" {
#endif

void BuiltinsDispatcher_Init(void);
void* FUN_14183b9b0(void* self, void* other, RValue* res, int argc, uint64_t routine_idx, RValue** argv);

#ifdef __cplusplus
}
#endif

#endif // RUNNER_BUILTINS_DISPATCHER_H
