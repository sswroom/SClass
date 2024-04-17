#ifndef _SM_IO_PROGCTRL_BLUETOOTHCTLPROGCTRL
#define _SM_IO_PROGCTRL_BLUETOOTHCTLPROGCTRL
#include "AnyType.h"
#include "Data/ArrayListUInt32.h"
#include "Data/FastMapNN.h"
#include "IO/BTScanner.h"
#include "Manage/ProcessExecution.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/String.h"

namespace IO
{
	namespace ProgCtrl
	{
		class BluetoothCtlProgCtrl : public IO::BTScanner
		{
		private:
			Manage::ProcessExecution *prog;
			Sync::Mutex devMut;
			Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3> devMap;
			Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3> randDevMap;
			Sync::Mutex lastCmdMut;
			Text::String *lastCmd;
			IO::BTScanner::RecordHandler recHdlr;
			AnyType recHdlrObj;
			Sync::Thread thread;
			Bool agentOn;
			Bool scanOn;
			Bool cmdReady;

			static void __stdcall ReadThread(NotNullPtr<Sync::Thread> thread);
			void SendCmd(const UTF8Char *cmd, UOSInt cmdLen);

			Optional<IO::BTScanLog::ScanRecord3> DeviceGetByStr(Text::CStringNN s);
			static void DeviceFree(NotNullPtr<IO::BTScanLog::ScanRecord3> dev);
		public:
			BluetoothCtlProgCtrl();
			virtual ~BluetoothCtlProgCtrl();

			virtual void HandleRecordUpdate(RecordHandler hdlr, AnyType userObj);
			
			virtual Bool IsScanOn();
			virtual void ScanOn();
			virtual void ScanOff();
			virtual void Close();
			virtual Bool SetScanMode(ScanMode scanMode);

			virtual NotNullPtr<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> GetPublicMap(NotNullPtr<Sync::MutexUsage> mutUsage);
			virtual NotNullPtr<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> GetRandomMap(NotNullPtr<Sync::MutexUsage> mutUsage);

			Bool WaitForCmdReady();
		};
	}
}
#endif
