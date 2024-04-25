#ifndef _SM_IO_RAWBTSCANNER
#define _SM_IO_RAWBTSCANNER
#include "IO/BTScanner.h"
#include "Sync/Thread.h"

namespace IO
{
	class RAWBTScanner : public BTScanner
	{
	private:
		struct ClassData;
		
		struct ClassData *clsData;
		Sync::Thread thread;
		Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3> pubRecMap;
		Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3> randRecMap;
		Sync::Mutex recMut;

		static void __stdcall RecvThread(NN<Sync::Thread> thread);
		static void FreeRec(NN<IO::BTScanLog::ScanRecord3> rec);
	public:
		RAWBTScanner(Bool noCtrl);
		virtual ~RAWBTScanner();

		Bool IsError();
		virtual void HandleRecordUpdate(RecordHandler hdlr, AnyType userObj);
		
		virtual Bool IsScanOn();
		virtual void ScanOn();
		virtual void ScanOff();
		virtual void Close();
		virtual Bool SetScanMode(ScanMode scanMode);

		virtual NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> GetPublicMap(NN<Sync::MutexUsage> mutUsage);
		virtual NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> GetRandomMap(NN<Sync::MutexUsage> mutUsage);

		void OnPacket(Int64 timeTicks, Data::ByteArrayR packet);
	};
}
#endif
