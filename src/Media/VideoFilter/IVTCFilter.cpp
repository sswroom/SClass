#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/VideoFilter/IVTCFilter.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/UTF8Writer.h"

#define IVTCINTERVAL 10
//#define _DEBUG

extern "C"
{
	void IVTCFilter_CalcField(UInt8 *oddPtr, UInt8 *evenPtr, UOSInt w, UOSInt h, UInt32 *fieldStats);
	void IVTCFilter_CalcFieldP(UInt8 *framePtr, UOSInt w, UOSInt h, UInt32 *fieldStats);
}

void Media::VideoFilter::IVTCFilter::ProcessVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	if (flags & Media::IVideoSource::FF_DISCONTTIME)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (this->enabled)
		{
			this->ClearIVTC();
		}
		mutUsage.EndUse();
	}
	if (frameType == Media::FT_DISCARD)
		return;

	while (this->ivtcTStatus == 2)
	{
		this->mainEvt->Wait(100);
	}
	Sync::MutexUsage mutUsage(this->mut);
	if (this->enabled)
	{
		if (this->fieldExist)
		{
			if (this->videoInfo->fourcc == *(UInt32*)"YV12")
			{
				if (this->fieldFrameType == Media::FT_MERGED_TF)
				{
					if (frameType == Media::FT_MERGED_BF)
					{
//						UOSInt dw = this->videoInfo->dispWidth;
						UOSInt dh = this->videoInfo->dispHeight >> 1;
						UOSInt sw = this->videoInfo->storeWidth;
//						UOSInt sh = this->videoInfo->storeHeight >> 1;
						UOSInt sw2 = sw << 1;
						UOSInt swh = sw >> 1;
//						UOSInt dwh = dw >> 1;
						UInt8 *srcPtr = imgData[0] + sw;
						UInt8 *destPtr = this->fieldBuff + sw;
						UOSInt lastH = dh;
						UOSInt currH;
						UOSInt copyThreadCnt = this->threadCnt;
						if (copyThreadCnt > 2)
						{
							copyThreadCnt = 2;
						}
						UOSInt i = copyThreadCnt;
						Bool found;
						while (i-- > 0)
						{
							currH = (dh * i / copyThreadCnt);
							this->threadStats[i].sw = sw;
							this->threadStats[i].h = lastH - currH;
							this->threadStats[i].oddPtr = srcPtr + sw2 * currH;
							this->threadStats[i].evenPtr = destPtr + sw2 * currH;
							this->threadStats[i].currCmd = 4;
							this->threadStats[i].evt->Set();
							lastH = currH;
						}
						while (true)
						{
							found = false;
							i = copyThreadCnt;
							while (i-- > 0)
							{
								if (this->threadStats[i].currCmd == 4)
								{
									found = true;
									break;
								}
							}
							if (!found)
								break;
							this->mainEvt->Wait(100);
						}
						srcPtr += sw2 * dh - swh;
						destPtr += sw2 * dh - swh;
						i = copyThreadCnt;
						while (i-- > 0)
						{
							currH = (dh * i / copyThreadCnt);
							this->threadStats[i].sw = swh;
							this->threadStats[i].h = lastH - currH;
							this->threadStats[i].oddPtr = srcPtr + sw * currH;
							this->threadStats[i].evenPtr = destPtr + sw * currH;
							this->threadStats[i].currCmd = 4;
							this->threadStats[i].evt->Set();
							lastH = currH;
						}
						while (true)
						{
							found = false;
							i = copyThreadCnt;
							while (i-- > 0)
							{
								if (this->threadStats[i].currCmd == 4)
								{
									found = true;
									break;
								}
							}
							if (!found)
								break;
							this->mainEvt->Wait(100);
						}

						frameType = Media::FT_INTERLACED_TFF;
						if (this->fieldIsDiscont)
						{
							flags = (Media::IVideoSource::FrameFlag)(flags | Media::IVideoSource::FF_DISCONTTIME);
						}
						this->fieldExist = false;
						mutUsage.EndUse();
						StartIVTC(frameTime, frameNum, this->fieldBuff, dataSize, frameStruct, frameType, flags, ycOfst);
						return;
					}
/*					else if (frameType == Media::FT_FIELD_BF)
					{
						Int32 w = me->videoInfo->width;
						Int32 h = me->videoInfo->height;
						Int32 w2 = w << 1;
						Int32 wh = w >> 1;
						UInt8 *srcPtr = me->fieldBuff;
						UInt8 *destPtr = imgData[0]; 
						Int32 i;
						i = 0;
						while (i < h)
						{
							MemCopy(destPtr, srcPtr, w);
							srcPtr += w2;
							destPtr += w2;
							i += 2;
						}
						i = 0;
						while (i < h)
						{
							MemCopy(destPtr, srcPtr, wh);
							srcPtr += w;
							destPtr += w;
							i += 2;
						}
						frameType = Media::FT_INTERLACED_TFF;
						me->fieldExist = false;
					}*/
					else
					{
						if (this->videoCb)
						{
							this->videoCb(this->fieldTime, this->fieldNum, &this->fieldBuff, this->fieldDataSize, frameStruct, this->userData, this->fieldFrameType, flags, ycOfst);
						}
						this->fieldExist = false;
					}
				}
				else if (this->fieldFrameType == Media::FT_MERGED_BF)
				{
					if (frameType == Media::FT_MERGED_TF)
					{
//						UOSInt dw = this->videoInfo->dispWidth;
						UOSInt dh = this->videoInfo->dispHeight >> 1;
						UOSInt sw = this->videoInfo->storeWidth;
//						UOSInt sh = this->videoInfo->storeHeight >> 1;
						UOSInt sw2 = sw << 1;
						UOSInt swh = sw >> 1;
//						UOSInt dwh = dw >> 1;
						UInt8 *srcPtr = imgData[0];
						UInt8 *destPtr = this->fieldBuff;
						UOSInt lastH = dh;
						UOSInt currH;
						UOSInt copyThreadCnt = this->threadCnt;
						if (copyThreadCnt > 2)
						{
							copyThreadCnt = 2;
						}
						UOSInt i = copyThreadCnt;
						Bool found;
						while (i-- > 0)
						{
							currH = (dh * i / copyThreadCnt);
							this->threadStats[i].sw = sw;
							this->threadStats[i].h = lastH - currH;
							this->threadStats[i].oddPtr = srcPtr + sw2 * currH;
							this->threadStats[i].evenPtr = destPtr + sw2 * currH;
							this->threadStats[i].currCmd = 4;
							this->threadStats[i].evt->Set();
							lastH = currH;
						}
						while (true)
						{
							found = false;
							i = copyThreadCnt;
							while (i-- > 0)
							{
								if (this->threadStats[i].currCmd == 4)
								{
									found = true;
									break;
								}
							}
							if (!found)
								break;
							this->mainEvt->Wait(100);
						}
						srcPtr += sw2 * dh;
						destPtr += sw2 * dh;
						i = copyThreadCnt;
						while (i-- > 0)
						{
							currH = (dh * i / copyThreadCnt);
							this->threadStats[i].sw = swh;
							this->threadStats[i].h = lastH - currH;
							this->threadStats[i].oddPtr = srcPtr + sw * currH;
							this->threadStats[i].evenPtr = destPtr + sw * currH;
							this->threadStats[i].currCmd = 4;
							this->threadStats[i].evt->Set();
							lastH = currH;
						}
						while (true)
						{
							found = false;
							i = copyThreadCnt;
							while (i-- > 0)
							{
								if (this->threadStats[i].currCmd == 4)
								{
									found = true;
									break;
								}
							}
							if (!found)
								break;
							this->mainEvt->Wait(100);
						}

						frameType = Media::FT_INTERLACED_BFF;
						if (this->fieldIsDiscont)
						{
							flags = (Media::IVideoSource::FrameFlag)(flags | Media::IVideoSource::FF_DISCONTTIME);
						}
						this->fieldExist = false;
						mutUsage.EndUse();
						//do_IVTC(frameTime, frameNum, this->fieldBuff, dataSize, frameStruct, frameType, flags, ycOfst);
						StartIVTC(frameTime, frameNum, this->fieldBuff, dataSize, frameStruct, frameType, flags, ycOfst);
						return;
					}
					else
					{
						if (this->videoCb)
						{
							this->videoCb(this->fieldTime, this->fieldNum, &this->fieldBuff, this->fieldDataSize, frameStruct, this->userData, this->fieldFrameType, flags, ycOfst);
						}
						this->fieldExist = false;
					}
				}
			}
		}
		if (!this->fieldExist)
		{
			if (this->videoInfo->fourcc == *(UInt32*)"YV12")
			{
				if (frameType == Media::FT_FIELD_TF || frameType == Media::FT_FIELD_BF)
				{
					if (this->fieldBuffSize < (dataSize * 2))
					{
						if (this->fieldBuff)
						{
							MemFreeA64(this->fieldBuff);
						}
						this->fieldBuffSize = dataSize * 2;
						this->fieldBuff = MemAllocA64(UInt8, this->fieldBuffSize);
					}
					UOSInt sw = this->videoInfo->storeWidth;
//					UOSInt sh = this->videoInfo->storeHeight;
//					UOSInt dw = this->videoInfo->dispWidth;
					UOSInt dh = this->videoInfo->dispHeight;
					UOSInt sw2 = sw << 1;
//					UOSInt dw2 = dw << 1;
					UOSInt swh = sw >> 1;
//					UOSInt dwh = dw >> 1;
					UInt8 *srcPtr = this->fieldBuff;
					UInt8 *destPtr = imgData[0]; 
					UOSInt i;
					i = 0;
					while (i < dh)
					{
						MemCopyNANC(destPtr, srcPtr, sw);
						MemCopyNANC(destPtr + sw, srcPtr, sw);
						srcPtr += sw;
						destPtr += sw2;
						i += 2;
					}
					srcPtr -= swh;
					destPtr -= swh;
					i = 0;
					while (i < dh)
					{
						MemCopyNANC(destPtr, srcPtr, swh);
						MemCopyNANC(destPtr + swh, srcPtr, swh);
						srcPtr += swh;
						destPtr += sw;
						i += 2;
					}

					MemCopyNANC(this->fieldBuff, imgData[0], dataSize);
					if (frameType == Media::FT_FIELD_TF)
					{
						this->fieldFrameType = Media::FT_MERGED_TF;
					}
					else
					{
						this->fieldFrameType = Media::FT_MERGED_BF;
					}
					this->fieldExist = true;
					this->fieldTime = frameTime;
					this->fieldNum = frameNum;
					this->fieldDataSize = dataSize;
					this->fieldIsDiscont = (flags & Media::IVideoSource::FF_DISCONTTIME);
					mutUsage.EndUse();
					return;
				}
				else if (frameType == Media::FT_MERGED_TF)
				{
					if (this->fieldBuffSize < dataSize)
					{
						if (this->fieldBuff)
						{
							MemFreeA64(this->fieldBuff);
						}
						this->fieldBuffSize = dataSize;
						this->fieldBuff = MemAllocA64(UInt8, this->fieldBuffSize);
					}
					MemCopyNANC(this->fieldBuff, imgData[0], dataSize);
					this->fieldFrameType = frameType;
					this->fieldExist = true;
					this->fieldTime = frameTime;
					this->fieldNum = frameNum;
					this->fieldDataSize = dataSize;
					this->fieldIsDiscont = (flags & Media::IVideoSource::FF_DISCONTTIME);
					mutUsage.EndUse();
					return;
				}
				else if (frameType == Media::FT_MERGED_BF)
				{
					if (this->fieldBuffSize < dataSize)
					{
						if (this->fieldBuff)
						{
							MemFreeA64(this->fieldBuff);
						}
						this->fieldBuffSize = dataSize;
						this->fieldBuff = MemAllocA64(UInt8, this->fieldBuffSize);
					}
					MemCopyNANC(this->fieldBuff, imgData[0], dataSize);
					this->fieldFrameType = frameType;
					this->fieldExist = true;
					this->fieldTime = frameTime;
					this->fieldNum = frameNum;
					this->fieldDataSize = dataSize;
					this->fieldIsDiscont = (flags & Media::IVideoSource::FF_DISCONTTIME);
					mutUsage.EndUse();
					return;
				}
			}
		}
	}
	mutUsage.EndUse();
	do_IVTC(frameTime, frameNum, imgData, dataSize, frameStruct, frameType, flags, ycOfst);
}

