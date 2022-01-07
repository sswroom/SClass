#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Math/ComplexNumber.h"
#include "Math/FFT.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRAudioViewerForm.h"
#define FFTSAMPLE 1024
#define FFTAVG 1

typedef enum
{
	MNU_NEXT_SAMPLE = 101,
	MNU_PREV_SAMPLE,
	MNU_NEXT_PAGE,
	MNU_PREV_PAGE,
	MNU_BEGIN,
	MNU_END
} MenuItems;

void __stdcall SSWR::AVIRead::AVIRAudioViewerForm::OnSizeChanged(void *userObj)
{
	SSWR::AVIRead::AVIRAudioViewerForm *me = (SSWR::AVIRead::AVIRAudioViewerForm*)userObj;
	me->UpdateImages();
}

void SSWR::AVIRead::AVIRAudioViewerForm::UpdateImages()
{
	Media::DrawImage *gimg;
	UOSInt w;
	UOSInt h;
	this->pbsSample->GetSizeP(&w, &h);
	if (w <= 0 || h <= 0)
		return;
	if (this->totalSample == 0 || this->totalSample == (UOSInt)-1)
	{
		Media::DrawBrush *b;
		Media::DrawFont *f;
		gimg = this->eng->CreateImage32(w, h, Media::AT_NO_ALPHA);
		b = gimg->NewBrushARGB(0xff000000);
		gimg->DrawRect(0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h), 0, b);
		gimg->DelBrush(b);
		b = gimg->NewBrushARGB(0xffffffff);
		f = gimg->NewFontPt((const UTF8Char*)"Arial", 10, Media::DrawEngine::DFS_ANTIALIAS, 0);
		gimg->DrawString(0, 0, (const UTF8Char*)"Format not supported", f, b);
		gimg->DelFont(f);
		gimg->DelBrush(b);
		this->pbsSample->SetImageDImg(gimg);
		if (this->sampleImg)
		{
			this->eng->DeleteImage(this->sampleImg);
		}
		this->sampleImg = gimg;
	}
	else
	{
		Media::DrawBrush *b;
		Media::DrawPen *p;
		UOSInt i;
		UOSInt j;
		UInt32 currCh;
		UOSInt currY;
		UOSInt channelH;
		UOSInt lastY;
		UOSInt thisY;
		UOSInt align = this->format->align;
		gimg = this->eng->CreateImage32(w, h, Media::AT_NO_ALPHA);

		UInt8 *buff = MemAlloc(UInt8, this->format->align * w);
		i = this->audSrc->ReadSample(currSample, w, buff);
		
		b = gimg->NewBrushARGB(0xff000000);
		gimg->DrawRect(0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h), 0, b);
		gimg->DelBrush(b);

		channelH = h / this->format->nChannels / 2;
		if (channelH > 0)
		{
			p = gimg->NewPenARGB(0xffffffff, 1, 0, 0);
			if (this->format->bitpersample == 16)
			{
				if (i != w)
				{
					i = i * align;
					j = w * align;
					while (i < j)
					{
						*(Int16*)&buff[i] = 0;
						i += 2;
					}
				}
				currCh = this->format->nChannels;
				while (currCh-- > 0)
				{
					currY = h * currCh / this->format->nChannels;
					lastY = currY + channelH;
					i = 0;
					j = 2 * currCh;
					while (i < w)
					{
						thisY = currY + channelH + ((ReadUInt16(&buff[j]) * channelH) >> 15);
						gimg->DrawLine(Math::UOSInt2Double(i - 1), Math::UOSInt2Double(lastY), Math::UOSInt2Double(i), Math::UOSInt2Double(thisY), p);
						
						lastY = thisY;
						i++;
						j += align;
					}
				}
			}
			else if (this->format->bitpersample == 8)
			{
			}
			gimg->DelPen(p);
		}

		MemFree(buff);
		this->pbsSample->SetImageDImg(gimg);
		if (this->sampleImg)
		{
			this->eng->DeleteImage(this->sampleImg);
		}
		this->sampleImg = gimg;
	}
	this->UpdateFreqImage();
}

