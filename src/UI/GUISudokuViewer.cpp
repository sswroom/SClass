#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "UI/GUISudokuViewer.h"

UI::GUISudokuViewer::GUISudokuViewer(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Optional<Media::ColorSess> colorSess, NN<Game::Sudoku::SudokuBoard> board) : UI::GUICustomDraw(ui, parent, eng, colorSess)
{
	this->board = board;
	this->selX = 0;
	this->selY = 0;
}

UI::GUISudokuViewer::~GUISudokuViewer()
{
}

Bool UI::GUISudokuViewer::OnMouseDown(Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		this->Focus();

		Math::Size2D<UOSInt> sz;
		UOSInt size;
		UOSInt tlx;
		UOSInt tly;
		sz = this->GetSizeP();
		if (sz.x > sz.y)
		{
			size = sz.y;
			tly = 0;
			tlx = (sz.x - sz.y) >> 1;
		}
		else
		{
			size = sz.x;
			tlx = 0;
			tly = (sz.y - sz.x) >> 1;
		}
		if (scnPos.x >= (OSInt)tlx && scnPos.x < (OSInt)(tlx + size) && scnPos.y >= (OSInt)tly && scnPos.y < (OSInt)(tly + size))
		{
			tlx = ((UOSInt)scnPos.x - tlx) * 9 / size;
			tly = ((UOSInt)scnPos.y - tly) * 9 / size;
			if (tlx != this->selX || tly != this->selY)
			{
				this->selX = tlx;
				this->selY = tly;
				this->Redraw();
			}
		}
	}
	return false;
}

Bool UI::GUISudokuViewer::OnKeyDown(UI::GUIControl::GUIKey key)
{
	UOSInt i;
	if (key >= UI::GUIControl::GK_0 && key <= UI::GUIControl::GK_9)
	{
		NumberInputEvent hdlr;
		i = this->numInputHdlr.GetCount();
		while (i-- > 0)
		{
			hdlr = this->numInputHdlr.GetItem(i);
			hdlr(this->numInputObj.GetItem(i), this->selX, this->selY, (UInt8)(key - UI::GUIControl::GK_0));
		}
		return true;
	}
	else if (key >= UI::GUIControl::GK_NUMPAD0 && key <= UI::GUIControl::GK_NUMPAD9)
	{
		NumberInputEvent hdlr;
		i = this->numInputHdlr.GetCount();
		while (i-- > 0)
		{
			hdlr = this->numInputHdlr.GetItem(i);
			hdlr(this->numInputObj.GetItem(i), this->selX, this->selY, (UInt8)(key - UI::GUIControl::GK_NUMPAD0));
		}
		return true;
	}
	else if (key == UI::GUIControl::GK_LEFT || key == UI::GUIControl::GK_A)
	{
		if (this->selX > 0)
		{
			this->selX -= 1;
			this->Redraw();
		}
		return true;
	}
	else if (key == UI::GUIControl::GK_RIGHT || key == UI::GUIControl::GK_D)
	{
		if (this->selX < 8)
		{
			this->selX += 1;
			this->Redraw();
		}
		return true;
	}
	else if (key == UI::GUIControl::GK_UP || key == UI::GUIControl::GK_W)
	{
		if (this->selY > 0)
		{
			this->selY -= 1;
			this->Redraw();
		}
		return true;
	}
	else if (key == UI::GUIControl::GK_DOWN || key == UI::GUIControl::GK_S || key == UI::GUIControl::GK_X)
	{
		if (this->selY < 8)
		{
			this->selY += 1;
			this->Redraw();
		}
		return true;
	}
	return false;
}

