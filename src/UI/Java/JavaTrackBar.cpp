#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Math/Math.h"
#include "UI/Java/JavaTrackBar.h"

UI::Java::JavaTrackBar::JavaTrackBar(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal) : UI::GUITrackBar(ui, parent)
{
}

UI::Java::JavaTrackBar::~JavaTrackBar()
{
}

OSInt UI::Java::JavaTrackBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Java::JavaTrackBar::SetPos(UOSInt pos)
{
}

void UI::Java::JavaTrackBar::SetRange(UOSInt minVal, UOSInt maxVal)
{
}

UOSInt UI::Java::JavaTrackBar::GetPos()
{
	return 0;
}
