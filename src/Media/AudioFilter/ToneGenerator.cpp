#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/AudioFilter/ToneGenerator.h"
#include "Text/MyString.h"

Media::AudioFilter::ToneGenerator::ToneGenerator(IAudioSource *sourceAudio) : Media::IAudioFilter(sourceAudio)
{
	this->sourceAudio = sourceAudio;
	sourceAudio->GetFormat(&this->format);
}

Media::AudioFilter::ToneGenerator::~ToneGenerator()
{
}

void Media::AudioFilter::ToneGenerator::GetFormat(AudioFormat *format)
{
	format->FromAudioFormat(&this->format);
}

UOSInt Media::AudioFilter::ToneGenerator::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	if (this->sourceAudio == 0)
		return 0;
	UOSInt readSize = this->sourceAudio->ReadBlock(buff, blkSize);

	if (this->format.bitpersample == 16)
	{
	}
	else if (this->format.bitpersample == 8)
	{
	}
	return readSize;
}

/*Bool Media::AudioFilter::ToneGenerator::GenTones(Int32 signalTime, Int32 breakTime, Double vol, const WChar *tones)
{
	return true;
}*/

Double Media::AudioFilter::ToneGenerator::GetToneFreq(ToneType tone)
{
	Int32 v;
	switch (tone)
	{
	case TT_A1:
		v = -36;
		break;
	case TT_A1S:
		v = -35;
		break;
	case TT_B1:
		v = -34;
		break;
	case TT_C1:
		v = -33;
		break;
	case TT_C1S:
		v = -32;
		break;
	case TT_D1:
		v = -31;
		break;
	case TT_D1S:
		v = -30;
		break;
	case TT_E1:
		v = -29;
		break;
	case TT_F1:
		v = -28;
		break;
	case TT_F1S:
		v = -27;
		break;
	case TT_G1:
		v = -26;
		break;
	case TT_G1S:
		v = -25;
		break;
	case TT_A2:
		v = -24;
		break;
	case TT_A2S:
		v = -23;
		break;
	case TT_B2:
		v = -22;
		break;
	case TT_C2:
		v = -21;
		break;
	case TT_C2S:
		v = -20;
		break;
	case TT_D2:
		v = -19;
		break;
	case TT_D2S:
		v = -18;
		break;
	case TT_E2:
		v = -17;
		break;
	case TT_F2:
		v = -16;
		break;
	case TT_F2S:
		v = -15;
		break;
	case TT_G2:
		v = -14;
		break;
	case TT_G2S:
		v = -13;
		break;
	case TT_A3:
		v = -12;
		break;
	case TT_A3S:
		v = -11;
		break;
	case TT_B3:
		v = -10;
		break;
	case TT_C3:
		v = -9;
		break;
	case TT_C3S:
		v = -8;
		break;
	case TT_D3:
		v = -7;
		break;
	case TT_D3S:
		v = -6;
		break;
	case TT_E3:
		v = -5;
		break;
	case TT_F3:
		v = -4;
		break;
	case TT_F3S:
		v = -3;
		break;
	case TT_G3:
		v = -2;
		break;
	case TT_G3S:
		v = -1;
		break;
	case TT_A4:
		v = 0;
		break;
	case TT_A4S:
		v = 1;
		break;
	case TT_B4:
		v = 2;
		break;
	case TT_C4:
		v = 3;
		break;
	case TT_C4S:
		v = 4;
		break;
	case TT_D4:
		v = 5;
		break;
	case TT_D4S:
		v = 6;
		break;
	case TT_E4:
		v = 7;
		break;
	case TT_F4:
		v = 8;
		break;
	case TT_F4S:
		v = 9;
		break;
	case TT_G4:
		v = 10;
		break;
	case TT_G4S:
		v = 11;
		break;
	case TT_A5:
		v = 12;
		break;
	case TT_A5S:
		v = 13;
		break;
	case TT_B5:
		v = 14;
		break;
	case TT_C5:
		v = 15;
		break;
	case TT_C5S:
		v = 16;
		break;
	case TT_D5:
		v = 17;
		break;
	case TT_D5S:
		v = 18;
		break;
	case TT_E5:
		v = 19;
		break;
	case TT_F5:
		v = 20;
		break;
	case TT_F5S:
		v = 21;
		break;
	case TT_G5:
		v = 22;
		break;
	case TT_G5S:
		v = 23;
		break;
	default:
		v = 0;
		break;
	}
	return 440.0 * Math::Pow(2, v / 12.0);
}

const WChar *Media::AudioFilter::ToneGenerator::GetToneName(ToneType tone)
{
	switch (tone)
	{
	case TT_A1:
		return L"A1";
	case TT_A1S:
		return L"A#1";
	case TT_B1:
		return L"B1";
	case TT_C1:
		return L"C1";
	case TT_C1S:
		return L"C#1";
	case TT_D1:
		return L"D1";
	case TT_D1S:
		return L"D#1";
	case TT_E1:
		return L"E1";
	case TT_F1:
		return L"F1";
	case TT_F1S:
		return L"F#1";
	case TT_G1:
		return L"G1";
	case TT_G1S:
		return L"G#1";
	case TT_A2:
		return L"A2";
	case TT_A2S:
		return L"A#2";
	case TT_B2:
		return L"B2";
	case TT_C2:
		return L"C2";
	case TT_C2S:
		return L"C#2";
	case TT_D2:
		return L"D2";
	case TT_D2S:
		return L"D#2";
	case TT_E2:
		return L"E2";
	case TT_F2:
		return L"F2";
	case TT_F2S:
		return L"F#2";
	case TT_G2:
		return L"G2";
	case TT_G2S:
		return L"G#2";
	case TT_A3:
		return L"A3";
	case TT_A3S:
		return L"A#3";
	case TT_B3:
		return L"B3";
	case TT_C3:
		return L"C3";
	case TT_C3S:
		return L"C#3";
	case TT_D3:
		return L"D3";
	case TT_D3S:
		return L"D#3";
	case TT_E3:
		return L"E3";
	case TT_F3:
		return L"F3";
	case TT_F3S:
		return L"F#3";
	case TT_G3:
		return L"G3";
	case TT_G3S:
		return L"G#3";
	case TT_A4:
		return L"A4";
	case TT_A4S:
		return L"A#4";
	case TT_B4:
		return L"B4";
	case TT_C4:
		return L"C4";
	case TT_C4S:
		return L"C#4";
	case TT_D4:
		return L"D4";
	case TT_D4S:
		return L"D#4";
	case TT_E4:
		return L"E4";
	case TT_F4:
		return L"F4";
	case TT_F4S:
		return L"F#4";
	case TT_G4:
		return L"G4";
	case TT_G4S:
		return L"G#4";
	case TT_A5:
		return L"A5";
	case TT_A5S:
		return L"A#5";
	case TT_B5:
		return L"B5";
	case TT_C5:
		return L"C5";
	case TT_C5S:
		return L"C#5";
	case TT_D5:
		return L"D5";
	case TT_D5S:
		return L"D#5";
	case TT_E5:
		return L"E5";
	case TT_F5:
		return L"F5";
	case TT_F5S:
		return L"F#5";
	case TT_G5:
		return L"G5";
	case TT_G5S:
		return L"G#5";
	default:
		return L"Unknown";
	}
}
