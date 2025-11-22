#include "Stdafx.h"

#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ArrayList.hpp"
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
#include "Media/MediaSource.h"
#include "Media/AudioSource.h"
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
#include "Sync/Thread.h"

#include "Text/MyString.h"

#define NTPHOST CSTR("stdtime.gov.hk")

Optional<Media::AudioRenderer> audOut;
NN<Data::DateTime> startDt;
Text::String *audioDevice; //L"Realtek HD Audio output"
NN<IO::ConsoleWriter> console;
NN<Net::SocketFactory> sockf;
NN<Net::TCPClientFactory> clif;
Optional<Net::SSLEngine> ssl;
NN<Text::EncodingFactory> encFact;
Net::NTPClient *timeCli;
NN<Data::DateTime> tmpDt;


void __stdcall PlayThread(NN<Sync::Thread> thread)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Parser::FileParser::WAVParser parser;
	Media::RefClock *clk;
	Data::DateTime currDt;
	Data::DateTime updateDt;
	Data::ArrayListNN<Media::MediaFile> stmList;
	NN<Media::MediaFile> file;
	Data::RandomOS random;
	UOSInt i;
	UOSInt currStm;
	Net::HKOWeather::WeatherSignal currSignal;
	Net::HKOWeather::WeatherSignal nextSignal;
	Bool typhoonStop = false;
	NN<Media::AudioRenderer> audRenderer;
	if (!audOut.SetTo(audRenderer))
		return;

	updateDt.SetCurrTimeUTC();
	updateDt.AddMinute(-15);
	currSignal = Net::HKOWeather::WS_NONE;

	i = 1;
	while (true)
	{
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Alarm"));
		sptr = Text::StrUOSInt(sptr, i);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".wav"));

		IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
		if (Optional<Media::MediaFile>::ConvertFrom(parser.ParseFile(fd, 0, IO::ParserType::MediaFile)).SetTo(file))
		{
			stmList.Add(file);
			i++;
		}
		else
		{
			break;
		}
	}

	NEW_CLASS(clk, Media::RefClock());

	while (!thread->IsStopping())
	{
		currDt.SetCurrTime();
		if (currDt.DiffMS(updateDt) >= 900000)
		{
			updateDt.SetCurrTime();
			if (timeCli->GetServerTime(NTPHOST, 123, tmpDt))
			{
				tmpDt->SetAsComputerTime();
			}

			nextSignal = Net::HKOWeather::GetSignalSummary(clif, ssl, encFact);
			if ((nextSignal & Net::HKOWeather::WS_TYPHOON_MASK) != 0 || (currSignal & Net::HKOWeather::WS_TYPHOON_MASK) != 0)
			{
				sptr = currDt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
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
				console->WriteLine(CSTRP(sbuff, sptr));
			}
			currSignal = nextSignal;
		}
		if (currDt > *startDt.Ptr() && !typhoonStop)
		{
			break;
		}
		else
		{
			thread->Wait(1000);
		}
	}
	if (!thread->IsStopping())
	{
		console->WriteLine(CSTR("Alerting!"));
	}
	currStm = (UInt32)-1;
	while (!thread->IsStopping())
	{
		if (stmList.GetCount() > 0)
		{
			if (stmList.GetCount() <= 1)
			{
				currStm = 0;
			}
			else
			{
				i = currStm;
				while (i == currStm)
				{
					i = (UInt32)random.NextInt32() % stmList.GetCount();
				}
				currStm = i;
			}
			file = stmList.GetItemNoCheck(currStm);

			if (audRenderer->BindAudio((Media::AudioSource*)file->GetStream(0, 0).OrNull()))
			{
				audRenderer->AudioInit(clk);
				audRenderer->Start();
			}

			while (!thread->IsStopping())
			{
				if (!audRenderer->IsPlaying())
				{
					break;
				}
				thread->Wait(10);
			}
		}
		if (!thread->IsStopping())
		{
			currDt.SetCurrTime();
			Media::AudioSource *astm = Media::ClockSpeechCh::GetSpeech(currDt);
			if (audRenderer->BindAudio(astm))
			{
				audRenderer->AudioInit(clk);
				audRenderer->Start();
				while (!thread->IsStopping())
				{
					if (!audRenderer->IsPlaying())
					{
						break;
					}
					thread->Wait(10);
				}
			}
			DEL_CLASS(astm);
		}
	}
	audRenderer->Stop();

	DEL_CLASS(clk);

	stmList.DeleteAll();
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char buff[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<NN<Text::String>> sel;
	UOSInt i;
	UOSInt devCnt;
	IO::ConsoleInput::InputReturnType irt;
	IO::LogTool log;

	NEW_CLASSNN(console, IO::ConsoleWriter());
	NEW_CLASSNN(sockf, Net::OSSocketFactory(true));
	NEW_CLASSNN(clif, Net::TCPClientFactory(sockf));
	ssl = Net::SSLEngineFactory::Create(clif, true);
	NEW_CLASSNN(encFact, Text::EncodingFactory());
	NEW_CLASS(timeCli, Net::NTPClient(sockf, 14562, log));
	NEW_CLASSNN(tmpDt, Data::DateTime());
	devCnt = i = Media::AudioDevice::GetDeviceCount();
	sel = MemAllocArr(NN<Text::String>, devCnt);
	while (i-- > 0)
	{
		sptr = Media::AudioDevice::GetDeviceName(buff, i).Or(buff);
		sel[i] = Text::String::New(buff, (UOSInt)(sptr - buff));
	}
	if (timeCli->GetServerTime(NTPHOST, 123, tmpDt))
	{
		tmpDt->SetAsComputerTime();
	}
	console->Write(CSTR("Select audio device: "));
	irt = IO::ConsoleInput::InputSelect(console, sel, devCnt, i);
	if (irt == IO::ConsoleInput::IRT_TAB || irt == IO::ConsoleInput::IRT_ENTER)
	{
		audioDevice = sel[i]->Clone().Ptr();
		i = devCnt;
		while (i-- > 0)
		{
			sel[i]->Release();
		}
		MemFreeArr(sel);

		audOut = Media::AudioDevice::CreateRenderer(audioDevice->ToCString());
		NN<Media::AudioRenderer> audRenderer;
		if (audOut.SetTo(audRenderer))
		{
			Int32 vol = audRenderer->GetDeviceVolume();
			console->Write(CSTR("Current Volume: "));
			irt = IO::ConsoleInput::InputInt32(console, vol, true);

			if (irt == IO::ConsoleInput::IRT_TAB || irt == IO::ConsoleInput::IRT_ENTER)
			{
				audRenderer->SetDeviceVolume((UInt16)vol);

				NEW_CLASSNN(startDt, Data::DateTime());
				startDt->SetCurrTime();
				sptr = Text::StrConcatC(buff, UTF8STRC("Curr Date: "));
				sptr = startDt->ToString(sptr, "yyyy-MM-dd HH:mm:ss");
				console->WriteLine(CSTRP(buff, sptr));

				startDt->AddHour(8);
				console->Write(CSTR("Input Alarm Time: "));
				irt = IO::ConsoleInput::InputDateTime(console, startDt);
				if (irt == IO::ConsoleInput::IRT_TAB || irt == IO::ConsoleInput::IRT_ENTER)
				{
					sptr = Text::StrConcatC(buff, UTF8STRC("Alarm Time: "));
					sptr = startDt->ToString(sptr, "yyyy-MM-dd HH:mm:ss");
					console->WriteLine(CSTRP(buff, sptr));

					console->SetBGColor(Text::StandardColor::ConsoleDarkGreen);
					console->SetTextColor(Text::StandardColor::Gray);
					console->WriteLine(CSTR("Press Ctrl+C to exit"));
					console->SetBGColor(Text::StandardColor::Black);
					console->SetTextColor(Text::StandardColor::Gray);
					Sync::Thread thread(PlayThread, 0, CSTR("PlayThread"));
					thread.Start();

					progCtrl->WaitForExit(progCtrl);

					thread.Stop();
				}
				startDt.Delete();
			}

			audOut.Delete();
		}
		else
		{
			console->WriteLine(CSTR("Error in creating renderer"));
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
		MemFreeArr(sel);
	}

	tmpDt.Delete();
	DEL_CLASS(timeCli);
	encFact.Delete();
	ssl.Delete();
	clif.Delete();
	sockf.Delete();
	console.Delete();
	return 0;
}
