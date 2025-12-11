#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JavaHScrollBar.h"

UI::JavaUI::JavaHScrollBar::JavaHScrollBar(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Double width) : UI::GUIHScrollBar(ui, parent)
{
}

UI::JavaUI::JavaHScrollBar::~JavaHScrollBar()
{
}

void UI::JavaUI::JavaHScrollBar::InitScrollBar(UOSInt minVal, UOSInt maxVal, UOSInt currVal, UOSInt largeChg)
{
}

void UI::JavaUI::JavaHScrollBar::SetPos(UOSInt pos)
{
}

UOSInt UI::JavaUI::JavaHScrollBar::GetPos()
{
	return 0;
}

void UI::JavaUI::JavaHScrollBar::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
}

void UI::JavaUI::JavaHScrollBar::SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn)
{
}

OSInt UI::JavaUI::JavaHScrollBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JavaHScrollBar::UpdatePos(Bool redraw)
{
}
