#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UInt8 *buff;
	UInt64 flen;
	
	IO::FileStream fs(CSTR("test.json"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	flen = fs.GetLength();
	if (flen > 0)
	{
		buff = MemAlloc(UInt8, (UIntOS)flen + 1);
		fs.Read(Data::ByteArray(buff, (UIntOS)flen));
		buff[flen] = 0;
		NN<Text::JSONBase> obj;
		if (Text::JSONBase::ParseJSONStr(Text::CStringNN(buff, (UIntOS)flen)).SetTo(obj))
		{
			IO::ConsoleWriter console;
			Text::StringBuilderUTF8 sb;
			console.WriteLine(CSTR("Original:"));
			console.WriteLine(Text::CStringNN(buff, (UIntOS)flen));
			console.WriteLine();
			console.WriteLine(CSTR("Parsed:"));
			obj->ToJSONString(sb);
			console.WriteLine(sb.ToCString());
			obj->EndUse();
		}
		MemFree(buff);
	}
	return 0;
}