void SSWR::AVIRead::AVIRAudioViewerForm::UpdateFreqImage()
{
	Media::DrawImage *gimg;
	UOSInt w;
	UOSInt h;
	this->pbsFreq->GetSizeP(&w, &h);
	if (w <= 0 || h <= 0)
		return;
	if (this->totalSample == 0 || this->totalSample == (UOSInt)-1)
	{
		Media::DrawBrush *b;
		Media::DrawFont *f;
		gimg = this->eng->CreateImage32(w, h, Media::AT_NO_ALPHA);
		b = gimg->NewBrushARGB(0xff000000);
		gimg->DrawRect(0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h), 0, b);
		gimg->DelBrush(b);
		b = gimg->NewBrushARGB(0xffffffff);
		f = gimg->NewFontPt((const UTF8Char*)"Arial", 10, Media::DrawEngine::DFS_ANTIALIAS, 0);
		gimg->DrawString(0, 0, (const UTF8Char*)"Format not supported", f, b);
		gimg->DelFont(f);
		gimg->DelBrush(b);
		this->pbsSample->SetImageDImg(gimg);
		if (this->fftImg)
		{
			this->eng->DeleteImage(this->fftImg);
		}
		this->fftImg = gimg;
	}
	else
	{
		Media::DrawBrush *b;
		Media::DrawPen *p;
		UOSInt i;
		UOSInt j;
		Double lastX;
		Double lastY;
		Double thisX;
		Double thisY;
//		OSInt align = this->format->align;
		gimg = this->eng->CreateImage32(w, h, Media::AT_NO_ALPHA);

		UInt8 *buff = MemAlloc(UInt8, this->format->align * (FFTSAMPLE + FFTAVG - 1));
		i = this->audSrc->ReadSample(currSample - FFTSAMPLE + 1, FFTSAMPLE + FFTAVG - 1, buff);
		
		b = gimg->NewBrushARGB(0xff000000);
		gimg->DrawRect(0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h), 0, b);
		gimg->DelBrush(b);

		Double *freqData;
		freqData = MemAlloc(Double, FFTSAMPLE);
		i = 0;
		while (i < this->format->nChannels)
		{
			Math::FFT::ForwardBits(buff + i * (UOSInt)(this->format->bitpersample >> 3), freqData, FFTSAMPLE, FFTAVG, this->format->bitpersample, this->format->nChannels, Math::FFT::WT_BLACKMANN_HARRIS, 1.0);

			if (i == 0)
			{
				p = gimg->NewPenARGB(0xffff0000, 1, 0, 0);
			}
			else if (i == 1)
			{
				p = gimg->NewPenARGB(0xff0000ff, 1, 0, 0);
			}
			else if (i == 2)
			{
				p = gimg->NewPenARGB(0xff00ff00, 1, 0, 0);
			}
			else
			{
				Data::RandomOS rand;
				p = gimg->NewPenARGB((UInt32)rand.NextInt30() | 0xff000000, 1, 0, 0);
			}

			Double rVal;
			lastX = -1;
			lastY = -1;
			j = 0;
			while (j < (FFTSAMPLE / 2))
			{
				rVal = freqData[j];

				thisX = Math::UOSInt2Double(j * w) / (Double)(FFTSAMPLE / 2 - 1);
				thisY = -Math::Log10(rVal / (FFTSAMPLE * 0.5)) * Math::UOSInt2Double(h) / 7.0;
				if (lastX >= 0)
				{
					gimg->DrawLine(lastX, lastY, thisX, thisY, p);
				}
				lastX = thisX;
				lastY = thisY;
				j++;
			}

			gimg->DelPen(p);
			i++;
		}
		MemFree(freqData);

		MemFree(buff);
		this->pbsFreq->SetImageDImg(gimg);
		if (this->fftImg)
		{
			this->eng->DeleteImage(this->fftImg);
		}
		this->fftImg = gimg;
	}
}

