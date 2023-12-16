#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUITimer.h"

UI::GUITimer::GUITimer(UI::UIEvent handler, void *userObj)
{
	this->handler = handler;
	this->userObj = userObj;
}

UI::GUITimer::~GUITimer()
{
}

void UI::GUITimer::EventTick()
{
	if (this->handler)
		this->handler(this->userObj);
}
