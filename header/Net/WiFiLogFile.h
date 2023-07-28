#ifndef _SM_NET_WIFILOGFILE
#define _SM_NET_WIFILOGFILE
#include "Data/ArrayList.h"
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
			Text::String *country;
			UInt8 ouis[3][3];
			UInt64 neighbour[20];
			UInt32 ieLen;
			UInt8 *ieBuff;
			Data::Timestamp lastScanTime;
			Double lastRSSI;
		} LogFileEntry;

	private:
		Data::ArrayList<LogFileEntry*> logList;

		UOSInt DirectInsert(LogFileEntry *log);
	public:
		WiFiLogFile();
		~WiFiLogFile();

		void LoadFile(Text::CString fileName);
		Bool StoreFile(Text::CString fileName);
		void Clear();
		LogFileEntry *Get(UInt64 iMAC);
		OSInt GetIndex(UInt64 iMAC);
		NotNullPtr<Data::ArrayList<LogFileEntry*>> GetLogList();
		const LogFileEntry *GetItem(UOSInt index);

		LogFileEntry *AddBSSInfo(Net::WirelessLAN::BSSInfo *bss, OSInt *lastIndex);
	};
}
#endif
