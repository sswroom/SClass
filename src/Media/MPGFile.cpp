#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Media/M2VStreamSource.h"
#include "Media/MPAStreamSource.h"
#include "Media/MPEGVideoParser.h"
#include "Media/MPGFile.h"
#include "Media/VOBAC3StreamSource.h"
#include "Media/VOBLPCMStreamSource.h"
#include "Sync/Thread.h"
#define BUFFSIZE 1048576

UInt32 __stdcall Media::MPGFile::PlayThread(void *userData)
{
	Media::MPGFile *me = (Media::MPGFile*)userData;
	OSInt buffSize;
	OSInt readSize;
	OSInt i;
	OSInt j;
	OSInt endOfst;
	Int32 thisSize;
	Int64 scr_base;
//	Int64 scr_ext;
	Int64 initScr = -1;
	Int64 last_scr_base = -1;
	Media::IMediaStream *mstm;
	Data::ArrayList<Media::IMediaStream *> *stmList;
	Int32 lastFrameTime = 0;

	me->playStarted = true;
	me->playing = 2;
	me->vstm->ClearFrameBuff();
	me->vstm->SetStreamTime(me->startTime);
	buffSize = 0;
	Bool firstAudio = true;
	Bool firstVideo = false;
	Bool endFound = false;
	while (!me->playToStop)
	{
		readSize = me->stmData->GetRealData(me->readOfst, BUFFSIZE - buffSize, &me->readBuff[buffSize]);
		buffSize += readSize;
		endOfst = buffSize - 3;
		if (readSize == 0)
		{
			if (!endFound)
			{
				me->vstm->EndFrameStream();
			}
			me->readOfst = 0;
			break;
		}
		i = 0;
		while (i < endOfst)
		{
			if (me->readBuff[i] == 0 && me->readBuff[i + 1] == 0 && me->readBuff[i + 2] == 1)
			{
				if (me->readBuff[i + 3] == 0xb9) // iso_11172_end_code (End Of File)
				{
					endFound = true;
					me->vstm->EndFrameStream();
					stmList = me->dataStms->GetValues();
					j = stmList->GetCount();
					while (j-- > 0)
					{
						mstm = stmList->GetItem(j);
						mstm->EndFrameStream();
					}

					i += 4;
				}
				else if (me->readBuff[i + 3] == 0xba) // pack_start_code
				{
					if (me->mpgVer == 1)
					{
						if (buffSize - i < 12)
						{
							break;
						}
						scr_base = (((Int64)(me->readBuff[i + 4] & 0xe)) << 29) | (me->readBuff[i + 5] << 22) | ((me->readBuff[i + 6] & 0xfe) << 14) | (me->readBuff[i + 7] << 7) | ((me->readBuff[i + 8] & 0xfe) >> 1);
//						scr_ext = 0;
						i += 12;
					}
					else
					{
						if (buffSize - i < 14)
						{
							break;
						}
						scr_base = (((Int64)(me->readBuff[i + 4] & 0x38)) << 27) | ((me->readBuff[i + 4] & 3) << 28) | (me->readBuff[i + 5] << 20) | ((me->readBuff[i + 6] & 0xf8) << 12) | ((me->readBuff[i + 6] & 3) << 13) | (me->readBuff[i + 7] << 5) | (me->readBuff[i + 8] >> 3);
//						scr_ext = ((me->readBuff[i + 8] & 3) << 7) | (me->readBuff[i + 9] >> 1);
						i += 14 + (me->readBuff[i + 13] & 7);
					}
					if (initScr == -1)
					{
						initScr = scr_base;
					}
					if (scr_base < last_scr_base || scr_base > last_scr_base + 9000)
					{
						firstAudio = true;
						firstVideo = false;
					}
					last_scr_base = scr_base;
				}
				else if (me->readBuff[i + 3] == 0xbb) //system_header_start_code
				{
					if (buffSize - i < 6)
						break;
					thisSize = ReadMUInt16(&me->readBuff[i + 4]);
					if (buffSize - i < 6 + thisSize)
					{
						break;
					}

					i += thisSize + 6;
				}
				else if (me->readBuff[i + 3] == 0xbc) //program_stream_map
				{
					if (buffSize - i < 6)
						break;
					thisSize = ReadMUInt16(&me->readBuff[i + 4]);
					if (buffSize - i < 6 + thisSize)
					{
						break;
					}

					i += thisSize + 6;
				}
				else if (me->readBuff[i + 3] == 0xbd) //Private Stream 1
				{
					if (buffSize - i < 6)
						break;
					thisSize = ReadMUInt16(&me->readBuff[i + 4]);
					if (buffSize - i < 6 + thisSize)
					{
						break;
					}

					Int64 pts = 0;
					Int64 dts = 0;
					UInt8 stmHdrSize = me->readBuff[i + 8];
					if ((me->readBuff[i + 7] & 0xc0) == 0x80)
					{
						pts = (((Int64)(me->readBuff[i + 9] & 0xe)) << 29) | (me->readBuff[i + 10] << 22) | ((me->readBuff[i + 11] & 0xfe) << 14) | (me->readBuff[i + 12] << 7) | (me->readBuff[i + 13] >> 1);
						dts = pts;
					}
					else if ((me->readBuff[i + 7] & 0xc0) == 0xc0)
					{
						pts = (((Int64)(me->readBuff[i + 9] & 0xe)) << 29) | (me->readBuff[i + 10] << 22) | ((me->readBuff[i + 11] & 0xfe) << 14) | (me->readBuff[i + 12] << 7) | (me->readBuff[i + 13] >> 1);
						dts = (((Int64)(me->readBuff[i + 14] & 0xe)) << 29) | (me->readBuff[i + 15] << 22) | ((me->readBuff[i + 16] & 0xfe) << 14) | (me->readBuff[i + 17] << 7) | (me->readBuff[i + 18] >> 1);
					}
					UInt8 stmType = me->readBuff[i + 9 + stmHdrSize];
					mstm = me->dataStms->Get(stmType);
					if (mstm)
					{
						if (firstAudio && dts != 0)
						{
							initScr = dts - 90 * mstm->GetFrameStreamTime();
							firstVideo = true;
							firstAudio = false;
						}
						mstm->WriteFrameStream(&me->readBuff[i + 10 + stmHdrSize], thisSize - 4 - stmHdrSize);
					}

					i += thisSize + 6;
				}
				else if (me->readBuff[i + 3] == 0xbe) //padding stream
				{
					if (buffSize - i < 6)
						break;
					thisSize = ReadMUInt16(&me->readBuff[i + 4]);
					if (buffSize - i < 6 + thisSize)
					{
						break;
					}

					i += thisSize + 6;
				}
				else if (me->readBuff[i + 3] == 0xbf) //Private Stream 2
				{
					if (buffSize - i < 6)
						break;
					thisSize = ReadMUInt16(&me->readBuff[i + 4]);
					if (buffSize - i < 6 + thisSize)
					{
						break;
					}

					i += thisSize + 6;
				}
				else if ((me->readBuff[i + 3] & 0xe0) == 0xc0) //Audio Stream 1
				{
					if (buffSize - i < 6)
						break;
					thisSize = ReadMUInt16(&me->readBuff[i + 4]);
					if (buffSize - i < 6 + thisSize)
					{
						break;
					}

					Int64 pts = 0;
					Int64 dts = 0;
					j = 6;
					if (me->mpgVer == 1)
					{
						while (me->readBuff[i + j] & 0x80)
						{
							j++;
						}
						if ((me->readBuff[i + j] & 0xc0) == 0x40)
						{
							j += 2;
						}

						if ((me->readBuff[i + j] & 0xf0) == 0x20)
						{
							pts = (((Int64)(me->readBuff[i + j] & 0xe)) << 29) | (me->readBuff[i + j + 1] << 22) | ((me->readBuff[i + j + 2] & 0xfe) << 14) | (me->readBuff[i + j + 3] << 7) | (me->readBuff[i + j + 4] >> 1);
							dts = pts;
							j += 5;
						}
						else if ((me->readBuff[i + j] & 0xf0) == 0x30)
						{
							pts = (((Int64)(me->readBuff[i + j] & 0xe)) << 29) | (me->readBuff[i + j + 1] << 22) | ((me->readBuff[i + j + 2] & 0xfe) << 14) | (me->readBuff[i + j + 3] << 7) | (me->readBuff[i + j + 4] >> 1);
							dts = (((Int64)(me->readBuff[i + j + 5] & 0xe)) << 29) | (me->readBuff[i + j + 6] << 22) | ((me->readBuff[i + j + 7] & 0xfe) << 14) | (me->readBuff[i + j + 8] << 7) | (me->readBuff[i + j + 9] >> 1);
							j += 10;
						}
						else if (me->readBuff[j] == 0xf)
						{
							j++;
						}
						else
						{
						}
					}
					else
					{
						j++;
						if ((me->readBuff[i + j] & 0xf0) == 0x20)
						{
							j += 2;
							pts = (((Int64)(me->readBuff[i + j] & 0xe)) << 29) | (me->readBuff[i + j + 1] << 22) | ((me->readBuff[i + j + 2] & 0xfe) << 14) | (me->readBuff[i + j + 3] << 7) | (me->readBuff[i + j + 4] >> 1);
							dts = pts;
							j += me->readBuff[i + j - 1];
						}
						else if ((me->readBuff[i + j] & 0xf0) == 0x30)
						{
							j += 2;
							pts = (((Int64)(me->readBuff[i + j] & 0xe)) << 29) | (me->readBuff[i + j + 1] << 22) | ((me->readBuff[i + j + 2] & 0xfe) << 14) | (me->readBuff[i + j + 3] << 7) | (me->readBuff[i + j + 4] >> 1);
							dts = (((Int64)(me->readBuff[i + j + 5] & 0xe)) << 29) | (me->readBuff[i + j + 6] << 22) | ((me->readBuff[i + j + 7] & 0xfe) << 14) | (me->readBuff[i + j + 8] << 7) | (me->readBuff[i + j + 9] >> 1);
							j += me->readBuff[i + j - 1];
						}
						else
						{
							j += me->readBuff[i + j + 1] + 2;
						}
					}
					UInt8 stmType = me->readBuff[i + 3] & 0x1f;
					mstm = me->dataStms->Get(stmType);
					if (mstm)
					{
						if (firstAudio && dts != 0)
						{
							initScr = dts - 90 * mstm->GetFrameStreamTime();
							firstVideo = true;
							firstAudio = false;
						}
						mstm->WriteFrameStream(&me->readBuff[i + j], thisSize - j + 6);
					}

					i += thisSize + 6;
				}
				else if ((me->readBuff[i + 3] & 0xf0) == 0xe0) //Video stream
				{
					if (buffSize - i < 6)
						break;
					thisSize = ReadMUInt16(&me->readBuff[i + 4]);
					Int64 pts = 0;
					Int64 dts = 0;
					Int32 stmHdrSize;

					if (buffSize - i < 6 + thisSize)
					{
						break;
					}
					if (me->mpgVer == 1)
					{
						Int32 buffOfst = 6;
						while (me->readBuff[i + buffOfst] & 0x80)
						{
							buffOfst++;
						}
						if (me->readBuff[i + buffOfst] & 0xc0)
						{
							buffOfst += 2;
						}
						if ((me->readBuff[i + buffOfst] & 0xf0) == 0x20)
						{
							pts = (((Int64)(me->readBuff[i + buffOfst] & 0xe)) << 29) | (me->readBuff[i + buffOfst + 1] << 22) | ((me->readBuff[i + buffOfst + 2] & 0xfe) << 14) | (me->readBuff[i + buffOfst + 3] << 7) | (me->readBuff[i + buffOfst + 4] >> 1);
							dts = pts;
							buffOfst += 5;
						}
						else if ((me->readBuff[i + buffOfst] & 0xf0) == 0x30)
						{
							pts = (((Int64)(me->readBuff[i + buffOfst] & 0xe)) << 29) | (me->readBuff[i + buffOfst + 1] << 22) | ((me->readBuff[i + buffOfst + 2] & 0xfe) << 14) | (me->readBuff[i + buffOfst + 3] << 7) | (me->readBuff[i + buffOfst + 4] >> 1);
							dts = (((Int64)(me->readBuff[i + buffOfst + 5] & 0xe)) << 29) | (me->readBuff[i + buffOfst + 6] << 22) | ((me->readBuff[i + buffOfst + 7] & 0xfe) << 14) | (me->readBuff[i + buffOfst + 8] << 7) | (me->readBuff[i + buffOfst + 9] >> 1);
							buffOfst += 10;
						}
						else
						{
							buffOfst++;
						}
						stmHdrSize = buffOfst - 9;
					}
					else
					{
						stmHdrSize = me->readBuff[i + 8];
						if ((me->readBuff[i + 7] & 0xc0) == 0x80)
						{
							pts = (((Int64)(me->readBuff[i + 9] & 0xe)) << 29) | (me->readBuff[i + 10] << 22) | ((me->readBuff[i + 11] & 0xfe) << 14) | (me->readBuff[i + 12] << 7) | (me->readBuff[i + 13] >> 1);
							dts = pts;
						}
						else if ((me->readBuff[i + 7] & 0xc0) == 0xc0)
						{
							pts = (((Int64)(me->readBuff[i + 9] & 0xe)) << 29) | (me->readBuff[i + 10] << 22) | ((me->readBuff[i + 11] & 0xfe) << 14) | (me->readBuff[i + 12] << 7) | (me->readBuff[i + 13] >> 1);
							dts = (((Int64)(me->readBuff[i + 14] & 0xe)) << 29) | (me->readBuff[i + 15] << 22) | ((me->readBuff[i + 16] & 0xfe) << 14) | (me->readBuff[i + 17] << 7) | (me->readBuff[i + 18] >> 1);
						}
					}
//					Bool isFrame = false;

					if (dts != 0)
					{
						Int32 frameTime = (Int32)((dts - initScr) / 90);
						if (frameTime < 0)
							frameTime = 0;
						if (firstVideo || frameTime > lastFrameTime)
						{
							if (!firstAudio)
							{
								me->vstm->SetStreamTime(frameTime);
								lastFrameTime = frameTime;
							}
							firstVideo = false;
						}
					}
					me->vstm->WriteFrameStream(&me->readBuff[i + 9 + stmHdrSize], thisSize - 3 - stmHdrSize);

					i += thisSize + 6;
				}
				else
				{
					i++;
				}
			}
			else if (me->readBuff[i] == 0 && me->readBuff[i + 1] == 0 && me->readBuff[i + 2] == 0)
			{
				i++;
			}
			else
			{
				i++;
			}
		}
		me->readOfst += readSize;
		if (i < buffSize)
		{
			MemCopyO(me->readBuff, &me->readBuff[i], buffSize - i);
			buffSize -= i;
		}
		else
		{
			buffSize = 0;
		}
	}
	me->playing = 0;
	me->vstm->Stop();

	return 1002;
}

