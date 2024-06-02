#include "Stdafx.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "SSWR/AVIRead/AVIRFontSelector.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRFontSelector::OnResized(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRFontSelector> me = userObj.GetNN<SSWR::AVIRead::AVIRFontSelector>();
	me->UpdateFontStyles();
}

void SSWR::AVIRead::AVIRFontSelector::OnDraw(NN<Media::DrawImage> img)
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
	UInt32 itemH = (UInt32)Double2Int32(48 * hdpi / ddpi);
	UInt32 itemTH = (UInt32)Double2Int32(80 * hdpi / ddpi);
	w = img->GetWidth();
	h = img->GetHeight();
	NN<Media::DrawEngine> deng = this->core->GetDrawEngine();
	NN<Media::DrawImage> tmpBmp;
	if (UOSInt2Double(w) >= (138 * hdpi / ddpi))
	{
		if (!deng->CreateImage32(Math::Size2D<UOSInt>((UInt32)Double2Int32(128 * hdpi / ddpi), itemH), Media::AT_NO_ALPHA).SetTo(tmpBmp))
			return;
	}
	else if (w >= 10)
	{
		if (!deng->CreateImage32(Math::Size2D<UOSInt>(w - 10, itemH), Media::AT_NO_ALPHA).SetTo(tmpBmp))
			return;
	}
	else
	{
		if (!deng->CreateImage32(Math::Size2D<UOSInt>(w, itemH), Media::AT_NO_ALPHA).SetTo(tmpBmp))
			return;
	}
	tmpBmp->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
	tmpBmp->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);

	NN<Media::DrawBrush> bWhite = img->NewBrushARGB(this->colorConv->ConvRGB8(0xffffffff));
	NN<Media::DrawBrush> bBlack = img->NewBrushARGB(this->colorConv->ConvRGB8(0xff000000));
	while (currPos < j && i < h)
	{
		if (currPos == defVal)
		{
			NN<Media::DrawBrush> bDef = img->NewBrushARGB(this->colorConv->ConvRGB8(0xffffffc0));
			img->DrawRect(Math::Coord2DDbl(0, UOSInt2Double(i)), Math::Size2DDbl(UOSInt2Double(w), itemTH), 0, bDef);
			img->DelBrush(bDef);
		}
		else
		{
			img->DrawRect(Math::Coord2DDbl(0, UOSInt2Double(i)), Math::Size2DDbl(UOSInt2Double(w), itemTH), 0, bWhite);
		}
		this->core->GenFontStylePreview(tmpBmp, deng, this->env, currPos, this->colorConv);
		if (currPos == this->currFontStyle)
		{
			NN<Media::DrawBrush> bRed = img->NewBrushARGB(this->colorConv->ConvRGB8(0xffff0000));
			img->DrawRect(Math::Coord2DDbl(0, UOSInt2Double(i)), Math::Size2DDbl(UOSInt2Double(w), itemTH), 0, bRed);
			img->DelBrush(bRed);
		}
		img->DrawImagePt(tmpBmp, Math::Coord2DDbl(UOSInt2Double((w - tmpBmp->GetWidth()) >> 1), UOSInt2Double(i + 1)));
		sbuff[0] = 0;
		UnsafeArray<UTF8Char> sptr;
		if (!this->env->GetFontStyleName(currPos, sbuff).SetTo(sptr) || sbuff[0] == 0)
		{
			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Style ")), currPos);
		}
		if (sbuff[0])
		{
			Math::Size2DDbl sz;
			NN<Media::DrawFont> fnt;
			if (this->CreateDrawFont(img).SetTo(fnt))
			{
				sz = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
				img->DrawString(Math::Coord2DDbl((UOSInt2Double(w) - sz.x) * 0.5, UOSInt2Double(i + itemH + 2)), CSTRP(sbuff, sptr), fnt, bBlack);
				img->DelFont(fnt);
			}
		}

		i += itemTH;
		currPos++;
	}
	if (i < h)
	{
		img->DrawRect(Math::Coord2DDbl(0, UOSInt2Double(i)), Math::Size2DDbl(UOSInt2Double(w), UOSInt2Double(h - i)), 0, bWhite);
	}
	img->DelBrush(bWhite);
	img->DelBrush(bBlack);
	deng->DeleteImage(tmpBmp);
}

void SSWR::AVIRead::AVIRFontSelector::OnMouseDown(OSInt scrollY, Math::Coord2D<OSInt> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys)
{
	OSInt i = scrollY + (pos.y / Double2Int32(80 * this->GetHDPI() / this->GetDDPI()));
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
			this->mnuLayers->ShowMenu(*this, this->GetScreenPosP() + pos);
		}
	}
}

