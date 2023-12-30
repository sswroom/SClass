#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/Java/JavaHScrollBar.h"

UI::Java::JavaHScrollBar::JavaHScrollBar(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Double width) : UI::GUIHScrollBar(ui, parent)
{
}

UI::Java::JavaHScrollBar::~JavaHScrollBar()
{
}

void UI::Java::JavaHScrollBar::InitScrollBar(UOSInt minVal, UOSInt maxVal, UOSInt currVal, UOSInt largeChg)
{
}

void UI::Java::JavaHScrollBar::SetPos(UOSInt pos)
{
}

UOSInt UI::Java::JavaHScrollBar::GetPos()
{
	return 0;
}

void UI::Java::JavaHScrollBar::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
}

void UI::Java::JavaHScrollBar::SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn)
{
}

OSInt UI::Java::JavaHScrollBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Java::JavaHScrollBar::UpdatePos(Bool redraw)
{
}
