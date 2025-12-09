#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "Text/CSSBuilder.h"
#include "UI/Java/JavaProgressBar.h"

UI::Java::JavaProgressBar::JavaProgressBar(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, UInt64 totalCnt) : UI::GUIProgressBar(ui, parent)
{
	this->totalCnt = totalCnt;
}

UI::Java::JavaProgressBar::~JavaProgressBar()
{
}

OSInt UI::Java::JavaProgressBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Java::JavaProgressBar::ProgressStart(Text::CStringNN name, UInt64 count)
{
	this->totalCnt = count;
}

void UI::Java::JavaProgressBar::ProgressUpdate(UInt64 currCount, UInt64 newTotalCount)
{
	this->totalCnt = newTotalCount;
}

void UI::Java::JavaProgressBar::ProgressEnd()
{
}

