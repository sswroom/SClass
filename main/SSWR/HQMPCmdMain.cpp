#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/Console.h"
#include "IO/ConsoleWriter.h"
#include "Manage/ExceptionRecorder.h"
#include "Media/ConsoleMediaPlayer.h"
#include "Media/Decoder/FFMPEGDecoder.h"
#include "Parser/FullParserList.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	UOSInt cmdCnt;
	UTF8Char **cmdLines = progCtrl->GetCommandLines(progCtrl, cmdCnt);
	Media::Decoder::FFMPEGDecoder::Enable();

	if (cmdCnt >= 2)
	{
		Manage::ExceptionRecorder exHdlr(CSTR("HQMPCmd.log"), Manage::ExceptionRecorder::EA_CLOSE);
		{
			Media::MonitorMgr monMgr;
			Media::ColorManager colorMgr;
			Parser::FullParserList parsers;
			Media::AudioDevice audioDev;
			{
				Media::ConsoleMediaPlayer player(&monMgr, &colorMgr, parsers, &audioDev);
				if (player.IsError())
				{
					console.WriteLineC(UTF8STRC("Error in initiaizing player, no Permission?"));
				}
				else
				{
					if (!player.OpenFile({cmdLines[1], Text::StrCharCnt(cmdLines[1])}, IO::ParserType::MediaFile))
					{
						console.WriteLineC(UTF8STRC("Error in loading file"));
					}
					else
					{
						IO::Console::GetChar();
		//				progCtrl->WaitForExit(progCtrl);
					}
				}
				player.OpenVideo(0);
			}
		}
	}
	else
	{
		console.WriteLineC(UTF8STRC("Missing file name parameter"));
	}

	return 0;
}