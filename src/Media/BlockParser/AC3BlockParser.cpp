#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/BlockParser/AC3BlockParser.h"
Media::BlockParser::AC3BlockParser::AC3BlockParser() : Media::AudioBlockParser()
{
}

Media::BlockParser::AC3BlockParser::~AC3BlockParser()
{
}

Media::AudioBlockSource *Media::BlockParser::AC3BlockParser::ParseStreamData(NotNullPtr<IO::StreamData> stmData)
{
	static UInt32 bitrate[] = {32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 448, 512, 576, 640};
	static UInt32 frameSize32[] = {96, 96, 120, 120, 144, 144, 168, 168, 192, 192, 240, 240, 288, 288, 336, 336, 384, 384, 480, 480, 576, 576, 672, 672, 768, 768, 960, 960, 1152, 1152, 1344, 1344, 1536, 1536, 1728, 1728, 1920, 1920};
	static UInt32 frameSize44[] = {69, 70, 87,  88,  104, 105, 121, 122, 139, 140, 174, 175, 208, 209, 243, 244, 278, 279, 348, 349, 417, 418, 487, 488, 557, 558, 696, 697, 835,  836,  975,  976,  1114, 1115, 1253, 1254, 1393, 1394};
	static UInt32 frameSize48[] = {64, 64, 80,  80,  96,  96,  112, 112, 128, 128, 160, 160, 192, 192, 224, 224, 256, 256, 320, 320, 384, 384, 448, 448, 512, 512, 640, 640, 768,  768,  896,  896,  1024, 1024, 1152, 1152, 1280, 1280};
	UInt64 leng = stmData->GetDataSize();
	UInt8 buff[2048];
	UInt64 currOfst = 0;
	stmData->GetRealData(0, 2048, BYTEARR(buff));
	currOfst = 0;
	while (currOfst < 1920)
	{
		if (buff[currOfst] == 0xb && buff[currOfst + 1] == 0x77)
		{
			if (currOfst > 0)
			{
				MemCopyO(buff, &buff[currOfst], 7);
			}
			break;
		}
		currOfst++;
	}
	if (buff[0] != 0x0b || buff[1] != 0x77)
	{
		return 0;
	}
	UInt8 fscod = (buff[4] & 0xc0) >> 6;
	UInt8 frmsizecod = buff[4] & 0x3f;
//	UInt8 bsid = (buff[5] & 0xf8) >> 3;
//	UInt8 bsmod = buff[5] & 7;
	UInt8 acmod = (buff[6] & 0xe0) >> 5;
	Bool lfeon = false;
	switch (acmod)
	{
	case 0:
	case 1:
		lfeon = (buff[6] & 0x10) != 0;
		break;
	case 2:
	case 3:
	case 4:
	case 6:
		lfeon = (buff[6] & 0x4) != 0;
		break;
	case 5:
	case 7:
		lfeon = (buff[6] & 1) != 0;
		break;
	}

	Media::AudioBlockSource *audio;
	Media::AudioFormat format;
	format.formatId = 0x2000;
	switch (acmod)
	{
	case 1:
		format.nChannels = 1;
		break;
	case 0:
	case 2:
		format.nChannels = 2;
		break;
	case 3:
	case 4:
		format.nChannels = 3;
		break;
	case 5:
	case 6:
		format.nChannels = 4;
		break;
	case 7:
		format.nChannels = 5;
		break;
	default:
		format.nChannels = 2;
	}
	if (lfeon)
		format.nChannels = (UInt16)(format.nChannels + 1);
	switch (fscod)
	{
	case 0:
		format.frequency = 48000;
		break;
	case 1:
		format.frequency = 44100;
		break;
	case 2:
		format.frequency = 32000;
		break;
	}
	format.bitRate = bitrate[frmsizecod >> 1] * 1000;
	format.bitpersample = 16;
	format.extraSize = 0;
	format.align = 1;
	format.other = 0;
	format.intType = Media::AudioFormat::IT_NORMAL;
	format.extra = 0;

	NEW_CLASS(audio, Media::AudioBlockSource(stmData, format, stmData->GetFullName(), 1536));
	if (currOfst > 0)
	{
		audio->AddBlock(0, (UInt32)currOfst);
	}
	while (currOfst < leng)
	{
		UInt32 frameSize;
		if (buff[0] != 0x0b || buff[1] != 0x77)
		{
			break;
		}
		frmsizecod = buff[4] & 0x3f;
		if (format.frequency == 48000)
		{
			frameSize = frameSize48[frmsizecod] << 1;
		}
		else if (format.frequency == 44100)
		{
			frameSize = frameSize44[frmsizecod] << 1;
		}
		else if (format.frequency == 32000)
		{
			frameSize = frameSize32[frmsizecod] << 1;
		}
		else
		{
			break;
		}
		audio->AddBlock(currOfst, frameSize);
		currOfst += frameSize;
		if (currOfst >= leng)
			break;
		stmData->GetRealData(currOfst, 7, BYTEARR(buff));
	}
	return audio;
}

Bool Media::BlockParser::AC3BlockParser::ParseStreamFormat(UInt8 *buff, UOSInt buffSize, NotNullPtr<Media::AudioFormat> fmt)
{
	static UInt32 bitrate[] = {32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 448, 512, 576, 640};
	while (buffSize > 6)
	{
		if (buff[0] == 0xb && buff[1] == 0x77)
			break;
		buffSize--;
		buff++;
	}

	if (buff[0] != 0x0b || buff[1] != 0x77)
	{
		return false;
	}
	UInt8 fscod = (buff[4] & 0xc0) >> 6;
	UInt8 frmsizecod = buff[4] & 0x3f;
//	UInt8 bsid = (buff[5] & 0xf8) >> 3;
//	UInt8 bsmod = buff[5] & 7;
	UInt8 acmod = (buff[6] & 0xe0) >> 5;
	Bool lfeon = false;
	switch (acmod)
	{
	case 0:
	case 1:
		lfeon = (buff[6] & 0x10) != 0;
		break;
	case 2:
	case 3:
	case 4:
	case 6:
		lfeon = (buff[6] & 0x4) != 0;
		break;
	case 5:
	case 7:
		lfeon = (buff[6] & 1) != 0;
		break;
	}

	fmt->formatId = 0x2000;
	switch (acmod)
	{
	case 1:
		fmt->nChannels = 1;
		break;
	case 0:
	case 2:
		fmt->nChannels = 2;
		break;
	case 3:
	case 4:
		fmt->nChannels = 3;
		break;
	case 5:
	case 6:
		fmt->nChannels = 4;
		break;
	case 7:
		fmt->nChannels = 5;
		break;
	default:
		fmt->nChannels = 2;
	}
	if (lfeon)
		fmt->nChannels = (UInt16)(fmt->nChannels + 1);
	switch (fscod)
	{
	case 0:
		fmt->frequency = 48000;
		break;
	case 1:
		fmt->frequency = 44100;
		break;
	case 2:
		fmt->frequency = 32000;
		break;
	}
	fmt->bitRate = bitrate[frmsizecod >> 1] * 1000;
	fmt->bitpersample = 16;
	fmt->extraSize = 0;
	fmt->align = 1;
	fmt->other = 0;
	fmt->intType = Media::AudioFormat::IT_NORMAL;
	fmt->extra = 0;
	return true;
}
