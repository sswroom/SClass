#ifndef _SM_IO_DEVICE_AXCAN
#define _SM_IO_DEVICE_AXCAN
#include "IO/CANHandler.h"
#include "IO/CANListener.h"
#include "IO/Reader.h"
#include "IO/Stream.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace Device
	{
		class AXCAN : public IO::CANListener
		{
		public:
			enum class CANBitRate
			{
				BR_5K,
				BR_10K,
				BR_20K,
				BR_40K,
				BR_50K,
				BR_80K,
				BR_100K,
				BR_125K,
				BR_200K,
				BR_250K,
				BR_400K,
				BR_500K,
				BR_600K,
				BR_800K,
				BR_1000K
			};
		private:
			CANHandler *hdlr;
			Sync::Mutex cmdMut;
			Sync::Event cmdEvent;
			UOSInt cmdResultCode;
			IO::Stream *stm;
			Bool threadRunning;

			Bool SendSetCANBitRate(CANBitRate bitRate);
			Bool SendOpenCANPort(UInt8 port, Bool silentMode, Bool loopback);
			Bool SendCloseCANPort(UInt8 port);
			Bool SendSetReportMode(Bool autoMode, Bool formattedData, Bool overwriteOnFull);
			Bool SendCommandMode();
			Bool SendCommand(Text::CString cmd, UOSInt timeout);
			
			static UInt32 __stdcall SerialThread(void *userObj);
		public:
			AXCAN(CANHandler *hdlr);
			virtual ~AXCAN();

			Bool OpenSerialPort(UOSInt portNum, UInt32 serialBaudRate, CANBitRate bitRate);
			Bool OpenStream(IO::Stream *stm, CANBitRate bitRate);
			void CloseSerialPort(Bool force);

			virtual void CANStop();
			virtual void ToString(Text::StringBuilderUTF8 *sb) const;

			void ParseReader(IO::Reader *reader);
		};
	}
}
#endif