void Media::VideoFilter::IVTCFilter::OnFrameChange(Media::IVideoSource::FrameChange fc)
{
	if (fc == Media::IVideoSource::FC_ENDPLAY || fc == Media::IVideoSource::FC_SRCCHG)
	{
		while (this->ivtcTStatus == 2)
		{
			this->mainEvt->Wait(100);
		}
	}
}

void Media::VideoFilter::IVTCFilter::do_IVTC(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->enabled)
	{
		if (frameType == Media::FT_INTERLACED_TFF || frameType == Media::FT_INTERLACED_BFF)
		{
			Media::FrameType diFrameType;
			Media::FrameType diFrameType2;
			if (frameType == Media::FT_INTERLACED_TFF)
			{
				diFrameType = Media::FT_INTERLACED_TFF;
				diFrameType2 = Media::FT_INTERLACED_BFF;
			}
			else
			{
				diFrameType = Media::FT_INTERLACED_TFF;
				diFrameType2 = Media::FT_INTERLACED_TFF;
			}
			UInt32 outFrameTime = frameTime;
			Bool mergedFrame = false;
			if (this->ivtcFrameBuffSize < dataSize)
			{
				this->ivtcFrameBuffSize = dataSize;
				if (this->ivtcLastFrame)
				{
					MemFreeA64(this->ivtcLastFrame);
					MemFreeA64(this->ivtcCurrFrame);
				}
				this->ivtcLastFrame = MemAllocA64(UInt8, this->ivtcFrameBuffSize);
				this->ivtcCurrFrame = MemAllocA64(UInt8, this->ivtcFrameBuffSize);
			}

			if (this->videoInfo->fourcc == *(UInt32*)"YV12")
			{
				UInt32 oddDiff = 0;
				UInt32 oddMDiff = 0;
				UInt32 evenDiff = 0;
				UInt32 evenMDiff = 0;
				FieldStat fieldStat;
				Int32 diff;
				Bool ivtcFirstFrame = false;
				if (this->ivtcExist > 0)
				{
					this->ivtcExist -= 1;
				}
				if (this->ivtcLastExist)
				{
					UInt8 *ivtcPtr = imgData[0];
					UInt8 *ivtcLPtr = this->ivtcLastFrame;
					UOSInt ivtcW;
					UOSInt ivtcH = this->videoInfo->dispHeight;

					while (ivtcH > 1)
					{
						ivtcW = this->videoInfo->dispWidth;
						while (ivtcW-- > 0)
						{
							diff = *ivtcPtr - *ivtcLPtr;
							if (diff < 0)
								diff = -diff;
							if ((UInt32)diff > oddMDiff)
								oddMDiff = (UInt32)diff;
							oddDiff += (UInt32)diff;
							ivtcPtr++;
							ivtcLPtr++;
						}
						ivtcW = this->videoInfo->dispWidth;
						while (ivtcW-- > 0)
						{
							diff = *ivtcPtr - *ivtcLPtr;
							if (diff < 0)
								diff = -diff;
							if ((UInt32)diff > evenMDiff)
								evenMDiff = (UInt32)diff;
							evenDiff += (UInt32)diff;
							ivtcPtr++;
							ivtcLPtr++;
						}

						ivtcH -= 2;
					}
				}

				Double pixelCnt = UOSInt2Double(this->videoInfo->dispWidth * this->videoInfo->dispHeight);
				Double oddDDiff = oddDiff / pixelCnt;
				Double evenDDiff = evenDiff / pixelCnt;
				
				///////////////////////////////////////
				if (this->ivtcLastFieldUsed)
				{
					outFrameTime = (this->ivtcLastFrameTime + frameTime) >> 1;
					if (this->ivtcLastSC == 0 && evenDiff > this->ivtcLastEven * 4 && oddDiff > this->ivtcLastOdd * 4 && evenDiff * 2 > pixelCnt && oddDiff * 2 > pixelCnt)
					{
						this->ivtcLastSC = 1;
						frameType = Media::FT_INTERLACED_NODEINT;
						this->ivtcLastFieldUsed = false;
						ivtcFirstFrame = true;
					}
					else
					{
						if (this->ivtcLastSC == 2)
						{
							UInt8 *oddPtr;
							UInt8 *evenPtr;
							FieldStat iFieldStat;
							FieldStat pFieldStat;
							UOSInt sw = this->videoInfo->storeWidth;
//							OSInt sh = this->videoInfo->storeHeight;
//							OSInt dw = this->videoInfo->dispWidth;
							UOSInt dh = this->videoInfo->dispHeight;
//							OSInt sw2 = sw << 1;
//							OSInt sw4 = sw << 2;
//							OSInt dw2 = dw << 1;
//							OSInt dw4 = dw << 2;
							if (frameType == Media::FT_INTERLACED_BFF)
							{
								oddPtr = this->ivtcLastFrame;
								evenPtr = imgData[0];
							}
							else
							{
								oddPtr = imgData[0];
								evenPtr = this->ivtcLastFrame;
							}
							this->CalcFieldStat(&iFieldStat, oddPtr, evenPtr, sw, dh);
							this->CalcFieldStatP(&pFieldStat, imgData[0], sw, dh);
							ivtcFirstFrame = true;
							this->ivtcLastSC = 0;
							if (iFieldStat.fieldDiff >= pFieldStat.fieldDiff)
							{
								frameType = Media::FT_INTERLACED_NODEINT;
								this->ivtcLastFieldUsed = false;
							}
							else
							{
								if (frameType == Media::FT_INTERLACED_TFF)
								{
									this->ivtcLastEven = evenDiff;
									this->ivtcLastOdd = evenDiff;
								}
								else
								{
									this->ivtcLastEven = oddDiff;
									this->ivtcLastOdd = oddDiff;
								}
							}
#ifdef _DEBUG
							if (this->debugLog)
							{
								UTF8Char sbuff[256];
								UTF8Char *sptr;
								sptr = Text::StrConcatC(sbuff, UTF8STRC("ScnChg\t"));
								sptr = Text::StrInt32(sptr, frameNum);
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, iFieldStat.fieldDiff);
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, iFieldStat.fieldCnt);
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, iFieldStat.fieldMDiff);
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, iFieldStat.field2Diff);
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, iFieldStat.field2Cnt);
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, iFieldStat.field2MDiff);
/*								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, iFieldStat.field3Cnt);
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, iFieldStat.field4Cnt);*/
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, pFieldStat.fieldDiff);
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, pFieldStat.fieldCnt);
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, pFieldStat.fieldMDiff);
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, pFieldStat.field2Diff);
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, pFieldStat.field2Cnt);
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
								sptr = Text::StrOSInt(sptr, pFieldStat.field2MDiff);
