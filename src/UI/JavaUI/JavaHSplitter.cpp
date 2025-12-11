#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JavaHSplitter.h"

UI::JavaUI::JavaHSplitter::JavaHSplitter(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Int32 width, Bool isRight) : UI::GUIHSplitter(ui, parent)
{
	this->dragMode = false;
	this->isRight = isRight;
}

UI::JavaUI::JavaHSplitter::~JavaHSplitter()
{
}

OSInt UI::JavaUI::JavaHSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JavaHSplitter::EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
}

void UI::JavaUI::JavaHSplitter::EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
}
