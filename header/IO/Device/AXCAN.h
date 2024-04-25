#ifndef _SM_IO_DEVICE_AXCAN
#define _SM_IO_DEVICE_AXCAN
#include "IO/CANHandler.h"
#include "IO/CANListener.h"
#include "IO/Reader.h"
#include "IO/Stream.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"

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
			NN<CANHandler> hdlr;
			Sync::Mutex cmdMut;
			Sync::Event cmdEvent;
			UOSInt cmdResultCode;
			Optional<IO::Stream> stm;
			Sync::Thread serialThread;

			Bool SendSetCANBitRate(CANBitRate bitRate);
			Bool SendOpenCANPort(UInt8 port, Bool silentMode, Bool loopback);
			Bool SendCloseCANPort(UInt8 port);
			Bool SendSetReportMode(Bool autoMode, Bool formattedData, Bool overwriteOnFull);
			Bool SendCommandMode();
			Bool SendCommand(Text::CString cmd, UOSInt timeout);
			
			static void __stdcall SerialThread(NN<Sync::Thread> userObj);
		public:
			AXCAN(NN<CANHandler> hdlr);
			virtual ~AXCAN();

			Bool OpenSerialPort(UOSInt portNum, UInt32 serialBaudRate, CANBitRate bitRate);
			Bool OpenStream(NN<IO::Stream> stm, CANBitRate bitRate);
			void CloseSerialPort(Bool force);

			virtual void CANStop();
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;

			void ParseReader(NN<IO::Reader> reader);
		};
	}
}
#endif
