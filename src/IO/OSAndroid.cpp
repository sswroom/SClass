#include "Stdafx.h"
#include "IO/OS.h"
#include "IO/UnixConfigFile.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

UnsafeArrayOpt<UTF8Char> IO::OS::GetDistro(UnsafeArray<UTF8Char> sbuff)
{
	return Text::StrConcatC(sbuff, UTF8STRC("Android"));
}

UnsafeArrayOpt<UTF8Char> IO::OS::GetVersion(UnsafeArray<UTF8Char> sbuff)
{
	Text::StringBuilderUTF8 sb;
	UIntOS i;
	NN<IO::ConfigFile> cfg;
	if (!IO::UnixConfigFile::Parse(CSTR("/system/build.prop")).SetTo(cfg))
	{
		Text::PString u8arr[2];
		Text::PString u8arr2[2];
		sb.ClearStr();
		Manage::Process::ExecuteProcess(CSTR("getprop"), sb);
		u8arr[1] = sb;
		NEW_CLASSNN(cfg, IO::ConfigFile());
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
						u8arr2[0].RemoveChars(1);
						u8arr2[1].RemoveChars(1);
						cfg->SetValue(CSTR(""),  u8arr2[0].ToCString().Substring(1), u8arr2[1].ToCString().Substring(1));
					}
				}
			}
			if (i != 2)
				break;
		}
	}
	UnsafeArrayOpt<UTF8Char> ret = nullptr;
	NN<Text::String> version;
	if (cfg->GetValue(CSTR("ro.build.version.sdk")).SetTo(version))
	{
		ret = version->ConcatTo(sbuff);
	}
	cfg.Delete();
	return ret;
}
