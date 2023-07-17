#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/AudioFilter/SoundGenerator.h"
#include "Media/AudioFilter/SoundGen/BellSoundGen.h"
#include "Text/MyString.h"

Media::AudioFilter::SoundGenerator::SoundGenerator(IAudioSource *sourceAudio) : Media::IAudioFilter(sourceAudio)
{
	this->sourceAudio = sourceAudio;
	sourceAudio->GetFormat(&this->format);
	Media::AudioFilter::SoundGen::ISoundGen *sndGen;

	NEW_CLASS(sndGen, Media::AudioFilter::SoundGen::BellSoundGen(this->format.frequency));
	this->sndGenMap.Put(sndGen->GetSoundType(), sndGen);
}

Media::AudioFilter::SoundGenerator::~SoundGenerator()
{
	Media::AudioFilter::SoundGen::ISoundGen *sndGen;
	UOSInt i;
	i = this->sndGenMap.GetCount();
	while (i-- > 0)
	{
		sndGen = this->sndGenMap.GetItem(i);
		DEL_CLASS(sndGen);
	}
}

void Media::AudioFilter::SoundGenerator::GetFormat(AudioFormat *format)
{
	format->FromAudioFormat(&this->format);
}

UOSInt Media::AudioFilter::SoundGenerator::ReadBlock(Data::ByteArray blk)
{
	if (this->sourceAudio == 0)
		return 0;
	UOSInt readSize = this->sourceAudio->ReadBlock(blk);
	Media::AudioFilter::SoundGen::ISoundGen *sndGen;
	UOSInt sampleCnt = readSize / (this->format.align);
	Double *sndBuff;
	UOSInt i;
	i = this->sndGenMap.GetCount();
	if (i <= 0)
	{
		return readSize;
	}
	sndBuff = MemAllocA(Double, sampleCnt);
	MemClear(sndBuff, sizeof(Double) * sampleCnt);
	while (i-- > 0)
	{
		sndGen = this->sndGenMap.GetItem(i);
		sndGen->GenSignals(sndBuff, sampleCnt);
	}
	if (this->format.bitpersample == 16)
	{
		UOSInt i;
		UOSInt j;
		UOSInt l;
		Double v2;
		UOSInt sampleCnt = readSize / 2 / this->format.nChannels;
		l = 0;
		i = 0;
		while (i < sampleCnt)
		{
			j = this->format.nChannels;
			while (j-- > 0)
			{
				v2 = sndBuff[i] * 32768.0 + ReadInt16(&blk[l]);
				if (v2 >= 32767.0)
				{
					WriteInt16(&blk[l], 32767);
				}
				else if (v2 <= -32768.0)
				{
					WriteInt16(&blk[l], -32768);
				}
				else
				{
					WriteInt16(&blk[l], Double2Int32(v2));
				}
				l += 2;
			}

			i++;
		}
	}
	else if (this->format.bitpersample == 8)
	{
		UOSInt i;
		UOSInt j;
		UOSInt l;
		Double v2;
		UOSInt sampleCnt = readSize / this->format.nChannels;
		l = 0;
		i = 0;
		while (i < sampleCnt)
		{
			j = this->format.nChannels;
			while (j-- > 0)
			{
				v2 = sndBuff[i] * 128.0 + blk[l];
				if (v2 >= 255.0)
				{
					blk[l] = 255;
				}
				else if (v2 <= 0)
				{
					blk[l] = 0;
				}
				else
				{
					blk[l] = (UInt8)Double2Int32(v2);
				}
				l += 1;
			}

			i++;
		}
	}
	MemFreeA(sndBuff);
	return readSize;
}

Bool Media::AudioFilter::SoundGenerator::GenSound(Media::AudioFilter::SoundGen::ISoundGen::SoundType sndType, Double sampleVol)
{
	Media::AudioFilter::SoundGen::ISoundGen *sndGen = this->sndGenMap.Get(sndType);
	if (sndGen)
	{
		return sndGen->GenSound(sampleVol);
	}
	else
	{
		return false;
	}
}
