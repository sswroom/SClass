#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AudioFilter/SoundGen/ISoundGen.h"

Media::AudioFilter::SoundGen::ISoundGen::ISoundGen(Int32 freq)
{
	this->freq = freq;
}

Media::AudioFilter::SoundGen::ISoundGen::~ISoundGen()
{
}