/*								sptr = Text::StrConcatC(sptr, UTF8STRC("\t");
								sptr = Text::StrOSInt(sptr, pFieldStat.field3Cnt);
								sptr = Text::StrConcatC(sptr, UTF8STRC("\t");
								sptr = Text::StrOSInt(sptr, pFieldStat.field4Cnt);*/
								this->debugLog->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
							}
#endif
						}

						if (!ivtcFirstFrame)
						{
							if (frameType == Media::FT_INTERLACED_BFF)
							{
								if (this->ivtcExist == IVTCINTERVAL - 1)
								{
								}
								else if (evenDDiff > 0.1 && oddDDiff < evenDDiff * 0.5)
								{
									frameType = Media::FT_INTERLACED_NODEINT;
								}
								else if (oddMDiff < evenMDiff * 0.4)
								{
									frameType = Media::FT_INTERLACED_NODEINT;
								}
								else if ((this->ivtcExist == IVTCINTERVAL - 2) && (oddMDiff < evenMDiff * 0.7))
								{
									frameType = Media::FT_INTERLACED_NODEINT;
								}

								if (frameType == Media::FT_INTERLACED_NODEINT)
								{
									this->ivtcLastFieldUsed = false;
									if (this->ivtcLastSC == 2)
									{
										this->ivtcLastEven = oddDiff;
										this->ivtcLastOdd = oddDiff;
										this->ivtcLastSC = 0;
										this->ivtcLastFieldUsed = true;
										frameType = Media::FT_INTERLACED_BFF;
										ivtcFirstFrame = true;
									}
									else if (this->ivtcLastSC)
									{
										this->ivtcLastEven = evenDiff;
										this->ivtcLastOdd = evenDiff;
										this->ivtcLastSC = 0;
										ivtcFirstFrame = true;
									}
									else
									{
										if (evenDiff > this->ivtcLastEven * 4 && evenDiff * 2 > pixelCnt)
										{
											if (oddDiff > this->ivtcLastOdd * 4)
											{
												this->ivtcLastSC = 1;
											}
											else
											{
												this->ivtcLastSC = 2;
											}
										}
										else if (evenDiff > this->ivtcLastEven)
										{
											this->ivtcLastEven = evenDiff;
										}
										else
										{
											this->ivtcLastEven = (this->ivtcLastEven + evenDiff) >> 1;
										}
									}
								}
							}
							else if (frameType == Media::FT_INTERLACED_TFF)
							{
								if (this->ivtcExist == IVTCINTERVAL - 1)
								{
								}
								else if (oddDDiff > 0.1 && evenDDiff < oddDDiff * 0.5)
								{
									frameType = Media::FT_INTERLACED_NODEINT;
								}
								else if (evenMDiff < oddMDiff * 0.4)
								{
									frameType = Media::FT_INTERLACED_NODEINT;
								}
								else if ((this->ivtcExist == IVTCINTERVAL - 2) && (evenMDiff < oddMDiff * 0.7))
								{
									frameType = Media::FT_INTERLACED_NODEINT;
								}

								if (frameType == Media::FT_INTERLACED_NODEINT)
								{
									this->ivtcLastFieldUsed = false;
									if (this->ivtcLastSC == 2)
									{
										this->ivtcLastEven = evenDiff;
										this->ivtcLastOdd = evenDiff;
										this->ivtcLastSC = 0;
										this->ivtcLastFieldUsed = true;
										frameType = Media::FT_INTERLACED_TFF;
										ivtcFirstFrame = true;
									}
									else if (this->ivtcLastSC == 1)
									{
										this->ivtcLastEven = oddDiff;
										this->ivtcLastOdd = oddDiff;
										this->ivtcLastSC = 0;
										ivtcFirstFrame = true;
									}
									else
									{
										if (oddDiff > this->ivtcLastOdd * 4 && oddDiff * 2 > pixelCnt)
										{
											if (evenDiff > this->ivtcLastEven * 4)
											{
												this->ivtcLastSC = 1;
											}
											else
											{
												this->ivtcLastSC = 2;
											}
										}
										else if (oddDiff > this->ivtcLastOdd)
										{
											this->ivtcLastOdd = oddDiff;
										}
										else
										{
											this->ivtcLastOdd = (this->ivtcLastOdd + oddDiff) >> 1;
										}
									}
								}
							}
						}
					
						if (frameType != Media::FT_INTERLACED_NODEINT && !ivtcFirstFrame)
						{
							if (this->ivtcLastSC == 2)
							{
								if (frameType == Media::FT_INTERLACED_TFF)
								{
									this->ivtcLastEven = evenDiff;
									this->ivtcLastOdd = evenDiff;
									ivtcFirstFrame = true;
								}
								else
								{
									this->ivtcLastEven = oddDiff;
									this->ivtcLastOdd = oddDiff;
									ivtcFirstFrame = true;
								}
								this->ivtcLastSC = 0;
							}
							else if (this->ivtcLastSC == 1)
							{
								this->ivtcLastEven = evenDiff;
								this->ivtcLastOdd = oddDiff;
								this->ivtcLastSC = 0;
								ivtcFirstFrame = true;
							}
							else
							{
								if (oddDiff > this->ivtcLastOdd * 4 && evenDiff > this->ivtcLastEven * 4 && oddDiff * 2 > pixelCnt && evenDiff * 2 > pixelCnt)
								{
									this->ivtcLastSC = 1;
								}
								else if ((oddDiff > this->ivtcLastOdd * 4 && oddDiff * 2 > pixelCnt) || (evenDiff > this->ivtcLastEven * 4 && evenDiff * 2 > pixelCnt))
								{
									this->ivtcLastSC = 2;
								}
								else
								{
									if (oddDiff > this->ivtcLastOdd)
									{
										this->ivtcLastOdd = oddDiff;
									}
									else
									{
										this->ivtcLastOdd = (this->ivtcLastOdd + oddDiff) >> 1;
									}
									if (evenDiff > this->ivtcLastEven)
									{
										this->ivtcLastEven = evenDiff;
									}
									else
									{
										this->ivtcLastEven = (this->ivtcLastEven + evenDiff) >> 1;
									}
								}
							}
						}

						if (frameType == Media::FT_INTERLACED_TFF || frameType == Media::FT_INTERLACED_BFF)
						{
							mergedFrame = true;
							UInt8 *oddBuff;
							UInt8 *evenBuff;
							UInt8 *outBuff = this->ivtcCurrFrame;
							if (frameType == Media::FT_INTERLACED_TFF)
							{
								oddBuff = imgData[0];
								evenBuff = this->ivtcLastFrame;
							}
							else
							{
								oddBuff = this->ivtcLastFrame;
								evenBuff = imgData[0];
							}
							UOSInt ivtcW = this->videoInfo->dispWidth;
							UOSInt ivtcH = this->videoInfo->dispHeight;
							UOSInt ivtcW2 = ivtcW << 1;
							UOSInt ivtcWH = ivtcW >> 1;
							UOSInt ivtcCnt = ivtcH;
							while (ivtcCnt > 0)
							{
								MemCopyNANC(outBuff, oddBuff, ivtcW);
								MemCopyNANC(outBuff + ivtcW, evenBuff + ivtcW, ivtcW);
								outBuff += ivtcW2;
								oddBuff += ivtcW2;
								evenBuff += ivtcW2;
								ivtcCnt -= 2;
							}
							ivtcCnt = ivtcH >> 1;
							while (ivtcCnt > 0)
							{
								MemCopyNANC(outBuff, oddBuff, ivtcWH);
								MemCopyNANC(outBuff + ivtcWH, evenBuff + ivtcWH, ivtcWH);
								outBuff += ivtcW;
								oddBuff += ivtcW;
								evenBuff += ivtcW;
								ivtcCnt -= 2;
							}
							ivtcCnt = ivtcH >> 1;
							while (ivtcCnt > 0)
							{
								MemCopyNANC(outBuff, oddBuff, ivtcWH);
								MemCopyNANC(outBuff + ivtcWH, evenBuff + ivtcWH, ivtcWH);
								outBuff += ivtcW;
								oddBuff += ivtcW;
								evenBuff += ivtcW;
								ivtcCnt -= 2;
							}
							frameType = Media::FT_INTERLACED_NODEINT;
						}
					}
				}
				else
				{
					if (this->ivtcLastSC == 0 && evenDiff > this->ivtcLastEven * 2 && oddDiff > this->ivtcLastOdd * 2 && evenDiff * 2 > pixelCnt && oddDiff * 2 > pixelCnt)
					{
						this->ivtcLastSC = 1;
						frameType = Media::FT_INTERLACED_NODEINT;
						this->ivtcLastFieldUsed = false;
						ivtcFirstFrame = true;
					}
					else if (frameType == Media::FT_INTERLACED_TFF)
					{
						if (this->ivtcLastSC == 2 && (oddDDiff > 0.1 && evenDDiff < oddDDiff * 0.3))
						{
							this->ivtcLastEven = oddDiff;
							this->ivtcLastOdd = oddDiff;
							frameType = Media::FT_INTERLACED_NODEINT;
							ivtcFirstFrame = true;
							this->ivtcLastSC = 0;
						}
						else if (evenDDiff > 0.1 && oddDDiff < evenDDiff * 0.5)
						{
							frameType = Media::FT_DISCARD;
						}
						else if (oddMDiff < evenMDiff * 0.3)
						{
							frameType = Media::FT_DISCARD;
						}
						else if (oddMDiff < evenMDiff * 0.4 && this->ivtcExist > 0)
						{
							frameType = Media::FT_DISCARD;
						}
						else if ((this->ivtcExist == IVTCINTERVAL - 5) && (oddMDiff < evenMDiff * 0.7))
						{
							frameType = Media::FT_DISCARD;
						}
						
						if (frameType == Media::FT_DISCARD)
						{
							this->ivtcExist = IVTCINTERVAL;
							this->ivtcLastFieldUsed = true;
							if (this->ivtcLastSC == 2)
							{
								this->ivtcLastEven = evenDiff;
								this->ivtcLastOdd = evenDiff;
								this->ivtcLastSC = 0;
								ivtcFirstFrame = true;

							}
							else if (this->ivtcLastSC == 1)
							{
								this->ivtcLastEven = evenDiff;
								this->ivtcLastOdd = evenDiff;
								this->ivtcLastSC = 0;
								ivtcFirstFrame = true;
							}
							else
							{
								if (evenDiff > this->ivtcLastEven * 4 && evenDiff * 2 > pixelCnt)
								{
									if (oddDiff > this->ivtcLastOdd * 4)
									{
										this->ivtcLastSC = 1;
									}
									else
									{
										this->ivtcLastSC = 2;
									}
								}
								else if (evenDiff > this->ivtcLastEven)
								{
									this->ivtcLastEven = evenDiff;
								}
								else
								{
									this->ivtcLastEven = (this->ivtcLastEven + evenDiff) >> 1;
								}
							}
						}
					}
					else if (frameType == Media::FT_INTERLACED_BFF)
					{
						if (this->ivtcLastSC == 2 && (evenDDiff > 0.1 && oddDDiff < evenDDiff * 0.3))
						{
							this->ivtcLastEven = evenDiff;
							this->ivtcLastOdd = evenDiff;
							frameType = Media::FT_INTERLACED_NODEINT;
							ivtcFirstFrame = true;
							this->ivtcLastSC = 0;
						}
						else if (oddDDiff > 0.1 && evenDDiff < oddDDiff * 0.5)
						{
							frameType = Media::FT_DISCARD;
						}
						else if (evenMDiff < oddMDiff * 0.3)
						{
							frameType = Media::FT_DISCARD;
						}
						else if (evenMDiff < oddMDiff * 0.4 && this->ivtcExist > 0)
						{
							frameType = Media::FT_DISCARD;
						}
						else if ((this->ivtcExist == IVTCINTERVAL - 5) && (evenMDiff < oddMDiff * 0.7))
						{
							frameType = Media::FT_DISCARD;
						}

						if (frameType == Media::FT_DISCARD)
						{
							this->ivtcExist = IVTCINTERVAL;
							this->ivtcLastFieldUsed = true;
							if (this->ivtcLastSC == 2)
							{
								this->ivtcLastEven = oddDiff;
								this->ivtcLastOdd = oddDiff;
								this->ivtcLastSC = 0;
								ivtcFirstFrame = true;
							}
							if (this->ivtcLastSC == 1)
							{
								this->ivtcLastEven = oddDiff;
								this->ivtcLastOdd = oddDiff;
								this->ivtcLastSC = 0;
								ivtcFirstFrame = true;
							}
							else
							{
								if (oddDiff > this->ivtcLastOdd * 4 && oddDiff * 2 > pixelCnt)
								{
									if (evenDiff > this->ivtcLastEven * 4)
									{
										this->ivtcLastSC = 1;
									}
									else
									{
										this->ivtcLastSC = 2;
									}
								}
								else if (oddDiff > this->ivtcLastOdd)
								{
									this->ivtcLastOdd = oddDiff;
								}
								else
								{
									this->ivtcLastOdd = (this->ivtcLastOdd + oddDiff) >> 1;
								}
							}
						}
					}
					if (frameType != Media::FT_DISCARD && !ivtcFirstFrame)
					{
						if (this->ivtcLastSC)
						{
							if (this->ivtcLastSC == 2)
							{
								ivtcFirstFrame = true;
							}
							this->ivtcLastEven = evenDiff;
							this->ivtcLastOdd = oddDiff;
							this->ivtcLastSC = 0;
						}
						else
						{
							if (oddDiff > this->ivtcLastOdd * 4 && evenDiff > this->ivtcLastEven * 4 && oddDiff * 2 > pixelCnt && evenDiff * 2 > pixelCnt)
							{
								this->ivtcLastSC = 1;
							}
							else if ((oddDiff > this->ivtcLastOdd * 4 && oddDiff * 2 > pixelCnt) || (evenDiff > this->ivtcLastEven * 4 && evenDiff * 2 > pixelCnt))
							{
								this->ivtcLastSC = 2;
							}
							else
							{
								if (oddDiff > this->ivtcLastOdd)
								{
									this->ivtcLastOdd = oddDiff;
								}
								else
								{
									this->ivtcLastOdd = (this->ivtcLastOdd + oddDiff) >> 1;
								}
								if (evenDiff > this->ivtcLastEven)
								{
									this->ivtcLastEven = evenDiff;
								}
								else
								{
									this->ivtcLastEven = (this->ivtcLastEven + evenDiff) >> 1;
								}
							}
						}
					}
				}

				{
					UInt8 *ivtcPtr;
					if (mergedFrame)
					{
						ivtcPtr = this->ivtcCurrFrame;
					}
					else
					{
						ivtcPtr = imgData[0];
					}
					this->CalcFieldStatP(&fieldStat, ivtcPtr, this->videoInfo->storeWidth, this->videoInfo->storeHeight);
				}
				UOSInt fieldRate;
				if (this->ivtcLastSC)
				{
					fieldRate = this->videoInfo->storeWidth * this->videoInfo->dispHeight * 2;
					if (frameType == Media::FT_DISCARD)
					{
						this->ivtcLastField = 0;
					}
					else
					{
						this->ivtcLastField = fieldStat.fieldDiff;
					}
				}
				else
				{
					if (this->ivtcLastField == 0)
					{
						fieldRate = this->videoInfo->storeWidth * this->videoInfo->dispHeight * 2;
					}
					else
					{
						fieldRate = this->videoInfo->storeWidth * this->videoInfo->dispHeight * 2;//me->ivtcLastField * 2;
					}
				}
				Bool needDI = false;
				if (fieldStat.field2MDiff > 150)
				{
					needDI = true;
				}
				else if (fieldStat.fieldDiff > fieldRate)
				{
					needDI = true;
				}
				else if (UOSInt2Double(fieldStat.fieldCnt * 2) > pixelCnt)
				{
					needDI = true;	
				}
				else
				{
					if (this->ivtcExist <= 0 || ivtcFirstFrame)
					{
						if (fieldStat.fieldDiff > this->ivtcLastOdd && fieldStat.fieldDiff > this->ivtcLastEven && fieldStat.fieldDiff > 10000)
						{
							needDI = true;
						}
/*						if (field3Cnt * 50 > field2Cnt && field3Cnt > 100)
						{
							needDI = true;
						}
						else if (field3Cnt * 100 > field2Cnt && field3Cnt > 100 && field4Cnt * 1000 > field2Cnt)
						{
							needDI = true;
						}
						else if (field3Cnt > 100 && field4Cnt * 4 > field3Cnt)
						{
							needDI = true;
						}
						else if (field2Cnt * 2 < field2Diff && field2Cnt * 10 > pixelCnt)
						{
							needDI = true;
						}*/
					}
				}
				if (frameType != Media::FT_DISCARD && needDI)
				{
					if (this->ivtcLastSC == 1)
					{
						this->ivtcLastSC = 2;
					}
					frameType = mergedFrame?diFrameType2:diFrameType;
				}
				else
				{
					if (frameType == Media::FT_INTERLACED_TFF || frameType == Media::FT_INTERLACED_BFF || frameType == Media::FT_INTERLACED_NODEINT)
					{
						frameType = Media::FT_INTERLACED_NODEINT;
					}
					else if (frameType == Media::FT_MERGED_TF || frameType == Media::FT_MERGED_BF)
					{
					}
					else if (frameType == Media::FT_NON_INTERLACE)
					{
					}
				}

