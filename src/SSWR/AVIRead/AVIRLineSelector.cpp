#include "Stdafx.h"
#include "Math/Math_C.h"
#include "SSWR/AVIRead/AVIRLineSelector.h"

void __stdcall SSWR::AVIRead::AVIRLineSelector::OnResized(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLineSelector> me = userObj.GetNN<SSWR::AVIRead::AVIRLineSelector>();
	me->UpdateLineStyles();
}

void SSWR::AVIRead::AVIRLineSelector::OnDraw(NN<Media::DrawImage> img)
{
	UIntOS defVal = this->env->GetDefLineStyle();
	UIntOS currPos = this->GetVScrollPos();
	UIntOS i = 0;
	UIntOS j = this->env->GetLineStyleCount();
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;

	UIntOS w = img->GetWidth();;
	UIntOS h = img->GetHeight();
	Double hdpi = this->GetHDPI();
	Double ddpi = this->GetDDPI();
	UInt32 itemH = (UInt32)Double2Int32(22 * hdpi / ddpi);
	UInt32 itemTH = (UInt32)Double2Int32(24 * hdpi / ddpi);
	UInt32 itemL = (UInt32)Double2Int32(5 * hdpi / ddpi);
	UInt32 itemW = (UInt32)Double2Int32(120 * hdpi / ddpi);
	NN<Media::DrawEngine> deng = this->core->GetDrawEngine();
	NN<Media::DrawImage> tmpBmp;
	if (w >= itemW)
	{
		if (!deng->CreateImage32(Math::Size2D<UIntOS>((UInt32)Double2Int32(110 * hdpi / ddpi), itemH), Media::AT_ALPHA_ALL_FF).SetTo(tmpBmp))
		{
			return;
		}
	}
	else if (w >= 10)
	{
		if (!deng->CreateImage32(Math::Size2D<UIntOS>(w - 10, itemH), Media::AT_ALPHA_ALL_FF).SetTo(tmpBmp))
		{
			return;
		}
	}
	else
	{
		if (!deng->CreateImage32(Math::Size2D<UIntOS>(w, itemH), Media::AT_ALPHA_ALL_FF).SetTo(tmpBmp))
		{
			return;
		}
	}
	tmpBmp->SetHDPI(hdpi / ddpi * 96.0);
	tmpBmp->SetVDPI(hdpi / ddpi * 96.0);

	NN<Media::DrawBrush> bWhite = img->NewBrushARGB(this->colorConv->ConvRGB8(0xffffffff));
	NN<Media::DrawBrush> bBlack = img->NewBrushARGB(this->colorConv->ConvRGB8(0xff000000));
	while (currPos < j && i < h)
	{
		if (currPos == defVal)
		{
			NN<Media::DrawBrush> bDef = img->NewBrushARGB(this->colorConv->ConvRGB8(0xffffffc0));
			img->DrawRect(Math::Coord2DDbl(0, UIntOS2Double(i)), Math::Size2DDbl(UIntOS2Double(w), UIntOS2Double(itemTH)), nullptr, bDef);
			img->DelBrush(bDef);
		}
		else
		{
			img->DrawRect(Math::Coord2DDbl(0, UIntOS2Double(i)), Math::Size2DDbl(UIntOS2Double(w), itemTH), nullptr, bWhite);
		}
		this->core->GenLineStylePreview(tmpBmp, deng, this->env, currPos, this->colorConv);
		if (currPos == this->currLineStyle)
		{
			NN<Media::DrawBrush> bRed = img->NewBrushARGB(this->colorConv->ConvRGB8(0xffff0000));
			img->DrawRect(Math::Coord2DDbl(0, UIntOS2Double(i)), Math::Size2DDbl(itemW, itemTH), nullptr, bRed);
			img->DelBrush(bRed);
		}
		if (w >= itemW)
		{
			img->DrawImagePt(tmpBmp, Math::Coord2DDbl(itemL, UIntOS2Double(i + 1)));
		}
		else if (w >= 10)
		{
			img->DrawImagePt(tmpBmp, Math::Coord2DDbl(5, UIntOS2Double(i + 1)));
		}
		else
		{
			img->DrawImagePt(tmpBmp, Math::Coord2DDbl(0, UIntOS2Double(i + 1)));
		}
		sbuff[0] = 0;
		if (this->env->GetLineStyleName(currPos, sbuff).SetTo(sptr) && sbuff[0])
		{
			NN<Media::DrawFont> fnt;
			if (this->CreateDrawFont(img).SetTo(fnt))
			{
				img->DrawString(Math::Coord2DDbl(itemW, UIntOS2Double(i + 1)), CSTRP(sbuff, sptr), fnt, bBlack);
				img->DelFont(fnt);
			}
		}

		i += itemTH;
		currPos++;
	}
	if (i < h)
	{
		img->DrawRect(Math::Coord2DDbl(0, UIntOS2Double(i)), Math::Size2DDbl(UIntOS2Double(w), UIntOS2Double(h - i)), nullptr, bWhite);
	}
	img->DelBrush(bWhite);
	img->DelBrush(bBlack);
	deng->DeleteImage(tmpBmp);
}

