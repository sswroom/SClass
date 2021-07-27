#ifndef _SM_IO_BTSCANNER
#define _SM_IO_BTSCANNER
#include "Data/ArrayListUInt32.h"
#include "Data/UInt64Map.h"
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
			const UTF8Char *name;
			Int32 rssi;
			Int32 txPower;
			Bool connected;
			Int64 lastSeenTime;
			Data::ArrayListUInt32 *keys;
		} ScanRecord;

		typedef enum
		{
			UT_NEW_DEVICE,
			UT_RSSI,
			UT_TXPOWER,
			UT_CONNECT,
			UT_NAME,
			UT_OTHER
		} UpdateType;
		
		typedef void (__stdcall *RecordHandler)(ScanRecord *rec, UpdateType updateType, void *userObj);
	public:
		virtual ~BTScanner() {};

		virtual void HandleRecordUpdate(RecordHandler hdlr, void *userObj) = 0;
		
		virtual Bool IsScanOn() = 0;
		virtual void ScanOn() = 0;
		virtual void ScanOff() = 0;
		virtual void Close() = 0;

		virtual Data::UInt64Map<ScanRecord*> *GetRecordMap(Sync::MutexUsage *mutUsage) = 0;

		static BTScanner *CreateScanner();
	};
}
#endif
