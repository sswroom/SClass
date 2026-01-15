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

void UI::JavaUI::JUIHScrollBar::InitScrollBar(UIntOS minVal, UIntOS maxVal, UIntOS currVal, UIntOS largeChg)
{
}

void UI::JavaUI::JUIHScrollBar::SetPos(UIntOS pos)
{
}

UIntOS UI::JavaUI::JUIHScrollBar::GetPos()
{
	return 0;
}

void UI::JavaUI::JUIHScrollBar::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
}

void UI::JavaUI::JUIHScrollBar::SetAreaP(IntOS left, IntOS top, IntOS right, IntOS bottom, Bool updateScn)
{
}

IntOS UI::JavaUI::JUIHScrollBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JUIHScrollBar::UpdatePos(Bool redraw)
{
}
