#include "Stdafx.h"
#include "IO/OS.h"
#include "IO/UnixConfigFile.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

UTF8Char *IO::OS::GetDistro(UTF8Char *sbuff)
{
	return Text::StrConcatC(sbuff, UTF8STRC("Android"));
}

UTF8Char *IO::OS::GetVersion(UTF8Char *sbuff)
{
	Text::StringBuilderUTF8 sb;
	OSInt i;
	IO::ConfigFile *cfg = IO::UnixConfigFile::Parse((const UTF8Char*)"/system/build.prop");
	if (cfg == 0)
	{
		UTF8Char *u8arr[2];
		UTF8Char *u8arr2[2];
		sb.ClearStr();
		Manage::Process::ExecuteProcess((const UTF8Char*)"getprop", &sb);
		u8arr[1] = sb.ToString();
		NEW_CLASS(cfg, IO::ConfigFile());
		while (1)
		{
			i = Text::StrSplit(u8arr, 2, u8arr[1], '\n');
			if (Text::StrSplitTrim(u8arr2, 2, u8arr[0], ':') == 2)
			{
				if (Text::StrEndsWith(u8arr2[0], (const UTF8Char*)"]") && Text::StrEndsWith(u8arr2[1], (const UTF8Char*)"]"))
				{
					u8arr2[0][Text::StrCharCnt(u8arr2[0]) - 1] = 0;
					u8arr2[1][Text::StrCharCnt(u8arr2[1]) - 1] = 0;
					if (u8arr2[0][0] == '[' && u8arr2[1][0] == '[')
					{
						cfg->SetValue(0, &u8arr2[0][1], &u8arr2[1][1]);
					}
				}
			}
			if (i != 2)
				break;
		}
	}
	if (cfg)
	{
		UTF8Char *ret = 0;
		const UTF8Char *version = cfg->GetValue((const UTF8Char*)"ro.build.version.sdk");
		if (version)
		{
			ret = Text::StrConcat(sbuff, version);
		}
		DEL_CLASS(cfg);
		return ret;
	}
	return 0;
}
