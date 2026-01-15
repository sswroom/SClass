
// Require Wtsapi32.lib

#ifndef _SM_WIN32_TERMINALSERVICE
#define _SM_WIN32_TERMINALSERVICE
#include "Data/ArrayListInt.h"

namespace Win32
{
	class TerminalService
	{
	private:
		void *hand;

	public:

		typedef enum
		{
			STATE_UNKNOWN = -1,
			STATE_ACTIVE,
			STATE_CONNECTED,
			STATE_CONNECTQUERY,
			STATE_SHADOW,
			STATE_DISCONNECTED,
			STATE_IDLE,
			STATE_LISTEN,
			STATE_RESET,
			STATE_DOWN,
			STATE_INIT 
		} SessionState;

		TerminalService(const WChar *svrName);
		~TerminalService();

		Bool IsError();
		IntOS GetSessions(Data::ArrayListInt *sessions);
		WChar *GetSessionUser(WChar *buff, Int32 sessId);
		WChar *GetSessionClientName(WChar *buff, Int32 sessId);
		UInt32 GetSessionClientIP(Int32 sessId);
		SessionState GetSessionState(Int32 sessId);
	};
};
#endif
