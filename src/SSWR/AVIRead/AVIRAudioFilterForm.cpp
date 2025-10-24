#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Math/Math_C.h"
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

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	if (me->audSrc.IsNull())
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
			NEW_CLASSOPT(me->audSrc, Media::SilentSource(frequency, nChannel, bitCount, CSTR("Silent"), (UInt64)-1));
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
		NN<Media::AudioRenderer> audRender;
		NN<Media::AudioSource> audSrc;
		UnsafeArray<UInt8> sampleBuff;
		if (me->audSrc.SetTo(audSrc))
		{
			NN<Media::AFilter::AudioAmplifier> audioAmp;
			NN<Media::AFilter::DTMFGenerator> dtmfGen;
			NN<Media::AFilter::FileMixFilter> fileMix;
			NN<Media::AFilter::SoundGenerator> sndGen;
			NN<Media::AFilter::AudioSweepFilter> sweepFilter;
			NN<Media::AFilter::DynamicVolBooster> volBooster;
			NN<Media::AFilter::DTMFDecoder> dtmfDec;
			NN<Media::AFilter::AudioSampleRipper> audioRipper;
			NN<Media::AFilter::AudioLevelMeter> audioLevel;
			NN<Media::AFilter::AudioCaptureFilter> audioCapture;
			NEW_CLASSNN(audioAmp, Media::AFilter::AudioAmplifier(audSrc));
			NEW_CLASSNN(dtmfGen, Media::AFilter::DTMFGenerator(audioAmp));
			NEW_CLASSNN(fileMix, Media::AFilter::FileMixFilter(dtmfGen, me->core->GetParserList()));
			NEW_CLASSNN(sndGen, Media::AFilter::SoundGenerator(fileMix));
			NEW_CLASSNN(sweepFilter, Media::AFilter::AudioSweepFilter(sndGen));
			NEW_CLASSNN(volBooster, Media::AFilter::DynamicVolBooster(sweepFilter));
			NEW_CLASSNN(dtmfDec, Media::AFilter::DTMFDecoder(volBooster, dtmfMS * frequency / 1000));
			NEW_CLASSNN(audioRipper, Media::AFilter::AudioSampleRipper(dtmfDec, FFTSAMPLE + FFTAVG - 1));
			NEW_CLASSNN(audioLevel, Media::AFilter::AudioLevelMeter(audioRipper));
			NEW_CLASSNN(audioCapture, Media::AFilter::AudioCaptureFilter(audioLevel));
			me->audioAmp = audioAmp;
			me->dtmfGen = dtmfGen;
			me->fileMix = fileMix;
			me->sndGen = sndGen;
			me->sweepFilter = sweepFilter;
			me->volBooster = volBooster;
			me->dtmfDec = dtmfDec;
			me->audioRipper = audioRipper;
			me->audioLevel = audioLevel;
			me->audioCapture = audioCapture;
			me->bitCount = bitCount;
			me->nChannels = nChannel;
			me->sampleBuff = sampleBuff = MemAllocArr(UInt8, (FFTSAMPLE + FFTAVG - 1) * (UOSInt)me->nChannels * (UOSInt)(me->bitCount >> 3));
			volBooster->SetEnabled(me->chkVolBoost->IsChecked());
			volBooster->SetBGLevel(Math_Pow(10, OSInt2Double((OSInt)me->tbVolBoostBG->GetPos() - 192) / 20.0));
			dtmfGen->SetVolume(Math_Pow(10, OSInt2Double((OSInt)me->tbDTMFVol->GetPos() - 960) / 20.0));
			audioAmp->SetLevel(UOSInt2Double(me->tbAmplifierVol->GetPos()) * 0.01);
			if (me->radOutputDevice->IsSelected())
			{
				me->audRender = me->core->BindAudio(audioCapture);
				me->audRenderType = 0;
			}
			else if (me->radOutputSilent->IsSelected())
			{
				NEW_CLASSNN(audRender, Media::NullRenderer());
				me->audRender = audRender;
				audRender->BindAudio(audioCapture);
				me->audRenderType = 1;
			}
			if (me->audRender.SetTo(audRender))
			{
				audRender->SetBufferTime(buffSize);
				audRender->AudioInit(&me->clk);
				audRender->Start();

				dtmfDec->HandleToneChange(OnDTMFToneChange, me);

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
				me->audioCapture.Delete();
				me->audioLevel.Delete();
				me->audioRipper.Delete();
				me->dtmfDec.Delete();
				me->volBooster.Delete();
				me->sweepFilter.Delete();
				me->sndGen.Delete();
				me->fileMix.Delete();
				me->dtmfGen.Delete();
				me->audioAmp.Delete();
				me->audSrc.Delete();
				MemFreeArr(sampleBuff);
				me->audSrc = 0;
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

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnVolBoostChg(AnyType userObj, Bool newState)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DynamicVolBooster> volBooster;
	if (me->volBooster.SetTo(volBooster))
	{
		volBooster->SetEnabled(newState);
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnVolBoostBGChg(AnyType userObj, UOSInt scrollPos)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	NN<Media::AFilter::DynamicVolBooster> volBooster;
	if (me->volBooster.SetTo(volBooster))
	{
		volBooster->SetBGLevel(Math_Pow(10, OSInt2Double((OSInt)scrollPos - 192) / 20.0));
	}
	sptr = Text::StrConcatC(Text::StrOSInt(sbuff, (OSInt)scrollPos - 192), UTF8STRC("dB"));
	me->lblVolBoostBGVol->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFClearClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	Sync::MutexUsage mutUsage(me->dtmfMut);
	me->dtmfSb.ClearStr();
	me->dtmfMod = true;
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF1UpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('1');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF2UpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('2');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF3UpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('3');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF4UpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('4');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF5UpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('5');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF6UpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('6');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF7UpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('7');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF8UpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('8');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF9UpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('9');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFStarUpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('*');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMF0UpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('0');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFSharpUpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('#');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFAUpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('A');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFBUpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('B');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFCUpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('C');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFDUpDown(AnyType userObj, Bool isDown)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		if (isDown)
		{
			dtmfGen->SetTone('D');
		}
		else
		{
			dtmfGen->SetTone(0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFVolChg(AnyType userObj, UOSInt scrollPos)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	Text::StringBuilderUTF8 sb;
	sb.AppendDouble(OSInt2Double((OSInt)scrollPos - 960) * 0.1);
	sb.AppendC(UTF8STRC("dB"));
	me->lblDTMFVolV->SetText(sb.ToCString());
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (me->dtmfGen.SetTo(dtmfGen))
	{
		dtmfGen->SetVolume(Math_Pow(10, OSInt2Double((OSInt)scrollPos - 960) / 200.0));
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFTonesClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::DTMFGenerator> dtmfGen;
	if (!me->dtmfGen.SetTo(dtmfGen))
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
	if (!dtmfGen->GenTones(signalTime, breakTime, vol, sb.ToString()))
	{
		me->ui->ShowMsgOK(CSTR("Error in generating tones"), CSTR("Generate Tones"), me);
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnLevelTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::AudioLevelMeter> audioLevel;
	if (me->audioLevel.SetTo(audioLevel))
	{
		Double v[2];
		v[0] = Math_Log10(audioLevel->GetLevel(0)) * 20;
		v[1] = Math_Log10(audioLevel->GetLevel(1)) * 20;
		me->rlcVolLevel->AddSample(v);
	}
	NN<Media::AFilter::AudioSampleRipper> audioRipper;
	UnsafeArray<UInt8> sampleBuff;
	if (me->audioRipper.SetTo(audioRipper) && me->sampleBuff.SetTo(sampleBuff))
	{
		if (audioRipper->IsChanged())
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
			NN<Media::DrawImage> img;
			NN<Media::DrawImage> dimg;
			NN<Media::DrawBrush> b;
			NN<Media::DrawPen> p;
			audioRipper->GetSamples(sampleBuff);
			sz = me->pbsSample->GetSizeP();
			if (sz.x <= 0 || sz.y <= 0)
			{
			}
			else if (me->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(img))
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
							thisY = (ReadInt16(&sampleBuff[k]) + 32768.0) * UOSInt2Double(sz.y) / 65536.0;
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
							thisY = UOSInt2Double(sampleBuff[k] * sz.y) / 256.0;
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
				me->pbsSample->SetImageDImg(img);
				if (me->sampleImg.SetTo(dimg))
				{
					me->eng->DeleteImage(dimg);
				}
				me->sampleImg = img;
			}

			sz = me->pbsFFT->GetSizeP();
			if (sz.x <= 0 || sz.y <= 0)
			{
			}
			else if (me->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(img))
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
						me->fft.ForwardBits(sampleBuff + ((UOSInt)(me->bitCount >> 3) * i), data, st, me->nChannels, 1.0);
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

				me->pbsFFT->SetImageDImg(img);
				if (me->fftImg.SetTo(dimg))
				{
					me->eng->DeleteImage(dimg);
				}
				me->fftImg = img;
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

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnDTMFToneChange(AnyType userObj, WChar tone)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
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

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnFileMixClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::FileMixFilter> fileMix;
	if (me->fileMix.SetTo(fileMix))
	{
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"AudioFilterFileMix", false);
		dlg->AddFilter(CSTR("*.wav"), CSTR("Wave file"));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			if (fileMix->LoadFile(dlg->GetFileName()))
			{
				me->txtFileMix->SetText(dlg->GetFileName()->ToCString());
			}
		}
		dlg.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnFileMixStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::FileMixFilter> fileMix;
	if (me->fileMix.SetTo(fileMix))
	{
		fileMix->StartMix();
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnFileMixStopClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::FileMixFilter> fileMix;
	if (me->fileMix.SetTo(fileMix))
	{
		fileMix->StopMix();
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnCaptureStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::AudioCaptureFilter> audioCapture;
	if (me->audioCapture.SetTo(audioCapture))
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UOSInt i;
		Data::DateTime dt;
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
		dt.SetCurrTimeUTC();
		sptr = Text::StrConcatC(Text::StrInt64(&sbuff[i + 1], dt.ToTicks()), UTF8STRC(".wav"));
		audioCapture->StartCapture(CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnCaptureStopClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::AudioCaptureFilter> audioCapture;
	if (me->audioCapture.SetTo(audioCapture))
	{
		audioCapture->StopCapture();
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnSoundGenBellClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::SoundGenerator> sndGen;
	if (me->sndGen.SetTo(sndGen))
	{
		sndGen->GenSound(Media::AFilter::SoundGen::SoundTypeGen::ST_BELL, 1.0);
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnSweepVolChg(AnyType userObj, UOSInt scrollPos)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::AudioSweepFilter> sweepFilter;
	Text::StringBuilderUTF8 sb;
	sb.AppendDouble(OSInt2Double((OSInt)scrollPos - 960) * 0.1);
	sb.AppendC(UTF8STRC("dB"));
	me->lblSweepVolV->SetText(sb.ToCString());
	if (me->sweepFilter.SetTo(sweepFilter))
	{
		sweepFilter->SetVolume(Math_Pow(10, OSInt2Double((OSInt)scrollPos - 960) / 200.0));
	}
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnSweepStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::AudioSweepFilter> sweepFilter;
	Text::StringBuilderUTF8 sb;
	Double startFreq;
	Double endFreq;
	UInt32 timeSeconds;
	if (!me->sweepFilter.SetTo(sweepFilter))
	{
		return;
	}
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
	sweepFilter->StartSweep(startFreq, endFreq, timeSeconds);
}

void __stdcall SSWR::AVIRead::AVIRAudioFilterForm::OnAmplifierVolChg(AnyType userObj, UOSInt scrollPos)
{
	NN<SSWR::AVIRead::AVIRAudioFilterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAudioFilterForm>();
	NN<Media::AFilter::AudioAmplifier> audioAmp;
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(scrollPos);
	sb.AppendC(UTF8STRC("%"));
	me->lblAmplifierVolV->SetText(sb.ToCString());
	if (me->audioAmp.SetTo(audioAmp))
	{
		audioAmp->SetLevel(UOSInt2Double(scrollPos) * 0.01);
	}
}

void SSWR::AVIRead::AVIRAudioFilterForm::StopAudio()
{
	UnsafeArray<UInt8> sampleBuff;
	if (this->audSrc.NotNull())
	{
		if (this->audRenderType == 1)
		{
			this->audRender.Delete();
		}
		else
		{
			this->core->BindAudio(0);
			this->audRender = 0;
		}
		this->audSrc.Delete();
		this->audioAmp.Delete();
		this->audioLevel.Delete();
		this->audioRipper.Delete();
		this->dtmfDec.Delete();
		this->dtmfGen.Delete();
		this->sndGen.Delete();
		this->fileMix.Delete();
		this->volBooster.Delete();
		this->audioCapture.Delete();
		if (this->sampleBuff.SetTo(sampleBuff)) MemFreeArr(sampleBuff);
		this->sampleBuff = 0;
	}
}

SSWR::AVIRead::AVIRAudioFilterForm::AVIRAudioFilterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Bool showMenu) : UI::GUIForm(parent, 1024, 768, ui), fft(FFTSAMPLE, Math::FFTCalc::WT_BLACKMANN_HARRIS)
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

	this->pnlInput = ui->NewPanel(*this);
	this->pnlInput->SetRect(0, 0, 100, 168, false);
	this->pnlInput->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pbsFFT = ui->NewPictureBoxSimple(*this, this->eng, false);
	this->pbsFFT->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pbsFFT->SetNoBGColor(true);
	this->vspSample = ui->NewVSplitter(*this, 3, true);
	this->tcFilter = ui->NewTabControl(*this);
	this->tcFilter->SetDockType(UI::GUIControl::DOCK_FILL);

	this->pnlAudioSource = ui->NewPanel(this->pnlInput);
	this->pnlAudioSource->SetRect(0, 0, 400, 23, false);
	this->lblAudioSource = ui->NewLabel(this->pnlAudioSource, CSTR("Audio Source"));
	this->lblAudioSource->SetRect(0, 0, 100, 23, false);
	this->radInputWaveIn = ui->NewRadioButton(this->pnlAudioSource, CSTR("WaveIn"), true);
	this->radInputWaveIn->SetRect(100, 0, 100, 23, false);
	this->radInputSilent = ui->NewRadioButton(this->pnlAudioSource, CSTR("Silent"), false);
	this->radInputSilent->SetRect(200, 0, 100, 23, false);
	this->pnlAudioOutput = ui->NewPanel(this->pnlInput);
	this->pnlAudioOutput->SetRect(0, 24, 400, 23, false);
	this->lblAudioOutput = ui->NewLabel(this->pnlAudioOutput, CSTR("Audio Output"));
	this->lblAudioOutput->SetRect(0, 0, 100, 23, false);
	this->radOutputDevice = ui->NewRadioButton(this->pnlAudioOutput, CSTR("Device"), true);
	this->radOutputDevice->SetRect(100, 0, 100, 23, false);
	this->radOutputSilent = ui->NewRadioButton(this->pnlAudioOutput, CSTR("Silent"), false);
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
	this->rlcVolLevel = ui->NewRealtimeLineChart(this->tpVolLevel, this->core->GetDrawEngine(), 2, 600, 100, 0);
	this->rlcVolLevel->SetRect(0, 0, 100, 200, false);
	this->rlcVolLevel->SetDockType(UI::GUIControl::DOCK_TOP);
	this->rlcVolLevel->SetUnit(CSTR("dB"));
	this->vspVolLevel = ui->NewVSplitter(this->tpVolLevel, 3, false);
	this->pbsSample = ui->NewPictureBoxSimple(this->tpVolLevel, this->eng, false);
	this->pbsSample->SetRect(0, 0, 100, 200, false);
	this->pbsSample->SetNoBGColor(true);
	this->pbsSample->SetDockType(UI::GUIControl::DOCK_FILL);
/*	NEW_CLASS(this->vspSample = ui->NewVSplitter(this->tpVolLevel, 3, false));
	NEW_CLASS(this->pbsFFT = ui->NewPictureBoxSimple(this->tpVolLevel, this->eng, false));
	this->pbsFFT->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbsFFT->SetNoBGColor(true);*/

	this->tpDTMF = this->tcFilter->AddTabPage(CSTR("DTMF"));
	this->pnlDTMF = ui->NewPanel(this->tpDTMF);
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
	this->tbDTMFVol = ui->NewTrackBar(this->tpDTMFGen, 0, 960, 960);
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
	this->tbDTMFTonesVol = ui->NewTrackBar(this->tpDTMFGen2, 0, 960, 960);
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
	this->tbVolBoostBG = ui->NewTrackBar(this->tpVolBoost, 0, 192, 132);
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
	this->tbSweepVol = ui->NewTrackBar(this->tpSweep, 0, 960, 960);
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
	this->tbAmplifierVol = ui->NewTrackBar(this->tpAmplifier, 0, 800, 100);
	this->tbAmplifierVol->SetRect(104, 4, 400, 23, false);
	this->tbAmplifierVol->HandleScrolled(OnAmplifierVolChg, this);
	this->lblAmplifierVolV = ui->NewLabel(this->tpAmplifier, CSTR("100%"));
	this->lblAmplifierVolV->SetRect(504, 4, 100, 23, false);

	if (showMenu)
	{
		NN<UI::GUIMainMenu> mmnu;
		NN<UI::GUIMenu> mnu;
		NEW_CLASSNN(mmnu, UI::GUIMainMenu());
		mnu = mmnu->AddSubMenu(CSTR("Setting"));
		mnu->AddItem(CSTR("&Set Audio Device"), MNU_SET_DEVICE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
		this->SetMenu(mmnu);
	}

	this->AddTimer(100, OnLevelTimerTick, this);
}

SSWR::AVIRead::AVIRAudioFilterForm::~AVIRAudioFilterForm()
{
	this->StopAudio();
	NN<Media::DrawImage> img;
	if (this->sampleImg.SetTo(img))
	{
		this->eng->DeleteImage(img);
		this->sampleImg = 0;
	}
	if (this->fftImg.SetTo(img))
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
