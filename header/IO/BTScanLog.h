#ifndef _SM_IO_BTSCANLOG
#define _SM_IO_BTSCANLOG
#include "Data/ArrayListUInt32.h"
#include "Data/UInt64Map.h"
#include "IO/ParsedObject.h"
#include "Text/String.h"

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

		enum AdvType
		{
			ADVT_UNKNOWN,
			ADVT_IBEACON,
			ADVT_FINDMY_BROADCAST,
			ADVT_HOMEKIT,
			ADVT_AIRDROP,
			ADVT_AIRPLAY_SRC,
			ADVT_AIRPLAY_TARGET,
			ADVT_AIRPRINT,
			ADVT_HANDOFF,
			ADVT_MAGIC_SWITCH,
			ADVT_NEARBY_ACTION,
			ADVT_NEARBY_INFO,
			ADVT_PROXIMITY_PAIRING,
			ADVT_TETHERING_SRC,
			ADVT_TETHERING_TARGET,
			ADVT_EDDYSTONE,
			ADVT_ALTBEACON
		};

		typedef struct
		{
			UInt8 mac[6];
			UInt64 macInt;
			Bool inRange;
			RadioType radioType;
			AddressType addrType;
			Text::String *name;
			Int8 rssi;
			Int8 measurePower;
			Int8 txPower;
			Bool connected;
			Int64 lastSeenTime;
			UInt16 company;
			AdvType advType;
		} ScanRecord3;

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
			AdvType lastAdvType;
			Text::String *name;
			Data::ArrayList<LogEntry*> *logs;
		};
	private:
		Data::UInt64Map<DevEntry*> pubDevs;
		Data::UInt64Map<DevEntry*> randDevs;
		Data::ArrayList<LogEntry*> logs;

		void FreeDev(DevEntry* dev);
	public:
		BTScanLog(Text::String *sourceName);
		virtual ~BTScanLog();

		virtual IO::ParserType GetParserType() const;

		LogEntry *AddEntry(Int64 timeTicks, UInt64 macInt, RadioType radioType, AddressType addrType, UInt16 company, Text::String *name, Int8 rssi, Int8 txPower, Int8 measurePower, AdvType advType);
		LogEntry *AddScanRec(const ScanRecord3 *rec);
		void AddBTRAWPacket(Int64 timeTicks, const UInt8 *buff, UOSInt buffSize);
		void ClearList();
		const Data::ArrayList<IO::BTScanLog::DevEntry*> *GetPublicList() const;
		const Data::ArrayList<IO::BTScanLog::DevEntry*> *GetRandomList() const;

		static Text::CString RadioTypeGetName(RadioType radioType);
		static Text::CString AddressTypeGetName(AddressType addrType);
		static Text::CString AdvTypeGetName(AdvType advType);
		static Bool ParseBTRAWPacket(ScanRecord3 *rec, Int64 timeTicks, const UInt8 *buff, UOSInt buffSize);
		static void ParseAdvisement(ScanRecord3 *rec, const UInt8 *buff, UOSInt ofst, UOSInt endOfst);
	};
}
#endif
