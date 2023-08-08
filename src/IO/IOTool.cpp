#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/IOTool.h"
#include "Text/MyString.h"

Bool IO::IOTool::EchoFile(Text::CStringNN fileName, Text::CString msg)
{
	IO::FileStream fs(fileName, IO::FileMode::CreateWrite, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	fs.Write(msg.v, msg.leng);
	return true;
}
