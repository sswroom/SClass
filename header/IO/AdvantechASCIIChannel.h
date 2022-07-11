#ifndef _SM_IO_ADVANTECHASCIICHANNEL
#define _SM_IO_ADVANTECHASCIICHANNEL
#include "IO/Stream.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace IO
{
	class AdvantechASCIIChannel
	{
	private:
		IO::Stream *stm;
		Bool stmRelease;
		Sync::Event cmdEvt;
		Sync::Mutex cmdResMut;
		UTF8Char *cmdResBuff;
		UTF8Char *cmdResEnd;

		Bool threadRunning;
		Bool threadToStop;
	private:
		static UInt32 __stdcall CmdThread(void *userObj);
	public:
		AdvantechASCIIChannel(IO::Stream *stm, Bool needRelease);
		~AdvantechASCIIChannel();

		IO::Stream *GetStream();
		
		UTF8Char *SendCommand(UTF8Char *replyBuff, const UTF8Char *cmd, UOSInt cmdLen);

		void Close();

		UTF8Char *GetFirmwareVer(UTF8Char *firmwareBuff, UInt8 addr);
		UTF8Char *GetModuleName(UTF8Char *moduleBuff, UInt8 addr);
	};
}
#endif
