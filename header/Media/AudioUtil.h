#ifndef _SM_MEDIA_AUDIOUTIL
#define _SM_MEDIA_AUDIOUTIL

extern "C"
{
	void AudioUtil_Amplify8(UInt8 *audioBuff, UOSInt buffSize, Int32 vol); //65536 = 1.0
	void AudioUtil_Amplify16(UInt8 *audioBuff, UOSInt buffSize, Int32 vol); //65536 = 1.0

	void AudioUtil_ConvI16_F32(const UInt8 *srcBuff, UInt8 *destBuff, UOSInt nSample);
	void AudioUtil_ConvF32_I16(const UInt8 *srcBuff, UInt8 *destBuff, UOSInt nSample);
}

#endif