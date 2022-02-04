#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/IOTool.h"
#include "Text/MyString.h"

Bool IO::IOTool::EchoFile(Text::CString fileName, Text::CString msg)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::CreateWrite, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	fs->Write(msg.v, msg.leng);
	DEL_CLASS(fs);
	return true;
}
