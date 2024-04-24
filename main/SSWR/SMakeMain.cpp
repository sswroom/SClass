#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/SMake.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Bool showHelp;
	Bool asmListing = false;
	UOSInt cmdCnt;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char **cmdLines = progCtrl->GetCommandLines(progCtrl, cmdCnt);
	showHelp = true;
	Bool asyncMode = true;

	Bool verbose = false;
	i = 1;
	while (i < cmdCnt)
	{
		if (cmdLines[i][0] == '-')
		{
			if (cmdLines[i][1] == 'V')
			{
				verbose = true;
			}
		}
		i++;
	}

	IO::SMake smake(CSTR("SMake.cfg"), 0, verbose?&console:0);
	if (smake.IsLoadFailed())
	{
		console.WriteLineC(UTF8STRC("Error in loading SMake.cfg"));
		Text::StringBuilderUTF8 sb;
		smake.GetLastErrorMsg(sb);
		console.WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		smake.SetCommandWriter(&console);
		i = 1;
		while (i < cmdCnt)
		{
			if (cmdLines[i][0] == '-')
			{
				if (cmdLines[i][1] == 'D')
				{
					smake.SetDebugObj({&cmdLines[i][2], Text::StrCharCnt(&cmdLines[i][2])});
				}
				else if (cmdLines[i][1] == 'V')
				{
					smake.SetMessageWriter(&console);
					NotNullPtr<const Data::ArrayListNN<IO::SMake::ConfigItem>> cfgList = smake.GetConfigList();
					NN<IO::SMake::ConfigItem> cfg;
					Text::StringBuilderUTF8 sb;
					j = 0;
					k = cfgList->GetCount();
					while (j < k)
					{
						cfg = cfgList->GetItemNoCheck(j);
						sb.ClearStr();
						sb.Append(cfg->name);
						sb.AppendC(UTF8STRC(" = "));
						sb.Append(cfg->value);
						console.WriteLineC(sb.ToString(), sb.GetLength());
						j++;
					}
				}
				else if (cmdLines[i][1] == 'a')
				{
					asmListing = true;
				}
				else if (cmdLines[i][1] == 'q')
				{
					smake.SetCommandWriter(0);
				}
				else if (cmdLines[i][1] == 's')
				{
					smake.SetThreadCnt(1);
				}
				else if (cmdLines[i][1] == 'S')
				{
					asyncMode = false;
				}
			}
			i++;
		}

		if (asyncMode)
		{
			smake.SetAsyncMode(true);
		}
		i = 1;
		while (i < cmdCnt)
		{
			if (cmdLines[i][0] != '-')
			{
				UOSInt cmdLineLen = Text::StrCharCnt(cmdLines[i]); 
				if (Text::StrEqualsC(cmdLines[i], cmdLineLen, UTF8STRC("clean")))
				{
					showHelp = false;
					smake.CleanFiles();
				}
				else
				{
					if (smake.HasProg({cmdLines[i], cmdLineLen}))
					{
						showHelp = false;
						if (!smake.CompileProg({cmdLines[i], cmdLineLen}, asmListing))
						{
							Text::StringBuilderUTF8 sb;
							smake.GetLastErrorMsg(sb);
							console.SetTextColor(Text::StandardColor::Red);
							console.WriteLineC(sb.ToString(), sb.GetLength());
							console.ResetTextColor();
						}
					}
					else
					{
						Text::StringBuilderUTF8 sb;
						sb.AppendC(UTF8STRC("Program "));
						sb.AppendC(cmdLines[i], cmdLineLen);
						sb.AppendC(UTF8STRC(" not found"));
						console.SetTextColor(Text::StandardColor::Red);
						console.WriteLineC(sb.ToString(), sb.GetLength());
						console.ResetTextColor();
					}
				}
			}
			i++;
		}
		if (asyncMode && !showHelp)
		{
			smake.AsyncPostCompile();
			if (smake.HasError())
			{
				Text::StringBuilderUTF8 sb;
				smake.GetLastErrorMsg(sb);
				console.SetTextColor(Text::StandardColor::Red);
				console.WriteLineC(sb.ToString(), sb.GetLength());
				console.ResetTextColor();
			}
		}
	}
	if (showHelp)
	{
		console.WriteLineC(UTF8STRC("Usage: smake [Options] [File To Compile]"));
		console.WriteLineC(UTF8STRC("Options:"));
		console.WriteLineC(UTF8STRC("-D[object name]    Display object related file"));
		console.WriteLineC(UTF8STRC("-V                 Verbose"));
		console.WriteLineC(UTF8STRC("-a                 Assembly listing"));
		console.WriteLineC(UTF8STRC("-q                 Quiet"));
		console.WriteLineC(UTF8STRC("-s                 Single Thread"));
		console.WriteLineC(UTF8STRC("-S                 Sync Mode (disable Async Mode)"));
	}
	return 0;
}
