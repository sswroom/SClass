#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "UI/Win/WinTimer.h"

#include <windows.h>

UI::Win::WinTimer::WinTimer(NotNullPtr<UI::GUIForm> parent, UOSInt id, UInt32 interval, UI::UIEvent handler, void *userObj) : UI::GUITimer(handler, userObj)
{
	this->parent = parent;
	this->id = id;
	this->interval = interval;

	SetTimer((HWND)parent->GetHandle(), this->id, interval, 0);
}

UI::Win::WinTimer::~WinTimer()
{
	KillTimer((HWND)this->parent->GetHandle(), this->id);
}

UOSInt UI::Win::WinTimer::GetId()
{
	return this->id;
}