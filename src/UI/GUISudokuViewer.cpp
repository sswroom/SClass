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
		OSInt size;
		OSInt tlx;
		OSInt tly;
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
		if (x >= tlx && x < tlx + size && y >= tly && y < tly + size)
		{
			x = (x - tlx) * 9 / size;
			y = (y - tly) * 9 / size;
			if (x != this->selX || y != this->selY)
			{
				this->selX = x;
				this->selY = y;
				this->Redraw();
			}
		}
	}
	return false;
}

Bool UI::GUISudokuViewer::OnKeyDown(UI::GUIControl::GUIKey key)
{
	OSInt i;
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
	OSInt size;
	OSInt blockSize;
	OSInt tlx;
	OSInt tly;
	Media::DrawBrush *b3;
	this->GetSizeP(&w, &h);
	b3 = img->NewBrushARGB(0xffffffff);
	if (w > h)
	{
		size = h;
		tly = 0;
		tlx = (w - h) >> 1;
		img->DrawRect(0, 0, Math::OSInt2Double(tlx), Math::OSInt2Double(h), 0, b3);
		img->DrawRect(Math::OSInt2Double(tlx + size), 0, Math::OSInt2Double(w - tlx - size), Math::OSInt2Double(h), 0, b3);
	}
	else
	{
		size = w;
		tlx = 0;
		tly = (h - w) >> 1;
		img->DrawRect(0, 0, Math::OSInt2Double(w), Math::OSInt2Double(tly), 0, b3);
		img->DrawRect(0, Math::OSInt2Double(tly + size), Math::OSInt2Double(w), Math::OSInt2Double(h - tly - size), 0, b3);
	}
	blockSize = (size / 9) - 2;

	Media::DrawPen *p;
	Media::DrawBrush *b;
	Media::DrawBrush *b2;
	Media::DrawFont *f;
	OSInt lastPos;
	OSInt currPos;
	OSInt i;
	OSInt j;

	p = img->NewPenARGB(0xff000000, 1, 0, 0);
	i = 9;
	lastPos = size;
	while (i-- > 0)
	{
		currPos = MulDivOS(size, i, 9);
		img->DrawLine(Math::OSInt2Double(tlx + lastPos - 1), Math::OSInt2Double(tly), Math::OSInt2Double(tlx + lastPos - 1), Math::OSInt2Double(tly + size), p);
		img->DrawLine(Math::OSInt2Double(tlx), Math::OSInt2Double(tly + lastPos - 1), Math::OSInt2Double(tlx + size), Math::OSInt2Double(tly + lastPos - 1), p);
		img->DrawLine(Math::OSInt2Double(tlx + currPos), Math::OSInt2Double(tly), Math::OSInt2Double(tlx + currPos), Math::OSInt2Double(tly + size), p);
		img->DrawLine(Math::OSInt2Double(tlx), Math::OSInt2Double(tly + currPos), Math::OSInt2Double(tlx + size), Math::OSInt2Double(tly + currPos), p);

		lastPos = currPos;
	}
	img->DelPen(p);

	p = img->NewPenARGB(0xffff0000, 1, 0, 0);
	i = MulDivOS(size, this->selX, 9);
	j = MulDivOS(size, this->selX + 1, 9);
	currPos = MulDivOS(size, this->selY, 9);
	lastPos = MulDivOS(size, this->selY + 1, 9);
	img->DrawRect(Math::OSInt2Double(tlx + i), Math::OSInt2Double(tly + currPos), Math::OSInt2Double(j - i - 1), Math::OSInt2Double(lastPos - currPos - 1), p, 0);
	img->DelPen(p);

	f = img->NewFontHUTF8((const UTF8Char*)"Arial", blockSize * 0.5, Media::DrawEngine::DFS_NORMAL, 65001);
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
			OSInt x;
			OSInt y;
			OSInt x2;
			OSInt y2;
			Double sz[2];
			val = this->board->GetBoardNum(i, j, &isDefault);
			x = tlx + 1 + MulDivOS(size, i, 9);
			y = tly + 1 + MulDivOS(size, j, 9);
			x2 = tlx - 1 + MulDivOS(size, i + 1, 9);
			y2 = tly - 1 + MulDivOS(size, j + 1, 9);
			if (val)
			{
				ch[0] = val + 0x30;
				if (isDefault)
				{
					img->DrawRect(Math::OSInt2Double(x), Math::OSInt2Double(y), Math::OSInt2Double(x2 - x), Math::OSInt2Double(y2 - y), 0, b);
				}
				else
				{
					img->DrawRect(Math::OSInt2Double(x), Math::OSInt2Double(y), Math::OSInt2Double(x2 - x), Math::OSInt2Double(y2 - y), 0, b3);
				}
				img->GetTextSizeUTF8(f, ch, 1, sz);
				img->DrawStringUTF8(x + (x2 - x - sz[0]) * 0.5, y + (y2 - y - sz[1]) * 0.5, ch, f, b2);
			}
			else
			{
				img->DrawRect(Math::OSInt2Double(x), Math::OSInt2Double(y), Math::OSInt2Double(x2 - x), Math::OSInt2Double(y2 - y), 0, b3);
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
