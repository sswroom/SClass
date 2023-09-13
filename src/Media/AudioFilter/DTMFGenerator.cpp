#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/AudioFilter/DTMFGenerator.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

Media::AudioFilter::DTMFGenerator::DTMFGenerator(NotNullPtr<IAudioSource> sourceAudio) : Media::IAudioFilter(sourceAudio)
{
	sourceAudio->GetFormat(this->format);

	this->tonesSignalSamples = 0;
	this->tonesBreakSamples = 0;
	this->tonesVol = 0;
	this->tonesCurrSample = 0;
	this->tonesVals = 0;

	this->SetVolume(1.0);
	this->SetTone(0);
}

Media::AudioFilter::DTMFGenerator::~DTMFGenerator()
{
	SDEL_STRING(this->tonesVals);
}

void Media::AudioFilter::DTMFGenerator::GetFormat(NotNullPtr<AudioFormat> format)
{
	format->FromAudioFormat(this->format);
}

UOSInt Media::AudioFilter::DTMFGenerator::ReadBlock(Data::ByteArray blk)
{
	UOSInt readSize = this->sourceAudio->ReadBlock(blk);

	if (this->format.bitpersample == 16)
	{
		UOSInt i = 0;
		UOSInt j;
		Double v;
		Int32 iv;
		Int32 ivCh;
		while (i < readSize)
		{
			v = (Math_Sin(this->freq1Curr) + Math_Sin(this->freq2Curr)) * 0.5;
			this->freq1Curr += this->freq1Step;
			this->freq2Curr += this->freq2Step;

			iv = Double2Int32(v * 32767 * this->vol);
			j = this->format.nChannels;
			while (j-- > 0)
			{
				ivCh = iv + ReadInt16(&blk[i]);
				if (ivCh > 32767)
					ivCh = 32767;
				if (ivCh < -32768)
					ivCh = -32768;
				WriteInt16(&blk[i], ivCh);
				i += 2;
			}
		}

		Sync::MutexUsage mutUsage(this->tonesMut);
		if (this->tonesVals)
		{
			UOSInt sampleCnt = readSize / this->format.align;
			UOSInt sampleLeft;
			UInt32 tonesOfst = this->tonesCurrSample / (this->tonesSignalSamples + this->tonesBreakSamples);
			UInt32 tonesStartOfst;
			UOSInt tonesCnt = this->tonesVals->leng;
			Int32 freq1;
			Int32 freq2;
			i = 0;
			while (sampleCnt > 0)
			{
				if (tonesOfst >= tonesCnt)
				{
					this->tonesVals->Release();
					this->tonesVals = 0;
					break;
				}

				tonesStartOfst = tonesOfst * (this->tonesSignalSamples + this->tonesBreakSamples);
				switch (this->tonesVals->v[tonesOfst])
				{
				case '1':
					freq1 = 697;
					freq2 = 1209;
					break;
				case '2':
					freq1 = 697;
					freq2 = 1336;
					break;
				case '3':
					freq1 = 697;
					freq2 = 1477;
					break;
				case '4':
					freq1 = 770;
					freq2 = 1209;
					break;
				case '5':
					freq1 = 770;
					freq2 = 1336;
					break;
				case '6':
					freq1 = 770;
					freq2 = 1477;
					break;
				case '7':
					freq1 = 852;
					freq2 = 1209;
					break;
				case '8':
					freq1 = 852;
					freq2 = 1336;
					break;
				case '9':
					freq1 = 852;
					freq2 = 1477;
					break;
				case '*':
					freq1 = 941;
					freq2 = 1209;
					break;
				case '0':
					freq1 = 941;
					freq2 = 1336;
					break;
				case '#':
					freq1 = 941;
					freq2 = 1477;
					break;
				case 'A':
					freq1 = 697;
					freq2 = 1633;
					break;
				case 'B':
					freq1 = 770;
					freq2 = 1633;
					break;
				case 'C':
					freq1 = 852;
					freq2 = 1633;
					break;
				case 'D':
					freq1 = 941;
					freq2 = 1633;
					break;
				default:
					freq1 = 697;
					freq2 = 1209;
					break;
				}
				if (this->tonesCurrSample - tonesStartOfst < this->tonesSignalSamples)
				{
					Double freq1Step;
					Double freq1Curr;
					Double freq2Step;
					Double freq2Curr;
					freq1Step = freq1 * Math::PI / this->format.frequency * 2.0;
					freq2Step = freq2 * Math::PI / this->format.frequency * 2.0;
					freq1Curr = freq1Step * (this->tonesCurrSample - tonesStartOfst);
					freq2Curr = freq2Step * (this->tonesCurrSample - tonesStartOfst);

					sampleLeft = this->tonesSignalSamples - (this->tonesCurrSample - tonesStartOfst);
					if (sampleLeft > sampleCnt)
					{
						sampleLeft = sampleCnt;
					}
					sampleCnt -= sampleLeft;
					this->tonesCurrSample += (UInt32)sampleLeft;
					while (sampleLeft-- > 0)
					{
						v = (Math_Sin(freq1Curr) + Math_Sin(freq2Curr)) * 0.5;
						freq1Curr += freq1Step;
						freq2Curr += freq2Step;

						iv = Double2Int32(v * 32767 * this->tonesVol);
						j = this->format.nChannels;
						while (j-- > 0)
						{
							ivCh = iv + ReadInt16(&blk[i]);
							if (ivCh > 32767)
								ivCh = 32767;
							if (ivCh < -32768)
								ivCh = -32768;
							WriteInt16(&blk[i], ivCh);
							i += 2;
						}
					}
				}
				if (sampleCnt <= 0)
					break;
				if (this->tonesBreakSamples > 0)
				{
					sampleLeft = this->tonesBreakSamples - (this->tonesCurrSample - tonesStartOfst - this->tonesSignalSamples);
					if (sampleLeft > sampleCnt)
					{
						sampleLeft = sampleCnt;
					}
					sampleCnt -= sampleLeft;
					this->tonesCurrSample += (UInt32)sampleLeft;
					i += sampleLeft * this->format.align;
				}

				tonesOfst++;
			}
		}
		mutUsage.EndUse();
	}
	else if (this->format.bitpersample == 8)
	{
		UOSInt i = 0;
		UOSInt j;
		Double v;
		Int32 iv;
		Int32 ivCh;
		while (i < readSize)
		{
			v = (Math_Sin(this->freq1Curr) + Math_Sin(this->freq2Curr)) * 0.5;
			this->freq1Curr += this->freq1Step;
			this->freq2Curr += this->freq2Step;

			iv = Double2Int32(v * 127 * this->vol);
			j = this->format.nChannels;
			while (j-- > 0)
			{
				ivCh = iv + blk[i];
				if (ivCh > 255)
					ivCh = 255;
				if (ivCh < 0)
					ivCh = 0;
				blk[i] = (UInt8)ivCh;
				i += 1;
			}
		}
	}
	return readSize;
}

