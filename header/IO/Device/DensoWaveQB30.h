#ifndef _SM_IO_DEVICE_DENSOWAVEQB30
#define _SM_IO_DEVICE_DENSOWAVEQB30
#include "AnyType.h"
#include "Data/ArrayList.h"
#include "IO/CodeScanner.h"
#include "IO/Stream.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"

namespace IO
{
	namespace Device
	{
		class DensoWaveQB30 : public IO::CodeScanner
		{
		private:
			NN<IO::Stream> stm;
			ModeType currMode;
			UInt32 scanDelay;

			ScanHandler scanHdlr;
			AnyType scanHdlrObj;
			Sync::Mutex reqMut;
			Sync::Mutex recvMut;
			UInt8 *recvBuff;
			UOSInt recvSize;
			Sync::Thread thread;
			Sync::Event recvEvt;

			static void __stdcall RecvThread(NN<Sync::Thread> userObj);
			Bool ScanModeStart();
			Bool ScanModeEnd();
			Bool SettingModeStart();
			Bool SettingModeEnd();
			void ToIdleMode();
			Bool WaitForReply(UInt32 timeToWait);
			Bool WaitForReplyVal(UInt32 timeToWait, OutParam<Int32> retVal);
			Int32 ReadCommand(UnsafeArray<const UTF8Char> cmdStr, UOSInt cmdLen);
			Bool WriteCommand(UnsafeArray<const UTF8Char> cmdStr, UOSInt cmdLen);
		public:
			DensoWaveQB30(NN<IO::Stream> stm);
			virtual ~DensoWaveQB30();

			virtual void SetCurrMode(ModeType currMode);
			virtual Bool SoftReset();
			virtual Bool ResetDefault();
			virtual void HandleCodeScanned(ScanHandler hdlr, AnyType userObj);

			virtual UOSInt GetCommandList(Data::ArrayList<DeviceCommand> *cmdList);
			virtual Text::CString GetCommandName(DeviceCommand dcmd);
			virtual CommandType GetCommandParamType(DeviceCommand dcmd, Int32 *minVal, Int32 *maxVal);
			virtual Text::CString GetCommandParamName(DeviceCommand dcmd, Int32 cmdVal);
			virtual Int32 GetCommand(DeviceCommand dcmd);
			virtual Bool SetCommand(DeviceCommand dcmd, Int32 val);
		};
	}
}
#endif
