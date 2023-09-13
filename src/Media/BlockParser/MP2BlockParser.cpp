#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/BlockParser/MP2BlockParser.h"
Media::BlockParser::MP2BlockParser::MP2BlockParser() : Media::AudioBlockParser()
{
}

Media::BlockParser::MP2BlockParser::~MP2BlockParser()
{
}

Media::AudioBlockSource *Media::BlockParser::MP2BlockParser::ParseStreamData(NotNullPtr<IO::StreamData> stmData)
{
	static UInt32 bitrateL2[] = {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384};
	UInt64 leng = stmData->GetDataSize();
	UInt8 buff[256];
	UInt64 currOfst = 0;
	stmData->GetRealData(0, 7, BYTEARR(buff));
	if (buff[0] != 0xff || (buff[1] & 0xfe) != 0xfc)
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
		frameLength = ((UInt32)(144 * bitrateL2[bitrateIndex] / 44.1)) + paddingBit;
	}
	else if (samplingFrequency == 1)
	{
		frameLength = 144 * bitrateL2[bitrateIndex] / 48;
	}
	else if (samplingFrequency == 2)
	{
		frameLength = 144 * bitrateL2[bitrateIndex] / 32;
	}

	Media::AudioBlockSource *audio;
	Media::AudioFormat format;
	format.formatId = 0x50;
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
	format.bitRate = bitrateL2[bitrateIndex] * 1000;
	format.bitpersample = 0;
	format.extraSize = 12;
	format.extra = MemAlloc(UInt8, 12);
	format.align = 1; //nBlockAlign
	format.other = 0;
	format.intType = Media::AudioFormat::IT_NORMAL;
	*(Int16*)&format.extra[0] = 2;
	*(Int32*)&format.extra[2] = 211712;
	*(Int32*)&format.extra[6] = 65537;
	*(Int16*)&format.extra[10] = 1;

	NEW_CLASS(audio, Media::AudioBlockSource(stmData, format, stmData->GetFullName(), 1152));
	while (currOfst < leng)
	{
		if (buff[0] != 0xff || (buff[1] & 0xfe) != 0xfc)
		{
			break;
		}
		bitrateIndex = (buff[2] & 0xF0) >> 4;
		samplingFrequency = ((buff[2] & 0xC) >> 2);
		paddingBit = (buff[2] & 2) >> 1;
		if (samplingFrequency == 0)
		{
			frameLength = ((UInt32)(144 * bitrateL2[bitrateIndex] / 44.1)) + paddingBit;
		}
		else if (samplingFrequency == 1)
		{
			frameLength = 144 * bitrateL2[bitrateIndex] / 48;
		}
		else if (samplingFrequency == 2)
		{
			frameLength = 144 * bitrateL2[bitrateIndex] / 32;
		}
		audio->AddBlock(currOfst, frameLength);
		currOfst += frameLength;
		if (currOfst >= leng)
			break;
		stmData->GetRealData(currOfst, 4, BYTEARR(buff));
	}
	return audio;
}

Bool Media::BlockParser::MP2BlockParser::ParseStreamFormat(UInt8 *buff, UOSInt buffSize, NotNullPtr<Media::AudioFormat> fmt)
{
	static UInt32 bitrateL2[] = {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384};
	if (buff[0] != 0xff || (buff[1] & 0xfe) != 0xfc)
	{
		return false;
	}
	UInt8 bitrateIndex = (buff[2] & 0xF0) >> 4;
	UInt8 samplingFrequency = ((buff[2] & 0xC) >> 2);
//	UInt8 paddingBit = (buff[2] & 2) >> 1;
	UInt8 mode = (buff[3] & 0xC0) >> 6;
//	Int32 frameLength = 0;

	if (samplingFrequency == 0)
	{
//		frameLength = ((Int32)(144 * bitrateL2[bitrateIndex] / 44.1)) + paddingBit;
	}
	else if (samplingFrequency == 1)
	{
//		frameLength = 144 * bitrateL2[bitrateIndex] / 48;
	}
	else if (samplingFrequency == 2)
	{
//		frameLength = 144 * bitrateL2[bitrateIndex] / 32;
	}

	fmt->formatId = 0x50;
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
	fmt->bitRate = bitrateL2[bitrateIndex] * 1000;
	fmt->bitpersample = 0;
	fmt->extraSize = 12;
	if (fmt->extra)
	{
		MemFree(fmt->extra);
	}
	fmt->extra = MemAlloc(UInt8, 12);
	fmt->align = 1; //nBlockAlign
	fmt->other = 0;
	fmt->intType = Media::AudioFormat::IT_NORMAL;
	*(Int16*)&fmt->extra[0] = 2;
	*(Int32*)&fmt->extra[2] = 211712;
	*(Int32*)&fmt->extra[6] = 65537;
	*(Int16*)&fmt->extra[10] = 1;
	return true;
}
