#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/SyncArrayList.h"
#include "Data/Compress/Inflate.h"
#include "IO/ConsoleWriter.h"
#include "IO/StmData/MemoryData.h"
#include "Net/HTTPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Sync/Thread.h"
#include "Text/JSON.h"
#include "Text/String.h"

#include <stdio.h>

class CesiumDownloader
{
private:
	enum ThreadState
	{
		NotRunning,
		Running,
		Exited
	};

	struct ThreadStatus
	{
		CesiumDownloader *me;
		Sync::Event *evt;
		UInt64 totalDownload;
		UInt64 totalUpload;
		UInt64 totalContentSize;
		UInt32 reqCnt;
		UInt32 succCnt;
		ThreadState state;
		Text::String *currURL;
	};
private:
	Net::SocketFactory *sockf;
	Net::SSLEngine *ssl;
	UOSInt threadCount;
	Bool threadToStop;
	Sync::Event mainEvt;
	ThreadStatus *stats;
	Data::SyncArrayList<Text::String*> urlList;
	Bool useComp;

	static void ParseJSONObj(ThreadStatus *stat, Text::String *url, Text::JSONBase *obj, Text::StringBuilderUTF8 *tmpSb)
	{
		if (obj == 0 || obj->GetType() != Text::JSONType::Object)
		{
			return;
		}
		Text::JSONObject *jobj = (Text::JSONObject*)obj;
		Text::String *contURL = jobj->GetString(UTF8STRC("content.url"));
		UOSInt i;
		if (contURL)
		{
			tmpSb->ClearStr();
			tmpSb->Append(url);
			i = tmpSb->IndexOf('?');
			if (i != INVALID_INDEX)
			{
				tmpSb->TrimToLength(i);
			}
			i = tmpSb->LastIndexOf('/');
			tmpSb->TrimToLength(i + 1);
			tmpSb->Append(contURL);
			stat->me->AddURL(tmpSb->ToCString());
		}

		obj = obj->GetValue(UTF8STRC("children"));
		if (obj && obj->GetType() == Text::JSONType::Array)
		{
			Text::JSONArray *children = (Text::JSONArray*)obj;
			i = children->GetArrayLength();
			while (i-- > 0)
			{
				ParseJSONObj(stat, url, children->GetArrayValue(i), tmpSb);
			}
		}
	}

	static void ParseJSON(ThreadStatus *stat, Text::String *url, IO::MemoryStream *mstm, Text::StringBuilderUTF8 *tmpSb)
	{
		mstm->Write((const UInt8*)"", 1);
		UOSInt i;
		const UInt8 *buff = mstm->GetBuff(&i);
		Text::JSONBase *json = Text::JSONBase::ParseJSONStr(Text::CString(buff, i - 1));
		if (json)
		{
			ParseJSONObj(stat, url, json->GetValue(UTF8STRC("root")), tmpSb);
			json->EndUse();
		}
	}

