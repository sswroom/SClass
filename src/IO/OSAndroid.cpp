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
		Text::PString u8arr[2];
		Text::PString u8arr2[2];
		sb.ClearStr();
		Manage::Process::ExecuteProcess(UTF8STRC("getprop"), &sb);
		u8arr[1].v = sb.ToString();
		u8arr[1].leng = sb.GetLength();
		NEW_CLASS(cfg, IO::ConfigFile());
		while (1)
		{
			i = Text::StrSplitP(u8arr, 2, u8arr[1], '\n');
			if (Text::StrSplitTrimP(u8arr2, 2, u8arr[0], ':') == 2)
			{
				if (Text::StrEndsWithC(u8arr2[0].v, u8arr2[0].leng, UTF8STRC("]")) && Text::StrEndsWithC(u8arr2[1].v, u8arr2[1].leng, UTF8STRC("]")))
				{
					u8arr2[0].v[u8arr2[0].leng - 1] = 0;
					u8arr2[1].v[u8arr2[1].leng - 1] = 0;
					if (u8arr2[0].v[0] == '[' && u8arr2[1].v[0] == '[')
					{
						cfg->SetValue(0, 0, &u8arr2[0].v[1], u8arr2[0].leng - 2, &u8arr2[1].v[1], u8arr2[1].leng - 2);
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
		Text::String *version = cfg->GetValue(UTF8STRC("ro.build.version.sdk"));
		if (version)
		{
			ret = version->ConcatTo(sbuff);
		}
		DEL_CLASS(cfg);
		return ret;
	}
	return 0;
}
