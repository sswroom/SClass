#ifndef _SM_NET_LORAMONITORCORE
#define _SM_NET_LORAMONITORCORE
#include "Data/FastMapNN.hpp"
#include "DB/SQLiteFile.h"
#include "Net/SocketMonitor.h"

namespace Net
{
	class LoRaMonitorCore
	{
	public: 
		struct DataPacket
		{
			UInt8 msgType;
			UnsafeArray<UInt8> msg;
			UOSInt msgSize;
		};

		struct GWInfo
		{
			UInt64 gweui;
			Optional<Text::String> name;
			Optional<Text::String> model;
			Optional<Text::String> sn;
			Optional<Text::String> imei;
			Optional<Text::String> location;
			Data::Timestamp lastSeenTime;
			Bool updated;
			Optional<DataPacket> lastData[16];
			UOSInt lastDataBegin;
			UOSInt lastDataEnd;
		};
	private:
		NN<Net::SocketFactory> sockf;
		Optional<Socket> s;
		UInt16 loraPort;
		Optional<Net::SocketMonitor> socMon;
		NN<DB::SQLiteFile> db;
		Sync::Mutex gwMut;
		Data::UInt64FastMapNN<GWInfo> gwMap;

		static void __stdcall OnRAWPacket(AnyType userData, UnsafeArray<const UInt8> packetData, UOSInt packetSize);
		void OnLoRaPacket(Bool toServer, UInt8 ver, UInt16 token, UInt8 msgType, UnsafeArray<const UInt8> msg, UOSInt msgSize);
		NN<GWInfo> GetGW(UInt64 gweui);
		void GWAddData(NN<GWInfo> gw, UInt8 msgType, UnsafeArray<const UInt8> msg, UOSInt msgSize);
		void LoadDB();
		void SaveGWList();
		static void __stdcall FreeGW(NN<GWInfo> gw);
	public:
		LoRaMonitorCore(NN<Net::SocketFactory> sockf, UInt16 loraPort, UInt16 uiPort);
		~LoRaMonitorCore();

		Bool IsError();
		NN<Data::ReadingListNN<GWInfo>> GetGWList(NN<Sync::MutexUsage> mutUsage);
	};
}
#endif