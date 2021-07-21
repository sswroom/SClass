#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/RandomOS.h"
#include "Math/Math.h"
#include "Media/GDIEngine.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/GUIClientControl.h"
#include "UI/GUICoreWin.h"
#include "UI/GUIRealtimeLineChart.h"
#include <windows.h>

#define CLASSNAME L"RealtimeLineChart"
Int32 UI::GUIRealtimeLineChart::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt __stdcall UI::GUIRealtimeLineChart::RLCWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam)
{
	UI::GUIRealtimeLineChart *me = (UI::GUIRealtimeLineChart*)UI::GUICoreWin::MSGetWindowObj(hWnd, GWL_USERDATA);
	switch (msg)
	{
	case WM_SIZE:
		break;
	case WM_ERASEBKGND:
		return 0;
	case WM_PAINT:
		{
			RECT rc;
			PAINTSTRUCT ps;
			GetClientRect((HWND)hWnd, &rc);
			BeginPaint((HWND)hWnd, &ps);
			Media::DrawImage *scn = ((Media::GDIEngine*)me->eng)->CreateImageScn(ps.hdc, rc.left, rc.top, rc.right, rc.bottom);
			me->OnPaint(scn);
			me->eng->DeleteImage(scn);
			EndPaint((HWND)hWnd, &ps);
		}
		return 0;
	case WM_TIMER:
		if (me->valueChanged)
		{
			me->valueChanged = false;
			me->Redraw();
		}
		return 0;
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::GUIRealtimeLineChart::OnPaint(Media::DrawImage *dimg)
{
	Media::DrawImage *img;
	Media::DrawPen *p;
	Media::DrawBrush *b;
	Media::DrawFont *f;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Double strWidth;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Double sz[2];
	Double lastX;
	Double lastY;
	Double thisX;
	Double thisY;
//	Double hdpi = this->hdpi;
//	Double ddpi = this->ddpi;

	this->valueChanged = false;
	img = this->eng->CreateImage32(dimg->GetWidth(), dimg->GetHeight(), Media::AT_NO_ALPHA);
	b = img->NewBrushARGB(this->bgColor);
	img->DrawRect(0, 0, Math::UOSInt2Double(img->GetWidth()), Math::UOSInt2Double(img->GetHeight()), 0, b);
	img->DelBrush(b);

	Sync::MutexUsage mutUsage(this->chartMut);
	if (this->chartMaxChg)
	{
		this->chartMaxChg = false;
		this->chartMax = this->chartMin = this->chartVal[0];
		i = 1;
		j = this->lineCnt * this->sampleCnt;
		while (i < j)
		{
			if (this->chartVal[i] > this->chartMax)
				this->chartMax = this->chartVal[i];
			if (this->chartVal[i] < this->chartMin)
				this->chartMin = this->chartVal[i];
			i++;
		}
		if (this->chartMin == this->chartMax)
			this->chartMax = this->chartMin + 1.0;
	}

	Double pw = 1 * this->hdpi / this->ddpi;
	if (pw < 1)
		pw = 1;
	f = img->NewFontPt((const UTF8Char*)"Arial", 9, Media::DrawEngine::DFS_ANTIALIAS, 0);
	p = img->NewPenARGB(this->fontColor, Math::Double2Int32(pw), 0, 0);
	b = img->NewBrushARGB(this->fontColor);
	sptr = Text::StrDoubleFmt(sbuff, this->chartMax, "0.##");
	if (this->unit)
		sptr =Text::StrConcat(sptr, this->unit);
	img->GetTextSizeC(f, sbuff, (UOSInt)(sptr - sbuff), sz);
	strWidth = sz[0];
	img->DrawString(0, 0, sbuff, f, b);
	sptr = Text::StrDoubleFmt(sbuff, this->chartMin, "0.##");
	if (this->unit)
		sptr =Text::StrConcat(sptr, this->unit);
	img->GetTextSizeC(f, sbuff, (UOSInt)(sptr - sbuff), sz);
	if (sz[0] > strWidth)
		strWidth = sz[0];
	img->DrawString(0, Math::UOSInt2Double(dimg->GetHeight()) - sz[1], sbuff, f, b);
	img->DrawLine(strWidth, 0, strWidth, Math::UOSInt2Double(dimg->GetHeight()), p);
	img->DelBrush(b);
	img->DelPen(p);
	img->DelFont(f);

	j = this->lineCnt * this->sampleCnt;
	k = this->lineCnt;
	while (k-- > 0)
	{
		lastX = -1;
		lastY = -1;
		p = img->NewPenARGB(this->lineColor[k], Math::Double2Int32(pw), 0, 0);
		i = k + this->chartOfst * this->lineCnt;
		l = 0;
		while (l < this->sampleCnt)
		{
			thisX = Math::UOSInt2Double(l) * (Math::UOSInt2Double(dimg->GetWidth()) - strWidth - 1) / Math::UOSInt2Double(this->sampleCnt);
			thisY = Math::UOSInt2Double(dimg->GetHeight()) - 1 - ((this->chartVal[i] - this->chartMin) * (Math::UOSInt2Double(dimg->GetHeight()) - 1) / (this->chartMax - this->chartMin));
			if (lastX > 0)
			{
				img->DrawLine(lastX + strWidth + 1, lastY, thisX + strWidth + 1, thisY, p);
			}
			lastX = thisX;
			lastY = thisY;
			i += this->lineCnt;
			if (i >= j)
			{
				i -= j;
			}
			l++;
		}
		img->DelPen(p);
	}
	mutUsage.EndUse();

	dimg->DrawImagePt(img, 0, 0);

	this->eng->DeleteImage(img);
}

void UI::GUIRealtimeLineChart::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::GUIRealtimeLineChart::RLCWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst; 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW); 
    wc.hbrBackground = 0; 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::GUIRealtimeLineChart::Deinit(void *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

UI::GUIRealtimeLineChart::GUIRealtimeLineChart(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateInterval) : UI::GUIControl(ui, parent)
{
	this->eng = eng;
	this->lineCnt = lineCnt;
	this->sampleCnt = sampleCnt;
	this->lineColor = MemAlloc(UInt32, lineCnt);
	this->bgColor = 0xffffffff;
	this->fontColor = 0xff000000;
	this->unit = 0;
	UOSInt i;
	if (lineCnt >= 1)
		this->lineColor[0] = 0xffff0000;
	if (lineCnt >= 2)
		this->lineColor[1] = 0xff0000ff;
	if (lineCnt >= 3)
		this->lineColor[2] = 0xff00ff00;
	if (this->lineCnt > 3)
	{
		Data::RandomOS rand;
		i = 3;
		while (i < lineCnt)
		{
			this->lineColor[i] = rand.NextInt30() | 0xff;
			i++;
		}
	}
	this->valueChanged = false;
	this->chartVal = MemAlloc(Double, this->lineCnt * this->sampleCnt);
	this->chartOfst = 0;
	this->chartMax = 0;
	this->chartMin = 0;
	this->chartMaxChg = false;
	NEW_CLASS(this->chartMut, Sync::Mutex());
	ClearChart();

	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
		Init(((UI::GUICoreWin*)this->ui)->GetHInst());
	}

	Int32 style = WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)this->ui)->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"", style, 0, 0, 0, 200, 200);

	SetTimer((HWND)this->hwnd, 1, updateInterval, 0);
}

