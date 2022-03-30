#include "Stdafx.h"
#include "MyMemory.h"
#include "Win32/TerminalService.h"
#include "Text/MyString.h"

#include <windows.h>
#include <Wtsapi32.h>

Win32::TerminalService::TerminalService(const WChar *svrName)
{
	this->hand = WTSOpenServerW((LPWSTR)svrName);
}

Win32::TerminalService::~TerminalService()
{
	if (!IsError())
	{
		WTSCloseServer(this->hand);
		this->hand = 0;
	}
}

Bool Win32::TerminalService::IsError()
{
	return this->hand == INVALID_HANDLE_VALUE || this->hand == 0;
}

OSInt Win32::TerminalService::GetSessions(Data::ArrayListInt *sessions)
{
	PWTS_SESSION_INFO sess;
	UInt32 cnt;
	if (WTSEnumerateSessions(this->hand, 0, 1, &sess, (DWORD*)&cnt) == 0)
		return 0;
	OSInt i = 0;
	while ((UInt32)i < cnt)
	{
		sessions->Add(sess[i].SessionId);
		i++;
	}
	WTSFreeMemory(sess);
	return cnt;
}

WChar *Win32::TerminalService::GetSessionUser(WChar *buff, Int32 sessId)
{
	WChar *name;
	WChar *wptr;
	UInt32 byteCnt;
	if (WTSQuerySessionInformationW(this->hand, sessId, WTSUserName, &name, (DWORD*)&byteCnt) == 0)
		return 0;
	wptr = Text::StrConcat(buff, name);
	WTSFreeMemory(name);
	return wptr;
}

WChar *Win32::TerminalService::GetSessionClientName(WChar *buff, Int32 sessId)
{
	WChar *name;
	WChar *wptr;
	UInt32 byteCnt;
	if (WTSQuerySessionInformationW(this->hand, sessId, WTSClientName, &name, (DWORD*)&byteCnt) == 0)
		return 0;
	wptr = Text::StrConcat(buff, name);
	WTSFreeMemory(name);
	return wptr;
}

UInt32 Win32::TerminalService::GetSessionClientIP(Int32 sessId)
{
	WTS_CLIENT_ADDRESS *addr;
	UInt32 byteCnt;
	if (WTSQuerySessionInformationW(this->hand, sessId, WTSClientName, (LPWSTR*)&addr, (DWORD*)&byteCnt) == 0)
		return 0;
	if (addr->AddressFamily == AF_INET)
	{
		byteCnt = *(UInt32*)&addr->Address[0];
	}
	else
	{
		byteCnt = 0;
	}

	WTSFreeMemory(addr);
	return byteCnt;
}

Win32::TerminalService::SessionState Win32::TerminalService::GetSessionState(Int32 sessId)
{
	SessionState stat;
	Int16 *state;
	UInt32 byteCnt;
	if (WTSQuerySessionInformationW(this->hand, sessId, WTSClientName, (LPWSTR*)&state, (DWORD*)&byteCnt) == 0)
		return Win32::TerminalService::STATE_UNKNOWN;
	stat = (Win32::TerminalService::SessionState)*state;
	WTSFreeMemory(state);
	return stat;
}