	static void ProcURL(ThreadStatus *stat, Text::String *url, Text::StringBuilderUTF8 *tmpSb)
	{
		UInt8 buff[4096];
		stat->reqCnt++;
		Net::HTTPClient *cli = Net::HTTPClient::CreateClient(stat->me->sockf, stat->me->ssl, url->ToCString(), true, url->StartsWithICase(UTF8STRC("HTTPS://")));
		if (cli->Connect(url->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, false))
		{
			cli->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
			cli->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
			cli->AddHeaderC(CSTR("User-Agent"), CSTR("sswr/1.0"));
			cli->AddHeaderC(CSTR("Connection"), CSTR("keep-alive"));
			if (stat->me->useComp)
			{
				cli->AddHeaderC(CSTR("Accept-Encoding"), CSTR("gzip, deflate"));
			}
			cli->EndRequest(0, 0);
			IO::MemoryStream *mstm;
			UInt64 totalRead = 0;
			UOSInt thisRead;
			NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("CesiumDownloader.ProcURL.mstm")));
			while ((thisRead = cli->Read(buff, 4096)) > 0)
			{
				mstm->Write(buff, thisRead);
				totalRead += thisRead;
			}
			stat->totalDownload += cli->GetTotalDownload();
			stat->totalUpload += cli->GetTotalUpload();
			tmpSb->ClearStr();
			if (cli->GetRespHeader(CSTR("Content-Encoding"), tmpSb))
			{
				if (tmpSb->Equals(UTF8STRC("gzip")))
				{
					UOSInt respSize;
					const UInt8 *respData = mstm->GetBuff(&respSize);
					if (respSize > 16 && respData[0] == 0x1F && respData[1] == 0x8B && respData[2] == 0x8)
					{
						IO::MemoryStream *mstm2;
						Data::Compress::Inflate inflate(false);
						thisRead = 10;
						IO::StmData::MemoryData mdata(&respData[thisRead], respSize - thisRead - 8);
						NEW_CLASS(mstm2, IO::MemoryStream(ReadUInt32(&respData[respSize - 4]), UTF8STRC("CesiumDownloader.ProcURL.mstm2")));
						if (inflate.Decompress(mstm2, &mdata))
						{
							DEL_CLASS(mstm);
							mstm = mstm2;
						}
						else
						{
							DEL_CLASS(mstm2);
						}
					}
				}
			}
			stat->totalContentSize += mstm->GetLength();
			if (cli->GetRespStatus() == 200)
			{
				stat->succCnt++;

				tmpSb->ClearStr();
				if (cli->GetRespHeader(CSTR("Content-Type"), tmpSb))
				{
					if (tmpSb->Equals(UTF8STRC("application/json")))
					{
						ParseJSON(stat, url, mstm, tmpSb);
					}
				}
			}
			DEL_CLASS(mstm);
		}
		DEL_CLASS(cli);
	}

	static UInt32 __stdcall ProcThread(void *userObj)
	{
		ThreadStatus *stat = (ThreadStatus*)userObj;
		Text::String *nextURL;
		{
			Text::StringBuilderUTF8 sb;
			Sync::Event evt;
			stat->evt = &evt;
			stat->state = ThreadState::Running;
			stat->me->mainEvt.Set();
			while (!stat->me->threadToStop)
			{
				if (stat->currURL)
				{
					ProcURL(stat, stat->currURL, &sb);
					while ((nextURL = stat->me->urlList.RemoveLast()) != 0)
					{
						stat->currURL->Release();
						stat->currURL = nextURL;
						ProcURL(stat, nextURL, &sb);
					}
					SDEL_STRING(stat->currURL);

					stat->me->mainEvt.Set();
				}

				evt.Wait(1000);
			}
		}
		stat->state = ThreadState::Exited;
		stat->me->mainEvt.Set();
		return 0;
	}

