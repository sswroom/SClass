#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "Media/NullRenderer.h"
#include "Media/SilentSource.h"
#include "Media/WaveInSource.h"
#include "SSWR/AVIRead/AVIRAudioFilterForm.h"
#include "SSWR/AVIRead/AVIRSetAudioForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"

#define FFTSAMPLE 1024
#define FFTAVG 1

typedef enum
{
	MNU_SET_DEVICE = 101
} MenuEvents;

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->audSrc == 0)
	{
		Text::StringBuilderUTF8 sb;
		UInt32 buffSize;
		UInt32 frequency;
		UInt16 nChannel;
		UInt16 bitCount;
		UInt32 dtmfMS;
		sb.ClearStr();
		me->txtBuffSize->GetText(sb);
		if (!sb.ToUInt32(buffSize))
		{
			me->ui->ShowMsgOK(CSTR("Error found in buffer size"), CSTR("Error"), me);
			return;
		}
		sb.ClearStr();
		me->txtFrequency->GetText(sb);
		if (!sb.ToUInt32(frequency))
		{
			me->ui->ShowMsgOK(CSTR("Error found in sampling rate"), CSTR("Error"), me);
			return;
		}
		sb.ClearStr();
		me->txtChannel->GetText(sb);
		if (!sb.ToUInt16(nChannel))
		{
			me->ui->ShowMsgOK(CSTR("Error found in no. of channels"), CSTR("Error"), me);
			return;
		}
		sb.ClearStr();
		me->txtBitCount->GetText(sb);
		if (!sb.ToUInt16(bitCount))
		{
			me->ui->ShowMsgOK(CSTR("Error found in bit per sample"), CSTR("Error"), me);
			return;
		}
		sb.ClearStr();
		me->txtDTMFInterval->GetText(sb);
		if (!sb.ToUInt32(dtmfMS))
		{
			me->ui->ShowMsgOK(CSTR("Error found in DTMF decode interval"), CSTR("Error"), me);
			return;
		}
		if (buffSize < 10 || buffSize > 1000)
		{
			me->ui->ShowMsgOK(CSTR("Buffer size out of range"), CSTR("Error"), me);
			return;
		}
		if (frequency < 8000 || frequency > 192000)
		{
			me->ui->ShowMsgOK(CSTR("Sampling rate out of range"), CSTR("Error"), me);
			return;
		}
		if (nChannel <= 0 || nChannel > 10)
		{
			me->ui->ShowMsgOK(CSTR("No. of channels out of range"), CSTR("Error"), me);
			return;
		}
		if (bitCount != 8 && bitCount != 16 && bitCount != 24 && bitCount != 32)
		{
			me->ui->ShowMsgOK(CSTR("Bit per sample out of range"), CSTR("Error"), me);
			return;
		}
		if (dtmfMS < 1 || dtmfMS > 1000)
		{
			me->ui->ShowMsgOK(CSTR("DTMF decode interval out of range"), CSTR("Error"), me);
			return;
		}
		if (me->radInputSilent->IsSelected())
		{
			NEW_CLASS(me->audSrc, Media::SilentSource(frequency, nChannel, bitCount, CSTR("Silent"), (UInt64)-1));
		}
		else if (me->radInputWaveIn->IsSelected())
		{
			Media::WaveInSource *wiSrc;
			UTF8Char sbuff[512];
			Media::WaveInSource::GetDeviceName(sbuff, 0);
			NEW_CLASS(wiSrc, Media::WaveInSource((UInt32)0, frequency, bitCount, nChannel));
			if (wiSrc->IsError())
			{
				DEL_CLASS(wiSrc);
			}
			else
			{
				me->audSrc = wiSrc;
			}
		}
		NotNullPtr<Media::IAudioSource> audSrc;
		if (audSrc.Set(me->audSrc))
		{
			NotNullPtr<Media::AudioFilter::AudioAmplifier> audioAmp;
			NotNullPtr<Media::AudioFilter::DTMFGenerator> dtmfGen;
			NotNullPtr<Media::AudioFilter::FileMixFilter> fileMix;
			NotNullPtr<Media::AudioFilter::SoundGenerator> sndGen;
			NotNullPtr<Media::AudioFilter::AudioSweepFilter> sweepFilter;
			NotNullPtr<Media::AudioFilter::DynamicVolBooster> volBooster;
			NotNullPtr<Media::AudioFilter::DTMFDecoder> dtmfDec;
			NotNullPtr<Media::AudioFilter::AudioSampleRipper> audioRipper;
			NotNullPtr<Media::AudioFilter::AudioLevelMeter> audioLevel;
			NotNullPtr<Media::AudioFilter::AudioCaptureFilter> audioCapture;
			NEW_CLASSNN(audioAmp, Media::AudioFilter::AudioAmplifier(audSrc));
			NEW_CLASSNN(dtmfGen, Media::AudioFilter::DTMFGenerator(audioAmp));
			NEW_CLASSNN(fileMix, Media::AudioFilter::FileMixFilter(dtmfGen, me->core->GetParserList()));
			NEW_CLASSNN(sndGen, Media::AudioFilter::SoundGenerator(fileMix));
			NEW_CLASSNN(sweepFilter, Media::AudioFilter::AudioSweepFilter(sndGen));
			NEW_CLASSNN(volBooster, Media::AudioFilter::DynamicVolBooster(sweepFilter));
			NEW_CLASSNN(dtmfDec, Media::AudioFilter::DTMFDecoder(volBooster, dtmfMS * frequency / 1000));
			NEW_CLASSNN(audioRipper, Media::AudioFilter::AudioSampleRipper(dtmfDec, FFTSAMPLE + FFTAVG - 1));
			NEW_CLASSNN(audioLevel, Media::AudioFilter::AudioLevelMeter(audioRipper));
			NEW_CLASSNN(audioCapture, Media::AudioFilter::AudioCaptureFilter(audioLevel));
			me->audioAmp = audioAmp.Ptr();
			me->dtmfGen = dtmfGen.Ptr();
			me->fileMix = fileMix.Ptr();
			me->sndGen = sndGen.Ptr();
			me->sweepFilter = sweepFilter.Ptr();
			me->volBooster = volBooster.Ptr();
			me->dtmfDec = dtmfDec.Ptr();
			me->audioRipper = audioRipper.Ptr();
			me->audioLevel = audioLevel.Ptr();
			me->audioCapture = audioCapture.Ptr();
			me->bitCount = bitCount;
			me->nChannels = nChannel;
			me->sampleBuff = MemAlloc(UInt8, (FFTSAMPLE + FFTAVG - 1) * (UOSInt)me->nChannels * (UOSInt)(me->bitCount >> 3));
			me->volBooster->SetEnabled(me->chkVolBoost->IsChecked());
			me->volBooster->SetBGLevel(Math_Pow(10, OSInt2Double((OSInt)me->tbVolBoostBG->GetPos() - 192) / 20.0));
			me->dtmfGen->SetVolume(Math_Pow(10, OSInt2Double((OSInt)me->tbDTMFVol->GetPos() - 960) / 20.0));
			me->audioAmp->SetLevel(UOSInt2Double(me->tbAmplifierVol->GetPos()) * 0.01);
			if (me->radOutputDevice->IsSelected())
			{
				me->audRender = me->core->BindAudio(me->audioCapture);
				me->audRenderType = 0;
			}
			else if (me->radOutputSilent->IsSelected())
			{
				NEW_CLASS(me->audRender, Media::NullRenderer());
				me->audRender->BindAudio(me->audioCapture);
				me->audRenderType = 1;
			}
			if (me->audRender)
			{
				me->audRender->SetBufferTime(buffSize);
				me->audRender->AudioInit(&me->clk);
				me->audRender->Start();

				me->dtmfDec->HandleToneChange(OnDTMFToneChange, me);

				me->radInputSilent->SetEnabled(false);
				me->radInputWaveIn->SetEnabled(false);
				me->radOutputDevice->SetEnabled(false);
				me->radOutputSilent->SetEnabled(false);
				me->txtFrequency->SetReadOnly(true);
				me->txtChannel->SetReadOnly(true);
				me->txtBitCount->SetReadOnly(true);
				me->txtBuffSize->SetReadOnly(true);
				me->txtDTMFInterval->SetReadOnly(true);
				me->rlcVolLevel->ClearChart();
			}
			else
			{
				DEL_CLASS(me->audioCapture);
				DEL_CLASS(me->audioLevel);
				DEL_CLASS(me->audioRipper);
				DEL_CLASS(me->dtmfDec);
				DEL_CLASS(me->volBooster);
				DEL_CLASS(me->sweepFilter);
				DEL_CLASS(me->sndGen);
				DEL_CLASS(me->fileMix);
				DEL_CLASS(me->dtmfGen);
				DEL_CLASS(me->audioAmp);
				DEL_CLASS(me->audSrc);
				MemFree(me->sampleBuff);
				me->audSrc = 0;
				me->volBooster = 0;
				me->dtmfGen = 0;
				me->fileMix = 0;
				me->sndGen = 0;
				me->dtmfDec = 0;
				me->audioLevel = 0;
				me->audioRipper = 0;
				me->audioAmp = 0;
				me->sampleBuff = 0;
			}
		}
	}
	else
	{
		me->StopAudio();
		me->radInputSilent->SetEnabled(true);
		me->radInputWaveIn->SetEnabled(true);
		me->radOutputDevice->SetEnabled(true);
		me->radOutputSilent->SetEnabled(true);
		me->txtFrequency->SetReadOnly(false);
		me->txtChannel->SetReadOnly(false);
		me->txtBitCount->SetReadOnly(false);
		me->txtBuffSize->SetReadOnly(false);
		me->txtDTMFInterval->SetReadOnly(false);
		me->txtFileMix->SetText(CSTR(""));
		me->dtmfSb.ClearStr();
		me->dtmfMod = true;
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnVolBoostChg(void *userObj, Bool newState)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->volBooster)
	{
		me->volBooster->SetEnabled(newState);
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnVolBoostBGChg(void *userObj, UOSInt scrollPos)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	if (me->volBooster)
	{
		me->volBooster->SetBGLevel(Math_Pow(10, OSInt2Double((OSInt)scrollPos - 192) / 20.0));
	}
	sptr = Text::StrConcatC(Text::StrOSInt(sbuff, (OSInt)scrollPos - 192), UTF8STRC("dB"));
	me->lblVolBoostBGVol->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFClearClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	Sync::MutexUsage mutUsage(me->dtmfMut);
	me->dtmfSb.ClearStr();
	me->dtmfMod = true;
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF1UpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('1');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF2UpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('2');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF3UpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('3');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF4UpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('4');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF5UpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('5');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF6UpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('6');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF7UpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('7');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF8UpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('8');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF9UpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('9');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFStarUpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('*');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF0UpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('0');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFSharpUpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('#');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFAUpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('A');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFBUpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('B');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFCUpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('C');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFDUpDown(void *userObj, Bool isDown)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen)
	{
		if (isDown)
		{
			me->dtmfGen->SetTone('D');
		}
		else
		{
			me->dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFVolChg(void *userObj, UOSInt scrollPos)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	Text::StringBuilderUTF8 sb;
	sb.AppendDouble(OSInt2Double((OSInt)scrollPos - 960) * 0.1);
	sb.AppendC(UTF8STRC("dB"));
	me->lblDTMFVolV->SetText(sb.ToCString());
	if (me->dtmfGen)
	{
		me->dtmfGen->SetVolume(Math_Pow(10, OSInt2Double((OSInt)scrollPos - 960) / 200.0));
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFTonesClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->dtmfGen == 0)
	{
		return;
	}
	Text::StringBuilderUTF8 sb;
	UInt32 signalTime;
	UInt32 breakTime;
	Double vol;
	vol = Math_Pow(10, OSInt2Double((OSInt)me->tbDTMFTonesVol->GetPos() - 960) / 200.0);
	me->txtDTMFSignalTime->GetText(sb);
	if (!sb.ToUInt32(signalTime))
	{
		me->ui->ShowMsgOK(CSTR("Signal Time is not valid"), CSTR("Generate Tones"), me);
		return;
	}
	if (signalTime <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Signal Time is out of range"), CSTR("Generate Tones"), me);
		return;
	}
	sb.ClearStr();
	me->txtDTMFBreakTime->GetText(sb);
	if (!sb.ToUInt32(breakTime))
	{
		me->ui->ShowMsgOK(CSTR("Break Time is not valid"), CSTR("Generate Tones"), me);
		return;
	}
	if (breakTime <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Break Time is out of range"), CSTR("Generate Tones"), me);
		return;
	}
	sb.ClearStr();
	me->txtDTMFTones->GetText(sb);
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter tones"), CSTR("Generate Tones"), me);
		return;
	}
	if (!me->dtmfGen->GenTones(signalTime, breakTime, vol, sb.ToString()))
	{
		me->ui->ShowMsgOK(CSTR("Error in generating tones"), CSTR("Generate Tones"), me);
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnLevelTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	if (me->audioLevel)
	{
		Double v[2];
		v[0] = Math_Log10(me->audioLevel->GetLevel(0)) * 20;
		v[1] = Math_Log10(me->audioLevel->GetLevel(1)) * 20;
		me->rlcVolLevel->AddSample(v);
	}
	if (me->audioRipper)
	{
		if (me->audioRipper->IsChanged())
		{
			Math::Size2D<UOSInt> sz;
			UOSInt i;
			UOSInt j;
			UOSInt k;
			UOSInt sampleInt;
			Double lastX;
			Double lastY;
			Double thisX;
			Double thisY;
			NotNullPtr<Media::DrawImage> img;
			NotNullPtr<Media::DrawImage> dimg;
			NotNullPtr<Media::DrawBrush> b;
			NotNullPtr<Media::DrawPen> p;
			me->audioRipper->GetSamples(me->sampleBuff);
			sz = me->pbsSample->GetSizeP();
			if (sz.x <= 0 || sz.y <= 0)
			{
			}
			else if (img.Set(me->eng->CreateImage32(sz, Media::AT_NO_ALPHA)))
			{
				b = img->NewBrushARGB(0xffffffff);
				img->DrawRect(Math::Coord2DDbl(0, 0), sz.ToDouble(), 0, b);
				img->DelBrush(b);
				sampleInt = me->nChannels * (UOSInt)me->bitCount >> 3;
				if (me->bitCount == 16)
				{
					i = 0;
					while (i < me->nChannels)
					{
						lastX = -1;
						lastY = -1;
						if (i == 0)
						{
							p = img->NewPenARGB(0xffff0000, 1, 0, 0);
						}
						else if (i == 1)
						{
							p = img->NewPenARGB(0xff0000ff, 1, 0, 0);
						}
						else if (i == 2)
						{
							p = img->NewPenARGB(0xff00ff00, 1, 0, 0);
						}
						else
						{
							Data::RandomOS rand;
							p = img->NewPenARGB(rand.NextInt30() | 0xff000000, 1, 0, 0);
						}
						k = i << 1;
						j = 0;
						while (j < FFTSAMPLE)
						{
							thisX = UOSInt2Double(j * sz.x) / (Double)(FFTSAMPLE - 1);
							thisY = (ReadInt16(&me->sampleBuff[k]) + 32768.0) * UOSInt2Double(sz.y) / 65536.0;
							if (lastX >= 0)
							{
								img->DrawLine(lastX, lastY, thisX, thisY, p);
							}
							lastX = thisX;
							lastY = thisY;
							k += sampleInt;
							j++;
						}
						img->DelPen(p);
						i++;
					}
				}
				else if (me->bitCount == 8)
				{
					i = 0;
					while (i < me->nChannels)
					{
						lastX = -1;
						lastY = -1;
						if (i == 0)
						{
							p = img->NewPenARGB(0xffff0000, 1, 0, 0);
						}
						else if (i == 1)
						{
							p = img->NewPenARGB(0xff0000ff, 1, 0, 0);
						}
						else if (i == 2)
						{
							p = img->NewPenARGB(0xff00ff00, 1, 0, 0);
						}
						else
						{
							Data::RandomOS rand;
							p = img->NewPenARGB(rand.NextInt30() | 0xff000000, 1, 0, 0);
						}
						k = i;
						j = 0;
						while (j < FFTSAMPLE)
						{
							thisX = UOSInt2Double(j * sz.x) / (Double)(FFTSAMPLE - 1);
							thisY = UOSInt2Double(me->sampleBuff[k] * sz.y) / 256.0;
							if (lastX >= 0)
							{
								img->DrawLine(lastX, lastY, thisX, thisY, p);
							}
							lastX = thisX;
							lastY = thisY;
							k += sampleInt;
							j++;
						}
						img->DelPen(p);
						i++;
					}
				}
				me->pbsSample->SetImageDImg(img.Ptr());
				if (dimg.Set(me->sampleImg))
				{
					me->eng->DeleteImage(dimg);
				}
				me->sampleImg = img.Ptr();
			}

			sz = me->pbsFFT->GetSizeP();
			if (sz.x <= 0 || sz.y <= 0)
			{
			}
			else if (img.Set(me->eng->CreateImage32(sz, Media::AT_NO_ALPHA)))
			{
				b = img->NewBrushARGB(0xffffffff);
				img->DrawRect(Math::Coord2DDbl(0, 0), sz.ToDouble(), 0, b);
				img->DelBrush(b);
				sampleInt = me->nChannels * (UOSInt)me->bitCount >> 3;

				Double *data = MemAlloc(Double, FFTSAMPLE);
				i = 0;
				while (i < me->nChannels)
				{
					Math::FFTCalc::SampleType st = Math::FFTCalc::ST_I16;
					Bool err = false;
					if (me->bitCount == 16)
					{
						st = Math::FFTCalc::ST_I16;
					}
					else if (me->bitCount == 24)
					{
						st = Math::FFTCalc::ST_I24;
					}
					else
					{
						err = true;
					}
					if (!err)
					{
						me->fft.ForwardBits(me->sampleBuff + ((UOSInt)(me->bitCount >> 3) * i), data, st, me->nChannels, 1.0);
					}
					if (i == 0)
					{
						p = img->NewPenARGB(0xffff0000, 1, 0, 0);
					}
					else if (i == 1)
					{
						p = img->NewPenARGB(0xff0000ff, 1, 0, 0);
					}
					else if (i == 2)
					{
						p = img->NewPenARGB(0xff00ff00, 1, 0, 0);
					}
					else
					{
						Data::RandomOS rand;
						p = img->NewPenARGB(rand.NextInt30() | 0xff000000, 1, 0, 0);
					}
					Double rVal;
					lastX = -1;
					lastY = -1;
					j = 0;
					while (j < (FFTSAMPLE / 2))
					{
						rVal = data[j];

						if (rVal < 0)
							rVal = -rVal;
						thisX = UOSInt2Double(j * sz.x) / (Double)(FFTSAMPLE / 2 - 1);
						thisY = -Math_Log10(rVal / (FFTSAMPLE * 0.5)) * UOSInt2Double(sz.y) / 7.0;
						if (lastX >= 0)
						{
							img->DrawLine(lastX, lastY, thisX, thisY, p);
						}
						lastX = thisX;
						lastY = thisY;
						j++;
					}

					img->DelPen(p);
					i++;
				}
				MemFree(data);

				me->pbsFFT->SetImageDImg(img.Ptr());
				if (dimg.Set(me->fftImg))
				{
					me->eng->DeleteImage(dimg);
				}
				me->fftImg = img.Ptr();
			}
		}
	}
	if (me->dtmfMod)
	{
		me->dtmfMod = false;
		Sync::MutexUsage mutUsage(me->dtmfMut);
		me->txtDTMFDecode->SetText(me->dtmfSb.ToCString());
		mutUsage.EndUse();
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFToneChange(void *userObj, WChar tone)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm*)userObj;
	if (tone == 0)
	{

	}
	else
	{
		Sync::MutexUsage mutUsage(me->dtmfMut);
		me->dtmfSb.AppendChar(tone, 1);
		me->dtmfMod = true;
		mutUsage.EndUse();
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnFileMixClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm*)userObj;
	if (me->fileMix)
	{
		NotNullPtr<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"AudioFilterFileMix", false);
		dlg->AddFilter(CSTR("*.wav"), CSTR("Wave file"));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			if (me->fileMix->LoadFile(dlg->GetFileName()))
			{
				me->txtFileMix->SetText(dlg->GetFileName()->ToCString());
			}
		}
		dlg.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnFileMixStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm*)userObj;
	if (me->fileMix)
	{
		me->fileMix->StartMix();
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnFileMixStopClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm*)userObj;
	if (me->fileMix)
	{
		me->fileMix->StopMix();
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnCaptureStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm*)userObj;
	if (me->audioCapture)
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		UOSInt i;
		Data::DateTime dt;
		sptr = IO::Path::GetProcessFileName(sbuff);
		i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
		dt.SetCurrTimeUTC();
		sptr = Text::StrConcatC(Text::StrInt64(&sbuff[i + 1], dt.ToTicks()), UTF8STRC(".wav"));
		me->audioCapture->StartCapture(CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnCaptureStopClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm*)userObj;
	if (me->audioCapture)
	{
		me->audioCapture->StopCapture();
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnSoundGenBellClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm*)userObj;
	if (me->sndGen)
	{
		me->sndGen->GenSound(Media::AudioFilter::SoundGen::ISoundGen::ST_BELL, 1.0);
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnSweepVolChg(void *userObj, UOSInt scrollPos)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	Text::StringBuilderUTF8 sb;
	sb.AppendDouble(OSInt2Double((OSInt)scrollPos - 960) * 0.1);
	sb.AppendC(UTF8STRC("dB"));
	me->lblSweepVolV->SetText(sb.ToCString());
	if (me->sweepFilter)
	{
		me->sweepFilter->SetVolume(Math_Pow(10, OSInt2Double((OSInt)scrollPos - 960) / 200.0));
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnSweepStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	Text::StringBuilderUTF8 sb;
	Double startFreq;
	Double endFreq;
	UInt32 timeSeconds;
	sb.ClearStr();
	me->txtSweepStartFreq->GetText(sb);
	if (!Text::StrToDouble(sb.ToString(), startFreq))
	{
		me->ui->ShowMsgOK(CSTR("Error in start frequency"), CSTR("Error"), me);
		return;
	}
	sb.ClearStr();
	me->txtSweepEndFreq->GetText(sb);
	if (!Text::StrToDouble(sb.ToString(), endFreq))
	{
		me->ui->ShowMsgOK(CSTR("Error in end frequency"), CSTR("Error"), me);
		return;
	}
	sb.ClearStr();
	me->txtSweepDur->GetText(sb);
	if (!sb.ToUInt32(timeSeconds))
	{
		me->ui->ShowMsgOK(CSTR("Error in duration"), CSTR("Error"), me);
		return;
	}
	me->sweepFilter->StartSweep(startFreq, endFreq, timeSeconds);
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnAmplifierVolChg(void *userObj, UOSInt scrollPos)
{
	SSWR::AVIRead::AVIRAudioFilterForm *me = (SSWR::AVIRead::AVIRAudioFilterForm *)userObj;
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(scrollPos);
	sb.AppendC(UTF8STRC("%"));
	me->lblAmplifierVolV->SetText(sb.ToCString());
	if (me->audioAmp)
	{
		me->audioAmp->SetLevel(UOSInt2Double(scrollPos) * 0.01);
	}
}

void SSWR::AVIRead::AVIRAudioFilterForm::StopAudio()
{
	if (this->audSrc)
	{
		if (this->audRenderType == 1)
		{
			SDEL_CLASS(this->audRender);
		}
		else
		{
			this->core->BindAudio(0);
			this->audRender = 0;
		}
		DEL_CLASS(this->audSrc);
		DEL_CLASS(this->audioAmp);
		DEL_CLASS(this->audioLevel);
		DEL_CLASS(this->audioRipper);
		DEL_CLASS(this->dtmfDec);
		DEL_CLASS(this->dtmfGen);
		DEL_CLASS(this->sndGen);
		DEL_CLASS(this->fileMix);
		DEL_CLASS(this->volBooster);
		DEL_CLASS(this->audioCapture);
		MemFree(this->sampleBuff);
		this->audSrc = 0;
		this->audioAmp = 0;
		this->audioLevel = 0;
		this->audioRipper = 0;
		this->dtmfDec = 0;
		this->dtmfGen = 0;
		this->fileMix = 0;
		this->sndGen = 0;
		this->volBooster = 0;
		this->audioCapture = 0;
		this->sampleBuff = 0;
	}
}

SSWR::AVIRead::AVIRAudioFilterForm::AVIRAudioFilterForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Bool showMenu) : UI::GUIForm(parent, 1024, 768, ui), fft(FFTSAMPLE, Math::FFTCalc::WT_BLACKMANN_HARRIS)
{
	this->SetText(CSTR("Audio Filter"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->audSrc = 0;
	this->audioCapture = 0;
	this->audioLevel = 0;
	this->audioRipper = 0;
	this->dtmfDec = 0;
	this->volBooster = 0;
	this->dtmfGen = 0;
	this->fileMix = 0;
	this->audRender = 0;
	this->audioAmp = 0;
	this->sampleBuff = 0;
	this->eng = this->core->GetDrawEngine();
	this->sampleImg = 0;
	this->fftImg = 0;
	this->dtmfMod = false;

	NEW_CLASSNN(this->pnlInput, UI::GUIPanel(ui, *this));
	this->pnlInput->SetRect(0, 0, 100, 168, false);
	this->pnlInput->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->pbsFFT, UI::GUIPictureBoxSimple(ui, *this, this->eng, false));
	this->pbsFFT->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pbsFFT->SetNoBGColor(true);
	this->vspSample = ui->NewVSplitter(*this, 3, true);
	this->tcFilter = ui->NewTabControl(*this);
	this->tcFilter->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASSNN(this->pnlAudioSource, UI::GUIPanel(ui, this->pnlInput));
	this->pnlAudioSource->SetRect(0, 0, 400, 23, false);
	this->lblAudioSource = ui->NewLabel(this->pnlAudioSource, CSTR("Audio Source"));
	this->lblAudioSource->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->radInputWaveIn, UI::GUIRadioButton(ui, this->pnlAudioSource, CSTR("WaveIn"), true));
	this->radInputWaveIn->SetRect(100, 0, 100, 23, false);
	NEW_CLASS(this->radInputSilent, UI::GUIRadioButton(ui, this->pnlAudioSource, CSTR("Silent"), false));
	this->radInputSilent->SetRect(200, 0, 100, 23, false);
	NEW_CLASSNN(this->pnlAudioOutput, UI::GUIPanel(ui, this->pnlInput));
	this->pnlAudioOutput->SetRect(0, 24, 400, 23, false);
	this->lblAudioOutput = ui->NewLabel(this->pnlAudioOutput, CSTR("Audio Output"));
	this->lblAudioOutput->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->radOutputDevice, UI::GUIRadioButton(ui, this->pnlAudioOutput, CSTR("Device"), true));
	this->radOutputDevice->SetRect(100, 0, 100, 23, false);
	NEW_CLASS(this->radOutputSilent, UI::GUIRadioButton(ui, this->pnlAudioOutput, CSTR("Silent"), false));
	this->radOutputSilent->SetRect(200, 0, 100, 23, false);
	this->lblBuffSize = ui->NewLabel(this->pnlInput, CSTR("Buffer Size(ms)"));
	this->lblBuffSize->SetRect(0, 48, 100, 23, false);
	this->txtBuffSize = ui->NewTextBox(this->pnlInput, CSTR("30"));
	this->txtBuffSize->SetRect(100, 48, 100, 23, false);
	this->lblFrequency = ui->NewLabel(this->pnlInput, CSTR("Sampling Rate"));
	this->lblFrequency->SetRect(0, 72, 100, 23, false);
	this->txtFrequency = ui->NewTextBox(this->pnlInput, CSTR("48000"));
	this->txtFrequency->SetRect(100, 72, 100, 23, false);
	this->lblChannel = ui->NewLabel(this->pnlInput, CSTR("No. of Channels"));
	this->lblChannel->SetRect(0, 96, 100, 23, false);
	this->txtChannel = ui->NewTextBox(this->pnlInput, CSTR("2"));
	this->txtChannel->SetRect(100, 96, 50, 23, false);
	this->lblBitCount = ui->NewLabel(this->pnlInput, CSTR("Bit per Sample"));
	this->lblBitCount->SetRect(0, 120, 100, 23, false);
	this->txtBitCount = ui->NewTextBox(this->pnlInput, CSTR("16"));
	this->txtBitCount->SetRect(100, 120, 50, 23, false);
	this->btnStart = ui->NewButton(this->pnlInput, CSTR("&Start"));
	this->btnStart->SetRect(100, 144, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);

	this->tpVolLevel = this->tcFilter->AddTabPage(CSTR("VolLevel"));
	NEW_CLASS(this->rlcVolLevel, UI::GUIRealtimeLineChart(ui, this->tpVolLevel, this->core->GetDrawEngine(), 2, 600, 100));
	this->rlcVolLevel->SetRect(0, 0, 100, 200, false);
	this->rlcVolLevel->SetDockType(UI::GUIControl::DOCK_TOP);
	this->rlcVolLevel->SetUnit(CSTR("dB"));
	this->vspVolLevel = ui->NewVSplitter(this->tpVolLevel, 3, false);
	NEW_CLASS(this->pbsSample, UI::GUIPictureBoxSimple(ui, this->tpVolLevel, this->eng, false));
	this->pbsSample->SetRect(0, 0, 100, 200, false);
	this->pbsSample->SetNoBGColor(true);
	this->pbsSample->SetDockType(UI::GUIControl::DOCK_FILL);
/*	NEW_CLASS(this->vspSample = ui->NewVSplitter(this->tpVolLevel, 3, false));
	NEW_CLASS(this->pbsFFT, UI::GUIPictureBoxSimple(ui, this->tpVolLevel, this->eng, false));
	this->pbsFFT->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbsFFT->SetNoBGColor(true);*/

	this->tpDTMF = this->tcFilter->AddTabPage(CSTR("DTMF"));
	NEW_CLASSNN(this->pnlDTMF, UI::GUIPanel(ui, this->tpDTMF));
	this->pnlDTMF->SetRect(0, 0, 100, 56, false);
	this->pnlDTMF->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblDTMFInterval = ui->NewLabel(this->pnlDTMF, CSTR("Decode Interval"));
	this->lblDTMFInterval->SetRect(4, 4, 100, 23, false);
	this->txtDTMFInterval = ui->NewTextBox(this->pnlDTMF, CSTR("20"));
	this->txtDTMFInterval->SetRect(104, 4, 100, 23, false);
	this->lblDTMFIntervalMS = ui->NewLabel(this->pnlDTMF, CSTR("ms"));
	this->lblDTMFIntervalMS->SetRect(204, 4, 100, 23, false);
	this->lblDTMFDecode = ui->NewLabel(this->pnlDTMF, CSTR("DTMF Decoded"));
	this->lblDTMFDecode->SetRect(4, 28, 100, 23, false);
	this->txtDTMFDecode = ui->NewTextBox(this->pnlDTMF, CSTR(""));
	this->txtDTMFDecode->SetRect(104, 28, 500, 23, false);
	this->txtDTMFDecode->SetReadOnly(true);
	this->btnDTMFClear = ui->NewButton(this->pnlDTMF, CSTR("&Clear"));
	this->btnDTMFClear->SetRect(604, 28, 75, 23, false);
	this->btnDTMFClear->HandleButtonClick(OnDTMFClearClicked, this);
	this->tcDTMF = ui->NewTabControl(this->tpDTMF);
	this->tcDTMF->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpDTMFGen = this->tcDTMF->AddTabPage(CSTR("Generate"));
	this->btnDTMF1 = ui->NewButton(this->tpDTMFGen, CSTR("1"));
	this->btnDTMF1->SetRect(4, 4, 150, 47, false);
	this->btnDTMF1->HandleButtonUpDown(OnDTMF1UpDown, this);
	this->btnDTMF2 = ui->NewButton(this->tpDTMFGen, CSTR("2"));
	this->btnDTMF2->SetRect(164, 4, 150, 47, false);
	this->btnDTMF2->HandleButtonUpDown(OnDTMF2UpDown, this);
	this->btnDTMF3 = ui->NewButton(this->tpDTMFGen, CSTR("3"));
	this->btnDTMF3->SetRect(324, 4, 150, 47, false);
	this->btnDTMF3->HandleButtonUpDown(OnDTMF3UpDown, this);
	this->btnDTMFA = ui->NewButton(this->tpDTMFGen, CSTR("A"));
	this->btnDTMFA->SetRect(484, 4, 150, 47, false);
	this->btnDTMFA->HandleButtonUpDown(OnDTMFAUpDown, this);
	this->btnDTMF4 = ui->NewButton(this->tpDTMFGen, CSTR("4"));
	this->btnDTMF4->SetRect(4, 60, 150, 47, false);
	this->btnDTMF4->HandleButtonUpDown(OnDTMF4UpDown, this);
	this->btnDTMF5 = ui->NewButton(this->tpDTMFGen, CSTR("5"));
	this->btnDTMF5->SetRect(164, 60, 150, 47, false);
	this->btnDTMF5->HandleButtonUpDown(OnDTMF5UpDown, this);
	this->btnDTMF6 = ui->NewButton(this->tpDTMFGen, CSTR("6"));
	this->btnDTMF6->SetRect(324, 60, 150, 47, false);
	this->btnDTMF6->HandleButtonUpDown(OnDTMF6UpDown, this);
	this->btnDTMFB = ui->NewButton(this->tpDTMFGen, CSTR("B"));
	this->btnDTMFB->SetRect(484, 60, 150, 47, false);
	this->btnDTMFB->HandleButtonUpDown(OnDTMFBUpDown, this);
	this->btnDTMF7 = ui->NewButton(this->tpDTMFGen, CSTR("7"));
	this->btnDTMF7->SetRect(4, 116, 150, 47, false);
	this->btnDTMF7->HandleButtonUpDown(OnDTMF7UpDown, this);
	this->btnDTMF8 = ui->NewButton(this->tpDTMFGen, CSTR("8"));
	this->btnDTMF8->SetRect(164, 116, 150, 47, false);
	this->btnDTMF8->HandleButtonUpDown(OnDTMF8UpDown, this);
	this->btnDTMF9 = ui->NewButton(this->tpDTMFGen, CSTR("9"));
	this->btnDTMF9->SetRect(324, 116, 150, 47, false);
	this->btnDTMF9->HandleButtonUpDown(OnDTMF9UpDown, this);
	this->btnDTMFC = ui->NewButton(this->tpDTMFGen, CSTR("C"));
	this->btnDTMFC->SetRect(484, 116, 150, 47, false);
	this->btnDTMFC->HandleButtonUpDown(OnDTMFCUpDown, this);
	this->btnDTMFStar = ui->NewButton(this->tpDTMFGen, CSTR("*"));
	this->btnDTMFStar->SetRect(4, 172, 150, 47, false);
	this->btnDTMFStar->HandleButtonUpDown(OnDTMFStarUpDown, this);
	this->btnDTMF0 = ui->NewButton(this->tpDTMFGen, CSTR("0"));
	this->btnDTMF0->SetRect(164, 172, 150, 47, false);
	this->btnDTMF0->HandleButtonUpDown(OnDTMF0UpDown, this);
	this->btnDTMFSharp = ui->NewButton(this->tpDTMFGen, CSTR("#"));
	this->btnDTMFSharp->SetRect(324, 172, 150, 47, false);
	this->btnDTMFSharp->HandleButtonUpDown(OnDTMFSharpUpDown, this);
	this->btnDTMFD = ui->NewButton(this->tpDTMFGen, CSTR("D"));
	this->btnDTMFD->SetRect(484, 172, 150, 47, false);
	this->btnDTMFD->HandleButtonUpDown(OnDTMFDUpDown, this);
	this->lblDTMFVol = ui->NewLabel(this->tpDTMFGen, CSTR("Volume"));
	this->lblDTMFVol->SetRect(4, 228, 100, 23, false);
	NEW_CLASS(this->tbDTMFVol, UI::GUITrackBar(ui, this->tpDTMFGen, 0, 960, 960));
	this->tbDTMFVol->SetRect(104, 228, 400, 23, false);
	this->tbDTMFVol->HandleScrolled(OnDTMFVolChg, this);
	this->lblDTMFVolV = ui->NewLabel(this->tpDTMFGen, CSTR("0dB"));
	this->lblDTMFVolV->SetRect(504, 228, 100, 23, false);
	this->tpDTMFGen2 = this->tcDTMF->AddTabPage(CSTR("Tones"));
	this->lblDTMFSignalTime = ui->NewLabel(this->tpDTMFGen2, CSTR("Signal Time"));
	this->lblDTMFSignalTime->SetRect(4, 4, 100, 23, false);
	this->txtDTMFSignalTime = ui->NewTextBox(this->tpDTMFGen2, CSTR("120"));
	this->txtDTMFSignalTime->SetRect(104, 4, 100, 23, false);
	this->lblDTMFSignalTimeMS = ui->NewLabel(this->tpDTMFGen2, CSTR("ms"));
	this->lblDTMFSignalTimeMS->SetRect(204, 4, 100, 23, false);
	this->lblDTMFBreakTime = ui->NewLabel(this->tpDTMFGen2, CSTR("Break Time"));
	this->lblDTMFBreakTime->SetRect(4, 28, 100, 23, false);
	this->txtDTMFBreakTime = ui->NewTextBox(this->tpDTMFGen2, CSTR("20"));
	this->txtDTMFBreakTime->SetRect(104, 28, 100, 23, false);
	this->lblDTMFBreakTimeMS = ui->NewLabel(this->tpDTMFGen2, CSTR("ms"));
	this->lblDTMFBreakTimeMS->SetRect(204, 28, 100, 23, false);
	this->lblDTMFTonesVol = ui->NewLabel(this->tpDTMFGen2, CSTR("Volume"));
	this->lblDTMFTonesVol->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->tbDTMFTonesVol, UI::GUITrackBar(ui, this->tpDTMFGen2, 0, 960, 960));
	this->tbDTMFTonesVol->SetRect(104, 52, 400, 23, false);
	this->lblDTMFTones = ui->NewLabel(this->tpDTMFGen2, CSTR("Tones"));
	this->lblDTMFTones->SetRect(4, 76, 100, 23, false);
	this->txtDTMFTones = ui->NewTextBox(this->tpDTMFGen2, CSTR(""));
	this->txtDTMFTones->SetRect(104, 76, 300, 23, false);
	this->btnDTMFTones = ui->NewButton(this->tpDTMFGen2, CSTR("&Generate"));
	this->btnDTMFTones->SetRect(104, 100, 75, 23, false);
	this->btnDTMFTones->HandleButtonClick(OnDTMFTonesClicked, this);

	this->tpVolBoost = this->tcFilter->AddTabPage(CSTR("VolBoost"));
	this->chkVolBoost = ui->NewCheckBox(this->tpVolBoost, CSTR("Enabled"), false);
	this->chkVolBoost->SetRect(4, 4, 100, 23, false);
	this->chkVolBoost->HandleCheckedChange(OnVolBoostChg, this);
	this->lblVolBoostBG = ui->NewLabel(this->tpVolBoost, CSTR("BG Volume"));
	this->lblVolBoostBG->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->tbVolBoostBG, UI::GUITrackBar(ui, this->tpVolBoost, 0, 192, 132));
	this->tbVolBoostBG->SetRect(104, 28, 300, 23, false);
	this->tbVolBoostBG->HandleScrolled(OnVolBoostBGChg, this);
	this->lblVolBoostBGVol = ui->NewLabel(this->tpVolBoost, CSTR("-60dB"));
	this->lblVolBoostBGVol->SetRect(404, 28, 100, 23, false);

	this->tpFileMix = this->tcFilter->AddTabPage(CSTR("File Mix"));
	this->lblFileMix = ui->NewLabel(this->tpFileMix, CSTR("Audio File"));
	this->lblFileMix->SetRect(4, 4, 100, 23, false);
	this->txtFileMix = ui->NewTextBox(this->tpFileMix, CSTR(""));
	this->txtFileMix->SetRect(104, 4, 400, 23, false);
	this->txtFileMix->SetReadOnly(true);
	this->btnFileMix = ui->NewButton(this->tpFileMix, CSTR("B&rowse"));
	this->btnFileMix->SetRect(504, 4, 75, 23, false);
	this->btnFileMix->HandleButtonClick(OnFileMixClicked, this);
	this->btnFileMixStart = ui->NewButton(this->tpFileMix, CSTR("&Start"));
	this->btnFileMixStart->SetRect(104, 28, 75, 23, false);
	this->btnFileMixStart->HandleButtonClick(OnFileMixStartClicked, this);
	this->btnFileMixStop = ui->NewButton(this->tpFileMix, CSTR("S&top"));
	this->btnFileMixStop->SetRect(184, 28, 75, 23, false);
	this->btnFileMixStop->HandleButtonClick(OnFileMixStopClicked, this);

	this->tpCapture = this->tcFilter->AddTabPage(CSTR("Capture"));
	this->btnCaptureStart = ui->NewButton(this->tpCapture, CSTR("Start"));
	this->btnCaptureStart->SetRect(4, 4, 75, 23, false);
	this->btnCaptureStart->HandleButtonClick(OnCaptureStartClicked, this);
	this->btnCaptureStop = ui->NewButton(this->tpCapture, CSTR("Stop"));
	this->btnCaptureStop->SetRect(84, 4, 75, 23, false);
	this->btnCaptureStop->HandleButtonClick(OnCaptureStopClicked, this);

	this->tpSoundGen = this->tcFilter->AddTabPage(CSTR("SoundGen"));
	this->btnSoundGenBell = ui->NewButton(this->tpSoundGen, CSTR("Bell"));
	this->btnSoundGenBell->SetRect(4, 4, 75, 23, false);
	this->btnSoundGenBell->HandleButtonClick(OnSoundGenBellClicked, this);

	this->tpSweep = this->tcFilter->AddTabPage(CSTR("Sweep"));
	this->lblSweepVol = ui->NewLabel(this->tpSweep, CSTR("Volume"));
	this->lblSweepVol->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->tbSweepVol, UI::GUITrackBar(ui, this->tpSweep, 0, 960, 960));
	this->tbSweepVol->SetRect(104, 4, 400, 23, false);
	this->tbSweepVol->HandleScrolled(OnSweepVolChg, this);
	this->lblSweepVolV = ui->NewLabel(this->tpSweep, CSTR("0dB"));
	this->lblSweepVolV->SetRect(504, 4, 100, 23, false);
	this->lblSweepStartFreq = ui->NewLabel(this->tpSweep, CSTR("Start Freq"));
	this->lblSweepStartFreq->SetRect(4, 28, 100, 23, false);
	this->txtSweepStartFreq = ui->NewTextBox(this->tpSweep, CSTR("10"));
	this->txtSweepStartFreq->SetRect(104, 28, 100, 23, false);
	this->lblSweepEndFreq = ui->NewLabel(this->tpSweep, CSTR("End Freq"));
	this->lblSweepEndFreq->SetRect(4, 52, 100, 23, false);
	this->txtSweepEndFreq = ui->NewTextBox(this->tpSweep, CSTR("24000"));
	this->txtSweepEndFreq->SetRect(104, 52, 100, 23, false);
	this->lblSweepDur = ui->NewLabel(this->tpSweep, CSTR("Duration"));
	this->lblSweepDur->SetRect(4, 76, 100, 23, false);
	this->txtSweepDur = ui->NewTextBox(this->tpSweep, CSTR("20"));
	this->txtSweepDur->SetRect(104, 76, 100, 23, false);
	this->btnSweepStart = ui->NewButton(this->tpSweep, CSTR("Start"));
	this->btnSweepStart->SetRect(104, 100, 75, 23, false);
	this->btnSweepStart->HandleButtonClick(OnSweepStartClicked, this);

	this->tpAmplifier = this->tcFilter->AddTabPage(CSTR("Amplifier"));
	this->lblAmplifierVol = ui->NewLabel(this->tpAmplifier, CSTR("Volume"));
	this->lblAmplifierVol->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->tbAmplifierVol, UI::GUITrackBar(ui, this->tpAmplifier, 0, 800, 100));
	this->tbAmplifierVol->SetRect(104, 4, 400, 23, false);
	this->tbAmplifierVol->HandleScrolled(OnAmplifierVolChg, this);
	this->lblAmplifierVolV = ui->NewLabel(this->tpAmplifier, CSTR("100%"));
	this->lblAmplifierVolV->SetRect(504, 4, 100, 23, false);

	if (showMenu)
	{
		UI::GUIMainMenu *mmnu;
		UI::GUIMenu *mnu;
		NEW_CLASS(mmnu, UI::GUIMainMenu());
		mnu = mmnu->AddSubMenu(CSTR("Setting"));
		mnu->AddItem(CSTR("&Set Audio Device"), MNU_SET_DEVICE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
		this->SetMenu(mmnu);
	}

	this->AddTimer(100, OnLevelTimerTick, this);
}

SSWR::AVIRead::AVIRAudioFilterForm::~AVIRAudioFilterForm()
{
	this->StopAudio();
	NotNullPtr<Media::DrawImage> img;
	if (img.Set(this->sampleImg))
	{
		this->eng->DeleteImage(img);
		this->sampleImg = 0;
	}
	if (img.Set(this->fftImg))
	{
		this->eng->DeleteImage(img);
		this->fftImg = 0;
	}
}

void SSWR::AVIRead::AVIRAudioFilterForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_SET_DEVICE:
		{
			SSWR::AVIRead::AVIRSetAudioForm frm(0, this->ui, this->core);
			frm.ShowDialog(this);
		}
		break;
	}
}

void SSWR::AVIRead::AVIRAudioFilterForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
