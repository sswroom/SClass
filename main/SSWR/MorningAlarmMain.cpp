#include "Stdafx.h"

#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/DateTime.h"
#include "Data/RandomOS.h"

//#include "IO/Console.h"
#include "IO/ConsoleInput.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/FileParser.h"
#include "IO/StreamData.h"
#include "IO/StmData/FileData.h"

#include "Media/AudioDevice.h"
#include "Media/IMediaSource.h"
#include "Media/IAudioSource.h"
#include "Media/MediaFile.h"
#include "Media/RefClock.h"
#include "Media/ClockSpeechCh.h"

#include "Net/HKOWeather.h"
#include "Net/NTPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"

#include "Parser/FileParser/WAVParser.h"

#include "Sync/Mutex.h"
#include "Sync/Event.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

#include "Text/MyString.h"

#define NTPHOST CSTR("stdtime.gov.hk")

Media::IAudioRenderer *audOut;
Bool ToStop;
Bool threadRunning;
Data::DateTime *startDt;
Text::String *audioDevice; //L"Realtek HD Audio output"
IO::ConsoleWriter *console;
NotNullPtr<Net::SocketFactory> sockf;
Net::SSLEngine *ssl;
Text::EncodingFactory *encFact;
Net::NTPClient *timeCli;
Data::DateTime *tmpDt;


