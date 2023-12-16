#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GTK/GTKTimer.h"
#include <gtk/gtk.h>

Int32 UI::GTK::GTKTimer::OnTick(void *userObj)
{
	UI::GTK::GTKTimer *me = (UI::GTK::GTKTimer*)userObj;
	me->EventTick();
	return 1;
}

UI::GTK::GTKTimer::GTKTimer(UInt32 interval, UI::UIEvent handler, void *userObj) : UI::GUITimer(handler, userObj)
{
	this->interval = interval;
	this->id = g_timeout_add(interval, OnTick, this);
}

UI::GTK::GTKTimer::~GTKTimer()
{
	g_source_remove((guint)this->id);
}

UOSInt UI::GTK::GTKTimer::GetId()
{
	return this->id;
}

