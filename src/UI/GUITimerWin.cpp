#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "UI/IWindowSystem.h"
#include "UI/GUITimer.h"

#include "Text/MyString.h"
#include "UI/MessageDialog.h"

#include <windows.h>

UI::GUITimer::GUITimer(UI::GUICore *ui, UI::GUIForm *parent, UOSInt id, UInt32 interval, UI::UIEvent handler, void *userObj)
{
	this->parent = parent;
	this->id = id;
	this->interval = interval;
	this->handler = handler;
	this->userObj = userObj;

	SetTimer((HWND)parent->GetHandle(), this->id, interval, 0);
}

UI::GUITimer::~GUITimer()
{
	KillTimer((HWND)parent->GetHandle(), this->id);
}

void UI::GUITimer::OnTick()
{
	this->handler(this->userObj);
}

UOSInt UI::GUITimer::GetId()
{
	return this->id;
}
