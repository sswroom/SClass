#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/AudioFilter/SoundGen/BellSoundGen.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

/*
Ting Sound
1406
3750
6937
10780
15280
19310*
21370
28680*
32710
37210*
41060
44250
46590
*/

Media::AudioFilter::SoundGen::BellSoundGen::BellSoundGen(UInt32 freq) : Media::AudioFilter::SoundGen::ISoundGen(freq)
{
	NEW_CLASS(this->soundMut, Sync::Mutex());
	this->sampleVol = 0;
	this->currSample = 0;
}

Media::AudioFilter::SoundGen::BellSoundGen::~BellSoundGen()
{
	DEL_CLASS(this->soundMut)
}

void Media::AudioFilter::SoundGen::BellSoundGen::GenSignals(Double *buff, UOSInt sampleCnt)
{
	Double params[] = {
	1406, -1.0, 1.0,
	3750, -1.3, 1.0,
	6937, -6.28, 1.0,
	10780, -9.44, 1.0,
	15280, -49.1, 1.0,
	19310, -20.0, 0.5,
	21370, -49.0, 0.5,
	28680, -1.3, 1.0,
	32710, -1.3, 1.0,
	37210, -1.3, 1.0,
	41060, -1.3, 1.0,
	44250, -1.3, 1.0,
	46590, -1.3, 1.0
	};
	UInt32 paramCnt = 13;
	Double norVol;
	if (this->freq <= 48000)
	{
		paramCnt = 7;
	}


	Sync::MutexUsage mutUsage(this->soundMut);
	norVol = this->sampleVol / paramCnt;
	if (norVol <= 0)
	{
		mutUsage.EndUse();
		return;
	}

	if (this->currSample / (Double)this->freq >= 10.0)
	{
		this->sampleVol = 0;
		this->currSample = 0;
		mutUsage.EndUse();
		return;
	}

	UOSInt i;
	UOSInt k;
	Double t;
	i = 0;
	while (i < sampleCnt)
	{
		t = this->currSample / (Double)this->freq;
		this->currSample++;

		k = paramCnt;
		while (k-- > 0)
		{
			buff[i] += norVol * Math::Sin(t * Math::PI * 2 * params[k * 3]) * Math::Pow(10, t * params[k * 3 + 1]) * params[k * 3 + 2];
		}

		i++;
	}
	mutUsage.EndUse();
}

Media::AudioFilter::SoundGen::ISoundGen::SoundType Media::AudioFilter::SoundGen::BellSoundGen::GetSoundType()
{
	return Media::AudioFilter::SoundGen::ISoundGen::ST_BELL;
}

Bool Media::AudioFilter::SoundGen::BellSoundGen::GenSound(Double sampleVol)
{
	Sync::MutexUsage mutUsage(this->soundMut);
	this->sampleVol = sampleVol;
	this->currSample = 0;
	mutUsage.EndUse();
	return true;
}