Int64 Media::MPGFile::GetBitRate()
{
	Int32 currTime = this->vstm->GetFrameStreamTime();
	if (currTime > 0 && this->readOfst > 0)
	{
		return (this->readOfst * 8000LL / currTime);
	}
	else
	{
		Int64 bitRate = this->vstm->GetBitRate();
		OSInt i;
		Data::ArrayList<Media::IMediaStream*> *stms = this->dataStms->GetValues();
		i = stms->GetCount();
		while (i-- > 0)
		{
			bitRate += stms->GetItem(i)->GetBitRate();
		}
		return bitRate;
	}
}

Bool Media::MPGFile::StartPlay()
{
	if (this->playing != 0)
		return true;
	this->playing = 1;
	this->playStarted = false;
	this->playToStop = false;
	Sync::Thread::Create(PlayThread, this);
	return true;
}

Bool Media::MPGFile::StopPlay()
{
	this->playToStop = true;
	while (this->playing != 0)
	{
		Sync::Thread::Sleep(10);
	}
	this->readOfst = 0;
	this->startTime = 0;
	return true;
}

Media::MPGFile::MPGFile(IO::IStreamData *stmData) : Media::MediaFile(stmData->GetFullName())
{
	this->stmData = stmData->GetPartialData(0, this->fleng = stmData->GetDataSize());
	this->bitRate = 0;
	this->readOfst = 0;
	this->readBuff = MemAlloc(UInt8, 1048576);
	this->playing = 0;
	this->playToStop = false;
	this->startTime = 0;
	this->vstm = 0;
	NEW_CLASS(this->dataStms, Data::Int32Map<Media::IMediaStream*>());
	NEW_CLASS(this->audStms, Data::ArrayList<Media::IAudioSource*>());

	if (stmData->GetRealData(0, 128, this->readBuff) != 128)
		return;
	if (*(Int32*)&this->readBuff[0] != (Int32)0xba010000)
		return;
	if ((this->readBuff[4] & 0xc0) == 0x40)
	{
		this->mpgVer = 2;
	}
	else if ((this->readBuff[4] & 0xf0) == 0x20)
	{
		this->mpgVer = 1;
	}
	else
	{
		return;
	}
	Int32 j;
	Int32 i;
	Int64 currOfst;
//	Int32 lastType = -1;
	if (this->mpgVer == 1)
	{
		currOfst = 12;
	}
	else
	{
		i = (this->readBuff[13] & 7);
		currOfst = 14 + i;
	}
	if (*(Int32*)&this->readBuff[currOfst] != (Int32)0xbb010000)
		return;

	i = ReadMInt16(&this->readBuff[currOfst + 4]);
	currOfst += 6 + i;
	while (true)
	{
		if (stmData->GetRealData(currOfst, 256, this->readBuff) < 4)
		{
			break;
		}
		j = 0;
		while (this->readBuff[j] == 0 && this->readBuff[j + 1] == 0 && this->readBuff[j + 2] == 0)
		{
			j++;
			if (j >= 252)
			{
				currOfst += 252;
				break;
			}
		}
		if (j >= 252)
			continue;
		if (this->readBuff[j] != 0 || this->readBuff[j + 1] != 0 || this->readBuff[j + 2] != 1)
		{
			break;
		}
		currOfst += j;
//		lastType = this->readBuff[j + 3];
		if (this->readBuff[j + 3] == 0xB9) //End Of File
			break;
		if (this->readBuff[j + 3] == 0xba) 
		{
			Int64 scr_base;
//			Int64 scr_ext;
			Int64 initScr = 0;
//			Int64 last_scr_base = 0;
			if (this->mpgVer == 1)
			{
				scr_base = (((Int64)(this->readBuff[j + 4] & 0xe)) << 29) | (this->readBuff[j + 5] << 22) | ((this->readBuff[j + 6] & 0xfe) << 14) | (this->readBuff[j + 7] << 7) | ((this->readBuff[j + 8] & 0xfe) >> 1);
//				scr_ext = 0;
				currOfst += 12;
			}
			else
			{
				scr_base = (((Int64)(this->readBuff[j + 4] & 0x38)) << 27) | ((this->readBuff[j + 4] & 3) << 28) | (this->readBuff[j + 5] << 20) | ((this->readBuff[j + 6] & 0xf8) << 12) | ((this->readBuff[j + 6] & 3) << 13) | (this->readBuff[j + 7] << 5) | (this->readBuff[j + 8] >> 3);
//				scr_ext = ((this->readBuff[j + 8] & 3) << 7) | (this->readBuff[j + 9] >> 1);
				currOfst += 14 + (this->readBuff[j + 13] & 7);
			}
			if (initScr == -1)
			{
				initScr = scr_base;
			}
//			last_scr_base = scr_base;
		}
		else
		{
			i = ReadMInt16(&this->readBuff[j + 4]);
			if (this->readBuff[j + 3] == 0xbd) //Private stream 1
			{
//				Int64 pts = 0;
//				Int64 dts = 0;
				UInt8 stmHdrSize = this->readBuff[j + 8];
				if ((this->readBuff[j + 7] & 0xc0) == 0x80)
				{
//					pts = (((Int64)(this->readBuff[j + 9] & 0xe)) << 29) | (this->readBuff[j + 10] << 22) | ((this->readBuff[j + 11] & 0xfe) << 14) | (this->readBuff[j + 12] << 7) | (this->readBuff[j + 13] >> 1);
//					dts = pts;
				}
				else if ((this->readBuff[7] & 0xc0) == 0xc0)
				{
//					pts = (((Int64)(this->readBuff[j + 9] & 0xe)) << 29) | (this->readBuff[j + 10] << 22) | ((this->readBuff[j + 11] & 0xfe) << 14) | (this->readBuff[j + 12] << 7) | (this->readBuff[j + 13] >> 1);
//					dts = (((Int64)(this->readBuff[j + 14] & 0xe)) << 29) | (this->readBuff[j + 15] << 22) | ((this->readBuff[j + 16] & 0xfe) << 14) | (this->readBuff[j + 17] << 7) | (this->readBuff[j + 18] >> 1);
				}

				UInt8 stmType = this->readBuff[j + 9 + stmHdrSize];
				if (stmType == 0xff)
				{
/*					stmId = buff[0x16];
					if (*(Int32*)&buff[0x17] == 0x64685353)
					{
						if (formats[stmId]->formatId == 0)
						{
							if (buff[0x14] == 0xa0)
							{
								formats[stmId]->formatId = 1;
								formats[stmId]->frequency = *(Int32*)&buff[0x23];
								formats[stmId]->nChannels = *(Int32*)&buff[0x27];
								formats[stmId]->bitpersample = 16;
								formats[stmId]->bitRate = formats[stmId]->frequency * formats[stmId]->nChannels << 4;
								formats[stmId]->align = formats[stmId]->nChannels << 1;
								formats[stmId]->other = (*(Int32*)&buff[0x2b]) >> 1;
								formats[stmId]->intType = Media::AudioFormat::IT_NORMAL;
								formats[stmId]->extraSize = 0;
								formats[stmId]->extra = 0;
								NEW_CLASS(stmData[stmId], IO::StmData::BlockStreamData(fd));
								stmData[stmId]->Append(currOfst + 0x3F, i - 0x39);
								audDelay[stmId] = (Int32)((dts - initScr) / 90);
							}
						}
					}
					else if (formats[stmId]->formatId == 1)
					{
						stmData[stmId]->Append(currOfst + 0x17, i - 0x11);
					}*/
				}
				else if ((stmType & 0xf0) == 0xa0)
				{
					if (this->readBuff[j + 10 + stmHdrSize] > 0)
					{
						Media::IMediaStream *mstm = this->dataStms->Get(stmType);
						if (mstm == 0)
						{
							Media::AudioFormat fmt;
							fmt.formatId = 1;
							fmt.nChannels = (this->readBuff[j + 14 + stmHdrSize] & 7) + 1;//?
							fmt.frequency = (this->readBuff[j + 14 + stmHdrSize] & 0x30)?96000:48000;
							switch ((this->readBuff[j + 14 + stmHdrSize] & 0xc0) >> 6)
							{
							case 0:
								fmt.bitpersample = 16;
								break;
							case 1:
								fmt.bitpersample = 20;
								break;
							case 2:
								fmt.bitpersample = 24;
								break;
							}

							fmt.bitRate = fmt.frequency * fmt.nChannels * fmt.bitpersample;
							fmt.align = fmt.nChannels * (fmt.bitpersample >> 3);
							fmt.other = 0;
							fmt.intType = Media::AudioFormat::IT_BIGENDIAN16;
							fmt.extraSize = 0;
							fmt.extra = 0;
							NEW_CLASS(mstm, Media::VOBLPCMStreamSource(this, &fmt));
							this->dataStms->Put(stmType, mstm);
							this->audStms->Add((Media::VOBLPCMStreamSource*)mstm);
						}
					}
					else
					{
					}
				}
				else if (stmType == 0x80)
				{
					if (this->readBuff[10 + stmHdrSize] > 0)
					{
						Media::VOBAC3StreamSource *mstm = (Media::VOBAC3StreamSource*)this->dataStms->Get(stmType);
						if (mstm == 0)
						{
							NEW_CLASS(mstm, Media::VOBAC3StreamSource(this));
							this->dataStms->Put(stmType, mstm);
							this->audStms->Add(mstm);
						}
						if (!mstm->IsReady())
						{
							UInt8 *frameBuff = MemAlloc(UInt8, i - 7 - stmHdrSize);
							stmData->GetRealData(currOfst + 13 + j + stmHdrSize, i - 7 - stmHdrSize, frameBuff);
							mstm->ParseHeader(frameBuff, i - 7 - stmHdrSize);
							MemFree(frameBuff);
						}
					}
				}
				else
				{
//					stmId = 2;
				}
			}
			else if ((this->readBuff[j + 3] & 0xe0) == 0xc0) //Audio stream
			{
//				Int64 pts = 0;
//				Int64 dts = 0;
				j += 6;
				if (this->mpgVer == 1)
				{
					while (this->readBuff[j] & 0x80)
					{
						j++;
					}
					if ((this->readBuff[j] & 0xc0) == 0x40)
					{
						j += 2;
					}
					if ((this->readBuff[j] & 0xf0) == 0x20)
					{
//						pts = (((Int64)this->readBuff[j] & 0xe) << 29) | (this->readBuff[j + 1] << 22) | ((this->readBuff[j + 2] & 0xfe) << 14) | (this->readBuff[j + 3] << 7) | (this->readBuff[j + 4] >> 1);;
						j += 5;
					}
					else if ((this->readBuff[j] & 0xf0) == 0x30)
					{
//						pts = (((Int64)this->readBuff[j] & 0xe) << 29) | (this->readBuff[j + 1] << 22) | ((this->readBuff[j + 2] & 0xfe) << 14) | (this->readBuff[j + 3] << 7) | (this->readBuff[j + 4] >> 1);;
//						dts = (((Int64)this->readBuff[j + 5] & 0xe) << 29) | (this->readBuff[j + 6] << 22) | ((this->readBuff[j + 7] & 0xfe) << 14) | (this->readBuff[j + 8] << 7) | (this->readBuff[j + 9] >> 1);;
						j += 10;
					}
					else if (this->readBuff[j] == 0xf)
					{
						j++;
					}
					else
					{
	//					valid = false;
						break;
					}
				}
				else
				{
					j++;
					if ((this->readBuff[j] & 0xf0) == 0x20)
					{
						j += 2;
//						pts = (((Int64)this->readBuff[j] & 0xe) << 29) | (this->readBuff[j + 1] << 22) | ((this->readBuff[j + 2] & 0xfe) << 14) | (this->readBuff[j + 3] << 7) | (this->readBuff[j + 4] >> 1);;
						j += this->readBuff[j - 1];
					}
					else if ((this->readBuff[j] & 0xf0) == 0x30)
					{
						j += 2;
//						pts = (((Int64)this->readBuff[j] & 0xe) << 29) | (this->readBuff[j + 1] << 22) | ((this->readBuff[j + 2] & 0xfe) << 14) | (this->readBuff[j + 3] << 7) | (this->readBuff[j + 4] >> 1);;
//						dts = (((Int64)this->readBuff[j + 5] & 0xe) << 29) | (this->readBuff[j + 6] << 22) | ((this->readBuff[j + 7] & 0xfe) << 14) | (this->readBuff[j + 8] << 7) | (this->readBuff[j + 9] >> 1);;
						j += this->readBuff[j - 1];
					}
					else
					{
						j += this->readBuff[j + 1] + 2;
					}
				}


				Int32 stmId = this->readBuff[3] & 0x1f;
				Media::IMediaStream *mstm = this->dataStms->Get(stmId);
				if (mstm == 0)
				{
					Int32 v = ReadMInt32(&this->readBuff[j]);
					if ((v & 0x80000000) != 0 && (v & 0x7fffffff) <= 2048)
					{
/*						formats[stmId]->formatId = 0x2080;
						formats[stmId]->nChannels = buff[j + 7];
						formats[stmId]->frequency = ReadMInt32(&buff[j + 8]);
						formats[stmId]->bitpersample = 16;
						formats[stmId]->bitRate = (formats[stmId]->frequency * formats[stmId]->nChannels * 9) >> 1;
						formats[stmId]->align = 18 * formats[stmId]->nChannels;
						formats[stmId]->other = 0;
						formats[stmId]->intType = Media::AudioFormat::IT_NORMAL;
						formats[stmId]->extraSize = 0;
						formats[stmId]->extra = 0;
						NEW_CLASS(stmData[stmId], IO::StmData::BlockStreamData(fd));
						stmData[stmId]->Append(currOfst + j + 4 + (v & 0x7fffffff), (Int32)(i - j + 2 - (v & 0x7fffffff)));
						audDelay[stmId] = (Int32)((pts - initScr) / 90);

						if (resync)
							resync = false;*/
					}
					else if (this->readBuff[j] == 0xff && ((this->readBuff[j + 1] & 0xfe) == 0xfc || (this->readBuff[j + 1] & 0xfe) == 0xfa))
					{
						Media::MPAStreamSource *mstm = (Media::MPAStreamSource*)this->dataStms->Get(stmId);
						if (mstm == 0)
						{
							NEW_CLASS(mstm, Media::MPAStreamSource(this));
							this->dataStms->Put(stmId, mstm);
							this->audStms->Add(mstm);
						}
						if (!mstm->IsReady())
						{
							mstm->ParseHeader(&this->readBuff[j], i - j + 6);
						}
					}
				}
			}
			else if ((this->readBuff[j + 3] & 0xf0) == 0xe0) //Video stream
			{
				if (vstm != 0 && currOfst > 1048576)
					break;

//				Int64 pts = 0;
//				Int64 dts = 0;
				UInt8 stmHdrSize;
				if (this->mpgVer == 1)
				{
					Int32 buffOfst = j + 6;
					while (this->readBuff[buffOfst] & 0x80)
					{
						buffOfst++;
					}
					if (this->readBuff[buffOfst] & 0xc0)
					{
						buffOfst += 2;
					}
					if ((this->readBuff[buffOfst] & 0xf0) == 0x20)
					{
//						pts = (((Int64)(this->readBuff[buffOfst] & 0xe)) << 29) | (this->readBuff[buffOfst + 1] << 22) | ((this->readBuff[buffOfst + 2] & 0xfe) << 14) | (this->readBuff[buffOfst + 3] << 7) | (this->readBuff[buffOfst + 4] >> 1);
//						dts = pts;
						buffOfst += 5;
					}
					else if ((this->readBuff[buffOfst] & 0xf0) == 0x30)
					{
//						pts = (((Int64)(this->readBuff[buffOfst] & 0xe)) << 29) | (this->readBuff[buffOfst + 1] << 22) | ((this->readBuff[buffOfst + 2] & 0xfe) << 14) | (this->readBuff[buffOfst + 3] << 7) | (this->readBuff[buffOfst + 4] >> 1);
//						dts = (((Int64)(this->readBuff[buffOfst + 5] & 0xe)) << 29) | (this->readBuff[buffOfst + 6] << 22) | ((this->readBuff[buffOfst + 7] & 0xfe) << 14) | (this->readBuff[buffOfst + 8] << 7) | (this->readBuff[buffOfst + 9] >> 1);
						buffOfst += 10;
					}
					else
					{
						buffOfst++;
					}
					stmHdrSize = buffOfst - 9;
				}
				else
				{
					stmHdrSize = this->readBuff[j + 8];
					if ((this->readBuff[j + 7] & 0xc0) == 0x80)
					{
//						pts = (((Int64)(this->readBuff[j + 9] & 0xe)) << 29) | (this->readBuff[j + 10] << 22) | ((this->readBuff[j + 11] & 0xfe) << 14) | (this->readBuff[j + 12] << 7) | (this->readBuff[j + 13] >> 1);
//						dts = pts;
					}
					else if ((this->readBuff[7] & 0xc0) == 0xc0)
					{
//						pts = (((Int64)(this->readBuff[j + 9] & 0xe)) << 29) | (this->readBuff[j + 10] << 22) | ((this->readBuff[j + 11] & 0xfe) << 14) | (this->readBuff[j + 12] << 7) | (this->readBuff[j + 13] >> 1);
//						dts = (((Int64)(this->readBuff[j + 14] & 0xe)) << 29) | (this->readBuff[j + 15] << 22) | ((this->readBuff[j + 16] & 0xfe) << 14) | (this->readBuff[j + 17] << 7) | (this->readBuff[j + 18] >> 1);
					}
				}
				Bool isFrame = false;
				if (this->readBuff[j + 9 + stmHdrSize] == 0 && this->readBuff[j + 10 + stmHdrSize] == 0 && this->readBuff[j + 11 + stmHdrSize] == 1 && this->readBuff[j + 12 + stmHdrSize] == 0xb3)
				{
					isFrame = true;
				}
				FrameInfo info;
				Int32 frameRateNorm;
				Int32 frameRateDenorm;
				if (isFrame && vstm == 0 && Media::MPEGVideoParser::GetFrameInfo(&this->readBuff[j + 9 + stmHdrSize], 256 - 9 - stmHdrSize, &info, &frameRateNorm, &frameRateDenorm, &this->bitRate, false))
				{
					NEW_CLASS(this->vstm, Media::M2VStreamSource(this));
					this->vstm->DetectStreamInfo(&this->readBuff[j + 9 + stmHdrSize], 256 - 9 - stmHdrSize);
				}
			}
			else if (this->readBuff[j + 3] == 0xbc) // program_stream_map
			{
			}
			else if (this->readBuff[j + 3] == 0xbe) // padding stream
			{
			}
			else if (this->readBuff[j + 3] == 0xbf) // private stream 2
			{
			}
			currOfst += 6 + i;
		}
	}
}

