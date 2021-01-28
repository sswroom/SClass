#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/ExceptionLogger.h"
#include "Manage/ExceptionRecorder.h"
#include "Text/MyString.h"

const UTF8Char *Manage::ExceptionRecorder::fileName;
Manage::ExceptionRecorder::ExceptionAction Manage::ExceptionRecorder::exAction;
Int32 (__stdcall *ExceptionRecorder_Handler)(void *);

const UTF8Char *Manage::ExceptionRecorder::GetExceptionCodeName(UInt32 exCode)
{
	return (const UTF8Char*)"Unknown Exception";
}

Int32 __stdcall Manage::ExceptionRecorder::ExceptionHandler(void *exInfo)
{
	return 0;
}

Manage::ExceptionRecorder::ExceptionRecorder(const UTF8Char *fileName, ExceptionAction exAction)
{
	this->fileName = 0;
}

Manage::ExceptionRecorder::~ExceptionRecorder()
{
	this->fileName = 0;
}
