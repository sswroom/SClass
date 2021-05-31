#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "UI/GUISudokuViewer.h"

UI::GUISudokuViewer::GUISudokuViewer(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *eng, Game::Sudoku::SudokuBoard *board) : UI::GUICustomDraw(ui, parent, eng)
{
	this->board = board;
	this->selX = 0;
	this->selY = 0;
	NEW_CLASS(this->numInputHdlr, Data::ArrayList<NumberInputEvent>());
	NEW_CLASS(this->numInputObj, Data::ArrayList<void *>());
}

UI::GUISudokuViewer::~GUISudokuViewer()
{
	DEL_CLASS(this->numInputHdlr);
	DEL_CLASS(this->numInputObj);
}

Bool UI::GUISudokuViewer::OnMouseDown(OSInt x, OSInt y, MouseButton btn)
{
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		this->Focus();

		UOSInt w;
		UOSInt h;
		UOSInt size;
		UOSInt tlx;
		UOSInt tly;
		this->GetSizeP(&w, &h);
		if (w > h)
		{
			size = h;
			tly = 0;
			tlx = (w - h) >> 1;
		}
		else
		{
			size = w;
			tlx = 0;
			tly = (h - w) >> 1;
		}
		if (x >= (OSInt)tlx && x < (OSInt)(tlx + size) && y >= (OSInt)tly && y < (OSInt)(tly + size))
		{
			tlx = ((UOSInt)x - tlx) * 9 / size;
			tly = ((UOSInt)y - tly) * 9 / size;
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
		i = this->numInputHdlr->GetCount();
		while (i-- > 0)
		{
			hdlr = this->numInputHdlr->GetItem(i);
			hdlr(this->numInputObj->GetItem(i), this->selX, this->selY, (UInt8)(key - UI::GUIControl::GK_0));
		}
		return true;
	}
	else if (key >= UI::GUIControl::GK_NUMPAD0 && key <= UI::GUIControl::GK_NUMPAD9)
	{
		NumberInputEvent hdlr;
		i = this->numInputHdlr->GetCount();
		while (i-- > 0)
		{
			hdlr = this->numInputHdlr->GetItem(i);
			hdlr(this->numInputObj->GetItem(i), this->selX, this->selY, (UInt8)(key - UI::GUIControl::GK_NUMPAD0));
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

void UI::GUISudokuViewer::OnDraw(Media::DrawImage *img)
{
	UOSInt w;
	UOSInt h;
	UOSInt size;
	UOSInt blockSize;
	UOSInt tlx;
	UOSInt tly;
	Media::DrawBrush *b3;
	this->GetSizeP(&w, &h);
	b3 = img->NewBrushARGB(0xffffffff);
	if (w > h)
	{
		size = h;
		tly = 0;
		tlx = (w - h) >> 1;
		img->DrawRect(0, 0, Math::UOSInt2Double(tlx), Math::UOSInt2Double(h), 0, b3);
		img->DrawRect(Math::UOSInt2Double(tlx + size), 0, Math::UOSInt2Double(w - tlx - size), Math::UOSInt2Double(h), 0, b3);
	}
	else
	{
		size = w;
		tlx = 0;
		tly = (h - w) >> 1;
		img->DrawRect(0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(tly), 0, b3);
		img->DrawRect(0, Math::UOSInt2Double(tly + size), Math::UOSInt2Double(w), Math::UOSInt2Double(h - tly - size), 0, b3);
	}
	blockSize = (size / 9) - 2;

	Media::DrawPen *p;
	Media::DrawBrush *b;
	Media::DrawBrush *b2;
	Media::DrawFont *f;
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
		img->DrawLine(Math::UOSInt2Double(tlx + lastPos - 1), Math::UOSInt2Double(tly), Math::UOSInt2Double(tlx + lastPos - 1), Math::UOSInt2Double(tly + size), p);
		img->DrawLine(Math::UOSInt2Double(tlx), Math::UOSInt2Double(tly + lastPos - 1), Math::UOSInt2Double(tlx + size), Math::UOSInt2Double(tly + lastPos - 1), p);
		img->DrawLine(Math::UOSInt2Double(tlx + currPos), Math::UOSInt2Double(tly), Math::UOSInt2Double(tlx + currPos), Math::UOSInt2Double(tly + size), p);
		img->DrawLine(Math::UOSInt2Double(tlx), Math::UOSInt2Double(tly + currPos), Math::UOSInt2Double(tlx + size), Math::UOSInt2Double(tly + currPos), p);

		lastPos = currPos;
	}
	img->DelPen(p);

	p = img->NewPenARGB(0xffff0000, 1, 0, 0);
	i = MulDivUOS(size, this->selX, 9);
	j = MulDivUOS(size, this->selX + 1, 9);
	currPos = MulDivUOS(size, this->selY, 9);
	lastPos = MulDivUOS(size, this->selY + 1, 9);
	img->DrawRect(Math::UOSInt2Double(tlx + i), Math::UOSInt2Double(tly + currPos), Math::UOSInt2Double(j - i - 1), Math::UOSInt2Double(lastPos - currPos - 1), p, 0);
	img->DelPen(p);

	f = img->NewFontPx((const UTF8Char*)"Arial", Math::UOSInt2Double(blockSize) * 0.5, Media::DrawEngine::DFS_NORMAL, 65001);
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
			UOSInt x;
			UOSInt y;
			UOSInt x2;
			UOSInt y2;
			Double sz[2];
			val = this->board->GetBoardNum(i, j, &isDefault);
			x = tlx + 1 + MulDivUOS(size, i, 9);
			y = tly + 1 + MulDivUOS(size, j, 9);
			x2 = tlx - 1 + MulDivUOS(size, i + 1, 9);
			y2 = tly - 1 + MulDivUOS(size, j + 1, 9);
			if (val)
			{
				ch[0] = (UTF8Char)(val + 0x30);
				if (isDefault)
				{
					img->DrawRect(Math::UOSInt2Double(x), Math::UOSInt2Double(y), Math::UOSInt2Double(x2 - x), Math::UOSInt2Double(y2 - y), 0, b);
				}
				else
				{
					img->DrawRect(Math::UOSInt2Double(x), Math::UOSInt2Double(y), Math::UOSInt2Double(x2 - x), Math::UOSInt2Double(y2 - y), 0, b3);
				}
				img->GetTextSize(f, ch, 1, sz);
				img->DrawString(Math::UOSInt2Double(x) + (Math::UOSInt2Double(x2 - x) - sz[0]) * 0.5, Math::UOSInt2Double(y) + (Math::UOSInt2Double(y2 - y) - sz[1]) * 0.5, ch, f, b2);
			}
			else
			{
				img->DrawRect(Math::UOSInt2Double(x), Math::UOSInt2Double(y), Math::UOSInt2Double(x2 - x), Math::UOSInt2Double(y2 - y), 0, b3);
			}
		}
	}
	img->DelBrush(b2);
	img->DelBrush(b);
	img->DelBrush(b3);
	img->DelFont(f);
}

void UI::GUISudokuViewer::HandleNumberInput(NumberInputEvent hdlr, void *userObj)
{
	this->numInputHdlr->Add(hdlr);
	this->numInputObj->Add(userObj);
}
