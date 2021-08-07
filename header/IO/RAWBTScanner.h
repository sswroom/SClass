#ifndef _SM_IO_RAWBTSCANNER
#define _SM_IO_RAWBTSCANNER
#include "IO/BTScanner.h"

namespace IO
{
	class RAWBTScanner : public BTScanner
	{
	private:
		struct ClassData;
		
		struct ClassData *clsData;
		Data::UInt64Map<IO::BTScanLog::ScanRecord*> *recMap;
		Sync::Mutex *recMut;

		static __stdcall UInt32 RecvThread(void *userObj);
	public:
		RAWBTScanner(Bool noCtrl);
		virtual ~RAWBTScanner();

		Bool IsError();
		virtual void HandleRecordUpdate(RecordHandler hdlr, void *userObj);
		
		virtual Bool IsScanOn();
		virtual void ScanOn();
		virtual void ScanOff();
		virtual void Close();
		virtual Bool SetScanMode(ScanMode scanMode);

		virtual Data::UInt64Map<IO::BTScanLog::ScanRecord*> *GetRecordMap(Sync::MutexUsage *mutUsage);

		void OnPacket(Int64 timeTicks, const UInt8 *packet, UOSInt packetSize);
	};
}
#endif
