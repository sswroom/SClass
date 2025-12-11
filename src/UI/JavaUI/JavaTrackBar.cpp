#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "Math/Math_C.h"
#include "UI/JavaUI/JavaTrackBar.h"

UI::JavaUI::JavaTrackBar::JavaTrackBar(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal) : UI::GUITrackBar(ui, parent)
{
}

UI::JavaUI::JavaTrackBar::~JavaTrackBar()
{
}

OSInt UI::JavaUI::JavaTrackBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JavaTrackBar::SetPos(UOSInt pos)
{
}

void UI::JavaUI::JavaTrackBar::SetRange(UOSInt minVal, UOSInt maxVal)
{
}

UOSInt UI::JavaUI::JavaTrackBar::GetPos()
{
	return 0;
}
