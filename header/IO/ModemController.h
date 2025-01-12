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
		NN<IO::ATCommandChannel> channel;
		Bool needRelease;
		Data::ArrayListStringNN cmdResults;
		Sync::Mutex cmdMut;

	protected:
		void ClearCmdResult();
		Bool IsCmdSucceed();

		UnsafeArrayOpt<UTF8Char> SendStringCommand(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen, Data::Duration timeout);
		Bool SendStringCommand(NN<Data::ArrayListStringNN> resList, UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen, Data::Duration timeout);
		UnsafeArrayOpt<UTF8Char> SendStringCommandDirect(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen, Data::Duration timeout);
		Bool SendStringListCommand(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen);
		Bool SendBoolCommandC(UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen);
		Bool SendBoolCommandC(UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen, Data::Duration timeout);
		DialResult SendDialCommand(UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen);

	public:
		ModemController(NN<IO::ATCommandChannel> channel, Bool needRelease);
		virtual ~ModemController();

		NN<IO::ATCommandChannel> GetChannel() const;
		
		// General Commands V.25TER
		Bool HangUp(); //ATH
		Bool ResetModem(); //ATZ
		Bool SetEcho(Bool showEcho); //ATE
	};
};
#endif
