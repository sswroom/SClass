#ifndef _SM_IO_TVCTRL_NECTVCONTROL
#define _SM_IO_TVCTRL_NECTVCONTROL
#include "AnyType.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "IO/TVControl.h"
#include "Sync/Mutex.h"

#define NECTVCONTROL_RECVBUFFSIZE 256

namespace IO
{
	namespace TVCtrl
	{
		class NECTVControl : public IO::TVControl
		{
		private:
			NN<IO::Stream> stm;
			Int32 monId;
			Data::DateTime nextTime;

			Sync::Mutex mut;
			UInt8 recvBuff[NECTVCONTROL_RECVBUFFSIZE];
			UIntOS recvSize;
			Bool recvRunning;
			Bool recvToStop;
			Sync::Event recvEvt;

			static UInt32 __stdcall RecvThread(AnyType userObj);
			Bool SendCommand(Text::CStringNN cmd, UnsafeArray<UTF8Char> cmdReply, Int32 cmdTimeout);
			Bool GetParameter(UInt8 opCodePage, UInt8 opCode, UInt16 *maxVal, UInt16 *currVal, Int32 cmdTimeout);
			Bool SetParameter(UInt8 opCodePage, UInt8 opCode, UInt16 val, Int32 cmdTimeout);
		public:
			NECTVControl(NN<IO::Stream> stm, Int32 monId);
			virtual ~NECTVControl();

			virtual Bool SendInstruction(CommandType ct);
			virtual Bool SendGetCommand(CommandType ct, Int32 *val, UnsafeArray<UTF8Char> sbuff);
			virtual Bool SendSetCommand(CommandType ct, Int32 val);
			virtual void GetSupportedCmd(Data::ArrayListNative<CommandType> *cmdList, Data::ArrayListNative<CommandFormat> *cmdFormats);
			static Bool GetInfo(IO::TVControl::TVInfo *info);
		};
	}
}
#endif
