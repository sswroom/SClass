#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::FileStream *fs;
	UInt8 *buff;
	UInt64 flen;
	
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"test.json", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	flen = fs->GetLength();
	if (flen > 0)
	{
		buff = MemAlloc(UInt8, (UOSInt)flen + 1);
		fs->Read(buff, (UOSInt)flen);
		buff[flen] = 0;
		Text::JSONBase *obj = Text::JSONBase::ParseJSONStr(buff);
		if (obj)
		{
			IO::ConsoleWriter console;
			Text::StringBuilderUTF8 sb;
			console.WriteLineC(UTF8STRC("Original:"));
			console.WriteLine(buff);
			console.WriteLine();
			console.WriteLineC(UTF8STRC("Parsed:"));
			obj->ToJSONString(&sb);
			console.WriteLineC(sb.ToString(), sb.GetLength());
			obj->EndUse();
		}
		MemFree(buff);
	}
	DEL_CLASS(fs);
	return 0;
}