public:
	CesiumDownloader(Net::SocketFactory *sockf, UOSInt threadCount)
	{
		this->sockf = sockf;
		this->ssl = Net::SSLEngineFactory::Create(sockf, true);
		this->threadCount = threadCount;
		this->threadToStop = false;
		this->useComp = true;
		this->stats = MemAlloc(ThreadStatus, this->threadCount);
		UOSInt i = this->threadCount;
		while (i-- > 0)
		{
			this->stats[i].me = this;
			this->stats[i].totalDownload = 0;
			this->stats[i].totalUpload = 0;
			this->stats[i].totalContentSize = 0;
			this->stats[i].reqCnt = 0;
			this->stats[i].succCnt = 0;
			this->stats[i].currURL = 0;
			this->stats[i].state = ThreadState::NotRunning;
			Sync::Thread::Create(ProcThread, &this->stats[i]);
		}
		Bool found = true;
		while (true)
		{
			found = false;
			i = this->threadCount;
			while (i-- > 0)
			{
				if (this->stats[i].state == ThreadState::NotRunning)
				{
					found = true;
				}
			}
			if (!found)
				break;
			this->mainEvt.Wait(100);
		}
	}

	~CesiumDownloader()
	{
		UOSInt i;
		this->threadToStop = true;
		i = this->threadCount;
		while (i-- > 0)
		{
			this->stats[i].evt->Set();
		}

		Bool found = true;
		while (true)
		{
			found = false;
			i = this->threadCount;
			while (i-- > 0)
			{
				if (this->stats[i].state != ThreadState::Exited)
				{
					found = true;
				}
			}
			if (!found)
				break;
			this->mainEvt.Wait(100);
		}
		
		i = this->urlList.GetCount();
		while (i-- > 0)
		{
			this->urlList.GetItem(i)->Release();
		}
		MemFree(this->stats);
		SDEL_CLASS(this->ssl);
	}

	void AddURL(Text::CString url)
	{
		UOSInt i = this->threadCount;
		while (i-- > 0)
		{
			if (this->stats[i].currURL == 0)
			{
				this->stats[i].currURL = Text::String::New(url);
				this->stats[i].evt->Set();
				return;
			}
		}
		this->urlList.Add(Text::String::New(url));
	}

	void WaitForIdle()
	{
		UOSInt i;
		Bool found = true;
		while (true)
		{
			found = false;
			i = this->threadCount;
			while (i-- > 0)
			{
				if (this->stats[i].currURL != 0)
				{
					found = true;
				}
			}
			if (!found)
				break;
			this->mainEvt.Wait(100);
		}
	}
	
	UOSInt GetTotalReqCnt()
	{
		UOSInt ret = 0;
		UOSInt i = this->threadCount;
		while (i-- > 0)
		{
			ret += this->stats[i].reqCnt;
		}
		return ret;
	}

	UOSInt GetTotalSuccCnt()
	{
		UOSInt ret = 0;
		UOSInt i = this->threadCount;
		while (i-- > 0)
		{
			ret += this->stats[i].succCnt;
		}
		return ret;
	}

	UInt64 GetTotalDownloadSize()
	{
		UInt64 ret = 0;
		UOSInt i = this->threadCount;
		while (i-- > 0)
		{
			ret += this->stats[i].totalDownload;
		}
		return ret;
	}

	UInt64 GetTotalUploadSize()
	{
		UInt64 ret = 0;
		UOSInt i = this->threadCount;
		while (i-- > 0)
		{
			ret += this->stats[i].totalUpload;
		}
		return ret;
	}

	UInt64 GetTotalContentSize()
	{
		UInt64 ret = 0;
		UOSInt i = this->threadCount;
		while (i-- > 0)
		{
			ret += this->stats[i].totalContentSize;
		}
		return ret;
	}

	void ClearStat()
	{
		UOSInt i = this->threadCount;
		while (i-- > 0)
		{
			this->stats[i].reqCnt = 0;
			this->stats[i].succCnt = 0;
			this->stats[i].totalDownload = 0;
			this->stats[i].totalUpload = 0;
			this->stats[i].totalContentSize = 0;
		}
	}
};

void TestURL(IO::Writer *console, CesiumDownloader *downloader, Text::CString url)
{
	Manage::HiResClock clk;
	downloader->AddURL(url);
	downloader->WaitForIdle();
	Double t = clk.GetTimeDiff();
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("URL: "));
	sb.Append(url);
	sb.AppendC(UTF8STRC("\r\nTotal Time used: "));
	sb.AppendDouble(t);
	sb.AppendC(UTF8STRC("\r\nTotal Request Count: "));
	sb.AppendUOSInt(downloader->GetTotalReqCnt());
	sb.AppendC(UTF8STRC("\r\nTotal Success Count: "));
	sb.AppendUOSInt(downloader->GetTotalSuccCnt());
	sb.AppendC(UTF8STRC("\r\nTotal Download: "));
	sb.AppendU64(downloader->GetTotalDownloadSize());
	sb.AppendC(UTF8STRC("\r\nTotal Upload: "));
	sb.AppendU64(downloader->GetTotalUploadSize());
	sb.AppendC(UTF8STRC("\r\nTotal Content Size: "));
	sb.AppendU64(downloader->GetTotalContentSize());
	console->WriteLineCStr(sb.ToCString());
	downloader->ClearStat();
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Net::OSSocketFactory sockf(true);
	IO::ConsoleWriter console;
	CesiumDownloader downloader(&sockf, 16);
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.8"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.4"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.2"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.1"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.05"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.025"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.0125"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.00625"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.003125"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.0015625"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.8"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.4"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.2"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.1"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.05"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.025"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.0125"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.00625"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.003125"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.0015625"));
	return 0;
}