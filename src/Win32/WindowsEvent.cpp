#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Win32/WindowsEvent.h"
#include <windows.h>

Win32::WindowsEvent::WindowsEvent(UnsafeArray<const WChar> progName)
{
    this->hand = RegisterEventSourceW(0, progName.Ptr());
	this->progName = Text::StrCopyNew(progName);
}

Win32::WindowsEvent::~WindowsEvent()
{
	if (this->hand)
	{
        DeregisterEventSource((HANDLE)this->hand);
	}
	Text::StrDelNew(this->progName);
}

Bool Win32::WindowsEvent::WriteEvent(UInt32 eventCode, EventType evtType, const WChar *evtMessage, const UInt8 *buff, UOSInt buffSize)
{
	if (this->hand == 0)
		return false;

    LPCWSTR lpszStrings[2];
	WORD nStr;
	WORD wType;
	switch (evtType)
	{
	default:
	case EventType::Success:
		wType = EVENTLOG_SUCCESS;
		break;
	case EventType::AuditFailure:
		wType = EVENTLOG_AUDIT_FAILURE;
		break;
	case EventType::AuditSuccess:
		wType = EVENTLOG_AUDIT_SUCCESS;
		break;
	case EventType::Error:
		wType = EVENTLOG_ERROR_TYPE;
		break;
	case EventType::Information:
		wType = EVENTLOG_INFORMATION_TYPE;
		break;
	case EventType::Warning:
		wType = EVENTLOG_WARNING_TYPE;
		break;
	}

	if (evtMessage)
	{
		nStr = 2;
		lpszStrings[0] = this->progName.Ptr();
		lpszStrings[1] = evtMessage;
	}
	else
	{
		nStr = 0;
	}

	return ReportEventW((HANDLE)this->hand, wType, 0, eventCode, 0, nStr, (DWORD)buffSize, lpszStrings, (void*)buff) != 0;
}