SSWR::AVIRead::AVIRAudioViewerForm::AVIRAudioViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::IAudioSource *audSrc) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	this->SetFont(0, 8.25, false);
	audSrc->GetSourceName(Text::StrConcatC(sbuff, UTF8STRC("Audio Viewer - ")));
	this->SetText(sbuff);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->audSrc = audSrc;
	this->eng = core->GetDrawEngine();
	this->sampleImg = 0;
	this->fftImg = 0;
	NEW_CLASS(this->format, Media::AudioFormat());
	this->audSrc->GetFormat(this->format);

	UI::GUIMenu *mnu;
	NEW_CLASS(this->mnu, UI::GUIMainMenu());
	mnu = this->mnu->AddSubMenu((const UTF8Char*)"&Navigate");
	mnu->AddItem((const UTF8Char*)"&Next Sample", MNU_NEXT_SAMPLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_RIGHT);
	mnu->AddItem((const UTF8Char*)"&Prev Sample", MNU_PREV_SAMPLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_LEFT);
	mnu->AddItem((const UTF8Char*)"N&ext Page", MNU_NEXT_PAGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_PAGEDOWN);
	mnu->AddItem((const UTF8Char*)"P&rev Page", MNU_PREV_PAGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_PAGEUP);
	mnu->AddItem((const UTF8Char*)"Go to &Beginning", MNU_BEGIN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_HOME);
	mnu->AddItem((const UTF8Char*)"Go to E&nd", MNU_END, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_END);
	this->SetMenu(this->mnu);

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpSample = this->tcMain->AddTabPage((const UTF8Char*)"Sample");
	NEW_CLASS(this->pbsSample, UI::GUIPictureBoxSimple(ui, this->tpSample, this->eng, false));
	this->pbsSample->SetNoBGColor(true);
	this->pbsSample->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpFreq = this->tcMain->AddTabPage((const UTF8Char*)"Frequency");
	NEW_CLASS(this->pbsFreq, UI::GUIPictureBoxSimple(ui, this->tpFreq, this->eng, false));
	this->pbsFreq->SetNoBGColor(true);
	this->pbsFreq->SetDockType(UI::GUIControl::DOCK_FILL);

	this->currSample = 0;
	this->totalSample = (UInt64)this->audSrc->GetSampleCount();

	this->HandleSizeChanged(OnSizeChanged, this);
	this->UpdateImages();
}

SSWR::AVIRead::AVIRAudioViewerForm::~AVIRAudioViewerForm()
{
	if (this->sampleImg)
	{
		this->eng->DeleteImage(this->sampleImg);
	}
	if (this->fftImg)
	{
		this->eng->DeleteImage(this->fftImg);
	}
	DEL_CLASS(this->format);
}

void SSWR::AVIRead::AVIRAudioViewerForm::EventMenuClicked(UInt16 cmdId)
{
	UOSInt w;
	UOSInt h;
	switch (cmdId)
	{
	case MNU_NEXT_SAMPLE:
		if (this->totalSample != (UOSInt)-1)
		{
			if (this->currSample + 1 < this->totalSample)
			{
				this->currSample++;
				this->UpdateImages();
			}
		}
		break;
	case MNU_PREV_SAMPLE:
		if (this->currSample > 0)
		{
			this->currSample--;
			this->UpdateImages();
		}
		break;
	case MNU_NEXT_PAGE:
		if (this->totalSample != (UOSInt)-1)
		{
			if (this->currSample + 1 < this->totalSample)
			{
				this->pbsSample->GetSizeP(&w, &h);
				this->currSample += w;
				if (this->currSample >= this->totalSample)
					this->currSample = this->totalSample - 1;
				this->UpdateImages();
			}
		}
		break;
	case MNU_PREV_PAGE:
		if (this->currSample > 0)
		{
			this->pbsSample->GetSizeP(&w, &h);
			this->currSample -= w;
			if (this->currSample < 0)
				this->currSample = 0;
			this->UpdateImages();
		}
		break;
	case MNU_BEGIN:
		if (this->currSample > 0)
		{
			this->currSample = 0;
			this->UpdateImages();
		}
		break;
	case MNU_END:
		if (this->totalSample != (UOSInt)-1)
		{
			if (this->currSample + 1 != this->totalSample)
			{
				this->currSample = this->totalSample - 1;
				this->UpdateImages();
			}
		}
		break;
	}
}

void SSWR::AVIRead::AVIRAudioViewerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
