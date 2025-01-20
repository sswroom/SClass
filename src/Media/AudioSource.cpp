#include "Stdafx.h"
#include "Sync/Event.h"
#include "Media/MediaSource.h"
#include "Media/AudioSource.h"

Media::MediaType Media::AudioSource::GetMediaType()
{
	return Media::MEDIA_TYPE_AUDIO;
}

UOSInt Media::AudioSource::ReadBlockLPCM(Data::ByteArray blk, NN<const AudioFormat> format)
{
	UOSInt ret;
	UOSInt i;
	UInt8 tmp;
	if (format->intType == Media::AudioFormat::IT_BIGENDIAN16)
	{
		ret = ReadBlock(blk);
		if (format->bitpersample == 16)
		{
			i = 0;
			while (i < ret)
			{
				tmp = blk[i];
				blk[i] = blk[i + 1];
				blk[i + 1] = tmp;
				i += 2;
			}
		}
		else if (format->bitpersample == 24)
		{
			if (format->nChannels == 2)
			{
/*
				buff[0]  = L1H
				buff[1]  = L1M
				buff[2]  = R1H
				buff[3]  = R1M
				buff[4]  = L2H
				buff[5]  = L2M
				buff[6]  = R2H
				buff[7]  = R2M
				buff[8]  = L1L
				buff[9]  = R1L
				buff[10] = L2L
				buff[11] = R2L

				buff[0]  = L1L
				buff[1]  = L1M
				buff[2]  = L1H
				buff[3]  = R1L
				buff[4]  = R1M
				buff[5]  = R1H
				buff[6]  = L2L
				buff[7]  = L2M
				buff[8]  = L2H
				buff[9]  = R2L
				buff[10] = R2M
				buff[11] = R2H
*/
				i = 0;
				while (i < ret)
				{
					tmp = blk[i + 0];
					blk[i + 0] = blk[i + 8];
					blk[i + 8] = blk[i + 4];
					blk[i + 4] = blk[i + 3];
					blk[i + 3] = blk[i + 9];
					blk[i + 9] = blk[i + 11];
					blk[i + 11] = blk[i + 6];
					blk[i + 6] = blk[i + 10];
					blk[i + 10] = blk[i + 7];
					blk[i + 7] = blk[i + 5];
					blk[i + 5] = blk[i + 2];
					blk[i + 2] = tmp;
					i += 12;
				}
			}
		}
		return ret;
	}
	else if (format->intType == Media::AudioFormat::IT_BIGENDIAN)
	{
		ret = ReadBlock(blk);
		if (format->bitpersample == 16)
		{
			i = 0;
			while (i < ret)
			{
				tmp = blk[i];
				blk[i] = blk[i + 1];
				blk[i + 1] = tmp;
				i += 2;
			}
		}
		else if (format->bitpersample == 24)
		{
			i = 0;
			while (i < ret)
			{
				tmp = blk[i];
				blk[i] = blk[i + 2];
				blk[i + 2] = tmp;
				i += 3;
			}
		}
		return ret;
	}
	else
	{
		return ReadBlock(blk);
	}
}

Bool Media::AudioSource::SupportSampleRead()
{
	return false;
}

UOSInt Media::AudioSource::ReadSample(UInt64 sampleOfst, UOSInt sampleCount, Data::ByteArray buff)
{
	return 0;
}

Int64 Media::AudioSource::GetSampleCount()
{
	return -1;
}
