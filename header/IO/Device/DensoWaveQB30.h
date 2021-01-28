#ifndef _SM_IO_DEVICE_DENSOWAVEQB30
#define _SM_IO_DEVICE_DENSOWAVEQB30
#include "Data/ArrayList.h"
#include "IO/CodeScanner.h"
#include "IO/Stream.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace Device
	{
		class DensoWaveQB30 : public IO::CodeScanner
		{
		private:
			IO::Stream *stm;
			ModeType currMode;
			Int32 scanDelay;

			ScanHandler scanHdlr;
			void *scanHdlrObj;
			Sync::Mutex *reqMut;
			Sync::Mutex *recvMut;
			UInt8 *recvBuff;
			OSInt recvSize;
			Bool recvRunning;
			Bool recvToStop;
			Sync::Event *recvEvt;

			static UInt32 __stdcall RecvThread(void *userObj);
			Bool ScanModeStart();
			Bool ScanModeEnd();
			Bool SettingModeStart();
			Bool SettingModeEnd();
			void ToIdleMode();
			Bool WaitForReply(Int32 timeToWait);
			Bool WaitForReplyVal(Int32 timeToWait, Int32 *retVal);
			Int32 ReadCommand(const Char *cmdStr, UOSInt cmdLen);
			Bool WriteCommand(const Char *cmdStr, UOSInt cmdLen);
		public:
			DensoWaveQB30(IO::Stream *stm);
			virtual ~DensoWaveQB30();

			virtual void SetCurrMode(ModeType currMode);
			virtual Bool SoftReset();
			virtual Bool ResetDefault();
			virtual void HandleCodeScanned(ScanHandler hdlr, void *userObj);

			virtual OSInt GetCommandList(Data::ArrayList<DeviceCommand> *cmdList);
			virtual const UTF8Char *GetCommandName(DeviceCommand dcmd);
			virtual CommandType GetCommandParamType(DeviceCommand dcmd, Int32 *minVal, Int32 *maxVal);
			virtual const UTF8Char *GetCommandParamName(DeviceCommand dcmd, Int32 cmdVal);
			virtual Int32 GetCommand(DeviceCommand dcmd);
			virtual Bool SetCommand(DeviceCommand dcmd, Int32 val);
		};
	}
}
#endif
