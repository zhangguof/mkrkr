#ifndef __TP_STUB1_H__
#define __TP_STUB1_H__
// build in APPLE

#include "tjsCommHead.h"

#include "tjsNative.h"
#include "tjsDebug.h"
#include "DebugIntf.h"
#include "MsgIntf.h"
#include "tjsDictionary.h"
#include "tjsArray.h"
#include "tjsBinarySerializer.h"
#include "ScriptMgnIntf.h"
#include "StorageIntf.h"
#include "StaticPluginImpl.h"
#include "TextStream.h"
#include "EventIntf.h"
#include "MsgImpl.h"

extern iTJSDispatch2 * TVPGetScriptDispatch();

//tp_stub.cpp
extern tjs_int TVPPluginGlobalRefCount;

extern void * TVPGetImportFuncPtr(const char *name);
extern bool TVPInitImportStub(iTVPFunctionExporter * exporter);
extern void TVPUninitImportStub();


#endif