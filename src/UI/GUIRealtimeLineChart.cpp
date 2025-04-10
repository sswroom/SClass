#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/RandomOS.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIRealtimeLineChart.h"

void UI::GUIRealtimeLineChart::OnPaint(NN<Media::DrawImage> dimg)
{
	NN<Media::DrawImage> img;
	NN<Media::DrawPen> p;
	NN<Media::DrawBrush> b;
	NN<Media::DrawFont> f;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Double strWidth;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Math::Size2DDbl sz;
	Double lastX;
	Double lastY;
	Double thisX;
	Double thisY;

	this->valueChanged = false;
	if (this->eng->CreateImage32(dimg->GetSize(), Media::AT_ALPHA_ALL_FF).SetTo(img))
	{
		b = img->NewBrushARGB(this->bgColor);
		img->DrawRect(Math::Coord2DDbl(0, 0), img->GetSize().ToDouble(), 0, b);
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

		UnsafeArray<const UTF8Char> nns;
		Double pw = 1 * this->hdpi / this->ddpi;
		if (pw < 1)
			pw = 1;
		f = img->NewFontPt(CSTR("Arial"), 9.0, Media::DrawEngine::DFS_ANTIALIAS, 0);
		p = img->NewPenARGB(this->fontColor, Double2Int32(pw), 0, 0);
		b = img->NewBrushARGB(this->fontColor);
		sptr = Text::StrDoubleFmt(sbuff, this->chartMax, "0.##");
		if (this->unit.SetTo(nns))
			sptr =Text::StrConcat(sptr, nns);
		sz = img->GetTextSize(f, CSTRP(sbuff, sptr));
		strWidth = sz.x;
		img->DrawString(Math::Coord2DDbl(0, 1), CSTRP(sbuff, sptr), f, b);
		sptr = Text::StrDoubleFmt(sbuff, this->chartMin, "0.##");
		if (this->unit.SetTo(nns))
			sptr =Text::StrConcat(sptr, nns);
		sz = img->GetTextSize(f, CSTRP(sbuff, sptr));
		if (sz.x > strWidth)
			strWidth = sz.x;
		img->DrawString(Math::Coord2DDbl(0, UOSInt2Double(dimg->GetHeight()) - sz.y), CSTRP(sbuff, sptr), f, b);
		img->DrawLine(strWidth, 0, strWidth, UOSInt2Double(dimg->GetHeight()), p);
		img->DelBrush(b);
		img->DelPen(p);
		img->DelFont(f);

		j = this->lineCnt * this->sampleCnt;
		k = this->lineCnt;
		while (k-- > 0)
		{
			lastX = -1;
			lastY = -1;
			p = img->NewPenARGB(this->lineColor[k], Double2Int32(pw), 0, 0);
			i = k + this->chartOfst * this->lineCnt;
			l = 0;
			while (l < this->sampleCnt)
			{
				thisX = UOSInt2Double(l) * (UOSInt2Double(dimg->GetWidth()) - strWidth - 1) / UOSInt2Double(this->sampleCnt);
				thisY = UOSInt2Double(dimg->GetHeight()) - 1 - ((this->chartVal[i] - this->chartMin) * (UOSInt2Double(dimg->GetHeight()) - 1) / (this->chartMax - this->chartMin));
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

		dimg->DrawImagePt(img, Math::Coord2DDbl(0, 0));

		this->eng->DeleteImage(img);
	}
}

UI::GUIRealtimeLineChart::GUIRealtimeLineChart(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt) : UI::GUIControl(ui, parent)
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
	ClearChart();
}

UI::GUIRealtimeLineChart::~GUIRealtimeLineChart()
{
	MemFree(this->chartVal);
	SDEL_TEXT(this->unit);
	MemFree(this->lineColor);
}

Text::CStringNN UI::GUIRealtimeLineChart::GetObjectClass() const
{
	return CSTR("RealtimeLineChart");
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

void UI::GUIRealtimeLineChart::SetUnit(Text::CStringNN unit)
{
	SDEL_TEXT(this->unit);
	if (unit.leng != 0)
		this->unit = Text::StrCopyNewC(unit.v, unit.leng).Ptr();
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
}
