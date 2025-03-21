#include "Stdafx.h"
#include "Data/ByteBuffer.h"
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

void __stdcall SSWR::AVIRead::AVIRAudioViewerForm::OnSizeChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRAudioViewerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioViewerForm>();
	me->UpdateImages();
}

void SSWR::AVIRead::AVIRAudioViewerForm::UpdateImages()
{
	NN<Media::DrawImage> gimg;
	NN<Media::DrawImage> img;
	Math::Size2D<UOSInt> sz = this->pbsSample->GetSizeP();
	if (sz.x <= 0 || sz.y <= 0)
		return;
	if (this->totalSample == 0 || this->totalSample == (UOSInt)-1)
	{
		NN<Media::DrawBrush> b;
		NN<Media::DrawFont> f;
		if (this->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(gimg))
		{
			b = gimg->NewBrushARGB(0xff000000);
			gimg->DrawRect(Math::Coord2DDbl(0, 0), sz.ToDouble(), 0, b);
			gimg->DelBrush(b);
			b = gimg->NewBrushARGB(0xffffffff);
			f = gimg->NewFontPt(CSTR("Arial"), 10, Media::DrawEngine::DFS_ANTIALIAS, 0);
			gimg->DrawString(Math::Coord2DDbl(0, 0), CSTR("Format not supported"), f, b);
			gimg->DelFont(f);
			gimg->DelBrush(b);
			this->pbsSample->SetImageDImg(gimg);
			if (this->sampleImg.SetTo(img))
			{
				this->eng->DeleteImage(img);
			}
			this->sampleImg = gimg;
		}
	}
	else
	{
		NN<Media::DrawBrush> b;
		NN<Media::DrawPen> p;
		UOSInt i;
		UOSInt j;
		UInt32 currCh;
		UOSInt currY;
		UOSInt channelH;
		UOSInt lastY;
		UOSInt thisY;
		UOSInt align = this->format.align;
		if (this->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(gimg))
		{
			Data::ByteBuffer buff(this->format.align * sz.x);
			i = this->audSrc->ReadSample(currSample, sz.x, buff);
			
			b = gimg->NewBrushARGB(0xff000000);
			gimg->DrawRect(Math::Coord2DDbl(0, 0), sz.ToDouble(), 0, b);
			gimg->DelBrush(b);

			channelH = sz.y / this->format.nChannels / 2;
			if (channelH > 0)
			{
				p = gimg->NewPenARGB(0xffffffff, 1, 0, 0);
				if (this->format.bitpersample == 16)
				{
					if (i != sz.x)
					{
						i = i * align;
						j = sz.x * align;
						while (i < j)
						{
							*(Int16*)&buff[i] = 0;
							i += 2;
						}
					}
					currCh = this->format.nChannels;
					while (currCh-- > 0)
					{
						currY = sz.y * currCh / this->format.nChannels;
						lastY = currY + channelH;
						i = 0;
						j = 2 * currCh;
						while (i < sz.x)
						{
							thisY = currY + channelH + ((ReadUInt16(&buff[j]) * channelH) >> 15);
							gimg->DrawLine(UOSInt2Double(i - 1), UOSInt2Double(lastY), UOSInt2Double(i), UOSInt2Double(thisY), p);
							
							lastY = thisY;
							i++;
							j += align;
						}
					}
				}
				else if (this->format.bitpersample == 8)
				{
				}
				gimg->DelPen(p);
			}

			this->pbsSample->SetImageDImg(gimg);
			if (this->sampleImg.SetTo(img))
			{
				this->eng->DeleteImage(img);
			}
			this->sampleImg = gimg;
		}
	}
	this->UpdateFreqImage();
}

