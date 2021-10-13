#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/IOTool.h"
#include "Text/MyString.h"

Bool IO::IOTool::EchoFile(const UTF8Char *fileName, const Char *msg)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FileMode::CreateWrite, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	UOSInt fileSize = Text::StrCharCnt(msg);
	fs->Write((const UInt8*)msg, fileSize);
	DEL_CLASS(fs);
	return true;
}
