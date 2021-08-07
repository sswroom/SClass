#ifndef _SM_IO_BTSCANLOG
#define _SM_IO_BTSCANLOG
#include "Data/ArrayListUInt32.h"
#include "Data/UInt64Map.h"
#include "IO/ParsedObject.h"

namespace IO
{
	class BTScanLog : public IO::ParsedObject
	{
	public:
		enum AddressType
		{
			AT_UNKNOWN,
			AT_PUBLIC,
			AT_RANDOM
		};

		enum RadioType
		{
			RT_UNKNOWN,
			RT_HCI,
			RT_LE
		};

		struct LogEntry
		{
			UInt64 macInt;
			Int64 timeTicks;
			Int8 rssi;
			Int8 txPower;
		};
		
		struct DevEntry
		{
			UInt64 macInt;
			RadioType radioType;
			AddressType addrType;
			UInt16 company;
			const UTF8Char *name;
			Data::ArrayList<LogEntry*> *logs;
		};
	private:
		Data::UInt64Map<DevEntry*> *devs;
		Data::ArrayList<LogEntry*> *logs;

		void FreeDev(DevEntry* dev);
	public:
		BTScanLog(const UTF8Char *sourceName);
		virtual ~BTScanLog();

		virtual IO::ParsedObject::ParserType GetParserType();

		LogEntry *AddEntry(Int64 timeTicks, UInt64 macInt, RadioType radioType, AddressType addrType, UInt16 company, const UTF8Char *name, Int8 rssi, Int8 txPower);
		void AddBTRAWPacket(Int64 timeTicks, const UInt8 *buff, UOSInt buffSize);
		void ClearList();
		Data::ArrayList<IO::BTScanLog::DevEntry*> *GetDevList();

		static const UTF8Char *RadioTypeGetName(RadioType radioType);
		static const UTF8Char *AddressTypeGetName(AddressType addrType);
	};
}
#endif
