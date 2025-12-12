#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JUIVSplitter.h"

UI::JavaUI::JUIVSplitter::JUIVSplitter(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Int32 height, Bool isBottom) : UI::GUIVSplitter(ui, parent)
{
	this->dragMode = false;
	this->isBottom = isBottom;
}

UI::JavaUI::JUIVSplitter::~JUIVSplitter()
{
}

OSInt UI::JavaUI::JUIVSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JUIVSplitter::EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
}

void UI::JavaUI::JUIVSplitter::EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
}