#ifdef _DEBUG
				if (this->debugLog)
				{
					UTF8Char sbuff[256];
					UTF8Char *sptr;
					sptr = Text::StrConcatC(sbuff, UTF8STRC("IVTC\t"));
					sptr = Text::StrInt32(sptr, frameNum);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrOSInt(sptr, fieldStat.fieldDiff);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrOSInt(sptr, fieldStat.fieldCnt);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrOSInt(sptr, fieldStat.fieldMDiff);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrOSInt(sptr, fieldStat.field2Diff);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrOSInt(sptr, fieldStat.field2Cnt);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrOSInt(sptr, fieldStat.field2MDiff);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
/*					sptr = Text::StrOSInt(sptr, fieldStat.field3Cnt);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrOSInt(sptr, fieldStat.field4Cnt);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));*/
					sptr = Text::StrInt32(sptr, oddDiff);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrInt32(sptr, oddMDiff);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrInt32(sptr, evenDiff);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrInt32(sptr, evenMDiff);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrInt32(sptr, this->ivtcLastSC);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrInt32(sptr, this->ivtcLastFieldUsed?1:0);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrInt32(sptr, this->ivtcLastOdd);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrInt32(sptr, this->ivtcLastEven);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					sptr = Text::StrInt32(sptr, outFrameTime);
					sptr = Text::StrConcatC(sptr, UTF8STRC("\t"));
					switch (frameType)
					{
					case Media::FT_NON_INTERLACE:
						sptr = Text::StrConcatC(sptr, UTF8STRC("Progressive"));
						break;
					case Media::FT_INTERLACED_TFF:
						sptr = Text::StrConcatC(sptr, UTF8STRC("Interlaced TFF"));
						break;
					case Media::FT_INTERLACED_BFF:
						sptr = Text::StrConcatC(sptr, UTF8STRC("Interlaced BFF"));
						break;
					case Media::FT_FIELD_TF:
						sptr = Text::StrConcatC(sptr, UTF8STRC("Top Field"));
						break;
					case Media::FT_FIELD_BF:
						sptr = Text::StrConcatC(sptr, UTF8STRC("Bottom Field"));
						break;
					case Media::FT_MERGED_TF:
						sptr = Text::StrConcatC(sptr, UTF8STRC("Merged Top Field"));
						break;
					case Media::FT_MERGED_BF:
						sptr = Text::StrConcatC(sptr, UTF8STRC("Merged Bottom Field"));
						break;
					case Media::FT_DISCARD:
						sptr = Text::StrConcatC(sptr, UTF8STRC("Discard"));
						break;
					case Media::FT_INTERLACED_NODEINT:
						sptr = Text::StrConcatC(sptr, UTF8STRC("Interlaced (No deinterlace)"));
						break;
					}
					this->debugLog->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
				}