void Media::AudioFilter::DTMFGenerator::SetTone(UTF8Char tone)
{
	Double freq1;
	Double freq2;
	switch (tone)
	{
	case '1':
		freq1 = 697;
		freq2 = 1209;
		break;
	case '2':
		freq1 = 697;
		freq2 = 1336;
		break;
	case '3':
		freq1 = 697;
		freq2 = 1477;
		break;
	case '4':
		freq1 = 770;
		freq2 = 1209;
		break;
	case '5':
		freq1 = 770;
		freq2 = 1336;
		break;
	case '6':
		freq1 = 770;
		freq2 = 1477;
		break;
	case '7':
		freq1 = 852;
		freq2 = 1209;
		break;
	case '8':
		freq1 = 852;
		freq2 = 1336;
		break;
	case '9':
		freq1 = 852;
		freq2 = 1477;
		break;
	case '*':
		freq1 = 941;
		freq2 = 1209;
		break;
	case '0':
		freq1 = 941;
		freq2 = 1336;
		break;
	case '#':
		freq1 = 941;
		freq2 = 1477;
		break;
	case 'A':
		freq1 = 697;
		freq2 = 1633;
		break;
	case 'B':
		freq1 = 770;
		freq2 = 1633;
		break;
	case 'C':
		freq1 = 852;
		freq2 = 1633;
		break;
	case 'D':
		freq1 = 941;
		freq2 = 1633;
		break;
	default:
		this->freq1Curr = 0;
		this->freq1Step = 0;
		this->freq2Curr = 0;
		this->freq2Step = 0;
		return;
	}
	this->freq1Step = freq1 / this->format.frequency * 2 * Math::PI;
	this->freq2Step = freq2 / this->format.frequency * 2 * Math::PI;
	this->freq1Curr = 0;
	this->freq2Curr = 0;
}

void Media::AudioFilter::DTMFGenerator::SetVolume(Double vol)
{
	this->vol = vol;
}

Bool Media::AudioFilter::DTMFGenerator::GenTones(UInt32 signalTime, UInt32 breakTime, Double vol, const UTF8Char *tones)
{
	if (tones == 0 || tones[0] == 0 || signalTime <= 0 || vol <= 0)
		return false;
	const UTF8Char *sptr = tones;
	UTF8Char c;
	while ((c = *sptr) != 0)
	{
		if (c >= '0' && c <= '9')
		{
		}
		else if (c >= 'A' && c <= 'D')
		{
		}
		else if (c == '*' || c == '#')
		{
		}
		else
		{
			return false;
		}
		sptr++;
	}

	Sync::MutexUsage mutUsage(this->tonesMut);
	SDEL_STRING(this->tonesVals);
	this->tonesVals = Text::String::New(tones, (UOSInt)(sptr - tones)).Ptr();
	this->tonesSignalSamples = this->format.frequency * signalTime / 1000;
	this->tonesBreakSamples = this->format.frequency * breakTime / 1000;
	this->tonesVol = vol;
	this->tonesCurrSample = 0;
	return true;
}
