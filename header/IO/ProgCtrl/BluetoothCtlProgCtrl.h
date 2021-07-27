#ifndef _SM_IO_PROGCTRL_BLUETOOTHCTLPROGCTRL
#define _SM_IO_PROGCTRL_BLUETOOTHCTLPROGCTRL
#include "Data/ArrayListUInt32.h"
#include "Data/UInt64Map.h"
#include "IO/BTScanner.h"
#include "Manage/ProcessExecution.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace IO
{
	namespace ProgCtrl
	{
		class BluetoothCtlProgCtrl : public IO::BTScanner
		{
		private:
			Manage::ProcessExecution *prog;
			Sync::Mutex *devMut;
			Data::UInt64Map<IO::BTScanner::ScanRecord*> *devMap;
			Sync::Mutex *lastCmdMut;
			const UTF8Char *lastCmd;
			IO::BTScanner::RecordHandler recHdlr;
			void *recHdlrObj;
			Bool threadRunning;
			Bool threadToStop;
			Bool agentOn;
			Bool scanOn;
			Bool cmdReady;

			static UInt32 __stdcall ReadThread(void *obj);
			void SendCmd(const Char *cmd);

			IO::BTScanner::ScanRecord *DeviceGetByStr(const UTF8Char *s);
			void DeviceFree(IO::BTScanner::ScanRecord *dev);
		public:
			BluetoothCtlProgCtrl();
			virtual ~BluetoothCtlProgCtrl();

			virtual void HandleRecordUpdate(RecordHandler hdlr, void *userObj);
			
			virtual Bool IsScanOn();
			virtual void ScanOn();
			virtual void ScanOff();
			virtual void Close();

			virtual Data::UInt64Map<IO::BTScanner::ScanRecord*> *GetRecordMap(Sync::MutexUsage *mutUsage);

			Bool WaitForCmdReady();
		};
	}
}
#endif