#endif
			}

			this->ivtcLastExist = true;
			this->ivtcLastFrameSize = dataSize;
			this->ivtcLastFrameType = frameType;
			this->ivtcLastFrameNum = frameNum;
			this->ivtcLastFrameTime = frameTime;
			MemCopyNANC(this->ivtcLastFrame, imgData[0], dataSize);
			if (frameType == Media::FT_DISCARD)
			{
				mutUsage.EndUse();
				return;
			}
			if (mergedFrame)
			{
				MemCopyNANC(imgData[0], this->ivtcCurrFrame, dataSize);
			}
			frameTime = outFrameTime;
		}
	}
	mutUsage.EndUse();
	if (this->videoCb)
	{
		this->videoCb(frameTime, frameNum, imgData, dataSize, frameStruct, this->userData, frameType, flags, ycOfst);
	}
}

void Media::VideoFilter::IVTCFilter::ClearIVTC()
{
	this->ivtcLastExist = false;
	this->ivtcLastField = 0;
	this->ivtcLastOdd = 0;
	this->ivtcLastEven = 0;
	this->ivtcLastSC = 1;
	this->ivtcExist = 0;

	this->fieldExist = false;
}

void Media::VideoFilter::IVTCFilter::StartIVTC(UInt32 frameTime, UInt32 frameNum, UInt8 *imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	this->ivtcTFrameTime = frameTime;
	this->ivtcTFrameNum = frameNum;
	this->ivtcTImgData = imgData;
	this->ivtcTDataSize = dataSize;
	this->ivtcTFrameStruct = frameStruct;
	this->ivtcTFrameType = frameType;
	this->ivtcTFlags = flags;
	this->ivtcTYCOfst = ycOfst;
	this->ivtcTRequest = true;
	this->ivtcTEvt->Set();
	while (this->ivtcTRequest == true && this->ivtcTStatus == 1)
	{
		this->mainEvt->Wait(100);
	}
}