void UI::GUISudokuViewer::OnDraw(NN<Media::DrawImage> img)
{
	Math::Size2D<UOSInt> sz;
	UOSInt size;
	UOSInt blockSize;
	UOSInt tlx;
	UOSInt tly;
	NN<Media::DrawBrush> b3;
	sz = this->GetSizeP();
	b3 = img->NewBrushARGB(0xffffffff);
	if (sz.x > sz.y)
	{
		size = sz.y;
		tly = 0;
		tlx = (sz.x - sz.y) >> 1;
		img->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(UOSInt2Double(tlx), UOSInt2Double(sz.y)), 0, b3);
		img->DrawRect(Math::Coord2DDbl(UOSInt2Double(tlx + size), 0), Math::Size2DDbl(UOSInt2Double(sz.x - tlx - size), UOSInt2Double(sz.y)), 0, b3);
	}
	else
	{
		size = sz.x;
		tlx = 0;
		tly = (sz.y - sz.x) >> 1;
		img->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(UOSInt2Double(sz.x), UOSInt2Double(tly)), 0, b3);
		img->DrawRect(Math::Coord2DDbl(0, UOSInt2Double(tly + size)), Math::Size2DDbl(UOSInt2Double(sz.x), UOSInt2Double(sz.y - tly - size)), 0, b3);
	}
	blockSize = (size / 9) - 2;

	NN<Media::DrawPen> p;
	NN<Media::DrawBrush> b;
	NN<Media::DrawBrush> b2;
	NN<Media::DrawFont> f;
	UOSInt lastPos;
	UOSInt currPos;
	UOSInt i;
	UOSInt j;

	p = img->NewPenARGB(0xff000000, 1, 0, 0);
	i = 9;
	lastPos = size;
	while (i-- > 0)
	{
		currPos = MulDivUOS(size, i, 9);
		img->DrawLine(UOSInt2Double(tlx + lastPos - 1), UOSInt2Double(tly), UOSInt2Double(tlx + lastPos - 1), UOSInt2Double(tly + size), p);
		img->DrawLine(UOSInt2Double(tlx), UOSInt2Double(tly + lastPos - 1), UOSInt2Double(tlx + size), UOSInt2Double(tly + lastPos - 1), p);
		img->DrawLine(UOSInt2Double(tlx + currPos), UOSInt2Double(tly), UOSInt2Double(tlx + currPos), UOSInt2Double(tly + size), p);
		img->DrawLine(UOSInt2Double(tlx), UOSInt2Double(tly + currPos), UOSInt2Double(tlx + size), UOSInt2Double(tly + currPos), p);

		lastPos = currPos;
	}
	img->DelPen(p);

	p = img->NewPenARGB(0xffff0000, 1, 0, 0);
	i = MulDivUOS(size, this->selX, 9);
	j = MulDivUOS(size, this->selX + 1, 9);
	currPos = MulDivUOS(size, this->selY, 9);
	lastPos = MulDivUOS(size, this->selY + 1, 9);
	img->DrawRect(Math::Coord2DDbl(UOSInt2Double(tlx + i), UOSInt2Double(tly + currPos)), Math::Size2DDbl(UOSInt2Double(j - i - 1), UOSInt2Double(lastPos - currPos - 1)), p, 0);
	img->DelPen(p);

	f = img->NewFontPx(CSTR("Arial"), UOSInt2Double(blockSize) * 0.5, Media::DrawEngine::DFS_NORMAL, 65001);
	b = img->NewBrushARGB(0xffcccccc);
	b2 = img->NewBrushARGB(0xff000000);
	i = 9;
	while (i-- > 0)
	{
		j = 9;
		while (j-- > 0)
		{
			Bool isDefault;
			UTF8Char ch[2];
			ch[1] = 0;
			UInt8 val;
			Math::Coord2D<UOSInt> pt;
			Math::Coord2D<UOSInt> pt2;
			Math::Size2DDbl sz;
			val = this->board->GetBoardNum(i, j, isDefault);
			pt.x = tlx + 1 + MulDivUOS(size, i, 9);
			pt.y = tly + 1 + MulDivUOS(size, j, 9);
			pt2.x = tlx - 1 + MulDivUOS(size, i + 1, 9);
			pt2.y = tly - 1 + MulDivUOS(size, j + 1, 9);
			if (val)
			{
				ch[0] = (UTF8Char)(val + 0x30);
				if (isDefault)
				{
					img->DrawRect(pt.ToDouble(), (pt2 - pt).ToDouble(), 0, b);
				}
				else
				{
					img->DrawRect(pt.ToDouble(), (pt2 - pt).ToDouble(), 0, b3);
				}
				sz = img->GetTextSize(f, {ch, 1});
				img->DrawString(pt.ToDouble() + ((pt2 - pt).ToDouble() - sz) * 0.5, {ch, 1}, f, b2);
			}
			else
			{
				img->DrawRect(pt.ToDouble(), (pt2 - pt).ToDouble(), 0, b3);
			}
		}
	}
	img->DelBrush(b2);
	img->DelBrush(b);
	img->DelBrush(b3);
	img->DelFont(f);
}

void UI::GUISudokuViewer::HandleNumberInput(NumberInputEvent hdlr, AnyType userObj)
{
	this->numInputHdlr.Add(hdlr);
	this->numInputObj.Add(userObj);
}
