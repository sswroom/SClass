#ifndef _SM_IO_BTDEVLOG
#define _SM_IO_BTDEVLOG
#include "Data/ArrayListUInt32.h"
#include "Data/FastMapNN.h"
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
			Optional<Text::String> name;
			IO::BTScanLog::RadioType radioType;
			IO::BTScanLog::AddressType addrType;
			Int8 txPower;
			Int8 measurePower;
			UInt16 company;
			IO::BTScanLog::AdvType advType;
		} DevEntry;
	private:
		Data::FastMapNN<UInt64, DevEntry> pubDevs;
		Data::FastMapNN<UInt64, DevEntry> randDevs;

		static Bool IsDefaultName(NN<Text::String> name);
		static void __stdcall FreeDev(NN<DevEntry> dev);
	public:
		BTDevLog();
		~BTDevLog();

		NN<DevEntry> AddEntry(UInt64 macInt, Optional<Text::String> name, Int8 txPower, Int8 measurePower, IO::BTScanLog::RadioType radioType, IO::BTScanLog::AddressType addrType, UInt16 company, IO::BTScanLog::AdvType advType);
		void AppendList(NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> devMap);
		void ClearList();
		Bool LoadFile(Text::CStringNN fileName);
		Bool StoreFile(Text::CStringNN fileName);

		NN<const Data::ReadingListNN<IO::BTDevLog::DevEntry>> GetPublicList() const;
		NN<const Data::ReadingListNN<IO::BTDevLog::DevEntry>> GetRandomList() const;
	};
}
#endif
