#ifndef _SM_IO_BTSCANNER
#define _SM_IO_BTSCANNER
#include "Data/ArrayListUInt32.h"
#include "Data/UInt64Map.h"
#include "IO/BTScanLog.h"
#include "Sync/MutexUsage.h"

namespace IO
{
	class BTScanner
	{
	public:
		typedef struct
		{
			UInt8 mac[6];
			UInt64 macInt;
			Bool inRange;
			IO::BTScanLog::RadioType radioType;
			IO::BTScanLog::AddressType addrType;
			const UTF8Char *name;
			Int8 rssi;
			Int8 txPower;
			Bool connected;
			Int64 lastSeenTime;
			UInt16 company;
		} ScanRecord2;

		typedef enum
		{
			UT_NEW_DEVICE,
			UT_RSSI,
			UT_TXPOWER,
			UT_CONNECT,
			UT_NAME,
			UT_COMPANY,
			UT_OTHER
		} UpdateType;

		typedef enum
		{
			SM_ACTIVE,
			SM_PASSIVE
		} ScanMode;
		
		typedef void (__stdcall *RecordHandler)(ScanRecord2 *rec, UpdateType updateType, void *userObj);
	public:
		virtual ~BTScanner() {};

		virtual void HandleRecordUpdate(RecordHandler hdlr, void *userObj) = 0;
		
		virtual Bool IsScanOn() = 0;
		virtual void ScanOn() = 0;
		virtual void ScanOff() = 0;
		virtual void Close() = 0;
		virtual Bool SetScanMode(ScanMode scanMode) = 0;

		virtual Data::UInt64Map<ScanRecord2*> *GetRecordMap(Sync::MutexUsage *mutUsage) = 0;

		static BTScanner *CreateScanner();
	};
}
#endif