Media::MPGFile::~MPGFile()
{
	if (this->playing != 0)
	{
		this->StopPlay();
	}
	Data::ArrayList<Media::IMediaStream*> *dataList = this->dataStms->GetValues();
	Media::IMediaStream *stm;
	OSInt i;
	i = dataList->GetCount();
	while (i-- > 0)
	{
		stm = dataList->GetItem(i);
		DEL_CLASS(stm);
	}
	DEL_CLASS(this->dataStms);
	DEL_CLASS(this->audStms);
	SDEL_CLASS(this->vstm);
	DEL_CLASS(this->stmData);
	MemFree(this->readBuff);
}

UOSInt Media::MPGFile::AddSource(Media::IMediaSource *src, Int32 syncTime)
{
	return -1;
}

Media::IMediaSource *Media::MPGFile::GetStream(UOSInt index, Int32 *syncTime)
{
	if (syncTime)
		*syncTime = 0;
	if (index == 0)
		return this->vstm;
	return this->audStms->GetItem(index - 1);
}

void Media::MPGFile::KeepStream(UOSInt index, Bool toKeep)
{
}

UTF8Char *Media::MPGFile::GetMediaName(UTF8Char *buff)
{
	return this->GetSourceName(buff);
}

Int32 Media::MPGFile::GetStreamTime()
{
	return (Int32)(this->fleng * 8000 / this->GetBitRate());
}

Bool Media::MPGFile::StartAudio()
{
	return StartPlay();
}

Bool Media::MPGFile::StopAudio()
{
	return StopPlay();
}

Bool Media::MPGFile::StartVideo()
{
	return StartPlay();
}

Bool Media::MPGFile::StopVideo()
{
	return StopPlay();
}

Bool Media::MPGFile::IsRunning()
{
	return (this->playing != 0) && !this->playToStop;
}

Int32 Media::MPGFile::SeekToTime(Int32 mediaTime)
{
	if (this->playing)
		return 0;
	this->readOfst = mediaTime * this->GetBitRate() / 8000;
	this->startTime = mediaTime;
	return mediaTime;
}

Bool Media::MPGFile::IsRealTimeSrc()
{
	return false;
}

Bool Media::MPGFile::CanSeek()
{
	return true;
}

OSInt Media::MPGFile::GetDataSeekCount()
{
	return this->stmData->GetSeekCount();
}
