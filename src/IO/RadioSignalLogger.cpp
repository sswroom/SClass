#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/RadioSignalLogger.h"
#include "Text/MyStringFloat.h"

void __stdcall IO::RadioSignalLogger::OnWiFiUpdate(Net::WirelessLAN::BSSInfo *bss, Int64 scanTime, void *userObj)
{
	IO::RadioSignalLogger *me = (IO::RadioSignalLogger*)userObj;
	if (me->fs)
	{
		UTF8Char sbuff[64];
		Data::DateTime dt;
		Sync::MutexUsage mutUsage(me->fsMut);
		Text::StringBuilderUTF8 sb;
		dt.SetTicks(scanTime);
		dt.ToLocalTime();
		dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		sb.Append(sbuff);
		sb.AppendChar('\t', 1);
		sb.Append((const UTF8Char*)"wifi");
		sb.AppendChar('\t', 1);
		sb.AppendHexBuff(bss->GetMAC(), 6, ':', Text::LBT_NONE);
		sb.AppendChar('\t', 1);
		Text::SBAppendF64(&sb, bss->GetRSSI());
		sb.Append((const UTF8Char*)"\r\n");
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
		Data::DateTime dt;
		Sync::MutexUsage mutUsage(me->fsMut);
		Text::StringBuilderUTF8 sb;
		dt.SetTicks(dev->lastSeenTime);
		dt.ToLocalTime();
		dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		sb.Append(sbuff);
		sb.AppendChar('\t', 1);
		if (dev->radioType == IO::BTScanLog::RT_HCI)
		{
			sb.Append((const UTF8Char*)"hci");
		}
		else if (dev->radioType == IO::BTScanLog::RT_LE)
		{
			if (dev->addrType == IO::BTScanLog::AT_RANDOM)
			{
				sb.Append((const UTF8Char*)"ler");
			}
			else
			{
				sb.Append((const UTF8Char*)"le");
			}
		}
		else
		{
			sb.Append((const UTF8Char*)"bt");
		}
		sb.AppendChar('\t', 1);
		sb.AppendHexBuff(dev->mac, 6, ':', Text::LBT_NONE);
		sb.AppendChar('\t', 1);
		sb.AppendI32(dev->rssi);
		sb.Append((const UTF8Char*)"\r\n");
		me->fs->Write(sb.ToString(), sb.GetLength());
		me->btCnt++;
	}
}

IO::RadioSignalLogger::RadioSignalLogger()
{
	this->fs = 0;
	this->fsMut = 0;
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
	i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"radio");
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".txt");
	NEW_CLASS(this->fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (this->fs->IsError())
	{
		DEL_CLASS(this->fs);
		this->fs = 0;
	}
	else
	{
		NEW_CLASS(this->fsMut, Sync::Mutex());
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
		DEL_CLASS(this->fsMut);
		this->fsMut = 0;
	}
}

UInt64 IO::RadioSignalLogger::GetWiFiCount()
{
	return this->wifiCnt;
}

UInt64 IO::RadioSignalLogger::GetBTCount()
{
	return this->btCnt;
}
