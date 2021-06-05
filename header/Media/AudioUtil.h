#ifndef _SM_MEDIA_AUDIOUTIL
#define _SM_MEDIA_AUDIOUTIL

extern "C"
{
	void AudioUtil_Amplify8(UInt8 *audioBuff, UOSInt buffSize, Int32 vol); //65536 = 1.0
	void AudioUtil_Amplify16(UInt8 *audioBuff, UOSInt buffSize, Int32 vol); //65536 = 1.0
}

#endif