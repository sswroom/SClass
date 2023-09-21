#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/AudioFilter/ToneGenerator.h"
#include "Text/MyString.h"

Media::AudioFilter::ToneGenerator::ToneGenerator(NotNullPtr<IAudioSource> sourceAudio) : Media::IAudioFilter(sourceAudio)
{
	this->sourceAudio = sourceAudio;
	this->instType = IT_SINCWAVE;
	sourceAudio->GetFormat(this->format);
}

Media::AudioFilter::ToneGenerator::~ToneGenerator()
{
}

void Media::AudioFilter::ToneGenerator::GetFormat(NotNullPtr<AudioFormat> format)
{
	format->FromAudioFormat(this->format);
}

UOSInt Media::AudioFilter::ToneGenerator::ReadBlock(Data::ByteArray buff)
{
	UOSInt readSize = this->sourceAudio->ReadBlock(buff);

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
	return 440.0 * Math_Pow(2, v / 12.0);
}

Text::CString Media::AudioFilter::ToneGenerator::GetToneName(ToneType tone)
{
	switch (tone)
	{
	case TT_A1:
		return CSTR("A1");
	case TT_A1S:
		return CSTR("A#1");
	case TT_B1:
		return CSTR("B1");
	case TT_C1:
		return CSTR("C1");
	case TT_C1S:
		return CSTR("C#1");
	case TT_D1:
		return CSTR("D1");
	case TT_D1S:
		return CSTR("D#1");
	case TT_E1:
		return CSTR("E1");
	case TT_F1:
		return CSTR("F1");
	case TT_F1S:
		return CSTR("F#1");
	case TT_G1:
		return CSTR("G1");
	case TT_G1S:
		return CSTR("G#1");
	case TT_A2:
		return CSTR("A2");
	case TT_A2S:
		return CSTR("A#2");
	case TT_B2:
		return CSTR("B2");
	case TT_C2:
		return CSTR("C2");
	case TT_C2S:
		return CSTR("C#2");
	case TT_D2:
		return CSTR("D2");
	case TT_D2S:
		return CSTR("D#2");
	case TT_E2:
		return CSTR("E2");
	case TT_F2:
		return CSTR("F2");
	case TT_F2S:
		return CSTR("F#2");
	case TT_G2:
		return CSTR("G2");
	case TT_G2S:
		return CSTR("G#2");
	case TT_A3:
		return CSTR("A3");
	case TT_A3S:
		return CSTR("A#3");
	case TT_B3:
		return CSTR("B3");
	case TT_C3:
		return CSTR("C3");
	case TT_C3S:
		return CSTR("C#3");
	case TT_D3:
		return CSTR("D3");
	case TT_D3S:
		return CSTR("D#3");
	case TT_E3:
		return CSTR("E3");
	case TT_F3:
		return CSTR("F3");
	case TT_F3S:
		return CSTR("F#3");
	case TT_G3:
		return CSTR("G3");
	case TT_G3S:
		return CSTR("G#3");
	case TT_A4:
		return CSTR("A4");
	case TT_A4S:
		return CSTR("A#4");
	case TT_B4:
		return CSTR("B4");
	case TT_C4:
		return CSTR("C4");
	case TT_C4S:
		return CSTR("C#4");
	case TT_D4:
		return CSTR("D4");
	case TT_D4S:
		return CSTR("D#4");
	case TT_E4:
		return CSTR("E4");
	case TT_F4:
		return CSTR("F4");
	case TT_F4S:
		return CSTR("F#4");
	case TT_G4:
		return CSTR("G4");
	case TT_G4S:
		return CSTR("G#4");
	case TT_A5:
		return CSTR("A5");
	case TT_A5S:
		return CSTR("A#5");
	case TT_B5:
		return CSTR("B5");
	case TT_C5:
		return CSTR("C5");
	case TT_C5S:
		return CSTR("C#5");
	case TT_D5:
		return CSTR("D5");
	case TT_D5S:
		return CSTR("D#5");
	case TT_E5:
		return CSTR("E5");
	case TT_F5:
		return CSTR("F5");
	case TT_F5S:
		return CSTR("F#5");
	case TT_G5:
		return CSTR("G5");
	case TT_G5S:
		return CSTR("G#5");
	default:
		return CSTR("Unknown");
	}
}