UInt32 __stdcall Media::VideoFilter::IVTCFilter::IVTCThread(void *userObj)
{
	Media::VideoFilter::IVTCFilter *me = (Media::VideoFilter::IVTCFilter *)userObj;
	Sync::Thread::SetPriority(Sync::Thread::TP_HIGHEST);
	me->ivtcTStatus = 1;
	me->mainEvt->Set();
	while (!me->ivtcTToStop)
	{
		if (me->ivtcTRequest)
		{
			UInt32 frameTime = me->ivtcTFrameTime;
			UInt32 frameNum = me->ivtcTFrameNum;
			UInt8 *imgData = me->ivtcTImgData;
			UOSInt dataSize = me->ivtcTDataSize;
			Media::IVideoSource::FrameStruct frameStruct = me->ivtcTFrameStruct;
			Media::FrameType frameType = me->ivtcTFrameType;
			Media::IVideoSource::FrameFlag flags = me->ivtcTFlags;
			Media::YCOffset ycOfst = me->ivtcTYCOfst;

			me->ivtcTStatus = 2;
			me->ivtcTRequest = false;
			me->mainEvt->Set();
			me->do_IVTC(frameTime, frameNum, &imgData, dataSize, frameStruct, frameType, flags, ycOfst);
			me->ivtcTStatus = 1;
			me->mainEvt->Set();
		}
		me->ivtcTEvt->Wait(1000);
	}
	me->ivtcTStatus = 0;
	me->mainEvt->Set();
	return 0;
}

UInt32 __stdcall Media::VideoFilter::IVTCFilter::CalcThread(void *userObj)
{
	ThreadStat *tStat = (ThreadStat*)userObj;
	Sync::Thread::SetPriority(Sync::Thread::TP_HIGHEST);
	tStat->threadStat = 1;
	tStat->me->mainEvt->Set();
	while (true)
	{
		if (tStat->currCmd == 1)
		{
			break;
		}
		else if (tStat->currCmd == 2)
		{
			tStat->threadStat = 2;
			do_CalcFieldStat(&tStat->fieldStat, tStat->oddPtr, tStat->evenPtr, tStat->sw, tStat->h);
			tStat->threadStat = 1;
			tStat->currCmd = 0;
			tStat->me->mainEvt->Set();
		}
		else if (tStat->currCmd == 3)
		{
			tStat->threadStat = 2;
			do_CalcFieldStatP(&tStat->fieldStat, tStat->oddPtr, tStat->sw, tStat->h);
			tStat->threadStat = 1;
			tStat->currCmd = 0;
			tStat->me->mainEvt->Set();
		}
		else if (tStat->currCmd == 4)
		{
			UOSInt i;
			UOSInt h;
			UOSInt sw;
			UOSInt sw2;
			UInt8 *srcPtr;
			UInt8 *destPtr;
			tStat->threadStat = 2;
			sw = tStat->sw;
			h = tStat->h;
			sw2 = sw << 1;
			srcPtr = tStat->oddPtr;
			destPtr = tStat->evenPtr;
			i = 0;
			while (i < h)
			{
				MemCopyNANC(destPtr, srcPtr, sw);
				srcPtr += sw2;
				destPtr += sw2;
				i++;
			}
			tStat->threadStat = 1;
			tStat->currCmd = 0;
			tStat->me->mainEvt->Set();
		}
		else if (tStat->currCmd == 0)
		{
		}
		else
		{
			tStat->currCmd = 0;
		}
		tStat->evt->Wait(1000);
	}
	tStat->currCmd = 0;
	tStat->threadStat = 0;
	tStat->me->mainEvt->Set();
	return 0;
}

void Media::VideoFilter::IVTCFilter::do_CalcFieldStat(FieldStat *fieldStat, UInt8 *oddPtr, UInt8 *evenPtr, UOSInt w, UOSInt h)
{
	if (w & 15)
	{
		UOSInt w2 = w << 1;
		UOSInt hLeft;
		UOSInt wLeft;
		UOSInt v1;
		UOSInt v2;
		UOSInt v3;
		UOSInt v4;
		UOSInt v5;
		fieldStat->fieldDiff = 0;
		fieldStat->fieldMDiff = 0;
		fieldStat->fieldCnt = 0;
		fieldStat->field2Diff = 0;
		fieldStat->field2MDiff = 0;
		fieldStat->field2Cnt = 0;
	//	fieldStat->field3Cnt = 0;
	//	fieldStat->field4Cnt = 0;

		hLeft = h - 2;
		while (hLeft > 0)
		{
			hLeft -= 2;
			wLeft = w;
			while (wLeft-- > 0)
			{
				v1 = oddPtr[0];
				v2 = oddPtr[w2];
				v3 = evenPtr[w];

				if (v1 >= v2)
				{
					if (v3 > v1)
					{
						v4 = v3 - v1;
						fieldStat->fieldDiff += v4;
						fieldStat->fieldCnt++;
						if (v4 > fieldStat->fieldMDiff)
						{
							fieldStat->fieldMDiff = v4;
						}
						if (v4 > v1 - v2)
						{
							v5 = v4 - (v1 - v2);
							fieldStat->field2Diff += v5;
							fieldStat->field2Cnt++;
							if (v5 > fieldStat->field2MDiff)
							{
								fieldStat->field2MDiff = v5;
							}
	/*						if (v5 > 16)
							{
								fieldStat->field3Cnt++;
							}
							if (v5 > 32)
							{
								fieldStat->field4Cnt++;
							}*/
						}
					}
					else if (v3 < v2)
					{
						v4 = v2 - v3;
						fieldStat->fieldDiff += v4;
						fieldStat->fieldCnt++;
						if (v4 > fieldStat->fieldMDiff)
						{
							fieldStat->fieldMDiff = v4;
						}
						if (v4 > v1 - v2)
						{
							v5 = v4 - (v1 - v2);
							fieldStat->field2Diff += v5;
							fieldStat->field2Cnt++;
							if (v5 > fieldStat->field2MDiff)
							{
								fieldStat->field2MDiff = v5;
							}
	/*						if (v5 > 16)
							{
								fieldStat->field3Cnt++;
							}
							if (v5 > 32)
							{
								fieldStat->field4Cnt++;
							}*/
						}
					}
				}
				else
				{
					if (v3 < v1)
					{
						v4 = v1 - v3;
						fieldStat->fieldDiff += v4;
						fieldStat->fieldCnt++;
						if (v4 > fieldStat->fieldMDiff)
						{
							fieldStat->fieldMDiff = v4;
						}
						if (v4 > v2 - v1)
						{
							v5 = v4 - (v2 - v1);
							fieldStat->field2Diff += v5;
							fieldStat->field2Cnt++;
							if (v5 > fieldStat->field2MDiff)
							{
								fieldStat->field2MDiff = v5;
							}
	/*						if (v5 > 16)
							{
								fieldStat->field3Cnt++;
							}
							if (v5 > 32)
							{
								fieldStat->field4Cnt++;
							}*/
						}
					}
					else if (v3 > v2)
					{
						v4 = v3 - v2;
						fieldStat->fieldDiff += v4;
						fieldStat->fieldCnt++;
						if (v4 > fieldStat->fieldMDiff)
						{
							fieldStat->fieldMDiff = v4;
						}
						if (v4 > v2 - v1)
						{
							v5 = v4 - (v2 - v1);
							fieldStat->field2Diff += v5;
							fieldStat->field2Cnt++;
							if (v5 > fieldStat->field2MDiff)
							{
								fieldStat->field2MDiff = v5;
							}
	/*						if (v5 > 16)
							{
								fieldStat->field3Cnt++;
							}
							if (v5 > 32)
							{
								fieldStat->field4Cnt++;
							}*/
						}
					}
				}

				v1 = evenPtr[w];
				v2 = evenPtr[w + w2];
				v3 = oddPtr[w2];

				if (v1 >= v2)
				{
					if (v3 > v1)
					{
						v4 = v3 - v1;
						fieldStat->fieldDiff += v4;
						fieldStat->fieldCnt++;
						if (v4 > fieldStat->fieldMDiff)
						{
							fieldStat->fieldMDiff = v4;
						}
						if (v4 > v1 - v2)
						{
							v5 = v4 - (v1 - v2);
							fieldStat->field2Diff += v5;
							fieldStat->field2Cnt++;
							if (v5 > fieldStat->field2MDiff)
							{
								fieldStat->field2MDiff = v5;
							}
	/*						if (v5 > 16)
							{
								fieldStat->field3Cnt++;
							}
							if (v5 > 32)
							{
								fieldStat->field4Cnt++;
							}*/
						}
					}
					else if (v3 < v2)
					{
						v4 = v2 - v3;
						fieldStat->fieldDiff += v4;
						fieldStat->fieldCnt++;
						if (v4 > fieldStat->fieldMDiff)
						{
							fieldStat->fieldMDiff = v4;
						}
						if (v4 > v1 - v2)
						{
							v5 = v4 - (v1 - v2);
							fieldStat->field2Diff += v5;
							fieldStat->field2Cnt++;
							if (v5 > fieldStat->field2MDiff)
							{
								fieldStat->field2MDiff = v5;
							}
	/*						if (v5 > 16)
							{
								fieldStat->field3Cnt++;
							}
							if (v5 > 32)
							{
								fieldStat->field4Cnt++;
							}*/
						}
					}
				}
				else
				{
					if (v3 < v1)
					{
						v4 = v1 - v3;
						fieldStat->fieldDiff += v4;
						fieldStat->fieldCnt++;
						if (v4 > fieldStat->fieldMDiff)
						{
							fieldStat->fieldMDiff = v4;
						}
						if (v4 > v2 - v1)
						{
							v5 = v4 - (v2 - v1);
							fieldStat->field2Diff += v5;
							fieldStat->field2Cnt++;
							if (v5 > fieldStat->field2MDiff)
							{
								fieldStat->field2MDiff = v5;
							}
	/*						if (v5 > 16)
							{
								fieldStat->field3Cnt++;
							}*/
	/*						if (v5 > 32)
							{
								fieldStat->field4Cnt++;
							}*/
						}
					}
					else if (v3 > v2)
					{
						v4 = v3 - v2;
						fieldStat->fieldDiff += v4;
						fieldStat->fieldCnt++;
						if (v4 > fieldStat->fieldMDiff)
						{
							fieldStat->fieldMDiff = v4;
						}
						if (v4 > v2 - v1)
						{
							v5 = v4 - (v2 - v1);
							fieldStat->field2Diff += v5;
							fieldStat->field2Cnt++;
							if (v5 > fieldStat->field2MDiff)
							{
								fieldStat->field2MDiff = v5;
							}
	/*						if (v5 > 16)
							{
								fieldStat->field3Cnt++;
							}
							if (v5 > 32)
							{
								fieldStat->field4Cnt++;
							}*/
						}
					}
				}
				oddPtr++;
				evenPtr++;
			}

			oddPtr += w;
			evenPtr += w;
		}
	}
	else
	{
		UInt32 fieldStats[6];
		fieldStats[0] = 0;		//OSInt fieldDiff = 0;
		fieldStats[1] = 0;		//OSInt fieldCnt = 0;
		fieldStats[2] = 0;		//OSInt field2Diff = 0;
		fieldStats[3] = 0;		//OSInt field2Cnt = 0;
		fieldStats[4] = 0;		//OSInt fieldMDiff;
		fieldStats[5] = 0;		//OSInt field2MDiff;

		IVTCFilter_CalcField(oddPtr, evenPtr, w, h, fieldStats);
		fieldStat->fieldDiff = fieldStats[0];
		fieldStat->fieldCnt = fieldStats[1];
		fieldStat->field2Diff = fieldStats[2];
		fieldStat->field2Cnt = fieldStats[3];
		fieldStat->fieldMDiff = fieldStats[4];
		fieldStat->field2MDiff = fieldStats[5];
	//	fieldStat->field3Cnt = 0;
	//	fieldStat->field4Cnt = 0;
	}
}

