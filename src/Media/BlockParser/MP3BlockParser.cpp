#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Media/BlockParser/MP3BlockParser.h"
Media::BlockParser::MP3BlockParser::MP3BlockParser() : Media::AudioBlockParser()
{
}

Media::BlockParser::MP3BlockParser::~MP3BlockParser()
{
}

Media::AudioBlockSource *Media::BlockParser::MP3BlockParser::ParseStreamData(IO::IStreamData *stmData)
{
	static UInt32 bitrateL3[] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320};
	UInt64 leng = stmData->GetDataSize();
	UInt8 buff[256];
	UInt64 currOfst = 0;
	stmData->GetRealData(0, 7, buff);
	if (buff[0] != 0xff || (buff[1] & 0xfe) != 0xfa)
	{
		return 0;
	}
	UInt8 bitrateIndex = (buff[2] & 0xF0) >> 4;
	UInt8 samplingFrequency = ((buff[2] & 0xC) >> 2);
	UInt8 paddingBit = (buff[2] & 2) >> 1;
	UInt8 mode = (buff[3] & 0xC0) >> 6;
	UInt32 frameLength = 0;

	if (samplingFrequency == 0)
	{
		frameLength = ((UInt32)(144 * bitrateL3[bitrateIndex] / 44.1)) + paddingBit;
	}
	else if (samplingFrequency == 1)
	{
		frameLength = 144 * bitrateL3[bitrateIndex] / 48;
	}
	else if (samplingFrequency == 2)
	{
		frameLength = 144 * bitrateL3[bitrateIndex] / 32;
	}

	Media::AudioBlockSource *audio;
	Media::AudioFormat format;
	format.formatId = 0x55;
	switch (mode)
	{
	case 0:
	case 1:
	case 2:
		format.nChannels = 2;
		break;
	case 3:
		format.nChannels = 1;
		break;
	}
	switch (samplingFrequency)
	{
	case 0:
		format.frequency = 44100;
		break;
	case 1:
		format.frequency = 48000;
		break;
	case 2:
		format.frequency = 32000;
		break;
	}
	//format.nAvgBytesPerSec = CInt(totalSize * format.nSamplesPerSec \ 1152 \ frameList.Count)
	format.bitpersample = 0;
	format.extraSize = 12;
	format.extra = MemAlloc(UInt8, 12);
	format.align = 1152; //nBlockAlign
	format.other = 0;
	format.intType = Media::AudioFormat::IT_NORMAL;
	WriteUInt16(&format.extra[0], 1);
	WriteUInt32(&format.extra[2], 2);
	WriteUInt32(&format.extra[6], 66236);
	WriteUInt16(&format.extra[10], 0);
	UInt32 blkCnt = 0;

	NEW_CLASS(audio, Media::AudioBlockSource(stmData, &format, stmData->GetFullName(), 1152));
	while (currOfst < leng)
	{
		if (buff[0] != 0xff || (buff[1] & 0xfe) != 0xfa)
		{
			break;
		}
		bitrateIndex = (buff[2] & 0xF0) >> 4;
		samplingFrequency = ((buff[2] & 0xC) >> 2);
		paddingBit = (buff[2] & 2) >> 1;
		if (samplingFrequency == 0)
		{
			frameLength = ((UInt32)(144 * bitrateL3[bitrateIndex] / 44.1)) + paddingBit;
		}
		else if (samplingFrequency == 1)
		{
			frameLength = 144 * bitrateL3[bitrateIndex] / 48;
		}
		else if (samplingFrequency == 2)
		{
			frameLength = 144 * bitrateL3[bitrateIndex] / 32;
		}
		audio->AddBlock(currOfst, frameLength);
		blkCnt++;
		currOfst += frameLength;
		if (currOfst >= leng)
			break;
		stmData->GetRealData(currOfst, 4, buff);
	}
	audio->UpdateBitRate((UInt32)(currOfst * format.frequency / 1152 / blkCnt));
	return audio;
}

Bool Media::BlockParser::MP3BlockParser::ParseStreamFormat(UInt8 *buff, UOSInt buffSize, Media::AudioFormat *fmt)
{
	if (buff[0] != 0xff || (buff[1] & 0xfe) != 0xfa)
	{
		return false;
	}
//	UInt8 bitrateIndex = (buff[2] & 0xF0) >> 4;
	UInt8 samplingFrequency = ((buff[2] & 0xC) >> 2);
//	UInt8 paddingBit = (buff[2] & 2) >> 1;
	UInt8 mode = (buff[3] & 0xC0) >> 6;
//	Int32 frameLength = 0;

	if (samplingFrequency == 0)
	{
//		frameLength = ((Int32)(144 * bitrateL3[bitrateIndex] / 44.1)) + paddingBit;
	}
	else if (samplingFrequency == 1)
	{
//		frameLength = 144 * bitrateL3[bitrateIndex] / 48;
	}
	else if (samplingFrequency == 2)
	{
//		frameLength = 144 * bitrateL3[bitrateIndex] / 32;
	}

	fmt->formatId = 0x55;
	switch (mode)
	{
	case 0:
	case 1:
	case 2:
		fmt->nChannels = 2;
		break;
	case 3:
		fmt->nChannels = 1;
		break;
	}
	switch (samplingFrequency)
	{
	case 0:
		fmt->frequency = 44100;
		break;
	case 1:
		fmt->frequency = 48000;
		break;
	case 2:
		fmt->frequency = 32000;
		break;
	}
	//format.nAvgBytesPerSec = CInt(totalSize * format.nSamplesPerSec \ 1152 \ frameList.Count)
	fmt->bitpersample = 0;
	fmt->extraSize = 12;
	if (fmt->extra)
	{
		MemFree(fmt->extra);
	}
	fmt->extra = MemAlloc(UInt8, 12);
	fmt->align = 1152; //nBlockAlign
	fmt->other = 0;
	fmt->intType = Media::AudioFormat::IT_NORMAL;
	*(Int16*)&fmt->extra[0] = 1;
	*(Int32*)&fmt->extra[2] = 2;
	*(Int32*)&fmt->extra[6] = 66236;
	*(Int16*)&fmt->extra[10] = 0;
	return true;
}
