#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "Math/Math_C.h"
#include "UI/JavaUI/JUITrackBar.h"

UI::JavaUI::JUITrackBar::JUITrackBar(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, UIntOS minVal, UIntOS maxVal, UIntOS currVal) : UI::GUITrackBar(ui, parent)
{
}

UI::JavaUI::JUITrackBar::~JUITrackBar()
{
}

IntOS UI::JavaUI::JUITrackBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JUITrackBar::SetPos(UIntOS pos)
{
}

void UI::JavaUI::JUITrackBar::SetRange(UIntOS minVal, UIntOS maxVal)
{
}

UIntOS UI::JavaUI::JUITrackBar::GetPos()
{
	return 0;
}
