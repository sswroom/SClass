#ifndef _SM_IO_MODEMCONTROLLER
#define _SM_IO_MODEMCONTROLLER
#include "Data/ArrayListStringNN.h"
#include "Data/DateTime.h"
#include "IO/ATCommandChannel.h"
#include "Text/SMSMessage.h"
#include "Text/StringBuilderUTF8.h"

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
		Data::ArrayListStringNN cmdResults;
		Sync::Mutex cmdMut;

	protected:
		void ClearCmdResult();
		Bool IsCmdSucceed();

		UTF8Char *SendStringCommand(UTF8Char *buff, const UTF8Char *cmd, UOSInt cmdLen, Data::Duration timeout);
		Bool SendStringCommand(Data::ArrayListStringNN *resList, const UTF8Char *cmd, UOSInt cmdLen, Data::Duration timeout);
		UTF8Char *SendStringCommandDirect(UTF8Char *buff, const UTF8Char *cmd, UOSInt cmdLen, Data::Duration timeout);
		Bool SendStringListCommand(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *cmd, UOSInt cmdLen);
		Bool SendBoolCommandC(const UTF8Char *cmd, UOSInt cmdLen);
		Bool SendBoolCommandC(const UTF8Char *cmd, UOSInt cmdLen, Data::Duration timeout);
		DialResult SendDialCommand(const UTF8Char *cmd, UOSInt cmdLen);

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
