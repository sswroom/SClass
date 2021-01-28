#include "Stdafx.h"
#include "UI/DObj/ClickAreaDObj.h"

UI::DObj::ClickAreaDObj::ClickAreaDObj(Media::DrawEngine *deng, OSInt left, OSInt top, OSInt width, OSInt height, UI::UIEvent clkHdlr, void *clkUserObj) : DirectObject(left, top)
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
	OSInt left;
	OSInt top;
	this->GetCurrPos(&left, &top);
	if (x >= left && x < left + this->width && y >= top && y < top + this->height)
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