UI::GUIRealtimeLineChart::~GUIRealtimeLineChart()
{
	KillTimer((HWND)this->hwnd, 1);
	if (Sync::Interlocked::Decrement(&useCnt) == 0)
	{
		Deinit(((UI::GUICoreWin*)this->ui)->GetHInst());
	}
	DEL_CLASS(this->chartMut);
	MemFree(this->chartVal);
	SDEL_TEXT(this->unit);
	MemFree(this->lineColor);
}

const UTF8Char *UI::GUIRealtimeLineChart::GetObjectClass()
{
	return (const UTF8Char*)"RealtimeLineChart";
}

OSInt UI::GUIRealtimeLineChart::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIRealtimeLineChart::AddSample(Double *samples)
{
	Sync::MutexUsage mutUsage(this->chartMut);
	UOSInt ofst = this->chartOfst * this->lineCnt;
	UOSInt i = 0;
	while (i < lineCnt)
	{
		if (this->chartVal[ofst + i] == this->chartMax || this->chartVal[ofst + i] == this->chartMin)
			this->chartMaxChg = true;
		this->chartVal[ofst + i] = samples[i];
		if (this->chartMax < samples[i])
			this->chartMax = samples[i];
		if (this->chartMin > samples[i])
			this->chartMin = samples[i];
		i++;
	}

	this->chartOfst = (this->chartOfst + 1) % this->sampleCnt;
	mutUsage.EndUse();
	this->valueChanged = true;
}

void UI::GUIRealtimeLineChart::SetLineColor(UOSInt lineId, UInt32 lineColor)
{
	if (lineId < this->lineCnt)
	{
		this->lineColor[lineId] = lineColor;
		this->valueChanged = true;
	}
}

void UI::GUIRealtimeLineChart::SetUnit(const UTF8Char *unit)
{
	SDEL_TEXT(this->unit);
	if (unit)
		this->unit = Text::StrCopyNew(unit);
	this->valueChanged = true;
}

void UI::GUIRealtimeLineChart::SetBGColor(UInt32 bgColor)
{
	this->bgColor = bgColor;
	this->valueChanged = true;
}

void UI::GUIRealtimeLineChart::SetFontColor(UInt32 fontColor)
{
	this->fontColor = fontColor;
	this->valueChanged = true;
}

void UI::GUIRealtimeLineChart::ClearChart()
{
	UOSInt i;
	UOSInt j;
	Sync::MutexUsage mutUsage(this->chartMut);
	i = 0;
	j = this->lineCnt * this->sampleCnt;
	while (i < j)
	{
		this->chartVal[i] = 0;
		i++;
	}
	this->chartMax = 1.0;
	this->chartMin = 0;
	this->chartMaxChg = false;
	this->chartOfst = 0;
	this->valueChanged = true;
	mutUsage.EndUse();
}