void SSWR::AVIRead::AVIRFontSelector::OnKeyDown(UInt32 keyCode)
{
	switch (UI::GUIControl::OSKey2GUIKey(keyCode))
	{
	case UI::GUIControl::GK_PAGEUP:
		{
			Math::Size2DDbl sz = this->GetSize();
			sz.y = sz.y / 80;
			if (sz.y <= 0)
			{
				sz.y = 1;
			}
			this->currFontStyle -= (UOSInt)sz.y;
			if (this->currFontStyle == INVALID_INDEX)
			{
				this->currFontStyle = 0;
			}
			if (!this->MakeVisible(this->currFontStyle, this->currFontStyle + 1))
			{
				this->Redraw();
			}
		}
		break;
	case UI::GUIControl::GK_PAGEDOWN:
		{
			Math::Size2DDbl sz = this->GetSize();
			sz.y = sz.y / 80;
			if (sz.y <= 0)
			{
				sz.y = 1;
			}
			this->currFontStyle += (UOSInt)sz.y;
			if (this->currFontStyle >= this->env->GetFontStyleCount())
			{
				this->currFontStyle = this->env->GetFontStyleCount() - 1;
			}
			if (!this->MakeVisible(this->currFontStyle, this->currFontStyle + 1))
			{
				this->Redraw();
			}
		}
		break;
	case UI::GUIControl::GK_HOME:
		this->currFontStyle = 0;
		if (!this->MakeVisible(0, 1))
		{
			this->Redraw();
		}
		break;
	case UI::GUIControl::GK_END:
		this->currFontStyle = this->env->GetFontStyleCount() - 1;
		if (!this->MakeVisible(this->currFontStyle, this->currFontStyle + 1))
		{
			this->Redraw();
		}
		break;
	case UI::GUIControl::GK_UP:
		if (this->currFontStyle > 0)
		{
			this->currFontStyle -= 1;
			if (!this->MakeVisible(this->currFontStyle, this->currFontStyle + 1))
			{
				this->Redraw();
			}
		}
		break;
	case UI::GUIControl::GK_DOWN:
		if (this->currFontStyle < this->env->GetFontStyleCount() - 1)
		{
			this->currFontStyle += 1;
			if (!this->MakeVisible(this->currFontStyle, this->currFontStyle + 1))
			{
				this->Redraw();
			}
		}
		break;
	case UI::GUIControl::GK_BACKSPACE:
	case UI::GUIControl::GK_TAB:
	case UI::GUIControl::GK_CLEAR:
	case UI::GUIControl::GK_ENTER:
	case UI::GUIControl::GK_SHIFT:
	case UI::GUIControl::GK_CONTROL:
	case UI::GUIControl::GK_ALT:
	case UI::GUIControl::GK_PAUSE:
	case UI::GUIControl::GK_CAPITAL:
	case UI::GUIControl::GK_KANA:
	case UI::GUIControl::GK_JUNJA:
	case UI::GUIControl::GK_FINAL:
	case UI::GUIControl::GK_KANJI:
	case UI::GUIControl::GK_ESCAPE:
	case UI::GUIControl::GK_CONVERT:
	case UI::GUIControl::GK_NONCONVERT:
	case UI::GUIControl::GK_ACCEPT:
	case UI::GUIControl::GK_MODECHANGE:
	case UI::GUIControl::GK_SPACE:
	case UI::GUIControl::GK_LEFT:
	case UI::GUIControl::GK_RIGHT:
	case UI::GUIControl::GK_SELECT:
	case UI::GUIControl::GK_PRINT:
	case UI::GUIControl::GK_EXECUTE:
	case UI::GUIControl::GK_PRINTSCREEN:
	case UI::GUIControl::GK_INSERT:
	case UI::GUIControl::GK_DELETE:
	case UI::GUIControl::GK_HELP:
	case UI::GUIControl::GK_0:
	case UI::GUIControl::GK_1:
	case UI::GUIControl::GK_2:
	case UI::GUIControl::GK_3:
	case UI::GUIControl::GK_4:
	case UI::GUIControl::GK_5:
	case UI::GUIControl::GK_6:
	case UI::GUIControl::GK_7:
	case UI::GUIControl::GK_8:
	case UI::GUIControl::GK_9:
	case UI::GUIControl::GK_A:
	case UI::GUIControl::GK_B:
	case UI::GUIControl::GK_C:
	case UI::GUIControl::GK_D:
	case UI::GUIControl::GK_E:
	case UI::GUIControl::GK_F:
	case UI::GUIControl::GK_G:
	case UI::GUIControl::GK_H:
	case UI::GUIControl::GK_I:
	case UI::GUIControl::GK_J:
	case UI::GUIControl::GK_K:
	case UI::GUIControl::GK_L:
	case UI::GUIControl::GK_M:
	case UI::GUIControl::GK_N:
	case UI::GUIControl::GK_O:
	case UI::GUIControl::GK_P:
	case UI::GUIControl::GK_Q:
	case UI::GUIControl::GK_R:
	case UI::GUIControl::GK_S:
	case UI::GUIControl::GK_T:
	case UI::GUIControl::GK_U:
	case UI::GUIControl::GK_V:
	case UI::GUIControl::GK_W:
	case UI::GUIControl::GK_X:
	case UI::GUIControl::GK_Y:
	case UI::GUIControl::GK_Z:
	case UI::GUIControl::GK_LWIN:
	case UI::GUIControl::GK_RWIN:
	case UI::GUIControl::GK_APPS:
	case UI::GUIControl::GK_SLEEP:
	case UI::GUIControl::GK_NUMPAD0:
	case UI::GUIControl::GK_NUMPAD1:
	case UI::GUIControl::GK_NUMPAD2:
	case UI::GUIControl::GK_NUMPAD3:
	case UI::GUIControl::GK_NUMPAD4:
	case UI::GUIControl::GK_NUMPAD5:
	case UI::GUIControl::GK_NUMPAD6:
	case UI::GUIControl::GK_NUMPAD7:
	case UI::GUIControl::GK_NUMPAD8:
	case UI::GUIControl::GK_NUMPAD9:
	case UI::GUIControl::GK_MULTIPLY:
	case UI::GUIControl::GK_ADD:
	case UI::GUIControl::GK_SEPARATOR:
	case UI::GUIControl::GK_SUBTRACT:
	case UI::GUIControl::GK_DECIMAL:
	case UI::GUIControl::GK_DIVIDE:
	case UI::GUIControl::GK_F1:
	case UI::GUIControl::GK_F2:
	case UI::GUIControl::GK_F3:
	case UI::GUIControl::GK_F4:
	case UI::GUIControl::GK_F5:
	case UI::GUIControl::GK_F6:
	case UI::GUIControl::GK_F7:
	case UI::GUIControl::GK_F8:
	case UI::GUIControl::GK_F9:
	case UI::GUIControl::GK_F10:
	case UI::GUIControl::GK_F11:
	case UI::GUIControl::GK_F12:
	case UI::GUIControl::GK_F13:
	case UI::GUIControl::GK_F14:
	case UI::GUIControl::GK_F15:
	case UI::GUIControl::GK_F16:
	case UI::GUIControl::GK_F17:
	case UI::GUIControl::GK_F18:
	case UI::GUIControl::GK_F19:
	case UI::GUIControl::GK_F20:
	case UI::GUIControl::GK_F21:
	case UI::GUIControl::GK_F22:
	case UI::GUIControl::GK_F23:
	case UI::GUIControl::GK_F24:
	case UI::GUIControl::GK_NUMLOCK:
	case UI::GUIControl::GK_SCROLLLOCK:
	case UI::GUIControl::GK_OEM_1:
	case UI::GUIControl::GK_OEM_PLUS:
	case UI::GUIControl::GK_OEM_MINUS:
	case UI::GUIControl::GK_OEM_COMMA:
	case UI::GUIControl::GK_OEM_PERIOD:
	case UI::GUIControl::GK_OEM_2:
	case UI::GUIControl::GK_OEM_3:
	case UI::GUIControl::GK_OEM_4:
	case UI::GUIControl::GK_OEM_5:
	case UI::GUIControl::GK_OEM_6:
	case UI::GUIControl::GK_OEM_7:
	case UI::GUIControl::GK_NONE:
	default:
		break;
	}
}

