#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Util.h"

#include <stdio.h>

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Text::StringBuilderUTF8 sb;
	IO::FileStream fs(CSTR("/mnt/raid2_3/Progs/DownList2.txt"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	Text::UTF8Reader reader(fs);
	while (reader.ReadLine(sb, 4096))
	{
		if (!Text::UTF8Util::ValidStr(sb.ToString()))
		{
			printf("Invalid char found before split, id = %s\r\n", sb.ToString());
		}
		else
		{
			printf("Invalid char found, %s\r\n", sb.ToString());
		}

		sb.ClearStr();
	}
	return 0;
}
