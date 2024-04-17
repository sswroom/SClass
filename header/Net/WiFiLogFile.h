#ifndef _SM_NET_WIFILOGFILE
#define _SM_NET_WIFILOGFILE
#include "Data/ArrayListNN.h"
#include "Net/WirelessLAN.h"

namespace Net
{
	class WiFiLogFile
	{
	public:
		typedef struct
		{
			UInt8 mac[6];
			UInt64 macInt;
			NotNullPtr<Text::String> ssid;
			Int32 phyType;
			Double freq;
			Text::String *manuf;
			Text::String *model;
			Text::String *serialNum;
			Optional<Text::String> country;
			UInt8 ouis[3][3];
			UInt64 neighbour[20];
			UInt32 ieLen;
			UInt8 *ieBuff;
			Data::Timestamp lastScanTime;
			Double lastRSSI;
		} LogFileEntry;

	private:
		Data::ArrayListNN<LogFileEntry> logList;

		UOSInt DirectInsert(NotNullPtr<LogFileEntry> log);
	public:
		WiFiLogFile();
		~WiFiLogFile();

		void LoadFile(Text::CStringNN fileName);
		Bool StoreFile(Text::CStringNN fileName);
		void Clear();
		Optional<LogFileEntry> Get(UInt64 iMAC);
		OSInt GetIndex(UInt64 iMAC);
		NotNullPtr<Data::ArrayListNN<LogFileEntry>> GetLogList();
		Optional<const LogFileEntry> GetItem(UOSInt index);

		NotNullPtr<LogFileEntry> AddBSSInfo(NotNullPtr<Net::WirelessLAN::BSSInfo> bss, OutParam<OSInt> lastIndex);
	};
}
#endif
