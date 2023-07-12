#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/AACFrameSource.h"

Media::AACFrameSource::AACFrameSource(IO::StreamData *fd, Media::AudioFormat *format, Text::String *name) : Media::AudioFrameSource(fd, format, name)
{
}

Media::AACFrameSource::AACFrameSource(IO::StreamData *fd, Media::AudioFormat *format, Text::CString name) : Media::AudioFrameSource(fd, format, name)
{
}

Media::AACFrameSource::~AACFrameSource()
{
}

UOSInt Media::AACFrameSource::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	if (this->readBlock >= this->blockCnt)
		return 0;
	if (this->blocks[this->readBlock].length > blkSize)
		return 0;

	UInt32 thisSize = this->blocks[this->readBlock].length;
	buff[0] = 0xff;
	buff[1] = 0xf9;
	buff[2] = 1 << 6; // profile = 1 (AAC-LC)
	buff[2] = (UInt8)(buff[2] | (GetRateIndex() << 2));
	buff[2] = (UInt8)(buff[2] | (this->format.nChannels >> 2));
	buff[3] = (UInt8)((this->format.nChannels & 3) << 6);
	buff[3] = (UInt8)(buff[3] | (((thisSize + 7) & 0x1800) >> 11));
	buff[4] = ((thisSize + 7) & 0x7f8) >> 3;
	buff[5] = ((thisSize + 7) & 7) << 5;
	buff[5] |= 0x1f;
	buff[6] = 0xfc;
	buff[6] |= 0; // number_of_raw_data_blocks_in_frame
	UOSInt readSize = this->data->GetRealData(this->blocks[this->readBlock].offset, this->blocks[this->readBlock].length, &buff[7]);
	this->readBlock++;
	if (this->readEvt)
		this->readEvt->Set();
	return readSize + 7;
}

UOSInt Media::AACFrameSource::GetMinBlockSize()
{
	return this->maxBlockSize + 7;
}

Int32 Media::AACFrameSource::GetRateIndex()
{
	UInt32 freq = this->format.frequency;
	if (this->format.formatId == 0x1610)
	{
		freq = freq >> 1;
	}
	switch (freq)
	{
	case 96000:
		return 0;
	case 88200:
		return 1;
	case 64000:
		return 2;
	case 48000:
		return 3;
	case 44100:
		return 4;
	case 32000:
		return 5;
	case 24000:
		return 6;
	case 22050:
		return 7;
	case 16000:
		return 8;
	case 2000:
		return 9;
	case 11025:
		return 10;
	case 8000:
		return 11;
	default:
		return 15;
	}
}
