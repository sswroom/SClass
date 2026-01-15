#include "Stdafx.h"
#include "UI/DObj/ClickAreaDObj.h"

UI::DObj::ClickAreaDObj::ClickAreaDObj(NN<Media::DrawEngine> deng, Math::Coord2D<IntOS> tl, IntOS width, IntOS height, UI::UIEvent clkHdlr, AnyType clkUserObj) : DirectObject(tl)
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

void UI::DObj::ClickAreaDObj::DrawObject(NN<Media::DrawImage> dimg)
{
}

Bool UI::DObj::ClickAreaDObj::IsObject(Math::Coord2D<IntOS> scnPos)
{
	Math::Coord2D<IntOS> tl = this->GetCurrPos();
	if (scnPos.x >= tl.x && scnPos.x < tl.x + this->width && scnPos.y >= tl.y && scnPos.y < tl.y + this->height)
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
