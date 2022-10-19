#ifndef _SM_IO_BTDEVLOG
#define _SM_IO_BTDEVLOG
#include "Data/ArrayListUInt32.h"
#include "Data/FastMap.h"
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
			Text::String *name;
			IO::BTScanLog::RadioType radioType;
			IO::BTScanLog::AddressType addrType;
			Int8 txPower;
			Int8 measurePower;
			UInt16 company;
			IO::BTScanLog::AdvType advType;
		} DevEntry;
	private:
		Data::FastMap<UInt64, DevEntry*> pubDevs;
		Data::FastMap<UInt64, DevEntry*> randDevs;

		static Bool IsDefaultName(Text::String *name);
		void FreeDev(DevEntry *dev);
	public:
		BTDevLog();
		~BTDevLog();

		DevEntry *AddEntry(UInt64 macInt, Text::String *name, Int8 txPower, Int8 measurePower, IO::BTScanLog::RadioType radioType, IO::BTScanLog::AddressType addrType, UInt16 company, IO::BTScanLog::AdvType advType);
		void AppendList(Data::FastMap<UInt64, IO::BTScanLog::ScanRecord3*> *devMap);
		void ClearList();
		Bool LoadFile(Text::CString fileName);
		Bool StoreFile(Text::CString fileName);

		const Data::ReadingList<IO::BTDevLog::DevEntry*> *GetPublicList() const;
		const Data::ReadingList<IO::BTDevLog::DevEntry*> *GetRandomList() const;
	};
}
#endif