UInt32 __stdcall PlayThread(void *obj)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Parser::FileParser::WAVParser *parser;
	Media::RefClock *clk;
	Data::DateTime *currDt;
	Data::DateTime *updateDt;
	Data::ArrayList<Media::MediaFile*> *stmList;
	Media::MediaFile *file;
	Data::Random *random;
	UOSInt i;
	UOSInt currStm;
	Net::HKOWeather::WeatherSignal currSignal;
	Net::HKOWeather::WeatherSignal nextSignal;
	Bool typhoonStop = false;

	threadRunning = true;

	NEW_CLASS(parser, Parser::FileParser::WAVParser());
	NEW_CLASS(currDt, Data::DateTime());
	NEW_CLASS(updateDt, Data::DateTime());
	NEW_CLASS(random, Data::RandomOS());
	NEW_CLASS(stmList, Data::ArrayList<Media::MediaFile*>());

	updateDt->SetCurrTimeUTC();
	updateDt->AddMinute(-15);
	currSignal = Net::HKOWeather::WS_NONE;

	i = 1;
	while (true)
	{
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Alarm"));
		sptr = Text::StrUOSInt(sptr, i);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".wav"));

		{
			IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
			file = (Media::MediaFile*)parser->ParseFile(fd, 0, IO::ParserType::MediaFile);
		}
		if (file)
		{
			stmList->Add(file);
			i++;
		}
		else
		{
			break;
		}
	}

	NEW_CLASS(clk, Media::RefClock());

	while (!ToStop)
	{
		currDt->SetCurrTime();
		if (currDt->DiffMS(updateDt) >= 900000)
		{
			updateDt->SetCurrTime();
			if (timeCli->GetServerTime(NTPHOST, 123, tmpDt))
			{
				tmpDt->SetAsComputerTime();
			}

			nextSignal = Net::HKOWeather::GetSignalSummary(sockf, ssl, encFact);
			if ((nextSignal & Net::HKOWeather::WS_TYPHOON_MASK) != 0 || (currSignal & Net::HKOWeather::WS_TYPHOON_MASK) != 0)
			{
				sptr = currDt->ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				sptr = Text::StrConcatC(sptr, UTF8STRC(" Typhoon now is "));
				if ((nextSignal & Net::HKOWeather::WS_TYPHOON_MASK) == Net::HKOWeather::WS_TYPHOON_1)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(" No. 1"));
					typhoonStop = false;
				}
				else if ((nextSignal & Net::HKOWeather::WS_TYPHOON_MASK) == Net::HKOWeather::WS_TYPHOON_3)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(" No. 3"));
					typhoonStop = false;
				}
				else if ((nextSignal & Net::HKOWeather::WS_TYPHOON_MASK) == Net::HKOWeather::WS_TYPHOON_8NE)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(" No. 8 NE"));
					typhoonStop = true;
				}
				else if ((nextSignal & Net::HKOWeather::WS_TYPHOON_MASK) == Net::HKOWeather::WS_TYPHOON_8SE)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(" No. 8 SE"));
					typhoonStop = true;
				}
				else if ((nextSignal & Net::HKOWeather::WS_TYPHOON_MASK) == Net::HKOWeather::WS_TYPHOON_8NW)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(" No. 8 NW"));
					typhoonStop = true;
				}
				else if ((nextSignal & Net::HKOWeather::WS_TYPHOON_MASK) == Net::HKOWeather::WS_TYPHOON_8SW)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(" No. 8 SW"));
					typhoonStop = true;
				}
				else if ((nextSignal & Net::HKOWeather::WS_TYPHOON_MASK) == Net::HKOWeather::WS_TYPHOON_9)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(" No. 9"));
					typhoonStop = true;
				}
				else if ((nextSignal & Net::HKOWeather::WS_TYPHOON_MASK) == Net::HKOWeather::WS_TYPHOON_10)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(" No. 10"));
					typhoonStop = true;
				}
				else
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(" none"));
					typhoonStop = false;
				}
				console->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
			}
			currSignal = nextSignal;
		}
		if (*currDt > *startDt && !typhoonStop)
		{
			break;
		}
		else
		{
			Sync::SimpleThread::Sleep(1000);
		}
	}
	if (!ToStop)
	{
		console->WriteLineC(UTF8STRC("Alerting!"));
	}
	currStm = (UInt32)-1;
	while (!ToStop)
	{
		if (stmList->GetCount() > 0)
		{
			if (stmList->GetCount() <= 1)
			{
				currStm = 0;
			}
			else
			{
				i = currStm;
				while (i == currStm)
				{
					i = (UInt32)random->NextInt32() % stmList->GetCount();
				}
				currStm = i;
			}
			file = (Media::MediaFile*)stmList->GetItem(currStm);

			if (audOut->BindAudio((Media::IAudioSource*)file->GetStream(0, 0)))
			{
				audOut->AudioInit(clk);
				audOut->Start();
			}

			while (!ToStop)
			{
				if (!audOut->IsPlaying())
				{
					break;
				}
				Sync::SimpleThread::Sleep(10);
			}
		}
		if (!ToStop)
		{
			currDt->SetCurrTime();
			Media::IAudioSource *astm = Media::ClockSpeechCh::GetSpeech(currDt);
			if (audOut->BindAudio(astm))
			{
				audOut->AudioInit(clk);
				audOut->Start();
				while (!ToStop)
				{
					if (!audOut->IsPlaying())
					{
						break;
					}
					Sync::SimpleThread::Sleep(10);
				}
			}
			DEL_CLASS(astm);
		}
	}
	audOut->Stop();

	DEL_CLASS(clk);

	i = stmList->GetCount();
	while (i-- > 0)
	{
		file = (Media::MediaFile *)stmList->RemoveAt(i);
		DEL_CLASS(file);
	}
	DEL_CLASS(currDt);
	DEL_CLASS(updateDt);
	DEL_CLASS(random);
	DEL_CLASS(parser);
	DEL_CLASS(stmList);

	threadRunning = false;
	return 0;
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UTF8Char buff[256];
	UTF8Char *sptr;
	Text::String **sel;
	ToStop = false;
	threadRunning = false;
	UOSInt i;
	UOSInt devCnt;
	IO::ConsoleInput::InputReturnType irt;

	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASSNN(sockf, Net::OSSocketFactory(true));
	ssl = Net::SSLEngineFactory::Create(sockf, true);
	NEW_CLASS(encFact, Text::EncodingFactory());
	NEW_CLASS(timeCli, Net::NTPClient(sockf, 14562));
	NEW_CLASS(tmpDt, Data::DateTime());
	devCnt = i = Media::AudioDevice::GetDeviceCount();
	sel = MemAlloc(Text::String*, devCnt);
	while (i-- > 0)
	{
		sptr = Media::AudioDevice::GetDeviceName(buff, i);
		sel[i] = Text::String::New(buff, (UOSInt)(sptr - buff)).Ptr();
	}
	if (timeCli->GetServerTime(NTPHOST, 123, tmpDt))
	{
		tmpDt->SetAsComputerTime();
	}
	console->WriteStrC(UTF8STRC("Select audio device: "));
	irt = IO::ConsoleInput::InputSelect(console, sel, devCnt, &i);
	if (irt == IO::ConsoleInput::IRT_TAB || irt == IO::ConsoleInput::IRT_ENTER)
	{
		audioDevice = sel[i]->Clone().Ptr();
		i = devCnt;
		while (i-- > 0)
		{
			sel[i]->Release();
		}
		MemFree(sel);

		audOut = Media::AudioDevice::CreateRenderer(audioDevice->ToCString());
		if (audOut)
		{
			Int32 vol = audOut->GetDeviceVolume();
			console->WriteStrC(UTF8STRC("Current Volume: "));
			irt = IO::ConsoleInput::InputInt32(console, &vol, true);

			if (irt == IO::ConsoleInput::IRT_TAB || irt == IO::ConsoleInput::IRT_ENTER)
			{
				audOut->SetDeviceVolume((UInt16)vol);

				NEW_CLASS(startDt, Data::DateTime());
				startDt->SetCurrTime();
				sptr = Text::StrConcatC(buff, UTF8STRC("Curr Date: "));
				sptr = startDt->ToString(sptr, "yyyy-MM-dd HH:mm:ss");
				console->WriteLineC(buff, (UOSInt)(sptr - buff));

				startDt->AddHour(8);
				console->WriteStrC(UTF8STRC("Input Alarm Time: "));
				irt = IO::ConsoleInput::InputDateTime(console, startDt);
				if (irt == IO::ConsoleInput::IRT_TAB || irt == IO::ConsoleInput::IRT_ENTER)
				{
					sptr = Text::StrConcatC(buff, UTF8STRC("Alarm Time: "));
					sptr = startDt->ToString(sptr, "yyyy-MM-dd HH:mm:ss");
					console->WriteLineC(buff, (UOSInt)(sptr - buff));

					console->SetBGColor(Text::StandardColor::ConsoleDarkGreen);
					console->SetTextColor(Text::StandardColor::Gray);
					console->WriteLineC(UTF8STRC("Press Ctrl+C to exit"));
					console->SetBGColor(Text::StandardColor::Black);
					console->SetTextColor(Text::StandardColor::Gray);
					Sync::ThreadUtil::Create(PlayThread, 0);

					progCtrl->WaitForExit(progCtrl);

					ToStop = true;
					while (threadRunning)
					{
						Sync::SimpleThread::Sleep(10);
					}
				}
				DEL_CLASS(startDt);
			}

			SDEL_CLASS(audOut);
		}
		else
		{
			console->WriteLineC(UTF8STRC("Error in creating renderer"));
		}
		audioDevice->Release();
	}
	else
	{
		i = devCnt;
		while (i-- > 0)
		{
			sel[i]->Release();
		}
		MemFree(sel);

	}

	DEL_CLASS(tmpDt);
	DEL_CLASS(timeCli);
	DEL_CLASS(encFact);
	SDEL_CLASS(ssl);
	sockf.Delete();
	DEL_CLASS(console);
	return 0;
}
