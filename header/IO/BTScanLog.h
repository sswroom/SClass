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

		typedef struct
		{
			UInt8 mac[6];
			UInt64 macInt;
			Bool inRange;
			RadioType radioType;
			AddressType addrType;
			const UTF8Char *name;
			Int8 rssi;
			Int8 measurePower;
			Int8 txPower;
			Bool connected;
			Int64 lastSeenTime;
			UInt16 company;
		} ScanRecord2;

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
			Int8 measurePower;
			const UTF8Char *name;
			Data::ArrayList<LogEntry*> *logs;
		};
	private:
		Data::UInt64Map<DevEntry*> *pubDevs;
		Data::UInt64Map<DevEntry*> *randDevs;
		Data::ArrayList<LogEntry*> *logs;

		void FreeDev(DevEntry* dev);
	public:
		BTScanLog(const UTF8Char *sourceName);
		virtual ~BTScanLog();

		virtual IO::ParsedObject::ParserType GetParserType();

		LogEntry *AddEntry(Int64 timeTicks, UInt64 macInt, RadioType radioType, AddressType addrType, UInt16 company, const UTF8Char *name, Int8 rssi, Int8 txPower, Int8 measurePower);
		LogEntry *AddScanRec(const ScanRecord2 *rec);
		void AddBTRAWPacket(Int64 timeTicks, const UInt8 *buff, UOSInt buffSize);
		void ClearList();
		Data::ArrayList<IO::BTScanLog::DevEntry*> *GetPublicList();
		Data::ArrayList<IO::BTScanLog::DevEntry*> *GetRandomList();

		static const UTF8Char *RadioTypeGetName(RadioType radioType);
		static const UTF8Char *AddressTypeGetName(AddressType addrType);
		static Bool ParseBTRAWPacket(ScanRecord2 *rec, Int64 timeTicks, const UInt8 *buff, UOSInt buffSize);
	};
}
#endif
