#include "Stdafx.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "SSWR/AVIRead/AVIRFontSelector.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRFontSelector::OnResized(void *userObj)
{
	SSWR::AVIRead::AVIRFontSelector *me = (SSWR::AVIRead::AVIRFontSelector*)userObj;
	me->UpdateFontStyles();
}

void SSWR::AVIRead::AVIRFontSelector::OnDraw(Media::DrawImage *img)
{
	UOSInt defVal = this->env->GetDefFontStyle();
	UOSInt currPos = (UOSInt)this->GetVScrollPos();
	UOSInt i = 0;
	UOSInt j = this->env->GetFontStyleCount();
	UTF8Char sbuff[256];

	UOSInt w;
	UOSInt h;
	Double hdpi = this->GetHDPI();
	Double ddpi = this->GetDDPI();
	UInt32 itemH = (UInt32)Math::Double2Int32(48 * hdpi / ddpi);
	UInt32 itemTH = (UInt32)Math::Double2Int32(80 * hdpi / ddpi);
	w = img->GetWidth();
	h = img->GetHeight();
	Media::DrawEngine *deng = this->core->GetDrawEngine();
	Media::DrawImage *tmpBmp;
	if (Math::UOSInt2Double(w) >= (138 * hdpi / ddpi))
	{
		tmpBmp = deng->CreateImage32((UInt32)Math::Double2Int32(128 * hdpi / ddpi), itemH, Media::AT_NO_ALPHA);
	}
	else if (w >= 10)
	{
		tmpBmp = deng->CreateImage32(w - 10, itemH, Media::AT_NO_ALPHA);
	}
	else
	{
		tmpBmp = deng->CreateImage32(w, itemH, Media::AT_NO_ALPHA);
	}
	tmpBmp->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
	tmpBmp->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);

	Media::DrawBrush *bWhite = img->NewBrushARGB(this->colorConv->ConvRGB8(0xffffffff));
	Media::DrawBrush *bBlack = img->NewBrushARGB(this->colorConv->ConvRGB8(0xff000000));
	while (currPos < j && i < h)
	{
		if (currPos == defVal)
		{
			Media::DrawBrush *bDef = img->NewBrushARGB(this->colorConv->ConvRGB8(0xffffffc0));
			img->DrawRect(0, Math::UOSInt2Double(i), Math::UOSInt2Double(w), itemTH, 0, bDef);
			img->DelBrush(bDef);
		}
		else
		{
			img->DrawRect(0, Math::UOSInt2Double(i), Math::UOSInt2Double(w), itemTH, 0, bWhite);
		}
		this->core->GenFontStylePreview(tmpBmp, deng, this->env, currPos, this->colorConv);
		if (currPos == this->currFontStyle)
		{
			Media::DrawBrush *bRed = img->NewBrushARGB(this->colorConv->ConvRGB8(0xffff0000));
			img->DrawRect(0, Math::UOSInt2Double(i), Math::UOSInt2Double(w), itemTH, 0, bRed);
			img->DelBrush(bRed);
		}
		img->DrawImagePt(tmpBmp, Math::UOSInt2Double((w - tmpBmp->GetWidth()) >> 1), Math::UOSInt2Double(i + 1));
		sbuff[0] = 0;
		UTF8Char *sptr = this->env->GetFontStyleName(currPos, sbuff);
		if (sbuff[0] == 0)
		{
			sptr = Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Style "), currPos);
		}
		if (sbuff[0])
		{
			Double sz[2];
			Media::DrawFont *fnt = this->CreateDrawFont(img);
			img->GetTextSizeC(fnt, sbuff, (UOSInt)(sptr - sbuff), sz);
			img->DrawString((Math::UOSInt2Double(w) - sz[0]) * 0.5, Math::UOSInt2Double(i + itemH + 2), sbuff, fnt, bBlack);
			img->DelFont(fnt);
		}

		i += itemTH;
		currPos++;
	}
	if (i < h)
	{
		img->DrawRect(0, Math::UOSInt2Double(i), Math::UOSInt2Double(w), Math::UOSInt2Double(h - i), 0, bWhite);
	}
	img->DelBrush(bWhite);
	img->DelBrush(bBlack);
	deng->DeleteImage(tmpBmp);
}

