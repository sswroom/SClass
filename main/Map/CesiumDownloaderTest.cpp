#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/Comparator.h"
#include "Data/SyncArrayList.h"
#include "Data/Compress/Inflate.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Net/HTTPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Sync/ThreadUtil.h"
#include "Text/JSON.h"
#include "Text/String.h"

#include <stdio.h>

class CesiumDownloader
{
public:
	struct FileEntry
	{
		NotNullPtr<Text::String> url;
		UInt64 downloadSize;
		UInt64 contentSize;
	};

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
	NotNullPtr<Net::SocketFactory> sockf;
	Optional<Net::SSLEngine> ssl;
	UOSInt threadCount;
	Bool threadToStop;
	Sync::Event mainEvt;
	ThreadStatus *stats;
	Data::SyncArrayList<Text::String*> urlList;
	Bool useComp;
	Sync::Mutex filesMut;
	Data::ArrayList<FileEntry*> filesList;

	static void ParseJSONObj(NotNullPtr<ThreadStatus> stat, Text::String *url, Text::JSONBase *obj, NotNullPtr<Text::StringBuilderUTF8> tmpSb)
	{
		if (obj == 0 || obj->GetType() != Text::JSONType::Object)
		{
			return;
		}
		Text::JSONObject *jobj = (Text::JSONObject*)obj;
		UOSInt i;
		NotNullPtr<Text::String> contURL;
		if (jobj->GetValueString(CSTR("content.url")).SetTo(contURL))
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

		obj = obj->GetValue(CSTR("children"));
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

	static void ParseJSON(NotNullPtr<ThreadStatus> stat, Text::String *url, IO::MemoryStream *mstm, NotNullPtr<Text::StringBuilderUTF8> tmpSb)
	{
		mstm->Write((const UInt8*)"", 1);
		UOSInt i;
		const UInt8 *buff = mstm->GetBuff(i);
		Text::JSONBase *json = Text::JSONBase::ParseJSONStr(Text::CStringNN(buff, i - 1));
		if (json)
		{
			ParseJSONObj(stat, url, json->GetValue(CSTR("root")), tmpSb);
			json->EndUse();
		}
	}

	static void ProcURL(NotNullPtr<ThreadStatus> stat, Text::String *url, NotNullPtr<Text::StringBuilderUTF8> tmpSb)
	{
		UInt8 buff[4096];
		stat->reqCnt++;
		NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(stat->me->sockf, stat->me->ssl, url->ToCString(), true, url->StartsWithICase(UTF8STRC("HTTPS://")));
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
//			UInt64 totalRead = 0;
			UOSInt thisRead;
			IO::MemoryStream *mstm;
			NEW_CLASS(mstm, IO::MemoryStream());
			while ((thisRead = cli->Read(BYTEARR(buff))) > 0)
			{
				mstm->Write(buff, thisRead);
//				totalRead += thisRead;
			}
			stat->totalDownload += cli->GetTotalDownload();
			stat->totalUpload += cli->GetTotalUpload();
			tmpSb->ClearStr();
			if (cli->GetRespHeader(CSTR("Content-Encoding"), tmpSb))
			{
				if (tmpSb->Equals(UTF8STRC("gzip")))
				{
					UOSInt respSize;
					const UInt8 *respData = mstm->GetBuff(respSize);
					if (respSize > 16 && respData[0] == 0x1F && respData[1] == 0x8B && respData[2] == 0x8)
					{
						NotNullPtr<IO::MemoryStream> mstm2;
						Data::Compress::Inflate inflate(false);
						thisRead = 10;
						IO::StmData::MemoryDataRef mdata(&respData[thisRead], respSize - thisRead - 8);
						NEW_CLASSNN(mstm2, IO::MemoryStream(ReadUInt32(&respData[respSize - 4])));
						if (inflate.Decompress(mstm2, mdata))
						{
							DEL_CLASS(mstm);
							mstm = mstm2.Ptr();
						}
						else
						{
							mstm2.Delete();
						}
					}
				}
			}
			stat->totalContentSize += mstm->GetLength();
			Sync::MutexUsage mutUsage(stat->me->filesMut);
			FileEntry *file = MemAlloc(FileEntry, 1);
			file->url = url->Clone();
			file->downloadSize = cli->GetTotalDownload();
			file->contentSize = mstm->GetLength();
			stat->me->filesList.Add(file);
			mutUsage.EndUse();

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
		cli.Delete();
	}

	static UInt32 __stdcall ProcThread(AnyType userObj)
	{
		NotNullPtr<ThreadStatus> stat = userObj.GetNN<ThreadStatus>();
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
					ProcURL(stat, stat->currURL, sb);
					while ((nextURL = stat->me->urlList.RemoveLast()) != 0)
					{
						stat->currURL->Release();
						stat->currURL = nextURL;
						ProcURL(stat, nextURL, sb);
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
	CesiumDownloader(NotNullPtr<Net::SocketFactory> sockf, UOSInt threadCount, Bool useComp)
	{
		this->sockf = sockf;
		this->ssl = Net::SSLEngineFactory::Create(sockf, true);
		this->threadCount = threadCount;
		this->threadToStop = false;
		this->useComp = useComp;
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
			Sync::ThreadUtil::Create(ProcThread, &this->stats[i]);
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
		this->ssl.Delete();

		this->ClearFiles();
	}

	void AddURL(Text::CString url)
	{
		UOSInt i = this->threadCount;
		while (i-- > 0)
		{
			if (this->stats[i].currURL == 0)
			{
				this->stats[i].currURL = Text::String::New(url).Ptr();
				this->stats[i].evt->Set();
				return;
			}
		}
		this->urlList.Add(Text::String::New(url).Ptr());
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

	void ClearFiles()
	{
		Sync::MutexUsage mutUsage(this->filesMut);
		UOSInt i = this->filesList.GetCount();
		FileEntry *file;
		while (i-- > 0)
		{
			file = this->filesList.GetItem(i);
			file->url->Release();
			MemFree(file);
		}
		this->filesList.Clear();
	}

	NotNullPtr<const Data::ArrayList<FileEntry*>> GetFilesList()
	{
		return this->filesList;
	}
};

class FilesComparator : public Data::Comparator<CesiumDownloader::FileEntry*>
{
	virtual OSInt Compare(CesiumDownloader::FileEntry *a, CesiumDownloader::FileEntry *b) const
	{
		return a->url->CompareToFast(b->url->ToCString());
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
	FilesComparator comparator;
	Data::ArrayList<CesiumDownloader::FileEntry*> filesList;
	filesList.AddAll(downloader->GetFilesList());
	Data::Sort::ArtificialQuickSort::Sort(&filesList, &comparator);
	
	IO::FileStream fs(CSTR("CesiumFiles.txt"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	CesiumDownloader::FileEntry *file;
	UOSInt i = 0;
	UOSInt j = filesList.GetCount();
	while (i < j)
	{
		file = filesList.GetItem(i);
		sb.ClearStr();
		sb.Append(file->url);
		sb.AppendUTF8Char('\t');
		sb.AppendU64(file->contentSize);
		sb.AppendUTF8Char('\t');
		sb.AppendU64(file->downloadSize);
		sb.AppendC(UTF8STRC("\r\n"));
		fs.Write(sb.ToString(), sb.GetLength());
		i++;
	}
	downloader->ClearFiles();
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Net::OSSocketFactory sockf(true);
	IO::ConsoleWriter console;
	CesiumDownloader downloader(sockf, 16, true);
/*	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.8"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.4"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.2"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.1"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.05"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.025"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.0125"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.00625"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.003125"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&range=114.22109831332,22.361996166922,114.2219974849,22.364057802242&minErr=0.0015625"));*/
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0"));
/*	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.8"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.4"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.2"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.1"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.05"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.025"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.0125"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.00625"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.003125"));
	TestURL(&console, &downloader, CSTR("http://127.0.0.1:12345/mapSvc/cesiumdata?file=20220411HAD01_Cesium.json&minErr=0.0015625"));*/
	return 0;
}