#ifndef _SM_IO_BTSCANLOG
#define _SM_IO_BTSCANLOG
#include "Data/ArrayListNN.h"
#include "Data/ArrayListUInt32.h"
#include "Data/ByteArray.h"
#include "Data/FastMapNN.h"
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
			Optional<Text::String> name;
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
			Optional<Text::String> name;
			NN<Data::ArrayListNN<LogEntry>> logs;
		};
	private:
		Data::FastMapNN<UInt64, DevEntry> pubDevs;
		Data::FastMapNN<UInt64, DevEntry> randDevs;
		Data::ArrayListNN<LogEntry> logs;

		static void FreeDev(NN<DevEntry> dev);
	public:
		BTScanLog(NN<Text::String> sourceName);
		virtual ~BTScanLog();

		virtual IO::ParserType GetParserType() const;

		NN<LogEntry> AddEntry(Int64 timeTicks, UInt64 macInt, RadioType radioType, AddressType addrType, UInt16 company, Optional<Text::String> name, Int8 rssi, Int8 txPower, Int8 measurePower, AdvType advType);
		NN<LogEntry> AddScanRec(NN<const ScanRecord3> rec);
		void AddBTRAWPacket(Int64 timeTicks, Data::ByteArrayR buff);
		void ClearList();
		NN<const Data::ReadingListNN<IO::BTScanLog::DevEntry>> GetPublicList() const;
		NN<const Data::ReadingListNN<IO::BTScanLog::DevEntry>> GetRandomList() const;

		static Text::CStringNN RadioTypeGetName(RadioType radioType);
		static Text::CStringNN AddressTypeGetName(AddressType addrType);
		static Text::CStringNN AdvTypeGetName(AdvType advType);
		static Bool ParseBTRAWPacket(NN<ScanRecord3> rec, Int64 timeTicks, Data::ByteArrayR buff);
		static void ParseAdvisement(NN<ScanRecord3> rec, UnsafeArray<const UInt8> buff, UOSInt ofst, UOSInt endOfst);
	};
}
#endif
