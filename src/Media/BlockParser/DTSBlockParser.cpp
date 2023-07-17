#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/BlockParser/DTSBlockParser.h"
Media::BlockParser::DTSBlockParser::DTSBlockParser() : Media::AudioBlockParser()
{
}

Media::BlockParser::DTSBlockParser::~DTSBlockParser()
{
}

Media::AudioBlockSource *Media::BlockParser::DTSBlockParser::ParseStreamData(IO::StreamData *stmData)
{
	UInt64 leng = stmData->GetDataSize();
	UInt8 buff[256];
	UInt64 currOfst = 0;
	stmData->GetRealData(0, 16, BYTEARR(buff));
	if (*(Int32*)&buff[0] != 0x180FE7F || (buff[4] & 0xfc) != 0xfc)
	{
		return 0;
	}
	UInt32 nblks = (UInt32)(((buff[4] & 1) << 6) + (buff[5] >> 2));
	UInt32 fsize = (UInt32)(((buff[5] & 3) << 12) + (buff[6] << 4) + (buff[7] >> 4));
	Int32 amode = ((buff[7] & 15) << 2) + ((buff[8] & 0xC0) >> 6);
	Int32 sfreq = (buff[8] & 0x3C) >> 2;
//	Int32 rate = ((buff[8] & 3) << 3) + ((buff[9] & 0xE0) >> 5);
//	Int32 ext_audio_id = ((buff[10] & 0xE0) >> 5);
//	Int32 ext_audio = ((buff[10] & 0x10) >> 4);
	Int32 lff = ((buff[10] & 6) >> 1);

	Media::AudioBlockSource *audio;
	Media::AudioFormat format;
	format.formatId = 0x2001;
	switch (amode)
	{
	case 0:
		format.nChannels = 1;
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		format.nChannels = 2;
		break;
	case 5:
	case 6:
		format.nChannels = 3;
		break;
	case 7:
	case 8:
		format.nChannels = 4;
		break;
	case 9:
		format.nChannels = 5;
		break;
	case 10:
	case 11:
	case 12:
		format.nChannels = 6;
		break;
	case 13:
		format.nChannels = 6;
		break;
	case 14:
	case 15:
		format.nChannels = 6;
		break;
	}
	if (lff == 1 || lff == 2)
		format.nChannels = (UInt16)(format.nChannels + 1);

	switch (sfreq)
	{
	case 1:
		format.frequency = 8000;
		break;
	case 2:
		format.frequency = 16000;
		break;
	case 3:
		format.frequency = 32000;
		break;
	case 6:
		format.frequency = 11025;
		break;
	case 7:
		format.frequency = 22050;
		break;
	case 8:
		format.frequency = 44100;
		break;
	case 11:
		format.frequency = 12000;
		break;
	case 12:
		format.frequency = 24000;
		break;
	case 13:
		format.frequency = 48000;
		break;
	default:
		format.formatId = 0;
		break;
	}
    format.bitRate = format.frequency * (fsize + 1) / (nblks + 1) / 4;
    format.align = 1;
    format.bitpersample = 0;
    format.extra = 0;
	format.extraSize = 0;
	format.other = 0;
	format.intType = Media::AudioFormat::IT_NORMAL;

	NEW_CLASS(audio, Media::AudioBlockSource(stmData, &format, stmData->GetFullName(), 1536));
	while (currOfst < leng)
	{
		if (*(Int32*)&buff[0] != 0x180FE7F || (buff[4] & 0xfc) != 0xfc)
		{
			return 0;
		}

		nblks = (UInt32)(((buff[4] & 1) << 6) + (buff[5] >> 2));
		fsize = (UInt32)(((buff[5] & 3) << 12) + (buff[6] << 4) + (buff[7] >> 4));
		amode = ((buff[7] & 15) << 2) + ((buff[8] & 0xC0) >> 6);
		sfreq = (buff[8] & 0x3C) >> 2;
//		rate = ((buff[8] & 3) << 3) + ((buff[9] & 0xE0) >> 5);
//		ext_audio_id = ((buff[10] & 0xE0) >> 5);
//		ext_audio = ((buff[10] & 0x10) >> 4);
		lff = ((buff[10] & 6) >> 1);

		audio->AddBlock(currOfst, fsize + 1);
		currOfst += fsize + 1;
		if (currOfst >= leng)
			break;
		stmData->GetRealData(currOfst, 16, BYTEARR(buff));
	}
	return audio;
}