void Media::VideoFilter::IVTCFilter::do_CalcFieldStatP(FieldStat *fieldStat, UInt8 *framePtr, UOSInt w, UOSInt h)
{
	if (w & 15)
	{
		UOSInt wLeft;
		UOSInt v1;
		UOSInt v2;
		UOSInt v3;
		UOSInt v4;
		UOSInt v5;
		UOSInt w2 = w << 1;
		fieldStat->fieldDiff = 0;
		fieldStat->fieldMDiff = 0;
		fieldStat->fieldCnt = 0;
		fieldStat->field2Diff = 0;
		fieldStat->field2MDiff = 0;
		fieldStat->field2Cnt = 0;
	//	fieldStat->field3Cnt = 0;
	//	fieldStat->field4Cnt = 0;

		wLeft = w * (h - 2);
		while (wLeft-- > 0)
		{
			v1 = framePtr[0];
			v2 = framePtr[w2];
			v3 = framePtr[w];

			if (v1 >= v2)
			{
				if (v3 > v1)
				{
					v4 = v3 - v1;
					fieldStat->fieldDiff += v4;
					fieldStat->fieldCnt++;
					if (v4 > fieldStat->fieldMDiff)
					{
						fieldStat->fieldMDiff = v4;
					}
					if (v4 > v1 - v2)
					{
						v5 = v4 - (v1 - v2);
						fieldStat->field2Diff += v5;
						fieldStat->field2Cnt++;
						if (v5 > fieldStat->field2MDiff)
						{
							fieldStat->field2MDiff = v5;
						}
	/*					if (v5 > 16)
						{
							fieldStat->field3Cnt++;
						}
						if (v5 > 32)
						{
							fieldStat->field4Cnt++;
						}*/
					}
				}
				else if (v3 < v2)
				{
					v4 = v2 - v3;
					fieldStat->fieldDiff += v4;
					fieldStat->fieldCnt++;
					if (v4 > fieldStat->fieldMDiff)
					{
						fieldStat->fieldMDiff = v4;
					}
					if (v4 > v1 - v2)
					{
						v5 = v4 - (v1 - v2);
						fieldStat->field2Diff += v5;
						fieldStat->field2Cnt++;
						if (v5 > fieldStat->field2MDiff)
						{
							fieldStat->field2MDiff = v5;
						}
	/*					if (v5 > 16)
						{
							fieldStat->field3Cnt++;
						}
						if (v5 > 32)
						{
							fieldStat->field4Cnt++;
						}*/
					}
				}
			}
			else
			{
				if (v3 < v1)
				{
					v4 = v1 - v3;
					fieldStat->fieldDiff += v4;
					fieldStat->fieldCnt++;
					if (v4 > fieldStat->fieldMDiff)
					{
						fieldStat->fieldMDiff = v4;
					}
					if (v4 > v2 - v1)
					{
						v5 = v4 - (v2 - v1);
						fieldStat->field2Diff += v5;
						fieldStat->field2Cnt++;
						if (v5 > fieldStat->field2MDiff)
						{
							fieldStat->field2MDiff = v5;
						}
	/*					if (v5 > 16)
						{
							fieldStat->field3Cnt++;
						}
						if (v5 > 32)
						{
							fieldStat->field4Cnt++;
						}*/
					}
				}
				else if (v3 > v2)
				{
					v4 = v3 - v2;
					fieldStat->fieldDiff += v4;
					fieldStat->fieldCnt++;
					if (v4 > fieldStat->fieldMDiff)
					{
						fieldStat->fieldMDiff = v4;
					}
					if (v4 > v2 - v1)
					{
						v5 = v4 - (v2 - v1);
						fieldStat->field2Diff += v5;
						fieldStat->field2Cnt++;
						if (v5 > fieldStat->field2MDiff)
						{
							fieldStat->field2MDiff = v5;
						}
	/*					if (v5 > 16)
						{
							fieldStat->field3Cnt++;
						}
						if (v5 > 32)
						{
							fieldStat->field4Cnt++;
						}*/
					}
				}
			}

			framePtr++;
		}
	}
	else
	{
		UInt32 fieldStats[6];
		fieldStats[0] = 0;			//OSInt fieldDiff = 0;
		fieldStats[1] = 0;			//OSInt fieldCnt = 0;
		fieldStats[2] = 0;			//OSInt field2Diff = 0;
		fieldStats[3] = 0;			//OSInt field2Cnt = 0;
		fieldStats[4] = 0;			//OSInt fieldMDiff;
		fieldStats[5] = 0;			//OSInt field2MDiff;

		IVTCFilter_CalcFieldP(framePtr, w, h, fieldStats);
		fieldStat->fieldDiff = fieldStats[0];
		fieldStat->fieldCnt = fieldStats[1];
		fieldStat->field2Diff = fieldStats[2];
		fieldStat->field2Cnt = fieldStats[3];
		fieldStat->fieldMDiff = fieldStats[4];
		fieldStat->field2MDiff = fieldStats[5];
	//	fieldStat->field3Cnt = 0;
	//	fieldStat->field4Cnt = 0;
	}
}

