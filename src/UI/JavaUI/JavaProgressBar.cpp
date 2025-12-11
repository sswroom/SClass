#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "Text/CSSBuilder.h"
#include "UI/JavaUI/JavaProgressBar.h"

UI::JavaUI::JavaProgressBar::JavaProgressBar(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, UInt64 totalCnt) : UI::GUIProgressBar(ui, parent)
{
	this->totalCnt = totalCnt;
}

UI::JavaUI::JavaProgressBar::~JavaProgressBar()
{
}

OSInt UI::JavaUI::JavaProgressBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JavaProgressBar::ProgressStart(Text::CStringNN name, UInt64 count)
{
	this->totalCnt = count;
}

void UI::JavaUI::JavaProgressBar::ProgressUpdate(UInt64 currCount, UInt64 newTotalCount)
{
	this->totalCnt = newTotalCount;
}

void UI::JavaUI::JavaProgressBar::ProgressEnd()
{
}