SSWR::AVIRead::AVIRFontSelector::AVIRFontSelector(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, UOSInt initFontStyle, NN<Media::ColorManagerSess> colorSess) : UI::GUICustomDrawVScroll(ui, parent, core->GetDrawEngine())
{
	this->core = core;
	this->env = env;
	this->colorSess = colorSess;
	this->currFontStyle = initFontStyle;
	this->mnuLayers = 0;
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASSNN(this->colorConv, Media::ColorConv(srcProfile, destProfile, this->colorSess.Ptr()));
	this->colorSess->AddHandler(*this);

	this->UpdateFontStyles();
	this->MakeVisible(this->currFontStyle, this->currFontStyle + 1);
	this->HandleSizeChanged(OnResized, this);
}

SSWR::AVIRead::AVIRFontSelector::~AVIRFontSelector()
{
	this->colorConv.Delete();
	this->colorSess->RemoveHandler(*this);
}

void SSWR::AVIRead::AVIRFontSelector::YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuvParam)
{
}

void SSWR::AVIRead::AVIRFontSelector::RGBParamChanged(NN<const Media::IColorHandler::RGBPARAM2> rgbParam)
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
	Math::Size2DDbl sz = this->GetSize();
	UOSInt pageCnt = (UOSInt)Double2Int32(sz.y / 80);
	if (pageCnt <= 0)
		pageCnt = 1;
	this->SetVScrollBar(0, this->env->GetFontStyleCount(), pageCnt);
}

UOSInt SSWR::AVIRead::AVIRFontSelector::GetSelectedFontStyle()
{
	return this->currFontStyle;
}
