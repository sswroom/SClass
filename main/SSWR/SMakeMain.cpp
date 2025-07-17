#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/SMake.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Bool showHelp;
	Bool asmListing = false;
	Bool infoMode = false;
	UOSInt cmdCnt;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UnsafeArray<UnsafeArray<UTF8Char>> cmdLines = progCtrl->GetCommandLines(progCtrl, cmdCnt);
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
		console.WriteLine(CSTR("Error in loading SMake.cfg"));
		Text::StringBuilderUTF8 sb;
		smake.GetLastErrorMsg(sb);
		console.WriteLine(sb.ToCString());
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
					NN<const Data::ArrayListNN<IO::SMake::ConfigItem>> cfgList = smake.GetConfigList();
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
						console.WriteLine(sb.ToCString());
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
				else if (cmdLines[i][1] == 'i')
				{
					infoMode = true;
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
					if (infoMode)
					{
						NN<const IO::SMake::ProgramItem> progItem;
						NN<Text::String> s;
						UOSInt i2;
						UOSInt j2;
						if (smake.GetProgItem({cmdLines[i], cmdLineLen}).SetTo(progItem))
						{
							showHelp = false;
							console.Write(CSTR("Program Name: "));
							console.SetTextColor(Text::StandardColor::Red);
							console.WriteLine(progItem->name->ToCString());
							console.ResetTextColor();
							console.Write(CSTR("Source File: "));
							if (progItem->srcFile.SetTo(s))
							{
								console.SetTextColor(Text::StandardColor::Red);
								console.WriteLine(s->ToCString());
								console.ResetTextColor();
							}
							else
							{
								console.WriteLine();
							}
							console.Write(CSTR("Compile Config: "));
							if (progItem->compileCfg.SetTo(s))
							{
								console.SetTextColor(Text::StandardColor::Red);
								console.WriteLine(s->ToCString());
								console.ResetTextColor();
							}
							else
							{
								console.WriteLine();
							}
							i2 = 0;
							j2 = progItem->libs.GetCount();
							while (i2 < j2)
							{
								console.Write(CSTR("Library: "));
								console.SetTextColor(Text::StandardColor::Red);
								console.WriteLine(progItem->libs.GetItemNoCheck(i2)->ToCString());
								console.ResetTextColor();
								i2++;
							}
							i2 = 0;
							j2 = progItem->subItems.GetCount();
							while (i2 < j2)
							{
								console.Write(CSTR("Sub-Items: "));
								console.SetTextColor(Text::StandardColor::Red);
								console.WriteLine(progItem->subItems.GetItemNoCheck(i2)->ToCString());
								console.ResetTextColor();
								i2++;
							}
						}
						else
						{
							Text::StringBuilderUTF8 sb;
							sb.AppendC(UTF8STRC("Program "));
							sb.AppendC(cmdLines[i], cmdLineLen);
							sb.AppendC(UTF8STRC(" not found"));
							console.SetTextColor(Text::StandardColor::Red);
							console.WriteLine(sb.ToCString());
							console.ResetTextColor();
						}
					}
					else if (smake.HasProg({cmdLines[i], cmdLineLen}))
					{
						showHelp = false;
						if (!smake.CompileProg({cmdLines[i], cmdLineLen}, asmListing))
						{
							Text::StringBuilderUTF8 sb;
							smake.GetLastErrorMsg(sb);
							console.SetTextColor(Text::StandardColor::Red);
							console.WriteLine(sb.ToCString());
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
						console.WriteLine(sb.ToCString());
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
				console.WriteLine(sb.ToCString());
				console.ResetTextColor();
			}
		}
	}
	if (showHelp)
	{
		console.WriteLine(CSTR("Usage: smake [Options] [File To Compile]"));
		console.WriteLine(CSTR("Options:"));
		console.WriteLine(CSTR("-D[object name]    Display object related file"));
		console.WriteLine(CSTR("-V                 Verbose"));
		console.WriteLine(CSTR("-a                 Assembly listing"));
		console.WriteLine(CSTR("-q                 Quiet"));
		console.WriteLine(CSTR("-s                 Single Thread"));
		console.WriteLine(CSTR("-S                 Sync Mode (disable Async Mode)"));
		console.WriteLine(CSTR("-i                 Show Object Info only"));
	}
	return 0;
}
