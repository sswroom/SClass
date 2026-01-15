#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "UI/Win/WinTimer.h"

#include <windows.h>

UI::Win::WinTimer::WinTimer(NN<UI::GUIForm> parent, UIntOS id, UInt32 interval, UI::UIEvent handler, AnyType userObj) : UI::GUITimer(handler, userObj)
{
	this->parent = parent;
	this->id = id;
	this->interval = interval;

	SetTimer((HWND)parent->GetHandle().OrNull(), this->id, interval, 0);
}

UI::Win::WinTimer::~WinTimer()
{
	KillTimer((HWND)this->parent->GetHandle().OrNull(), this->id);
}

UIntOS UI::Win::WinTimer::GetId()
{
	return this->id;
}
