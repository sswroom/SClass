#include "Stdafx.h"
#include <windows.h>
#include <mmreg.h>
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Sync/Mutex.h"
#include "Media/IAudioFilter.h"
#include "Media/AudioStream.h"
#include "IO/IStreamData.h"
#include "IO/StmData/FileData.h"
#include "IO/DataSegment.h"

AudioStream::AudioStream(WAVEFORMATEX *fmt, Data::ArrayList<IO::IStreamData*>* dList, WChar *name, Int32 audDelay)
{
	audioSource = MemAlloc(AUDIOSOURCE, 1);
	Int32 i;
	if (fmt->wFormatTag == 1)
	{
		audioSource->format = (WAVEFORMATEX*)MAlloc(16);
		MemCopy(audioSource->format, fmt, 16);
	}
	else
	{
		audioSource->format = (WAVEFORMATEX*)MAlloc(i = 20 + fmt->cbSize);
		MemCopy(audioSource->format, fmt, i);
	}
	audioSource->dataList = dList;
	NEW_CLASS(audioSource->filterList, Data::ArrayList<IAudioFilter*>());
	audioSource->audioDelay = audDelay;

	if (name == 0)
	{
		const WChar *nameW;
		nameW = ((IO::DataSegment*) dList->GetItem(0))->GetFd()->GetShortName();
		i = 0;
		while (nameW[i++]);
		audioSource->stmNameW = MemAlloc(WChar, i);
		MemCopy(audioSource->stmNameW, nameW, i * sizeof(WChar));
		audioSource->stmName = 0;
	}
	else
	{
		i = 0;
		while (name[i++]);
		audioSource->stmNameW = MemAlloc(WChar, i);
		MemCopy(audioSource->stmNameW, name, i * sizeof(WChar));
		audioSource->stmName = 0;
	}
	audioSource->noOfUsing = 1;
	//addAudioStream(this);
}

AudioStream::AudioStream(WAVEFORMATEX *fmt, Data::ArrayList<IO::IStreamData*>* dList, Data::ArrayList<Int32> *blockList, WChar *name)
{
}

AudioStream::~AudioStream()
{
	if (--(audioSource->noOfUsing) == 0)
	{
		long i;
		if (audioSource->dataList)
		{
			i = audioSource->dataList->GetCount();
			while (i--)
			{
				DEL_CLASS((IO::DataSegment*)audioSource->dataList->RemoveAt(i));
			}
			DEL_CLASS(audioSource->dataList);
		}
		if (audioSource->stmName)
		{
			MemFree(audioSource->stmName);
			audioSource->stmName = 0;
		}
		if (audioSource->stmNameW)
		{
			MemFree(audioSource->stmNameW);
			audioSource->stmNameW = 0;
		}
		
		while ((i = audioSource->filterList->GetCount()) > 0)
		{
			delete audioSource->filterList->RemoveAt(--i);
		}
		DEL_CLASS(audioSource->filterList);
		MemFree(audioSource->format);
		MemFree(audioSource);
		audioSource = 0;
	}
}

WAVEFORMATEX *AudioStream::getFormat()
{
	if (audioSource == 0)
		return 0;
  	if (audioSource->filterList->GetCount() == 0)
		return audioSource->format;
	else
		return ((IAudioFilter*)audioSource->filterList->GetItem(audioSource->filterList->GetCount() - 1))->getOutputFormat();
}

__int64 AudioStream::GetVBRPos(__int64 decOfst)
{
	if (audioSource->format->wFormatTag == 0x55)
	{
		long nPack = (long)(decOfst / 1152 / audioSource->format->nChannels / 2);

		if (audioSource->format->nAvgBytesPerSec % 2000)
		{
			long i = 0;
			long j = audioSource->dataList->GetCount();
			long k;
			IO::DataSegment *ds;
			__int64 length = 0;
			while (i < j)
			{
				ds = (IO::DataSegment*)audioSource->dataList->GetItem(i++);
				k = ds->GetCount();
				while (k--)
				{
					if (nPack-- <= 0)
						return length;
					length += ds->datas[k].length;
				}
			}
			return length;
		}
		else
		{
			long frSize = 1152 * audioSource->format->nAvgBytesPerSec / audioSource->format->nSamplesPerSec;
			return nPack * frSize;
		}
	}
	else
	{
		__int64 outOfst = decOfst * audioSource->format->nAvgBytesPerSec / audioSource->format->nSamplesPerSec / 2;
		return outOfst / audioSource->format->nBlockAlign * audioSource->format->nBlockAlign;
	}
}

__int64 AudioStream::getDataLength()
{
	long i = 0;
	long j = audioSource->dataList->GetCount();
	long k;
	IO::DataSegment *ds;
	__int64 length = 0;
	while (i < j)
	{
		ds = (IO::DataSegment*)audioSource->dataList->GetItem(i++);
		k = ds->GetCount();
		while (k--)
			length += ds->datas[k].length;
	}
	if (audioSource->filterList->GetCount() == 0)
		return length;
	else
		return length * ((IAudioFilter*)audioSource->filterList->GetItem(audioSource->filterList->GetCount() - 1))->getOutputFormat()->nAvgBytesPerSec / audioSource->format->nAvgBytesPerSec;
}

