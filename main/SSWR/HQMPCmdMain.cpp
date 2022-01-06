#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/Console.h"
#include "IO/ConsoleWriter.h"
#include "Manage/ExceptionRecorder.h"
#include "Media/ConsoleMediaPlayer.h"
#include "Media/Decoder/FFMPEGDecoder.h"
#include "Parser/FullParserList.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	UOSInt cmdCnt;
	UTF8Char **cmdLines = progCtrl->GetCommandLines(progCtrl, &cmdCnt);
	Media::Decoder::FFMPEGDecoder::Enable();

	if (cmdCnt >= 2)
	{
		Manage::ExceptionRecorder *exHdlr;
		Media::ConsoleMediaPlayer *player;
		Media::MonitorMgr *monMgr;
		Media::ColorManager *colorMgr;
		Parser::ParserList *parsers;
		Media::AudioDevice *audioDev;
		NEW_CLASS(exHdlr, Manage::ExceptionRecorder((const UTF8Char*)"HQMPCmd.log", Manage::ExceptionRecorder::EA_CLOSE));
		NEW_CLASS(monMgr, Media::MonitorMgr());
		NEW_CLASS(colorMgr, Media::ColorManager());
		NEW_CLASS(parsers, Parser::FullParserList());
		NEW_CLASS(audioDev, Media::AudioDevice());
		NEW_CLASS(player, Media::ConsoleMediaPlayer(monMgr, colorMgr, parsers, audioDev));
		if (player->IsError())
		{
			console.WriteLineC(UTF8STRC("Error in initiaizing player, no Permission?"));
		}
		else
		{
			if (!player->OpenFile(cmdLines[1]))
			{
				console.WriteLineC(UTF8STRC("Error in loading file"));
			}
			else
			{
				IO::Console::GetChar();
//				progCtrl->WaitForExit(progCtrl);
			}
		}
		player->OpenVideo(0);
		DEL_CLASS(player);
		DEL_CLASS(audioDev);
		DEL_CLASS(parsers);
		DEL_CLASS(colorMgr);
		DEL_CLASS(monMgr);
		DEL_CLASS(exHdlr);
	}
	else
	{
		console.WriteLineC(UTF8STRC("Missing file name parameter"));
	}

	return 0;
}