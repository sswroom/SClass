#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/SMake.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter *console;
	IO::SMake *smake;
	Bool showHelp;
	Bool asmListing = false;
	UOSInt cmdCnt;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char **cmdLines = progCtrl->GetCommandLines(progCtrl, &cmdCnt);
	NEW_CLASS(console, IO::ConsoleWriter());
	showHelp = true;

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

	NEW_CLASS(smake, IO::SMake((const UTF8Char*)"SMake.cfg", 0, verbose?console:0));
	if (smake->IsLoadFailed())
	{
		console->WriteLineC(UTF8STRC("Error in loading SMake.cfg"));
		Text::StringBuilderUTF8 sb;
		smake->GetErrorMsg(&sb);
		console->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		smake->SetCommandWriter(console);
		i = 1;
		while (i < cmdCnt)
		{
			if (cmdLines[i][0] == '-')
			{
				if (cmdLines[i][1] == 'D')
				{
					smake->SetDebugObj(&cmdLines[i][2]);
				}
				else if (cmdLines[i][1] == 'V')
				{
					smake->SetMessageWriter(console);
					Data::ArrayList<IO::SMake::ConfigItem*> *cfgList = smake->GetConfigList();
					IO::SMake::ConfigItem *cfg;
					Text::StringBuilderUTF8 sb;
					j = 0;
					k = cfgList->GetCount();
					while (j < k)
					{
						cfg = cfgList->GetItem(j);
						sb.ClearStr();
						sb.Append(cfg->name);
						sb.AppendC(UTF8STRC(" = "));
						sb.Append(cfg->value);
						console->WriteLineC(sb.ToString(), sb.GetLength());
						j++;
					}
				}
				else if (cmdLines[i][1] == 'a')
				{
					asmListing = true;
				}
				else if (cmdLines[i][1] == 'q')
				{
					smake->SetCommandWriter(0);
				}
				else if (cmdLines[i][1] == 's')
				{
					smake->SetThreadCnt(1);
				}
			}
			i++;
		}

		i = 1;
		while (i < cmdCnt)
		{
			if (cmdLines[i][0] != '-')
			{
				if (Text::StrEquals(cmdLines[i], (const UTF8Char*)"clean"))
				{
					showHelp = false;
					smake->CleanFiles();
				}
				else
				{
					if (smake->HasProg(cmdLines[i]))
					{
						showHelp = false;
						if (!smake->CompileProg(cmdLines[i], asmListing))
						{
							Text::StringBuilderUTF8 sb;
							smake->GetErrorMsg(&sb);
							console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
							console->WriteLineC(sb.ToString(), sb.GetLength());
							console->ResetTextColor();
						}
					}
					else
					{
						Text::StringBuilderUTF8 sb;
						sb.AppendC(UTF8STRC("Program "));
						sb.Append(cmdLines[i]);
						sb.AppendC(UTF8STRC(" not found"));
						console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
						console->WriteLineC(sb.ToString(), sb.GetLength());
						console->ResetTextColor();
					}
				}
			}
			i++;
		}
	}
	if (showHelp)
	{
		console->WriteLineC(UTF8STRC("Usage: smake [Options] [File To Compile]"));
		console->WriteLineC(UTF8STRC("Options:"));
		console->WriteLineC(UTF8STRC("-D[object name]    Display object related file"));
		console->WriteLineC(UTF8STRC("-V                 Verbose"));
		console->WriteLineC(UTF8STRC("-a                 Assembly listing"));
		console->WriteLineC(UTF8STRC("-q                 Quiet"));
		console->WriteLineC(UTF8STRC("-s                 Single Thread"));
	}
	DEL_CLASS(smake);
	DEL_CLASS(console);
	return 0;
}
