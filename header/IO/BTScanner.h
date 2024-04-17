#ifndef _SM_IO_BTSCANNER
#define _SM_IO_BTSCANNER
#include "AnyType.h"
#include "Data/FastMapNN.h"
#include "IO/BTScanLog.h"
#include "Sync/MutexUsage.h"

namespace IO
{
	class BTScanner
	{
	public:
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
		
		typedef void (__stdcall *RecordHandler)(NotNullPtr<IO::BTScanLog::ScanRecord3> rec, UpdateType updateType, AnyType userObj);
	public:
		virtual ~BTScanner() {};

		virtual void HandleRecordUpdate(RecordHandler hdlr, AnyType userObj) = 0;
		
		virtual Bool IsScanOn() = 0;
		virtual void ScanOn() = 0;
		virtual void ScanOff() = 0;
		virtual void Close() = 0;
		virtual Bool SetScanMode(ScanMode scanMode) = 0;

		virtual NotNullPtr<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> GetPublicMap(NotNullPtr<Sync::MutexUsage> mutUsage) = 0;
		virtual NotNullPtr<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> GetRandomMap(NotNullPtr<Sync::MutexUsage> mutUsage) = 0;

		static Optional<BTScanner> CreateScanner();
	};
}
#endif
