#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 *buff;
	UInt64 flen;
	
	IO::FileStream fs(CSTR("test.json"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	flen = fs.GetLength();
	if (flen > 0)
	{
		buff = MemAlloc(UInt8, (UOSInt)flen + 1);
		fs.Read(buff, (UOSInt)flen);
		buff[flen] = 0;
		Text::JSONBase *obj = Text::JSONBase::ParseJSONStr(Text::CString(buff, flen));
		if (obj)
		{
			IO::ConsoleWriter console;
			Text::StringBuilderUTF8 sb;
			console.WriteLineC(UTF8STRC("Original:"));
			console.WriteLineC(buff, flen);
			console.WriteLine();
			console.WriteLineC(UTF8STRC("Parsed:"));
			obj->ToJSONString(&sb);
			console.WriteLineC(sb.ToString(), sb.GetLength());
			obj->EndUse();
		}
		MemFree(buff);
	}
	return 0;
}
