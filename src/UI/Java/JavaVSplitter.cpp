#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/Java/JavaVSplitter.h"

UI::Java::JavaVSplitter::JavaVSplitter(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Int32 height, Bool isBottom) : UI::GUIVSplitter(ui, parent)
{
	this->dragMode = false;
	this->isBottom = isBottom;
}

UI::Java::JavaVSplitter::~JavaVSplitter()
{
}

OSInt UI::Java::JavaVSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Java::JavaVSplitter::EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
}

void UI::Java::JavaVSplitter::EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
}
