#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/Java/JavaHSplitter.h"

UI::Java::JavaHSplitter::JavaHSplitter(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Int32 width, Bool isRight) : UI::GUIHSplitter(ui, parent)
{
	this->dragMode = false;
	this->isRight = isRight;
}

UI::Java::JavaHSplitter::~JavaHSplitter()
{
}

OSInt UI::Java::JavaHSplitter::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Java::JavaHSplitter::EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
}

void UI::Java::JavaHSplitter::EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos)
{
}
