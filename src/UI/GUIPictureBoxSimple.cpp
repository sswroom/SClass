#include "Stdafx.h"
#include "UI/GUIPictureBoxSimple.h"

UI::GUIPictureBoxSimple::GUIPictureBoxSimple(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, Bool hasBorder) : UI::GUIControl(ui, parent)
{
	this->hasBorder = hasBorder;
	this->eng = eng;
	this->noBGColor = false;
}

UI::GUIPictureBoxSimple::~GUIPictureBoxSimple()
{
}

Text::CStringNN UI::GUIPictureBoxSimple::GetObjectClass() const
{
	return CSTR("PictureBoxSimple");
}

void UI::GUIPictureBoxSimple::SetNoBGColor(Bool noBGColor)
{
	this->noBGColor = noBGColor;
}

void UI::GUIPictureBoxSimple::HandleMouseDown(MouseEventHandler hdlr, void *userObj)
{
	this->mouseDownHdlrs.Add(hdlr);
	this->mouseDownObjs.Add(userObj);
}

void UI::GUIPictureBoxSimple::HandleMouseMove(MouseEventHandler hdlr, void *userObj)
{
	this->mouseMoveHdlrs.Add(hdlr);
	this->mouseMoveObjs.Add(userObj);
}

void UI::GUIPictureBoxSimple::HandleMouseUp(MouseEventHandler hdlr, void *userObj)
{
	this->mouseUpHdlrs.Add(hdlr);
	this->mouseUpObjs.Add(userObj);
}

void UI::GUIPictureBoxSimple::EventButtonDown(Math::Coord2D<OSInt> pos, UI::GUIControl::MouseButton btn)
{
	UOSInt i;
	i = this->mouseDownHdlrs.GetCount();
	while (i-- > 0)
	{
		this->mouseDownHdlrs.GetItem(i)(this->mouseDownObjs.GetItem(i), pos, btn);
	}
}

void UI::GUIPictureBoxSimple::EventButtonUp(Math::Coord2D<OSInt> pos, UI::GUIControl::MouseButton btn)
{
	UOSInt i;
	i = this->mouseUpHdlrs.GetCount();
	while (i-- > 0)
	{
		this->mouseUpHdlrs.GetItem(i)(this->mouseUpObjs.GetItem(i), pos, btn);
	}
}

void UI::GUIPictureBoxSimple::EventMouseMove(Math::Coord2D<OSInt> pos)
{
	UOSInt i = this->mouseMoveHdlrs.GetCount();
	while (i-- > 0)
	{
		this->mouseMoveHdlrs.GetItem(i)(this->mouseMoveObjs.GetItem(i), pos, MBTN_MIDDLE);
	}
}
