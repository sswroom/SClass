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
		console->WriteLine((const UTF8Char*)"Error in loading SMake.cfg");
		Text::StringBuilderUTF8 sb;
		smake->GetErrorMsg(&sb);
		console->WriteLine(sb.ToString());
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
						sb.Append((const UTF8Char*)" = ");
						sb.Append(cfg->value);
						console->WriteLine(sb.ToString());
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
							console->WriteLine(sb.ToString());
							console->ResetTextColor();
						}
					}
					else
					{
						Text::StringBuilderUTF8 sb;
						sb.Append((const UTF8Char*)"Program ");
						sb.Append(cmdLines[i]);
						sb.Append((const UTF8Char*)" not found");
						console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
						console->WriteLine(sb.ToString());
						console->ResetTextColor();
					}
				}
			}
			i++;
		}
	}
	if (showHelp)
	{
		console->WriteLine((const UTF8Char*)"Usage: smake [Options] [File To Compile]");
		console->WriteLine((const UTF8Char*)"Options:");
		console->WriteLine((const UTF8Char*)"-D[object name]    Display object related file");
		console->WriteLine((const UTF8Char*)"-V                 Verbose");
		console->WriteLine((const UTF8Char*)"-a                 Assembly listing");
		console->WriteLine((const UTF8Char*)"-q                 Quiet");
		console->WriteLine((const UTF8Char*)"-s                 Single Thread");
	}
	DEL_CLASS(smake);
	DEL_CLASS(console);
	return 0;
}
