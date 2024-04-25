#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/G711muLawRAWSource.h"

Media::G711muLawRAWSource::G711muLawRAWSource(NN<IO::StreamData> fd, Int64 ofst, Int64 length, const WChar *name) : Media::LPCMSource(name)
{
	Media::AudioFormat fmt;
	fmt.formatId = 7;
	fmt.frequency = 8000;
	fmt.bitpersample = 8;
	fmt.nChannels = 1;
	fmt.bitRate = 64000;
	fmt.align = 1;
	fmt.other = 0;
	fmt.intType = Media::AudioFormat::IT_NORMAL;
	fmt.extraSize = 0;
	fmt.extra = 0;
	this->SetData(fd, ofst, length, &fmt);
}

Media::G711muLawRAWSource::~G711muLawRAWSource()
{
}