void SSWR::AVIRead::AVIRAudioViewerForm::UpdateFreqImage()
{
	NN<Media::DrawImage> gimg;
	NN<Media::DrawImage> img;
	Math::Size2D<UOSInt> sz = this->pbsFreq->GetSizeP();
	if (sz.x <= 0 || sz.y <= 0)
		return;
	if (this->totalSample == 0 || this->totalSample == (UOSInt)-1)
	{
		NN<Media::DrawBrush> b;
		NN<Media::DrawFont> f;
		if (this->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(gimg))
		{
			b = gimg->NewBrushARGB(0xff000000);
			gimg->DrawRect(Math::Coord2DDbl(0, 0), sz.ToDouble(), 0, b);
			gimg->DelBrush(b);
			b = gimg->NewBrushARGB(0xffffffff);
			f = gimg->NewFontPt(CSTR("Arial"), 10, Media::DrawEngine::DFS_ANTIALIAS, 0);
			gimg->DrawString(Math::Coord2DDbl(0, 0), CSTR("Format not supported"), f, b);
			gimg->DelFont(f);
			gimg->DelBrush(b);
			this->pbsSample->SetImageDImg(gimg);
			if (this->fftImg.SetTo(img))
			{
				this->eng->DeleteImage(img);
			}
			this->fftImg = gimg;
		}
	}
	else
	{
		NN<Media::DrawBrush> b;
		NN<Media::DrawPen> p;
		UOSInt i;
		UOSInt j;
		Double lastX;
		Double lastY;
		Double thisX;
		Double thisY;
//		OSInt align = this->format->align;
		if (this->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(gimg))
		{

			Data::ByteBuffer buff(this->format.align * (FFTSAMPLE + FFTAVG - 1));
			i = this->audSrc->ReadSample(currSample - FFTSAMPLE + 1, FFTSAMPLE + FFTAVG - 1, buff);
			
			b = gimg->NewBrushARGB(0xff000000);
			gimg->DrawRect(Math::Coord2DDbl(0, 0), sz.ToDouble(), 0, b);
			gimg->DelBrush(b);

			UnsafeArray<Double> freqData;
			freqData = MemAllocArr(Double, FFTSAMPLE);
			i = 0;
			while (i < this->format.nChannels)
			{
				Math::FFT::ForwardBits(buff.Arr() + i * (UOSInt)(this->format.bitpersample >> 3), freqData, FFTSAMPLE, FFTAVG, this->format.bitpersample, this->format.nChannels, Math::FFT::WT_BLACKMANN_HARRIS, 1.0);

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

					thisX = UOSInt2Double(j * sz.x) / (Double)(FFTSAMPLE / 2 - 1);
					thisY = -Math_Log10(rVal / (FFTSAMPLE * 0.5)) * UOSInt2Double(sz.y) / 7.0;
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
			MemFreeArr(freqData);

			this->pbsFreq->SetImageDImg(gimg);
			if (this->fftImg.SetTo(img))
			{
				this->eng->DeleteImage(img);
			}
			this->fftImg = gimg;
		}
	}
}

SSWR::AVIRead::AVIRAudioViewerForm::AVIRAudioViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::AudioSource> audSrc) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	this->SetFont(0, 0, 8.25, false);
	sptr = Text::StrConcatC(sbuff, UTF8STRC("Audio Viewer - "));
	sptr = audSrc->GetSourceName(sptr).Or(sptr);
	this->SetText(CSTRP(sbuff, sptr));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->audSrc = audSrc;
	this->eng = core->GetDrawEngine();
	this->sampleImg = 0;
	this->fftImg = 0;
	this->audSrc->GetFormat(this->format);

	NN<UI::GUIMenu> mnu;
	NEW_CLASSNN(this->mnu, UI::GUIMainMenu());
	mnu = this->mnu->AddSubMenu(CSTR("&Navigate"));
	mnu->AddItem(CSTR("&Next Sample"), MNU_NEXT_SAMPLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_RIGHT);
	mnu->AddItem(CSTR("&Prev Sample"), MNU_PREV_SAMPLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_LEFT);
	mnu->AddItem(CSTR("N&ext Page"), MNU_NEXT_PAGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_PAGEDOWN);
	mnu->AddItem(CSTR("P&rev Page"), MNU_PREV_PAGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_PAGEUP);
	mnu->AddItem(CSTR("Go to &Beginning"), MNU_BEGIN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_HOME);
	mnu->AddItem(CSTR("Go to E&nd"), MNU_END, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_END);
	this->SetMenu(this->mnu);

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpSample = this->tcMain->AddTabPage(CSTR("Sample"));
	this->pbsSample = ui->NewPictureBoxSimple(this->tpSample, this->eng, false);
	this->pbsSample->SetNoBGColor(true);
	this->pbsSample->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpFreq = this->tcMain->AddTabPage(CSTR("Frequency"));
	this->pbsFreq = ui->NewPictureBoxSimple(this->tpFreq, this->eng, false);
	this->pbsFreq->SetNoBGColor(true);
	this->pbsFreq->SetDockType(UI::GUIControl::DOCK_FILL);

	this->currSample = 0;
	this->totalSample = (UInt64)this->audSrc->GetSampleCount();

	this->HandleSizeChanged(OnSizeChanged, this);
	this->UpdateImages();
}

SSWR::AVIRead::AVIRAudioViewerForm::~AVIRAudioViewerForm()
{
	NN<Media::DrawImage> img;
	if (this->sampleImg.SetTo(img))
	{
		this->eng->DeleteImage(img);
	}
	if (this->fftImg.SetTo(img))
	{
		this->eng->DeleteImage(img);
	}
}

void SSWR::AVIRead::AVIRAudioViewerForm::EventMenuClicked(UInt16 cmdId)
{
	Math::Size2D<UOSInt> sz;
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
				sz = this->pbsSample->GetSizeP();
				this->currSample += sz.x;
				if (this->currSample >= this->totalSample)
					this->currSample = this->totalSample - 1;
				this->UpdateImages();
			}
		}
		break;
	case MNU_PREV_PAGE:
		if (this->currSample > 0)
		{
			sz = this->pbsSample->GetSizeP();
			this->currSample -= sz.x;
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
