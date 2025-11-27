#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/RadioSignalLogger.h"
#include "Text/MyStringFloat.h"

void __stdcall IO::RadioSignalLogger::OnWiFiUpdate(NN<Net::WirelessLAN::BSSInfo> bss, const Data::Timestamp &scanTime, AnyType userObj)
{
	NN<IO::RadioSignalLogger> me = userObj.GetNN<IO::RadioSignalLogger>();
	NN<IO::FileStream> fs;
	if (me->fs.SetTo(fs))
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		Data::DateTime dt;
		Sync::MutexUsage mutUsage(me->fsMut);
		Text::StringBuilderUTF8 sb;
		sptr = scanTime.ToLocalTime().ToStringNoZone(sbuff);
		sb.AppendP(sbuff, sptr);
		sb.AppendUTF8Char('\t');
		sb.AppendC(UTF8STRC("wifi"));
		sb.AppendUTF8Char('\t');
		sb.AppendHexBuff(bss->GetMAC(), 6, ':', Text::LineBreakType::None);
		sb.AppendUTF8Char('\t');
		sb.AppendDouble(bss->GetRSSI());
		sb.AppendC(UTF8STRC("\r\n"));
		fs->Write(sb.ToByteArray());
		me->wifiCnt++;
	}
}

void __stdcall IO::RadioSignalLogger::OnBTUpdate(NN<IO::BTScanLog::ScanRecord3> dev, IO::BTScanner::UpdateType updateType, AnyType userObj)
{
	NN<IO::RadioSignalLogger> me = userObj.GetNN<IO::RadioSignalLogger>();
	NN<IO::FileStream> fs;
	if (updateType == IO::BTScanner::UT_RSSI && me->fs.SetTo(fs))
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		Data::DateTime dt;
		Sync::MutexUsage mutUsage(me->fsMut);
		Text::StringBuilderUTF8 sb;
		dt.SetTicks(dev->lastSeenTime);
		dt.ToLocalTime();
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		sb.AppendP(sbuff, sptr);
		sb.AppendUTF8Char('\t');
		if (dev->radioType == IO::BTScanLog::RT_HCI)
		{
			sb.AppendC(UTF8STRC("hci"));
		}
		else if (dev->radioType == IO::BTScanLog::RT_LE)
		{
			if (dev->addrType == IO::BTScanLog::AT_RANDOM)
			{
				sb.AppendC(UTF8STRC("ler"));
			}
			else
			{
				sb.AppendC(UTF8STRC("le"));
			}
		}
		else
		{
			sb.AppendC(UTF8STRC("bt"));
		}
		sb.AppendUTF8Char('\t');
		sb.AppendHexBuff(dev->mac, 6, ':', Text::LineBreakType::None);
		sb.AppendUTF8Char('\t');
		sb.AppendI32(dev->rssi);
		sb.AppendC(UTF8STRC("\r\n"));
		fs->Write(sb.ToByteArray());
		me->btCnt++;
	}
}

IO::RadioSignalLogger::RadioSignalLogger()
{
	this->fs = 0;
	this->wifiCapture = 0;
	this->btCapture = 0;
	this->wifiCnt = 0;
	this->btCnt = 0;

	UTF8Char sbuff[512];
	NN<IO::FileStream> fs;
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	Data::DateTime dt;
	dt.SetCurrTime();
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];
	sptr = Text::StrConcatC(sptr, UTF8STRC("radio"));
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".txt"));
	NEW_CLASSNN(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		fs.Delete();
	}
}

IO::RadioSignalLogger::~RadioSignalLogger()
{
	this->Stop();
}

void IO::RadioSignalLogger::CaptureWiFi(NN<Net::WiFiCapturer> wifiCapture)
{
	NN<Net::WiFiCapturer> nnwifiCapture;
	if (this->fs.NotNull())
	{
		if (this->wifiCapture.SetTo(nnwifiCapture))
		{
			nnwifiCapture->SetUpdateHandler(0, 0);
		}
		this->wifiCapture = wifiCapture;
		wifiCapture->SetUpdateHandler(OnWiFiUpdate, this);
	}
}

void IO::RadioSignalLogger::CaptureBT(NN<IO::BTCapturer> btCapture)
{
	NN<IO::BTCapturer> nnbtCapture;
	if (this->fs.NotNull())
	{
		if (this->btCapture.SetTo(nnbtCapture))
		{
			nnbtCapture->SetUpdateHandler(0, 0);
		}
		this->btCapture = btCapture;
		btCapture->SetUpdateHandler(OnBTUpdate, this);
	}
}

void IO::RadioSignalLogger::Stop()
{
	NN<Net::WiFiCapturer> wifiCapture;
	NN<IO::BTCapturer> btCapture;
	if (this->fs.NotNull())
	{
		if (this->btCapture.SetTo(btCapture))
		{
			btCapture->SetUpdateHandler(0, 0);
			this->btCapture = 0;
		}
		if (this->wifiCapture.SetTo(wifiCapture))
		{
			wifiCapture->SetUpdateHandler(0, 0);
			this->wifiCapture = 0;
		}
		this->fs.Delete();
	}
}

UInt64 IO::RadioSignalLogger::GetWiFiCount() const
{
	return this->wifiCnt;
}

UInt64 IO::RadioSignalLogger::GetBTCount() const
{
	return this->btCnt;
}
