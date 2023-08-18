#ifndef _SM_IO_RADIOSIGNALLOGGER
#define _SM_IO_RADIOSIGNALLOGGER
#include "IO/BTCapturer.h"
#include "IO/FileStream.h"
#include "Net/WiFiCapturer.h"
#include "Text/UTF8Writer.h"

namespace IO
{
	class RadioSignalLogger
	{
	private:
		IO::FileStream *fs;
		Sync::Mutex fsMut;
		UInt64 wifiCnt;
		UInt64 btCnt;
		IO::BTCapturer *btCapture;
		Net::WiFiCapturer *wifiCapture;
		
		static void __stdcall OnWiFiUpdate(Net::WirelessLAN::BSSInfo *bss, const Data::Timestamp &scanTime, void *userObj);
		static void __stdcall OnBTUpdate(IO::BTScanLog::ScanRecord3 *dev, IO::BTScanner::UpdateType updateType, void *userObj);
	public:
		RadioSignalLogger();
		~RadioSignalLogger();

		void CaptureWiFi(Net::WiFiCapturer *wifiCapture);
		void CaptureBT(IO::BTCapturer *btCapture);
		void Stop();

		UInt64 GetWiFiCount() const;
		UInt64 GetBTCount() const;
	};
}

#endif
