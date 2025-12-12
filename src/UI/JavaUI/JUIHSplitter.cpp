#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JUIHSplitter.h"

UI::JavaUI::JUIHSplitter::JUIHSplitter(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Int32 width, Bool isRight) : UI::GUIHSplitter(ui, parent)
{
	this->dragMode = false;
	this->isRight = isRight;
}

UI::JavaUI::JUIHSplitter::~JUIHSplitter()
{
}

OSInt UI::JavaUI::JUIHSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JUIHSplitter::EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
}

void UI::JavaUI::JUIHSplitter::EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
}
