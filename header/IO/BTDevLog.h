#ifndef _SM_IO_BTDEVLOG
#define _SM_IO_BTDEVLOG
#include "Data/ArrayListUInt32.h"
#include "Data/UInt64Map.h"
#include "IO/BTScanLog.h"

namespace IO
{
	class BTDevLog
	{
	public:
		typedef struct
		{
			UInt8 mac[6];
			UInt64 macInt;
			const UTF8Char *name;
			IO::BTScanLog::RadioType radioType;
			IO::BTScanLog::AddressType addrType;
			Int8 txPower;
			Int8 measurePower;
			UInt16 company;
		} DevEntry;
	private:
		Data::UInt64Map<DevEntry*> *pubDevs;
		Data::UInt64Map<DevEntry*> *randDevs;

		static Bool IsDefaultName(const UTF8Char *name);
		void FreeDev(DevEntry *dev);
	public:
		BTDevLog();
		~BTDevLog();

		DevEntry *AddEntry(UInt64 macInt, const UTF8Char *name, Int8 txPower, Int8 measurePower, IO::BTScanLog::RadioType radioType, IO::BTScanLog::AddressType addrType, UInt16 company);
		void AppendList(Data::UInt64Map<IO::BTScanLog::ScanRecord2*> *devMap);
		void ClearList();
		Bool LoadFile(const UTF8Char *fileName);
		Bool StoreFile(const UTF8Char *fileName);

		Data::ArrayList<IO::BTDevLog::DevEntry*> *GetPublicList();
		Data::ArrayList<IO::BTDevLog::DevEntry*> *GetRandomList();
	};
}
#endif
