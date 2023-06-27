#include "Stdafx.h"
#include "UI/DObj/ClickAreaDObj.h"

UI::DObj::ClickAreaDObj::ClickAreaDObj(Media::DrawEngine *deng, Math::Coord2D<OSInt> tl, OSInt width, OSInt height, UI::UIEvent clkHdlr, void *clkUserObj) : DirectObject(tl)
{
	this->deng = deng;
	this->width = width;
	this->height = height;
	this->clkHdlr = clkHdlr;
	this->clkUserObj = clkUserObj;
}

UI::DObj::ClickAreaDObj::~ClickAreaDObj()
{
}

Bool UI::DObj::ClickAreaDObj::IsChanged()
{
	return false;
}

Bool UI::DObj::ClickAreaDObj::DoEvents()
{
	return false;
}

void UI::DObj::ClickAreaDObj::DrawObject(Media::DrawImage *dimg)
{
}

Bool UI::DObj::ClickAreaDObj::IsObject(OSInt x, OSInt y)
{
	Math::Coord2D<OSInt> tl = this->GetCurrPos();
	if (x >= tl.x && x < tl.x + this->width && y >= tl.y && y < tl.y + this->height)
		return true;
	return false;
}

/*
virtual System::Windows::Forms::Cursor ^GetCursor()
{
}
*/

void UI::DObj::ClickAreaDObj::OnMouseDown()
{
}

void UI::DObj::ClickAreaDObj::OnMouseUp()
{
}

void UI::DObj::ClickAreaDObj::OnMouseClick()
{
	this->clkHdlr(this->clkUserObj);
}
