#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JavaVSplitter.h"

UI::JavaUI::JavaVSplitter::JavaVSplitter(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Int32 height, Bool isBottom) : UI::GUIVSplitter(ui, parent)
{
	this->dragMode = false;
	this->isBottom = isBottom;
}

UI::JavaUI::JavaVSplitter::~JavaVSplitter()
{
}

OSInt UI::JavaUI::JavaVSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JavaVSplitter::EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
}

void UI::JavaUI::JavaVSplitter::EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
}
