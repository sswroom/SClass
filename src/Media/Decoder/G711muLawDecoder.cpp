#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/Decoder/G711muLawDecoder.h"

Media::Decoder::G711muLawDecoder::G711muLawDecoder(Media::IAudioSource *sourceAudio)
{
	Media::AudioFormat fmt;
	this->sourceAudio = 0;
	this->readBuff = 0;
	this->readBuffSize = 0;
	this->align = 0;
	sourceAudio->GetFormat(&fmt);
	if (fmt.formatId != 0x7)
		return;
	if (fmt.bitpersample != 8)
		return;
	this->sourceAudio = sourceAudio;
	this->align = fmt.nChannels << 1;
}

Media::Decoder::G711muLawDecoder::~G711muLawDecoder()
{
	if (this->readBuff)
	{
		MemFree(this->readBuff);
		this->readBuff = 0;
	}
}

void Media::Decoder::G711muLawDecoder::GetFormat(AudioFormat *format)
{
	if (this->sourceAudio)
	{
		Media::AudioFormat fmt;
		this->sourceAudio->GetFormat(&fmt);
		format->formatId = 1;
		format->bitpersample = 16;
		format->frequency = fmt.frequency;
		format->nChannels = fmt.nChannels;
		format->bitRate = fmt.frequency * fmt.nChannels << 4;
		format->align = fmt.nChannels * 2;
		format->other = 0;
		format->intType = Media::AudioFormat::IT_NORMAL;
		format->extraSize = 0;
		format->extra = 0;
	}
	else
	{
		format->formatId = 0;
		format->bitpersample = 0;
		format->frequency = 0;
		format->nChannels = 0;
		format->bitRate = 0;
		format->align = 0;
		format->other = 0;
		format->intType = Media::AudioFormat::IT_NORMAL;
		format->extraSize = 0;
		format->extra = 0;
	}
}

Int32 Media::Decoder::G711muLawDecoder::SeekToTime(Int32 time)
{
	if (this->sourceAudio)
	{
		return this->sourceAudio->SeekToTime(time);
	}
	return 0;
}

Bool Media::Decoder::G711muLawDecoder::Start(Sync::Event *evt, UOSInt blkSize)
{
	if (this->sourceAudio)
	{
		this->sourceAudio->Start(0, blkSize >> 1);
		this->readEvt = evt;
		if (this->readEvt)
			this->readEvt->Set();
		return true;
	}
	return false;
}

void Media::Decoder::G711muLawDecoder::Stop()
{
	if (this->sourceAudio)
	{
		this->sourceAudio->Stop();
	}
	this->readEvt = 0;
}

UOSInt Media::Decoder::G711muLawDecoder::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	static Int16 table[] = {
    -32124, -31100, -30076, -29052, -28028, -27004, -25980, -24956,
    -23932, -22908, -21884, -20860, -19836, -18812, -17788, -16764,
    -15996, -15484, -14972, -14460, -13948, -13436, -12924, -12412,
    -11900, -11388, -10876, -10364,  -9852,  -9340,  -8828,  -8316,
     -7932,  -7676,  -7420,  -7164,  -6908,  -6652,  -6396,  -6140,
     -5884,  -5628,  -5372,  -5116,  -4860,  -4604,  -4348,  -4092,
     -3900,  -3772,  -3644,  -3516,  -3388,  -3260,  -3132,  -3004,
     -2876,  -2748,  -2620,  -2492,  -2364,  -2236,  -2108,  -1980,
     -1884,  -1820,  -1756,  -1692,  -1628,  -1564,  -1500,  -1436,
     -1372,  -1308,  -1244,  -1180,  -1116,  -1052,   -988,   -924,
      -876,   -844,   -812,   -780,   -748,   -716,   -684,   -652,
      -620,   -588,   -556,   -524,   -492,   -460,   -428,   -396,
      -372,   -356,   -340,   -324,   -308,   -292,   -276,   -260,
      -244,   -228,   -212,   -196,   -180,   -164,   -148,   -132,
      -120,   -112,   -104,    -96,    -88,    -80,    -72,    -64,
       -56,    -48,    -40,    -32,    -24,    -16,     -8,      0,
     32124,  31100,  30076,  29052,  28028,  27004,  25980,  24956,
     23932,  22908,  21884,  20860,  19836,  18812,  17788,  16764,
     15996,  15484,  14972,  14460,  13948,  13436,  12924,  12412,
     11900,  11388,  10876,  10364,   9852,   9340,   8828,   8316,
      7932,   7676,   7420,   7164,   6908,   6652,   6396,   6140,
      5884,   5628,   5372,   5116,   4860,   4604,   4348,   4092,
      3900,   3772,   3644,   3516,   3388,   3260,   3132,   3004,
      2876,   2748,   2620,   2492,   2364,   2236,   2108,   1980,
      1884,   1820,   1756,   1692,   1628,   1564,   1500,   1436,
      1372,   1308,   1244,   1180,   1116,   1052,    988,    924,
       876,    844,    812,    780,    748,    716,    684,    652,
       620,    588,    556,    524,    492,    460,    428,    396,
       372,    356,    340,    324,    308,    292,    276,    260,
       244,    228,    212,    196,    180,    164,    148,    132,
       120,    112,    104,     96,     88,     80,     72,     64,
        56,     48,     40,     32,     24,     16,      8,      0};

	if (this->align == 0 || this->sourceAudio == 0)
	{
		if (this->readEvt)
			this->readEvt->Set();
		return 0;
	}
	blkSize = blkSize / this->align * this->align;
	UOSInt readSize;
	UOSInt sofst = blkSize >> 1;
	UOSInt dofst = 0;
	UOSInt cnt;
	readSize = this->sourceAudio->ReadBlock(&buff[sofst], sofst);
	cnt = readSize;
	while (cnt-- > 0)
	{
		*(Int16*)&buff[dofst] = table[buff[sofst]];
		dofst += 2;
		sofst++;
	}

	if (this->readEvt)
		this->readEvt->Set();
	return readSize << 1;
}

UOSInt Media::Decoder::G711muLawDecoder::GetMinBlockSize()
{
	return this->align;
}
