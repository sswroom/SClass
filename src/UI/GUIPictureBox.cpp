#include "Stdafx.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "UI/GUIPictureBox.h"

UI::GUIPictureBox::GUIPictureBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize) : UI::GUIControl(ui, parent)
{
	this->hasBorder = hasBorder;
	this->allowResize = allowResize;
	this->eng = eng;
	this->currImage = nullptr;
	this->noBGColor = false;
	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizerRGB_C8(4, 3, color, color, nullptr, Media::AT_ALPHA_ALL_FF));
	this->resizer->SetResizeAspectRatio(Media::ImageResizer::RAR_SQUAREPIXEL);
	this->resizer->SetTargetSize(Math::Size2D<UOSInt>(200, 200));
}

UI::GUIPictureBox::~GUIPictureBox()
{
	DEL_CLASS(this->resizer);
}

Text::CStringNN UI::GUIPictureBox::GetObjectClass() const
{
	return CSTR("PictureBox");
}

void UI::GUIPictureBox::OnSizeChanged(Bool updateScn)
{
	UI::GUIControl::OnSizeChanged(updateScn);
	Math::Size2D<UOSInt> sz = GetSizeP();
	this->resizer->SetTargetSize(sz);
	if (this->allowResize)
	{
		this->UpdatePreview();
	}
}
void UI::GUIPictureBox::SetImage(Optional<Media::StaticImage> currImage)
{
	this->currImage = currImage;
	this->UpdatePreview();
}

void UI::GUIPictureBox::SetNoBGColor(Bool noBGColor)
{
	this->noBGColor = noBGColor;
}

void UI::GUIPictureBox::HandleMouseDown(MouseEventHandler hdlr, AnyType userObj)
{
	this->mouseDownHdlrs.Add(hdlr);
	this->mouseDownObjs.Add(userObj);
}

void UI::GUIPictureBox::HandleMouseMove(MouseEventHandler hdlr, AnyType userObj)
{
	this->mouseMoveHdlrs.Add(hdlr);
	this->mouseMoveObjs.Add(userObj);
}

void UI::GUIPictureBox::HandleMouseUp(MouseEventHandler hdlr, AnyType userObj)
{
	this->mouseUpHdlrs.Add(hdlr);
	this->mouseUpObjs.Add(userObj);
}

void UI::GUIPictureBox::EventButtonDown(Math::Coord2D<OSInt> pos, UI::GUIControl::MouseButton btn)
{
	UOSInt i;
	i = this->mouseDownHdlrs.GetCount();
	while (i-- > 0)
	{
		this->mouseDownHdlrs.GetItem(i)(this->mouseDownObjs.GetItem(i), pos, btn);
	}
}

void UI::GUIPictureBox::EventButtonUp(Math::Coord2D<OSInt> pos, UI::GUIControl::MouseButton btn)
{
	UOSInt i;
	i = this->mouseUpHdlrs.GetCount();
	while (i-- > 0)
	{
		this->mouseUpHdlrs.GetItem(i)(this->mouseUpObjs.GetItem(i), pos, btn);
	}
}

void UI::GUIPictureBox::EventMouseMove(Math::Coord2D<OSInt> pos)
{
	UOSInt i = this->mouseMoveHdlrs.GetCount();
	while (i-- > 0)
	{
		this->mouseMoveHdlrs.GetItem(i)(this->mouseMoveObjs.GetItem(i), pos, MBTN_MIDDLE);
	}
}