void SSWR::AVIRead::AVIRFontSelector::OnMouseDown(OSInt scrollY, Int32 xPos, Int32 yPos, UI::GUIClientControl::MouseButton btn, KeyButton keys)
{
	OSInt i = scrollY + (yPos / Math::Double2Int32(80 * this->GetHDPI() / this->GetDDPI()));
	if (i >= (OSInt)this->env->GetFontStyleCount())
	{
		i = -1;
	}
	else
	{
		if (i != (OSInt)this->currFontStyle)
		{
			this->currFontStyle = (UOSInt)i;
			this->EventSelChg();
			this->Redraw();
		}
		if (btn == UI::GUIClientControl::MBTN_RIGHT && this->mnuLayers)
		{
			OSInt x;
			OSInt y;
			this->GetScreenPosP(&x, &y);
			this->mnuLayers->ShowMenu(this, xPos + x, yPos + y);
		}
	}
}

void SSWR::AVIRead::AVIRFontSelector::OnKeyDown(UInt32 keyCode)
{
	switch (UI::GUIControl::OSKey2GUIKey(keyCode))
	{
	case UI::GUIControl::GK_PAGEUP:
		{
			Double w;
			Double h;
			this->GetSize(&w, &h);
			h = h / 80;
			if (h <= 0)
			{
				h = 1;
			}
			this->currFontStyle -= (UOSInt)h;
			if (this->currFontStyle == INVALID_INDEX)
			{
				this->currFontStyle = 0;
			}
			if (!this->MakeVisible(this->currFontStyle))
			{
				this->Redraw();
			}
		}
		break;
	case UI::GUIControl::GK_PAGEDOWN:
		{
			Double w;
			Double h;
			this->GetSize(&w, &h);
			h = h / 80;
			if (h <= 0)
			{
				h = 1;
			}
			this->currFontStyle += (UOSInt)h;
			if (this->currFontStyle >= this->env->GetFontStyleCount())
			{
				this->currFontStyle = this->env->GetFontStyleCount() - 1;
			}
			if (!this->MakeVisible(this->currFontStyle))
			{
				this->Redraw();
			}
		}
		break;
	case UI::GUIControl::GK_HOME:
		this->currFontStyle = 0;
		if (!this->MakeVisible(0))
		{
			this->Redraw();
		}
		break;
	case UI::GUIControl::GK_END:
		this->currFontStyle = this->env->GetFontStyleCount() - 1;
		if (!this->MakeVisible(this->currFontStyle))
		{
			this->Redraw();
		}
		break;
	case UI::GUIControl::GK_UP:
		if (this->currFontStyle > 0)
		{
			this->currFontStyle -= 1;
			if (!this->MakeVisible(this->currFontStyle))
			{
				this->Redraw();
			}
		}
		break;
	case UI::GUIControl::GK_DOWN:
		if (this->currFontStyle < this->env->GetFontStyleCount() - 1)
		{
			this->currFontStyle += 1;
			if (!this->MakeVisible(this->currFontStyle))
			{
				this->Redraw();
			}
		}
		break;
	default:
		break;
	}
}

SSWR::AVIRead::AVIRFontSelector::AVIRFontSelector(UI::GUICore *ui, UI::GUIClientControl *parent, SSWR::AVIRead::AVIRCore *core, Map::MapEnv *env, UOSInt initFontStyle, Media::ColorManagerSess *colorSess) : UI::GUICustomDrawVScroll(ui, parent, core->GetDrawEngine())
{
	this->core = core;
	this->env = env;
	this->colorSess = colorSess;
	this->currFontStyle = initFontStyle;
	this->mnuLayers = 0;
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->colorConv, Media::ColorConv(&srcProfile, &destProfile, this->colorSess));
	if (this->colorSess)
	{
		this->colorSess->AddHandler(this);
	}

	this->UpdateFontStyles();
	this->MakeVisible(this->currFontStyle);
	this->HandleSizeChanged(OnResized, this);
}

SSWR::AVIRead::AVIRFontSelector::~AVIRFontSelector()
{
	DEL_CLASS(this->colorConv);
	if (this->colorSess)
	{
		this->colorSess->RemoveHandler(this);
	}
}

void SSWR::AVIRead::AVIRFontSelector::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
}

void SSWR::AVIRead::AVIRFontSelector::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	this->colorConv->RGBParamChanged(rgbParam);
	this->Redraw();
}

void SSWR::AVIRead::AVIRFontSelector::SetPopupMenu(UI::GUIPopupMenu *mnuLayers)
{
	this->mnuLayers = mnuLayers;
}

void SSWR::AVIRead::AVIRFontSelector::UpdateFontStyles()
{
	Double w;
	Double h;

	this->GetSize(&w, &h);
	UOSInt pageCnt = (UOSInt)Math::Double2Int32(h / 80);
	if (pageCnt <= 0)
		pageCnt = 1;
	this->SetVScrollBar(0, (Int32)this->env->GetFontStyleCount(), pageCnt);
}

UOSInt SSWR::AVIRead::AVIRFontSelector::GetSelectedFontStyle()
{
	return this->currFontStyle;
}
