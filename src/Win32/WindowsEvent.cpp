#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Win32/WindowsEvent.h"
#include <windows.h>

Win32::WindowsEvent::WindowsEvent(const WChar *progName)
{
    this->hand = RegisterEventSourceW(0, progName);
	this->progName = Text::StrCopyNew(progName);
}

Win32::WindowsEvent::~WindowsEvent()
{
	if (this->hand)
	{
        DeregisterEventSource((HANDLE)this->hand);
	}
	SDEL_TEXT(this->progName);
}

Bool Win32::WindowsEvent::WriteEvent(UInt32 eventCode, EventType evtType, const WChar *evtMessage, const UInt8 *buff, OSInt buffSize)
{
	if (this->hand == 0)
		return false;

    LPCWSTR lpszStrings[2];
	WORD nStr;
	WORD wType;
	switch (evtType)
	{
	default:
	case ET_SUCCESS:
		wType = EVENTLOG_SUCCESS;
		break;
	case ET_AUDIT_FAILURE:
		wType = EVENTLOG_AUDIT_FAILURE;
		break;
	case ET_AUDIT_SUCCESS:
		wType = EVENTLOG_AUDIT_SUCCESS;
		break;
	case ET_ERROR:
		wType = EVENTLOG_ERROR_TYPE;
		break;
	case ET_INFORMATION:
		wType = EVENTLOG_INFORMATION_TYPE;
		break;
	case ET_WARNING:
		wType = EVENTLOG_WARNING_TYPE;
		break;
	}

	if (evtMessage)
	{
		nStr = 2;
		lpszStrings[0] = this->progName;
		lpszStrings[1] = evtMessage;
	}
	else
	{
		nStr = 0;
	}

	return ReportEventW((HANDLE)this->hand, wType, 0, eventCode, 0, nStr, (DWORD)buffSize, lpszStrings, (void*)buff) != 0;
}
