#ifndef _SM_IO_MODEMCONTROLLER
#define _SM_IO_MODEMCONTROLLER
#include "IO/ATCommandChannel.h"
#include "Data/DateTime.h"
#include "Text/SMSMessage.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	class ModemController
	{
	public:
		typedef enum
		{
			DR_ERROR,
			DR_CONNECT,		// To Connect mode, Requre hangup to return normal
			DR_NO_CARRIER,  //
			DR_NO_DIALTONE, // Line not plugged
			DR_BUSY			// Busy
		} DialResult;

	protected:
		IO::ATCommandChannel *channel;
		Bool needRelease;
		Data::ArrayList<const Char *> *cmdResults;
		Sync::Mutex *cmdMut;

	protected:
		void ClearCmdResult();
		Bool IsCmdSucceed();

		UTF8Char *SendStringCommand(UTF8Char *buff, const Char *cmd, Int32 timeoutMS);
		Bool SendStringCommand(Data::ArrayList<const Char*> *resList, const Char *cmd, Int32 timeoutMS);
		UTF8Char *SendStringCommandDirect(UTF8Char *buff, const Char *cmd, Int32 timeoutMS);
		Bool SendStringListCommand(Text::StringBuilderUTF *sb, const Char *cmd);
		Bool SendBoolCommand(const Char *cmd);
		Bool SendBoolCommand(const Char *cmd, Int32 timeoutMS);
		DialResult SendDialCommand(const Char *cmd);

	public:
		ModemController(IO::ATCommandChannel *channel, Bool needRelease);
		virtual ~ModemController();

		IO::ATCommandChannel *GetChannel();
		
		// General Commands V.25TER
		Bool HangUp(); //ATH
		Bool ResetModem(); //ATZ
		Bool SetEcho(Bool showEcho); //ATE
	};
};
#endif
