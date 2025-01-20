#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/AFilter/SoundGenerator.h"
#include "Media/AFilter/SoundGen/BellSoundGen.h"
#include "Text/MyString.h"

Media::AFilter::SoundGenerator::SoundGenerator(NN<AudioSource> sourceAudio) : Media::AudioFilter(sourceAudio)
{
	sourceAudio->GetFormat(this->format);
	NN<Media::AFilter::SoundGen::SoundTypeGen> sndGen;

	NEW_CLASSNN(sndGen, Media::AFilter::SoundGen::BellSoundGen(this->format.frequency));
	this->sndGenMap.Put(sndGen->GetSoundType(), sndGen);
}

Media::AFilter::SoundGenerator::~SoundGenerator()
{
	this->sndGenMap.DeleteAll();
}

void Media::AFilter::SoundGenerator::GetFormat(NN<AudioFormat> format)
{
	format->FromAudioFormat(this->format);
}

UOSInt Media::AFilter::SoundGenerator::ReadBlock(Data::ByteArray blk)
{
	UOSInt readSize = this->sourceAudio->ReadBlock(blk);
	NN<Media::AFilter::SoundGen::SoundTypeGen> sndGen;
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
		sndGen = this->sndGenMap.GetItemNoCheck(i);
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

Bool Media::AFilter::SoundGenerator::GenSound(Media::AFilter::SoundGen::SoundTypeGen::SoundType sndType, Double sampleVol)
{
	NN<Media::AFilter::SoundGen::SoundTypeGen> sndGen;
	if (this->sndGenMap.Get(sndType).SetTo(sndGen))
	{
		return sndGen->GenSound(sampleVol);
	}
	else
	{
		return false;
	}
}
