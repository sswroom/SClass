#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUITimer.h"
#include <gtk/gtk.h>

Int32 GUITimer_OnTick(void *userObj)
{
	UI::GUITimer *me = (UI::GUITimer*)userObj;
	me->OnTick();
	return 1;
}

UI::GUITimer::GUITimer(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIForm> parent, UOSInt id, UInt32 interval, UI::UIEvent handler, void *userObj)
{
	this->interval = interval;
	this->handler = handler;
	this->userObj = userObj;
	this->id = g_timeout_add(interval, GUITimer_OnTick, this);
}

UI::GUITimer::~GUITimer()
{
	g_source_remove((guint)this->id);
}

void UI::GUITimer::OnTick()
{
	if (this->handler)
		this->handler(this->userObj);
}

UOSInt UI::GUITimer::GetId()
{
	return this->id;
}