char *AudioStream::getName()
{
	return audioSource->stmName;
}

WChar *AudioStream::getNameW()
{
	return audioSource->stmNameW;
}

Int32 AudioStream::getDelay()
{
	return audioSource->audioDelay;
}

Int32 AudioStream::getRAWData(Int64 offset, UInt32 length, UInt8 *buffer)
{
	long i = -1;
	long j = audioSource->dataList->GetCount();
	long k;
	long l;
	IO::DataSegment *ds;
	long writtenSize = 0;
	__int64 currSize;
	while (++i < j && length)
	{
		ds = (IO::DataSegment*)audioSource->dataList->GetItem(i);
		l = 0;
		while (l < ds->GetCount() && length)
		{
			//currSize = ((FileData*)audioSource->dataList->get(i))->getDataSize();
			currSize = ds->GetLength(l);
			if (currSize <= offset)
			{
				offset -= currSize;
			}
			else
			{
				k = ds->GetFd()->GetRealData(ds->datas[l].offset + offset, (length + offset > ds->datas[l].length)?(ds->datas[l].length - offset):length, buffer);
				offset = 0;
				buffer += k;
				length -= k;
				writtenSize += k;
			}
			l++;
		}
	}
	if (writtenSize == 0)
		if (length != 0)
			writtenSize = 0;
	return writtenSize;
}

Int32 AudioStream::getData(Int64 offset, UInt32 length, UInt8 *buffer)
{
	if (audioSource->filterList->GetCount() == 0)
		return getRAWData(offset, length, buffer);

	return audioSource->filterList->GetItem(audioSource->filterList->GetCount() - 1)->getData(audioSource->filterList->GetCount() - 1, AudioCallback, (long) this, offset, length, buffer);
}

long _stdcall AudioStream::AudioCallback(Int32 filterNo, UINT msg, Int32 userData, Int32 msgData1, Int32 msgData2, Int32 msgData3, Int32 msgData4)
{
	AudioStream *as = (AudioStream*)userData;
	__int64 size;
	long i;
	IAudioFilter *filter;
	switch (msg)
	{
	case IAFM_GETDATA:
		if (filterNo--)
			return as->audioSource->filterList->GetItem(filterNo)->getData(filterNo, AudioCallback, (long)as, msgData1 + ((Int64)msgData2 << 32), msgData3, (UInt8 *)msgData4);
		else
			return as->getRAWData(msgData1 + ((Int64)msgData2 << 32), msgData3, (UInt8*)msgData4);
	case IAFM_GETSTMSIZE:
		size = as->getDataLength();
		i = -1;
		while (++i < filterNo)
		{
			filter = (IAudioFilter*)as->audioSource->filterList->GetItem(i);
			size = size * filter->getOutputFormat()->nAvgBytesPerSec / filter->getSourceFormat()->nAvgBytesPerSec;
		}
		*(__int64*)msgData1 = size;
		return 1;
	case IAFM_GETBLKCOUNT:
		if (filterNo--)
		{
			*(long*)msgData1 = 1;
			return 0;
		}
		else
		{
			*(long*)msgData1 = ((IO::DataSegment*)as->audioSource->dataList->GetItem(0))->GetCount();

		}
		break;
	case IAFM_GETVBROFST:
		*(__int64*)msgData2 = *(__int64*)msgData1;
		return as->GetVBRPos(*(__int64*)msgData1);
	case IAFM_GETBLKOFFSET:
//AudioCallback(long filterNo, UINT msg, long userData, long srcOffset, long* output, long unused2, long unused3);
		if (filterNo--)
		{
			*(long*)msgData2 = 0;
			return 0;
		}
		else
		{
			if (((IO::DataSegment*)as->audioSource->dataList->GetItem(0))->GetCount() > msgData1)
			{
				*(long*)msgData2 = (long)as->getDataLength();
				return 0;
			}
			else
			{
				////////////////////////////////////////
				*(long*)msgData2 = (long)as->getDataLength();
				return 0;
			}
		}
		break;
	}
	return 0;
}

Int32 AudioStream::removeLastFilter()
{
	if (audioSource->filterList->GetCount())
	{
		delete ((IAudioFilter*)audioSource->filterList->RemoveAt(audioSource->filterList->GetCount() - 1));
		return 0;
	}
	return 1;
}

Int32 AudioStream::addFilter(IAudioFilter *filter)
{
	if (filter->hasError())
	{
		delete filter;
		return 1;
	}
	audioSource->filterList->Add(filter);
	return 0;
}

Int64 AudioStream::getOffset()
{
	if (audioSource->filterList->GetCount())
	{
		return ((IAudioFilter*)audioSource->filterList->GetItem(audioSource->filterList->GetCount() - 1))->getOffset();
	}
	return 0;
}