void SSWR::AVIRead::AVIRLineSelector::OnMouseDown(IntOS scrollY, Math::Coord2D<IntOS> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys)
{
	IntOS i = scrollY + (pos.y / Double2Int32(24 * this->GetHDPI() / this->GetDDPI()));
	if (i >= (IntOS)this->env->GetLineStyleCount())
	{
		i = -1;
	}
	else
	{
		if (i != (IntOS)this->currLineStyle)
		{
			this->currLineStyle = (UIntOS)i;
			this->EventSelChg();
			this->Redraw();
		}
		NN<UI::GUIPopupMenu> mnu;
		if (btn == UI::GUIClientControl::MBTN_RIGHT && this->mnuLayers.SetTo(mnu))
		{
			Math::Coord2D<IntOS> scnPos = this->GetScreenPosP();
			mnu->ShowMenu(*this, scnPos + pos);
		}
	}
}

void SSWR::AVIRead::AVIRLineSelector::OnKeyDown(UInt32 keyCode)
{
}

SSWR::AVIRead::AVIRLineSelector::AVIRLineSelector(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, UIntOS initLineStyle, NN<Media::ColorManagerSess> colorSess) : UI::GUICustomDrawVScroll(ui, parent, core->GetDrawEngine(), colorSess)
{
	this->core = core;
	this->env = env;
	this->colorSess = colorSess;
	this->currLineStyle = initLineStyle;
	this->mnuLayers = nullptr;
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASSNN(this->colorConv, Media::ColorConv(srcProfile, destProfile, this->colorSess.Ptr()));
	this->colorSess->AddHandler(*this);

	this->HandleSizeChanged(OnResized, this);

	this->UpdateLineStyles();
}

SSWR::AVIRead::AVIRLineSelector::~AVIRLineSelector()
{
	this->colorConv.Delete();
	this->colorSess->RemoveHandler(*this);
}

void SSWR::AVIRead::AVIRLineSelector::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void SSWR::AVIRead::AVIRLineSelector::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	this->colorConv->RGBParamChanged(rgbParam);
	this->Redraw();
}

void SSWR::AVIRead::AVIRLineSelector::SetPopupMenu(Optional<UI::GUIPopupMenu> mnuLayers)
{
	this->mnuLayers = mnuLayers;
}

void SSWR::AVIRead::AVIRLineSelector::UpdateLineStyles()
{
	Math::Size2DDbl sz = this->GetSize();
	UIntOS page = (UIntOS)(sz.y / 24);
	if (page <= 0)
		page = 1;
	this->SetVScrollBar(0, this->env->GetLineStyleCount(), page);
}

UIntOS SSWR::AVIRead::AVIRLineSelector::GetSelectedLineStyle()
{
	return this->currLineStyle;
}
