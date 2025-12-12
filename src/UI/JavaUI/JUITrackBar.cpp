#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "Math/Math_C.h"
#include "UI/JavaUI/JUITrackBar.h"

UI::JavaUI::JUITrackBar::JUITrackBar(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal) : UI::GUITrackBar(ui, parent)
{
}

UI::JavaUI::JUITrackBar::~JUITrackBar()
{
}

OSInt UI::JavaUI::JUITrackBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JUITrackBar::SetPos(UOSInt pos)
{
}

void UI::JavaUI::JUITrackBar::SetRange(UOSInt minVal, UOSInt maxVal)
{
}

UOSInt UI::JavaUI::JUITrackBar::GetPos()
{
	return 0;
}
