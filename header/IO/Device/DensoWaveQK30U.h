#ifndef _SM_IO_DEVICE_DENSOWAVEQK30U
#define _SM_IO_DEVICE_DENSOWAVEQK30U
#include "AnyType.h"
#include "Data/ArrayList.h"
#include "IO/CodeScanner.h"
#include "IO/Stream.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace Device
	{
		class DensoWaveQK30U : public IO::CodeScanner
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
			Bool recvRunning;
			Bool recvToStop;
			Sync::Event recvEvt;

			static UInt32 __stdcall RecvThread(AnyType userObj);
			Bool ScanModeStart();
			Bool ScanModeEnd();
			Bool SettingModeStart();
			Bool SettingModeEnd();
			void ToIdleMode();
			Bool WaitForReply(UInt32 timeToWait);
			Bool WaitForReplyVal(UInt32 timeToWait, OutParam<Int32> retVal);
			Int32 ReadCommand(const Char *cmdStr, UOSInt cmdLen);
			Bool WriteCommand(const Char *cmdStr, UOSInt cmdLen);
		public:
			DensoWaveQK30U(NN<IO::Stream> stm);
			virtual ~DensoWaveQK30U();

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
