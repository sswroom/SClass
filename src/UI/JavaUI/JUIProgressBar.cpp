#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "Text/CSSBuilder.h"
#include "UI/JavaUI/JUIProgressBar.h"

UI::JavaUI::JUIProgressBar::JUIProgressBar(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, UInt64 totalCnt) : UI::GUIProgressBar(ui, parent)
{
	this->totalCnt = totalCnt;
}

UI::JavaUI::JUIProgressBar::~JUIProgressBar()
{
}

OSInt UI::JavaUI::JUIProgressBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JUIProgressBar::ProgressStart(Text::CStringNN name, UInt64 count)
{
	this->totalCnt = count;
}

void UI::JavaUI::JUIProgressBar::ProgressUpdate(UInt64 currCount, UInt64 newTotalCount)
{
	this->totalCnt = newTotalCount;
}

void UI::JavaUI::JUIProgressBar::ProgressEnd()
{
}

