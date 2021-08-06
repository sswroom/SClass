#ifndef _SM_IO_BTDEVLOG
#define _SM_IO_BTDEVLOG
#include "Data/ArrayListUInt32.h"
#include "Data/UInt64Map.h"
#include "IO/BTScanner.h"

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
			Int32 txPower;
			Data::ArrayListUInt32 *keys;
		} DevEntry2;
	private:
		Data::UInt64Map<DevEntry2*> *logs;

		static Bool IsDefaultName(const UTF8Char *name);
	public:
		BTDevLog();
		~BTDevLog();

		DevEntry2 *AddEntry(UInt64 macInt, const UTF8Char *name, Int32 txPower, IO::BTScanLog::RadioType radioType, IO::BTScanLog::AddressType addrType);
		void AppendList(Data::UInt64Map<IO::BTScanner::ScanRecord2*> *devMap);
		void ClearList();
		Bool LoadFile(const UTF8Char *fileName);
		Bool StoreFile(const UTF8Char *fileName);

		Data::ArrayList<IO::BTDevLog::DevEntry2*> *GetLogList();
	};
}
#endif
