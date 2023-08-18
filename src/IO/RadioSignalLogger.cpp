#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/RadioSignalLogger.h"
#include "Text/MyStringFloat.h"

void __stdcall IO::RadioSignalLogger::OnWiFiUpdate(Net::WirelessLAN::BSSInfo *bss, const Data::Timestamp &scanTime, void *userObj)
{
	IO::RadioSignalLogger *me = (IO::RadioSignalLogger*)userObj;
	if (me->fs)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
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
		me->fs->Write(sb.ToString(), sb.GetLength());
		me->wifiCnt++;
	}
}

void __stdcall IO::RadioSignalLogger::OnBTUpdate(IO::BTScanLog::ScanRecord3 *dev, IO::BTScanner::UpdateType updateType, void *userObj)
{
	IO::RadioSignalLogger *me = (IO::RadioSignalLogger*)userObj;
	if (updateType == IO::BTScanner::UT_RSSI)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
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
		me->fs->Write(sb.ToString(), sb.GetLength());
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
	UTF8Char *sptr;
	UOSInt i;
	Data::DateTime dt;
	dt.SetCurrTime();
	sptr = IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];
	sptr = Text::StrConcatC(sptr, UTF8STRC("radio"));
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".txt"));
	NEW_CLASS(this->fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (this->fs->IsError())
	{
		DEL_CLASS(this->fs);
		this->fs = 0;
	}
}

IO::RadioSignalLogger::~RadioSignalLogger()
{
	this->Stop();
}

void IO::RadioSignalLogger::CaptureWiFi(Net::WiFiCapturer *wifiCapture)
{
	if (this->fs)
	{
		if (this->wifiCapture)
		{
			this->wifiCapture->SetUpdateHandler(0, 0);
		}
		this->wifiCapture = wifiCapture;
		this->wifiCapture->SetUpdateHandler(OnWiFiUpdate, this);
	}
}

void IO::RadioSignalLogger::CaptureBT(IO::BTCapturer *btCapture)
{
	if (this->fs)
	{
		if (this->btCapture)
		{
			this->btCapture->SetUpdateHandler(0, 0);
		}
		this->btCapture = btCapture;
		this->btCapture->SetUpdateHandler(OnBTUpdate, this);
	}
}

void IO::RadioSignalLogger::Stop()
{
	if (this->fs)
	{
		if (this->btCapture)
		{
			this->btCapture->SetUpdateHandler(0, 0);
			this->btCapture = 0;
		}
		if (this->wifiCapture)
		{
			this->wifiCapture->SetUpdateHandler(0, 0);
			this->wifiCapture = 0;
		}
		DEL_CLASS(this->fs);
		this->fs = 0;
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
