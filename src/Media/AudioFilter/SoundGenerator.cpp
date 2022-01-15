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
	NEW_CLASS(this->sndGenMap, Data::Int32Map<Media::AudioFilter::SoundGen::ISoundGen*>());
	Media::AudioFilter::SoundGen::ISoundGen *sndGen;

	NEW_CLASS(sndGen, Media::AudioFilter::SoundGen::BellSoundGen(this->format.frequency));
	this->sndGenMap->Put(sndGen->GetSoundType(), sndGen);
}

Media::AudioFilter::SoundGenerator::~SoundGenerator()
{
	Data::ArrayList<Media::AudioFilter::SoundGen::ISoundGen*> *sndGenList = this->sndGenMap->GetValues();
	Media::AudioFilter::SoundGen::ISoundGen *sndGen;
	UOSInt i;
	i = sndGenList->GetCount();
	while (i-- > 0)
	{
		sndGen = sndGenList->GetItem(i);
		DEL_CLASS(sndGen);
	}
	DEL_CLASS(this->sndGenMap);
}

void Media::AudioFilter::SoundGenerator::GetFormat(AudioFormat *format)
{
	format->FromAudioFormat(&this->format);
}

UOSInt Media::AudioFilter::SoundGenerator::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	if (this->sourceAudio == 0)
		return 0;
	UOSInt readSize = this->sourceAudio->ReadBlock(buff, blkSize);
	Data::ArrayList<Media::AudioFilter::SoundGen::ISoundGen*> *sndGenList = this->sndGenMap->GetValues();
	Media::AudioFilter::SoundGen::ISoundGen *sndGen;
	UOSInt sampleCnt = readSize / (this->format.align);
	Double *sndBuff;
	UOSInt i;
	i = sndGenList->GetCount();
	if (i <= 0)
	{
		return readSize;
	}
	sndBuff = MemAllocA(Double, sampleCnt);
	MemClear(sndBuff, sizeof(Double) * sampleCnt);
	while (i-- > 0)
	{
		sndGen = sndGenList->GetItem(i);
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
				v2 = sndBuff[i] * 32768.0 + ReadInt16(&buff[l]);
				if (v2 >= 32767.0)
				{
					WriteInt16(&buff[l], 32767);
				}
				else if (v2 <= -32768.0)
				{
					WriteInt16(&buff[l], -32768);
				}
				else
				{
					WriteInt16(&buff[l], Double2Int32(v2));
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
				v2 = sndBuff[i] * 128.0 + buff[l];
				if (v2 >= 255.0)
				{
					buff[l] = 255;
				}
				else if (v2 <= 0)
				{
					buff[l] = 0;
				}
				else
				{
					buff[l] = (UInt8)Double2Int32(v2);
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
	Media::AudioFilter::SoundGen::ISoundGen *sndGen = this->sndGenMap->Get(sndType);
	if (sndGen)
	{
		return sndGen->GenSound(sampleVol);
	}
	else
	{
		return false;
	}
}
