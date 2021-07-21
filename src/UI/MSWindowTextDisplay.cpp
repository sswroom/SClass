#include "stdafx.h"
#include "Text/MyString.h"
#include "UI/MSWindowTextDisplay.h"
#include <windows.h>
#include <ddraw.h>

UI::MSWindowTextDisplay::MSWindowTextDisplay(void *hInst, UI::MSWindowClientControl *parent, Media::GDIEngine *eng) : UI::MSWindowDDrawControl(hInst, parent, false)
{
	this->currText = 0;
	this->eng = eng;
}

UI::MSWindowTextDisplay::~MSWindowTextDisplay()
{
	SDEL_TEXT(this->currText);
}

const WChar *UI::MSWindowTextDisplay::GetObjectClass()
{
	return L"TextDisplay";
}

OSInt UI::MSWindowTextDisplay::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::MSWindowTextDisplay::OnSurfaceCreated()
{
	UpdateScreen();
}

void UI::MSWindowTextDisplay::SetDispText(const WChar *dispText)
{
	SDEL_TEXT(this->currText);
	this->currText = Text::StrCopyNew(dispText);
	UpdateScreen();
}

void UI::MSWindowTextDisplay::UpdateScreen()
{
	if (this->currText)
	{
		OSInt i;
		OSInt j;
		WChar *strBuff;
		Double sz[2];
		Double currY;
		Data::ArrayList<WChar *> strArr;
		i = Text::StrCharCnt(this->currText);
		strBuff = MemAlloc(WChar, i + 1);
		Text::StrConcat(strBuff, this->currText);
		i = 0;
		while (true)
		{
			strArr.Add(&strBuff[i]);
			j = Text::StrIndexOf(&strBuff[i], '\r');
			if (j < 0)
				break;
			strBuff[i + j] = 0;
			if (strBuff[i + j + 1] == '\n')
			{
				i = i + j + 2;
			}
			else
			{
				i = i + j + 1;
			}
		}

		this->surfaceMut->Lock();
		Media::GDIImage *img = (Media::GDIImage*)this->eng->CreateImage32(this->surfaceW, this->surfaceH);
		Media::DrawBrush *b = img->NewBrushARGB(0xff000000);
		Media::DrawFont *f = img->NewFontH(L"Arial", 120, Media::DrawEngine::DFS_NORMAL, 0);
		img->DrawRect(0, 0, this->surfaceW, this->surfaceH, 0, b);
		img->DelBrush(b);
		b = img->NewBrushARGB(0xffffffff);
		img->SetTextAlign(Media::DrawEngine::DRAW_POS_TOPCENTER);
		img->GetTextSize(f, strArr.GetItem(0), -1, sz);
		currY = 0;
		i = 0;
		j = strArr.GetCount();
		while (i < j)
		{
			img->DrawString(this->surfaceW * 0.5, currY, strArr.GetItem(i), f, b);
			currY += sz[1];
			i++;
		}
		img->DelBrush(b);
		img->DelFont(f);
		MemFree(strBuff);

		LPDIRECTDRAWSURFACE7 surface = (LPDIRECTDRAWSURFACE7)this->surfaceBuff;
		if (surface)
		{
			RECT rc;
			DDSURFACEDESC2 ddsd;
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);

			rc.left = 0;
			rc.top = 0;
			rc.right = this->surfaceW;
			rc.bottom = this->surfaceH;

			if (DD_OK == surface->Lock(&rc, &ddsd, DDLOCK_WAIT, 0))
			{
				UInt8 *dptr = (UInt8*)ddsd.lpSurface;
				UInt8 *sptr = (UInt8*)img->bmpBits;
				OSInt dbpl = ddsd.lPitch;
				OSInt sbpl = img->GetWidth() << 2;
				OSInt i = this->surfaceH;
				sptr += sbpl * this->surfaceH;
				while (i-- > 0)
				{
					sptr -= sbpl;
					MemCopy(dptr, sptr, sbpl);
					dptr += dbpl;
				}

				surface->Unlock(0);
				this->DrawToScreen();
			}
		}
		this->surfaceMut->Unlock();
		this->eng->DeleteImage(img);
	}
}
