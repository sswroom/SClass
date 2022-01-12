#ifndef _SM_IO_PROGCTRL_BLUETOOTHCTLPROGCTRL
#define _SM_IO_PROGCTRL_BLUETOOTHCTLPROGCTRL
#include "Data/ArrayListUInt32.h"
#include "Data/UInt64Map.h"
#include "IO/BTScanner.h"
#include "Manage/ProcessExecution.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Text/String.h"

namespace IO
{
	namespace ProgCtrl
	{
		class BluetoothCtlProgCtrl : public IO::BTScanner
		{
		private:
			Manage::ProcessExecution *prog;
			Sync::Mutex *devMut;
			Data::UInt64Map<IO::BTScanLog::ScanRecord3*> *devMap;
			Data::UInt64Map<IO::BTScanLog::ScanRecord3*> *randDevMap;
			Sync::Mutex *lastCmdMut;
			Text::String *lastCmd;
			IO::BTScanner::RecordHandler recHdlr;
			void *recHdlrObj;
			Bool threadRunning;
			Bool threadToStop;
			Bool agentOn;
			Bool scanOn;
			Bool cmdReady;

			static UInt32 __stdcall ReadThread(void *obj);
			void SendCmd(const UTF8Char *cmd, UOSInt cmdLen);

			IO::BTScanLog::ScanRecord3 *DeviceGetByStr(const UTF8Char *s, UOSInt len);
			void DeviceFree(IO::BTScanLog::ScanRecord3 *dev);
		public:
			BluetoothCtlProgCtrl();
			virtual ~BluetoothCtlProgCtrl();

			virtual void HandleRecordUpdate(RecordHandler hdlr, void *userObj);
			
			virtual Bool IsScanOn();
			virtual void ScanOn();
			virtual void ScanOff();
			virtual void Close();
			virtual Bool SetScanMode(ScanMode scanMode);

			virtual Data::UInt64Map<IO::BTScanLog::ScanRecord3*> *GetPublicMap(Sync::MutexUsage *mutUsage);
			virtual Data::UInt64Map<IO::BTScanLog::ScanRecord3*> *GetRandomMap(Sync::MutexUsage *mutUsage);

			Bool WaitForCmdReady();
		};
	}
}
#endif
