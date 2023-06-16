#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/StreamWriter.h"
#include "Manage/HiResClock.h"
#include "Math/FFTCalc.h"
#include "Math/Math.h"
#include "Media/AudioFilter/DTMFDecoder.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

#define FFTAVG 1
//#define SHOWLOG

UInt32 __stdcall Media::AudioFilter::DTMFDecoder::CalcThread(void *userObj)
{
	Media::AudioFilter::DTMFDecoder *me = (Media::AudioFilter::DTMFDecoder *)userObj;

	Double *avgData = MemAlloc(Double, me->sampleCnt);
	UInt8 *tmpBuff = MemAlloc(UInt8, me->sampleBuffSize);
	Double v;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt align;
	UOSInt ind697;
	UOSInt ind770;
	UOSInt ind852;
	UOSInt ind941;
	UOSInt ind1209;
	UOSInt ind1336;
	UOSInt ind1477;
	UOSInt ind1633;
	Double maxLo;
	Double maxHi;
	UOSInt maxHiInd;
	UOSInt maxLoInd;

	Int32 minAbsVol;
	Int32 maxAbsVol;
	Int32 currAbsVol;
	Int32 thisVol;
	Int32 thisAbsVol;
	UInt32 volSamples = (me->frequency / 697 / 2 + 1) * me->nChannels * (me->bitCount >> 3);
	Double maxAbsVolD;
	Double minAbsVolD;

	Int32 hiFreq;
	Int32 loFreq;
	WChar tone;
	Double dSampleCnt = UOSInt2Double(me->sampleCnt);
	Double dFreq = dSampleCnt / (Double)me->frequency;
	ind697 = (UInt32)Double2Int32(697.0 * dFreq);
	ind770 = (UInt32)Double2Int32(770.0 * dFreq);
	ind852 = (UInt32)Double2Int32(852.0 * dFreq);
	ind941 = (UInt32)Double2Int32(941.0 * dFreq);
	ind1209 = (UInt32)Double2Int32(1209.0 * dFreq);
	ind1336 = (UInt32)Double2Int32(1336.0 * dFreq);
	ind1477 = (UInt32)Double2Int32(1477.0 * dFreq);
	ind1633 = (UInt32)Double2Int32(1633.0 * dFreq);
	maxAbsVolD = 0;
	minAbsVolD = 0;
	maxAbsVol = 0;
	minAbsVol = 0;

	{
		me->threadRunning = true;
#ifdef SHOWLOG
		Manage::HiResClock clk;
		IO::FileStream debugFS(CSTR("DTMFDecoder.log"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Writer debugWriter(&debugFS);
		Text::StringBuilderUTF8 sb;
#endif
		Math::FFTCalc fft(me->sampleCnt, Math::FFTCalc::WT_BLACKMANN_HARRIS);
		while (!me->threadToStop)
		{
			if (me->calcReady)
			{
				me->calcReady = false;

				if (me->bitCount == 16)
				{
					align = me->nChannels << 1;

					Sync::MutexUsage mutUsage(&me->calcMut);
					MemCopyNO(tmpBuff, me->calcBuff, me->sampleBuffSize);
					mutUsage.EndUse();
					
					i = 0;
					j = me->sampleBuffSize - volSamples;
					while (i < j)
					{
						thisVol = ReadInt16(&tmpBuff[i]);
						if (thisVol < 0)
							thisAbsVol = -thisVol;
						else
							thisAbsVol = thisVol;
						currAbsVol = thisAbsVol;
						k = 2;
						while (k < volSamples)
						{
							thisVol = ReadInt16(&tmpBuff[i + k]);
							if (thisVol < 0)
								thisAbsVol = -thisVol;
							else
								thisAbsVol = thisVol;
							if (thisAbsVol > currAbsVol)
							{
								currAbsVol = thisAbsVol;
							}
							k += 2;
						}
						if (i == 0 || currAbsVol > maxAbsVol)
						{
							maxAbsVol = currAbsVol;
						}
						if (i == 0 || currAbsVol < minAbsVol)
						{
							minAbsVol = currAbsVol;
						}
						i += align;
					}
					maxAbsVolD = maxAbsVol / 32768.0;
					minAbsVolD = minAbsVol / 32768.0;

					fft.ForwardBits(tmpBuff, avgData, Math::FFTCalc::ST_I16, me->nChannels, 1 / maxAbsVolD);
				}
				else
				{
					align = me->nChannels * me->bitCount >> 3;
				}
				
				
				i = ind697 - 1;
				j = ind941 + 1;
				maxLo = avgData[i];
				maxLoInd = i;
				i++;
				while (i <= j)
				{
					v = avgData[i];
					if (v > maxLo)
					{
						maxLo = v;
						maxLoInd = i;
					}
					i++;
				}
				i = ind1209 - 1;
				j = ind1633 + 1;
				maxHi = avgData[i];
				maxHiInd = i;
				i++;
				while (i <= j)
				{
					v = avgData[i];
					if (v > maxHi)
					{
						maxHi = v;
						maxHiInd = i;
					}
					i++;
				}
				maxLo = maxLo / UOSInt2Double(me->sampleCnt);
				maxHi = maxHi / UOSInt2Double(me->sampleCnt);

				if (maxLo > 0.04)
				{
					if (maxLoInd == ind697 || maxLoInd == ind697 - 1 || maxLoInd == ind697 + 1)
					{
						loFreq = 697;
					}
					else if (maxLoInd == ind770 || maxLoInd == ind770 - 1 || maxLoInd == ind770 + 1)
					{
						loFreq = 770;
					}
					else if (maxLoInd == ind852 || maxLoInd == ind852 - 1 || maxLoInd == ind852 + 1)
					{
						loFreq = 852;
					}
					else if (maxLoInd == ind941 || maxLoInd == ind941 - 1 || maxLoInd == ind941 + 1)
					{
						loFreq = 941;
					}
					else
					{
						loFreq = 0;
					}
				}
				else
				{
					loFreq = -1;
				}
				if (maxHi > 0.04)
				{
					if (maxHiInd == ind1209 || maxHiInd == ind1209 - 1 || maxHiInd == ind1209 + 1)
					{
						hiFreq = 1209;
					}
					else if (maxHiInd == ind1336 || maxHiInd == ind1336 - 1 || maxHiInd == ind1336 + 1)
					{
						hiFreq = 1336;
					}
					else if (maxHiInd == ind1477 || maxHiInd == ind1477 - 1 || maxHiInd == ind1477 + 1)
					{
						hiFreq = 1477;
					}
					else if (maxHiInd == ind1633 || maxHiInd == ind1633 - 1 || maxHiInd == ind1633 + 1)
					{
						hiFreq = 1633;
					}
					else
					{
						hiFreq = 0;
					}
				}
				else
				{
					hiFreq = -1;
				}

				tone = '?';
				Bool valid = false;
				if (maxAbsVolD >= 0.2)
				{
					if (minAbsVolD / maxAbsVolD >= 0.1)
					{
						if (maxLo >= 0.05 && maxHi >= 0.05)
						{
							valid = true;
						}
					}
					else
					{
						Int32 lowLev = Double2Int32(maxAbsVolD * 0.1 * 32768.0);
						Bool isStart = true;
						Bool isLow = true;
						OSInt lowCnt = 0;
						valid = true;
						i = 0;
						j = me->sampleBuffSize - volSamples;
						while (i < j)
						{
							thisVol = ReadInt16(&tmpBuff[i]);
							if (thisVol < 0)
								thisAbsVol = -thisVol;
							else
								thisAbsVol = thisVol;
							currAbsVol = thisAbsVol;
							k = 2;
							while (k < volSamples)
							{
								thisVol = ReadInt16(&tmpBuff[i + k]);
								if (thisVol < 0)
									thisAbsVol = -thisVol;
								else
									thisAbsVol = thisVol;
								if (thisAbsVol > currAbsVol)
								{
									currAbsVol = thisAbsVol;
								}
								k += 2;
							}
							if (currAbsVol < lowLev)
							{
								isLow = true;
								lowCnt++;
							}
							else
							{
								if (isStart)
								{
									isStart = false;
									isLow = false;
								}
								else if (isLow)
								{
									valid = false;
									break;
								}
							}
							i += align;
						}
						if (OSInt2Double(lowCnt) > UOSInt2Double(j / align) * 0.3)
						{
							valid = false;
						}
					}
				}

				if (valid)
				{
					if (loFreq == 697)
					{
						if (hiFreq == 1209)
						{
							tone = '1';
						}
						else if (hiFreq == 1336)
						{
							tone = '2';
						}
						else if (hiFreq == 1477)
						{
							tone = '3';
						}
						else if (hiFreq == 1633)
						{
							tone = 'A';
						}
					}
					else if (loFreq == 770)
					{
						if (hiFreq == 1209)
						{
							tone = '4';
						}
						else if (hiFreq == 1336)
						{
							tone = '5';
						}
						else if (hiFreq == 1477)
						{
							tone = '6';
						}
						else if (hiFreq == 1633)
						{
							tone = 'B';
						}
					}
					else if (loFreq == 852)
					{
						if (hiFreq == 1209)
						{
							tone = '7';
						}
						else if (hiFreq == 1336)
						{
							tone = '8';
						}
						else if (hiFreq == 1477)
						{
							tone = '9';
						}
						else if (hiFreq == 1633)
						{
							tone = 'C';
						}
					}
					else if (loFreq == 941)
					{
						if (hiFreq == 1209)
						{
							tone = '*';
						}
						else if (hiFreq == 1336)
						{
							tone = '0';
						}
						else if (hiFreq == 1477)
						{
							tone = '#';
						}
						else if (hiFreq == 1633)
						{
							tone = 'D';
						}
					}
					else
					{
						if (hiFreq == -1 && loFreq == -1)
						{
							tone = 0;
						}
					}
				}
				else if (maxLo < 0.04 && maxHi < 0.04)
				{
					tone = 0;
				}
				else if (maxAbsVolD < 0.2 || minAbsVolD / maxAbsVolD < 0.1)
				{
					tone = 0;
				}
#ifdef SHOWLOG
				sb.ClearStr();
				sb.AppendDouble(clk.GetTimeDiff());
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendUOSInt(maxLoInd);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendDouble(maxLo);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendUOSInt(maxHiInd);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendDouble(maxHi);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendChar(tone, 1);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendDouble(minAbsVolD);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendDouble(maxAbsVolD);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendUOSInt(maxLoInd * me->frequency / me->sampleCnt);
				sb.AppendC(UTF8STRC("\t"));
				sb.AppendUOSInt(maxHiInd * me->frequency / me->sampleCnt);
				debugWriter.WriteLineCStr(sb.ToCString());
#endif

				if (me->currTone != tone && tone != '?')
				{
					me->currTone = tone;
					if (me->toneChgHdlr)
					{
						me->toneChgHdlr(me->toneChgObj, tone);
					}
				}
			}
			me->threadEvt.Wait(1000);
		}
		MemFree(avgData);
		MemFree(tmpBuff);
	}
	me->threadRunning = false;
	return 0;
}

void Media::AudioFilter::DTMFDecoder::ResetStatus()
{
	Sync::MutexUsage mutUsage(&this->sampleMut);
	this->calcReady = false;
	this->calcLeft = this->sampleCnt;
	this->sampleOfst = 0;
	this->currTone = 0;
}

Media::AudioFilter::DTMFDecoder::DTMFDecoder(Media::IAudioSource *audSrc, UOSInt calcInt) : Media::IAudioFilter(audSrc)
{
	UOSInt i;
	Media::AudioFormat fmt;
	audSrc->GetFormat(&fmt);
	i = 1;
	while ((fmt.frequency * 2 / i) > 73)
	{
		i = i << 1;
	}
	this->sampleCnt = i;
	this->sampleBuffSize = fmt.align * (i + FFTAVG - 1);
	this->sampleBuff = MemAlloc(UInt8, this->sampleBuffSize);
	this->sampleOfst = 0;
	this->calcInt = calcInt;
	this->calcLeft = this->sampleCnt;
	this->calcBuff = MemAlloc(UInt8, this->sampleBuffSize);
	this->nChannels = fmt.nChannels;
	this->bitCount = fmt.bitpersample;
	this->align = fmt.align;
	this->frequency = fmt.frequency;
	this->toneChgHdlr = 0;
	this->toneChgObj = 0;
	this->threadToStop = false;
	this->threadRunning = false;
	Sync::ThreadUtil::Create(CalcThread, this);
	this->ResetStatus();
}

Media::AudioFilter::DTMFDecoder::~DTMFDecoder()
{
	this->threadToStop = true;
	this->threadEvt.Set();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	MemFree(this->sampleBuff);
	MemFree(this->calcBuff);
}

UInt32 Media::AudioFilter::DTMFDecoder::SeekToTime(UInt32 time)
{
	if (this->sourceAudio)
	{
		this->ResetStatus();
		return this->sourceAudio->SeekToTime(time);
	}
	return 0;
}

UOSInt Media::AudioFilter::DTMFDecoder::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	if (this->sourceAudio == 0)
		return 0;

	UOSInt readSize = this->sourceAudio->ReadBlock(buff, blkSize);
	UOSInt sizeLeft = readSize;
	UOSInt thisSize;
	UOSInt samples = readSize / this->align;
	while (samples >= this->calcLeft)
	{
		thisSize = this->calcLeft * this->align;
		if (this->sampleOfst + thisSize >= this->sampleBuffSize)
		{
			MemCopyNO(&this->sampleBuff[this->sampleOfst], buff, this->sampleBuffSize - this->sampleOfst);
			buff += this->sampleBuffSize - this->sampleOfst;
			thisSize -= this->sampleBuffSize - this->sampleOfst;
			sizeLeft -= this->sampleBuffSize - this->sampleOfst;
			this->sampleOfst = 0;
		}
		else if (thisSize > 0)
		{
			MemCopyNO(&this->sampleBuff[this->sampleOfst], buff, thisSize);
			buff += thisSize;
			sizeLeft -= thisSize;
			this->sampleOfst += thisSize;
		}
		Sync::MutexUsage mutUsage(&this->calcMut);
		MemCopyNO(this->calcBuff, &this->sampleBuff[this->sampleOfst], this->sampleBuffSize - this->sampleOfst);
		if (this->sampleBuffSize > 0)
		{
			MemCopyNO(&this->calcBuff[this->sampleBuffSize - this->sampleOfst], this->sampleBuff, this->sampleOfst);
		}
		this->calcReady = true;
		mutUsage.EndUse();
		this->threadEvt.Set();
		this->calcLeft = this->calcInt;
		samples = sizeLeft / this->align;
	}

	if (this->sampleOfst + sizeLeft >= this->sampleBuffSize)
	{
		MemCopyNO(&this->sampleBuff[this->sampleOfst], buff, this->sampleBuffSize - this->sampleOfst);
		buff += this->sampleBuffSize - this->sampleOfst;
		sizeLeft -= this->sampleBuffSize - this->sampleOfst;
		this->calcLeft -= (this->sampleBuffSize - this->sampleOfst) / this->align;
		this->sampleOfst = 0;
	}
	if (sizeLeft > 0)
	{
		MemCopyNO(&this->sampleBuff[this->sampleOfst], buff, sizeLeft);
		buff += sizeLeft;
		this->sampleOfst += sizeLeft;
		this->calcLeft -= sizeLeft / this->align;
	}
	return readSize;
}

void Media::AudioFilter::DTMFDecoder::HandleToneChange(ToneChangeEvent hdlr, void *userObj)
{
	this->toneChgHdlr = hdlr;
	this->toneChgObj = userObj;
}
