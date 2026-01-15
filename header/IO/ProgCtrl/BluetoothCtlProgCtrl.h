#ifndef _SM_IO_PROGCTRL_BLUETOOTHCTLPROGCTRL
#define _SM_IO_PROGCTRL_BLUETOOTHCTLPROGCTRL
#include "AnyType.h"
#include "Data/ArrayListUInt32.h"
#include "Data/FastMapNN.hpp"
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
			NN<Manage::ProcessExecution> prog;
			Sync::Mutex devMut;
			Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3> devMap;
			Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3> randDevMap;
			Sync::Mutex lastCmdMut;
			Optional<Text::String> lastCmd;
			IO::BTScanner::RecordHandler recHdlr;
			AnyType recHdlrObj;
			Sync::Thread thread;
			Bool agentOn;
			Bool scanOn;
			Bool cmdReady;

			static void __stdcall ReadThread(NN<Sync::Thread> thread);
			void SendCmd(UnsafeArray<const UTF8Char> cmd, UIntOS cmdLen);

			Optional<IO::BTScanLog::ScanRecord3> DeviceGetByStr(Text::CStringNN s);
			static void DeviceFree(NN<IO::BTScanLog::ScanRecord3> dev);
		public:
			BluetoothCtlProgCtrl();
			virtual ~BluetoothCtlProgCtrl();

			virtual void HandleRecordUpdate(RecordHandler hdlr, AnyType userObj);
			
			virtual Bool IsScanOn();
			virtual void ScanOn();
			virtual void ScanOff();
			virtual void Close();
			virtual Bool SetScanMode(ScanMode scanMode);

			virtual NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> GetPublicMap(NN<Sync::MutexUsage> mutUsage);
			virtual NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> GetRandomMap(NN<Sync::MutexUsage> mutUsage);

			Bool WaitForCmdReady();
		};
	}
}
#endif
