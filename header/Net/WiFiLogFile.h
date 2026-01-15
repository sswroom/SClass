#ifndef _SM_NET_WIFILOGFILE
#define _SM_NET_WIFILOGFILE
#include "Data/ArrayListNN.hpp"
#include "Net/WirelessLAN.h"

namespace Net
{
	class WiFiLogFile
	{
	public:
		typedef struct
		{
			UInt8 mac[6];
			UInt64 mac64Int;
			NN<Text::String> ssid;
			Int32 phyType;
			Double freq;
			Optional<Text::String> manuf;
			Optional<Text::String> model;
			Optional<Text::String> serialNum;
			Optional<Text::String> country;
			UInt8 ouis[3][3];
			UInt64 neighbour[20];
			UInt32 ieLen;
			UnsafeArrayOpt<UInt8> ieBuff;
			Data::Timestamp lastScanTime;
			Double lastRSSI;
		} LogFileEntry;

	private:
		Data::ArrayListNN<LogFileEntry> logList;

		UIntOS DirectInsert(NN<LogFileEntry> log);
	public:
		WiFiLogFile();
		~WiFiLogFile();

		void LoadFile(Text::CStringNN fileName);
		Bool StoreFile(Text::CStringNN fileName);
		void Clear();
		Optional<LogFileEntry> Get(UInt64 iMAC);
		IntOS GetIndex(UInt64 iMAC);
		NN<Data::ArrayListNN<LogFileEntry>> GetLogList();
		Optional<const LogFileEntry> GetItem(UIntOS index);

		NN<LogFileEntry> AddBSSInfo(NN<Net::WirelessLAN::BSSInfo> bss, OutParam<IntOS> lastIndex);
	};
}
#endif