void Media::VideoFilter::IVTCFilter::CalcFieldStat(Media::VideoFilter::IVTCFilter::FieldStat *fieldStat, UInt8 *oddPtr, UInt8 *evenPtr, UOSInt w, UOSInt h)
{
//	do_CalcFieldStat(fieldStat, oddPtr, evenPtr, w, h);
//	return;
	h = (h - 2) >> 1;
	UOSInt currH;
	UOSInt lastH = h;
	UOSInt i;
	Bool found;
	fieldStat->fieldDiff = 0;
	fieldStat->fieldMDiff = 0;
	fieldStat->fieldCnt = 0;
	fieldStat->field2Diff = 0;
	fieldStat->field2MDiff = 0;
	fieldStat->field2Cnt = 0;
//	fieldStat->field3Cnt = 0;
//	fieldStat->field4Cnt = 0;
	i = this->threadCnt;
	while (i-- > 0)
	{
		currH = MulDivUOS(h, i, this->threadCnt);
		this->threadStats[i].oddPtr = oddPtr + w * 2 * currH;
		this->threadStats[i].evenPtr = evenPtr + w * 2 * currH;
		this->threadStats[i].sw = w;
		this->threadStats[i].h = (lastH - currH) * 2 + 2;
		this->threadStats[i].currCmd = 2;
		this->threadStats[i].evt->Set();
		lastH = currH;
	}

	while (true)
	{
		found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threadStats[i].currCmd != 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
			break;
		this->mainEvt->Wait(100);
	}
	i = this->threadCnt;
	while (i-- > 0)
	{
		fieldStat->fieldDiff += this->threadStats[i].fieldStat.fieldDiff;
		if (fieldStat->fieldMDiff < this->threadStats[i].fieldStat.fieldMDiff) fieldStat->fieldMDiff = this->threadStats[i].fieldStat.fieldMDiff;
		fieldStat->fieldCnt += this->threadStats[i].fieldStat.fieldCnt;
		fieldStat->field2Diff += this->threadStats[i].fieldStat.field2Diff;
		if (fieldStat->field2MDiff < this->threadStats[i].fieldStat.field2MDiff) fieldStat->field2MDiff = this->threadStats[i].fieldStat.field2MDiff;
		fieldStat->field2Cnt += this->threadStats[i].fieldStat.field2Cnt;
	//	fieldStat->field3Cnt += this->threadStats[i].fieldStat.field3Cnt;
	//	fieldStat->field4Cnt += this->threadStats[i].fieldStat.field4Cnt;
	}
}

void Media::VideoFilter::IVTCFilter::CalcFieldStatP(FieldStat *fieldStat, UInt8 *framePtr, UOSInt w, UOSInt h)
{
	h = h - 2;
	UOSInt currH;
	UOSInt lastH = h;
	UOSInt i;
	Bool found;
	fieldStat->fieldDiff = 0;
	fieldStat->fieldMDiff = 0;
	fieldStat->fieldCnt = 0;
	fieldStat->field2Diff = 0;
	fieldStat->field2MDiff = 0;
	fieldStat->field2Cnt = 0;
//	fieldStat->field3Cnt = 0;
//	fieldStat->field4Cnt = 0;
	i = this->threadCnt;
	while (i-- > 0)
	{
		currH = MulDivUOS(h, i, this->threadCnt);
		this->threadStats[i].oddPtr = framePtr + w * currH;
		this->threadStats[i].sw = w;
		this->threadStats[i].h = (lastH - currH) + 2;
		this->threadStats[i].currCmd = 3;
		this->threadStats[i].evt->Set();
		lastH = currH;
	}

	while (true)
	{
		found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threadStats[i].currCmd != 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
			break;
		this->mainEvt->Wait(100);
	}
	i = this->threadCnt;
	while (i-- > 0)
	{
		fieldStat->fieldDiff += this->threadStats[i].fieldStat.fieldDiff;
		if (fieldStat->fieldMDiff < this->threadStats[i].fieldStat.fieldMDiff) fieldStat->fieldMDiff = this->threadStats[i].fieldStat.fieldMDiff;
		fieldStat->fieldCnt += this->threadStats[i].fieldStat.fieldCnt;
		fieldStat->field2Diff += this->threadStats[i].fieldStat.field2Diff;
		if (fieldStat->field2MDiff < this->threadStats[i].fieldStat.field2MDiff) fieldStat->field2MDiff = this->threadStats[i].fieldStat.field2MDiff;
		fieldStat->field2Cnt += this->threadStats[i].fieldStat.field2Cnt;
	//	fieldStat->field3Cnt += this->threadStats[i].fieldStat.field3Cnt;
	//	fieldStat->field4Cnt += this->threadStats[i].fieldStat.field4Cnt;
	}
}

Media::VideoFilter::IVTCFilter::IVTCFilter(Media::IVideoSource *srcVideo) : Media::VideoFilter::VideoFilterBase(srcVideo)
{
	UOSInt i;
	Bool found;
	this->enabled = true;
	NEW_CLASS(this->mut, Sync::Mutex());
	this->fieldBuff = 0;
	this->fieldBuffSize = 0;
	this->fieldExist = false;

	this->ivtcLastFrame = 0;
	this->ivtcCurrFrame = 0;
	this->ivtcLastFrameSize = 0;
	this->ivtcFrameBuffSize = 0;
	this->ivtcLastExist = false;
	this->ivtcLastFieldUsed = false;
#ifdef _DEBUG
	NEW_CLASS(this->debugFS, IO::FileStream(CSTR("IVTC.log"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(this->debugLog, Text::UTF8Writer(this->debugFS));
#endif
	this->threadCnt = Sync::Thread::GetThreadCnt();
	if (this->threadCnt > 8)
	{
		this->threadCnt = 8;
	}
	else if (this->threadCnt <= 0)
	{
		this->threadCnt = 1;
	}
	NEW_CLASS(this->mainEvt, Sync::Event(true));
	this->threadStats = MemAlloc(ThreadStat, this->threadCnt);
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threadStats[i].threadStat = 0;
		this->threadStats[i].me = this;
		this->threadStats[i].currCmd = 0;
		NEW_CLASS(this->threadStats[i].evt, Sync::Event(true));
		Sync::Thread::Create(CalcThread, &this->threadStats[i]);
	}

	while (true)
	{
		found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (threadStats[i].threadStat == 0)
				found = true;
		}
		if (!found)
			break;
		this->mainEvt->Wait(100);
	}
	this->ivtcTToStop = false;
	NEW_CLASS(this->ivtcTEvt, Sync::Event(true));
	this->ivtcTStatus = 0;
	this->ivtcTRequest = false;
	Sync::Thread::Create(IVTCThread, this);
	while (this->ivtcTStatus == 0)
	{
		this->mainEvt->Wait(100);
	}
}

Media::VideoFilter::IVTCFilter::~IVTCFilter()
{
	UOSInt i;
	Bool found = true;
	this->ivtcTToStop = true;
	this->ivtcTEvt->Set();
	while (this->ivtcTStatus != 0)
	{
		this->mainEvt->Wait(100);
	}
	while (true)
	{
		found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threadStats[i].threadStat != 0)
			{
				if (this->threadStats[i].currCmd == 0)
				{
					this->threadStats[i].currCmd = 1;
					this->threadStats[i].evt->Set();
				}
				found = true;
			}
		}
		if (!found)
			break;
		this->mainEvt->Wait(100);
	}
	
	if (this->ivtcLastFrame)
	{
		MemFreeA64(this->ivtcLastFrame);
		MemFreeA64(this->ivtcCurrFrame);
		this->ivtcLastFrame = 0;
		this->ivtcCurrFrame = 0;
	}
	if (this->fieldBuff)
	{
		MemFreeA64(this->fieldBuff);
		this->fieldBuff = 0;
	}
	DEL_CLASS(this->mut);
	i = this->threadCnt;
	while (i-- > 0)
	{
		DEL_CLASS(this->threadStats[i].evt);
	}
	DEL_CLASS(this->ivtcTEvt);
	MemFree(this->threadStats);
	DEL_CLASS(this->mainEvt);
#ifdef _DEBUG
	DEL_CLASS(this->debugLog);
	DEL_CLASS(this->debugFS);
#endif
}

Text::CString Media::VideoFilter::IVTCFilter::GetFilterName()
{
	return CSTR("IVTCFilter");
}

void Media::VideoFilter::IVTCFilter::SetEnabled(Bool enabled)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->enabled = enabled;
	this->ClearIVTC();
	mutUsage.EndUse();
}

void Media::VideoFilter::IVTCFilter::Stop()
{
	if (this->srcVideo)
	{
		this->srcVideo->Stop();
		while (this->ivtcTStatus == 2)
		{
			this->mainEvt->Wait(100);
		}
	}
}
