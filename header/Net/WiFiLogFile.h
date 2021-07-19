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
			const UTF8Char *ssid;
			Int32 phyType;
			Double freq;
			const UTF8Char *manuf;
			const UTF8Char *model;
			const UTF8Char *serialNum;
			const UTF8Char *country;
			UInt8 ouis[3][3];
			UInt64 neighbour[20];
			UInt32 ieLen;
			UInt8 *ieBuff;
			Int64 lastScanTimeTicks;
		} LogFileEntry;

	private:
		Data::ArrayList<LogFileEntry*> *logList;

		UOSInt DirectInsert(LogFileEntry *log);
	public:
		WiFiLogFile();
		~WiFiLogFile();

		void LoadFile(const UTF8Char *fileName);
		Bool StoreFile(const UTF8Char *fileName);
		void Clear();
		LogFileEntry *Get(UInt64 iMAC);
		OSInt GetIndex(UInt64 iMAC);
		Data::ArrayList<LogFileEntry*> *GetLogList();
		const LogFileEntry *GetItem(UOSInt index);

		LogFileEntry *AddBSSInfo(Net::WirelessLAN::BSSInfo *bss, OSInt *lastIndex);
	};
}
#endif
