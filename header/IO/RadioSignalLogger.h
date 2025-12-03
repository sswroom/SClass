#ifndef _SM_IO_RADIOSIGNALLOGGER
#define _SM_IO_RADIOSIGNALLOGGER
#include "IO/BTCapturer.h"
#include "IO/FileStream.h"
#include "IO/GSMCellCapturer.h"
#include "Net/WiFiCapturer.h"
#include "Text/UTF8Writer.h"

namespace IO
{
	class RadioSignalLogger
	{
	private:
		Optional<IO::FileStream> fs;
		Sync::Mutex fsMut;
		UInt64 wifiCnt;
		UInt64 btCnt;
		UInt64 gsmCnt;
		Optional<IO::BTCapturer> btCapture;
		Optional<Net::WiFiCapturer> wifiCapture;
		Optional<IO::GSMCellCapturer> gsmCapturer;
		
		static void __stdcall OnWiFiUpdate(NN<Net::WirelessLAN::BSSInfo> bss, const Data::Timestamp &scanTime, AnyType userObj);
		static void __stdcall OnBTUpdate(NN<IO::BTScanLog::ScanRecord3> dev, IO::BTScanner::UpdateType updateType, AnyType userObj);
		static void __stdcall OnGSMUpdate(NN<Data::ArrayListNN<IO::GSMModemController::CellSignal>> cells, AnyType userObj);
	public:
		RadioSignalLogger();
		~RadioSignalLogger();

		void CaptureWiFi(NN<Net::WiFiCapturer> wifiCapture);
		void CaptureBT(NN<IO::BTCapturer> btCapture);
		void CaptureGSM(NN<IO::GSMCellCapturer> gsmCapturer);
		void Stop();

		UInt64 GetWiFiCount() const;
		UInt64 GetBTCount() const;
		UInt64 GetGSMCount() const;
	};
}

#endif
