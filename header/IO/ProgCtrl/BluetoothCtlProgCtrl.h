#ifndef _SM_IO_PROGCTRL_BLUETOOTHCTLPROGCTRL
#define _SM_IO_PROGCTRL_BLUETOOTHCTLPROGCTRL
#include "Data/ArrayListUInt32.h"
#include "Data/UInt64Map.h"
#include "Manage/ProcessExecution.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace IO
{
	namespace ProgCtrl
	{
		class BluetoothCtlProgCtrl
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
				Data::ArrayListUInt32 *keys;
			} DeviceInfo;

			typedef void (__stdcall *DeviceHandler)(DeviceInfo *dev, void *userObj);
		private:
			Manage::ProcessExecution *prog;
			Sync::Mutex *devMut;
			Data::UInt64Map<DeviceInfo*> *devMap;
			Sync::Mutex *lastCmdMut;
			const UTF8Char *lastCmd;
			DeviceHandler devHdlr;
			void *devHdlrObj;
			Bool threadRunning;
			Bool threadToStop;
			Bool agentOn;
			Bool scanOn;
			Bool cmdReady;

			static UInt32 __stdcall ReadThread(void *obj);
			void SendCmd(const Char *cmd);

			DeviceInfo *DeviceGetByStr(const UTF8Char *s);
			void DeviceFree(DeviceInfo *dev);
		public:
			BluetoothCtlProgCtrl();
			~BluetoothCtlProgCtrl();

			void HandleDeviceUpdate(DeviceHandler hdlr, void *userObj);
			
			Bool IsScanOn();
			void ScanOn();
			void ScanOff();
			void Exit();

			Bool WaitForCmdReady();

			Data::UInt64Map<DeviceInfo*> *GetDeviceMap(Sync::MutexUsage *mutUsage);
		};
	}
}
#endif
