#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JUIHScrollBar.h"

UI::JavaUI::JUIHScrollBar::JUIHScrollBar(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Double width) : UI::GUIHScrollBar(ui, parent)
{
}

UI::JavaUI::JUIHScrollBar::~JUIHScrollBar()
{
}

void UI::JavaUI::JUIHScrollBar::InitScrollBar(UOSInt minVal, UOSInt maxVal, UOSInt currVal, UOSInt largeChg)
{
}

void UI::JavaUI::JUIHScrollBar::SetPos(UOSInt pos)
{
}

UOSInt UI::JavaUI::JUIHScrollBar::GetPos()
{
	return 0;
}

void UI::JavaUI::JUIHScrollBar::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
}

void UI::JavaUI::JUIHScrollBar::SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn)
{
}

OSInt UI::JavaUI::JUIHScrollBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JUIHScrollBar::UpdatePos(Bool redraw)
{
}
