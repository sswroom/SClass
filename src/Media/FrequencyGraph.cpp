#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/IChart.h"
#include "Math/FFT.h"
#include "Math/Math.h"
#include "Media/FrequencyGraph.h"
#include "Media/CS/TransferFunc.h"
#include "Text/MyString.h"

Media::DrawImage *Media::FrequencyGraph::CreateGraph(Media::DrawEngine *eng, Media::IAudioSource *audio, UOSInt fftSize, UOSInt timeRes, Math::FFTCalc::WindowType wtype, Double fontSizePx)
{
	if (!audio->SupportSampleRead())
	{
		return 0;
	}

	UTF8Char sbuff[32];
	Media::DrawImage *retImg = 0;
	Media::DrawPen *p;
	Media::DrawBrush *b;
	Media::DrawFont *f;
	Data::ArrayListDbl chartPos;
	Data::ArrayList<Text::String *> chartLabels;
	UInt32 ihFontSize = (UInt32)Double2Int32(fontSizePx * 0.5);
	UInt32 yAxis;
	UInt32 xAxis;
	Double sz[2];
	UOSInt currSample;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Double *allFreqs;
	Double *freqs;
	Double *maxFreq;
	Double *minFreq;
	UInt32 iVal;
	Double dfftSize = UOSInt2Double(fftSize);
	Media::AudioFormat fmt;
	Math::FFTCalc fft(fftSize, wtype);
	audio->GetFormat(&fmt);
	if (fmt.formatId == 1)
	{
		UInt64 sampleCnt = (UInt64)audio->GetSampleCount();
		UInt8 *samples = MemAlloc(UInt8, fftSize * fmt.nChannels * fmt.bitpersample >> 3);
		allFreqs = MemAllocA(Double, fftSize * (timeRes + 1));
		maxFreq = MemAlloc(Double, fftSize);
		minFreq = MemAlloc(Double, fftSize);

		retImg = eng->CreateImage32(32, 32, Media::AT_NO_ALPHA);
		f = retImg->NewFontPx(CSTR("Arial"), fontSizePx, Media::DrawEngine::DFS_NORMAL, 0);
		Data::IChart::CalScaleMarkDbl(&chartPos, &chartLabels, 0, fmt.frequency * 0.0005, dfftSize * 0.5, fontSizePx, "0", 1, 0);
		yAxis = 0;
		j = chartLabels.GetCount();
		while (j-- > 0)
		{
			i = (UOSInt)(Text::StrConcatC(chartLabels.GetItem(j)->ConcatTo(sbuff), UTF8STRC("kHz")) - sbuff);
			retImg->GetTextSize(f, {sbuff, i}, sz);
			iVal = (UInt32)Double2Int32(sz[0]);
			if (iVal > yAxis)
			{
				yAxis = iVal;
			}
			chartLabels.GetItem(j)->Release();
		}

		chartPos.Clear();
		chartLabels.Clear();
		Data::IChart::CalScaleMarkDbl(&chartPos, &chartLabels, 0, (Double)sampleCnt / (Double)fmt.frequency, UOSInt2Double(timeRes), fontSizePx, "0.#", 1, 0);
		xAxis = 0;
		j = chartLabels.GetCount();
		while (j-- > 0)
		{
			i = (UOSInt)(Text::StrConcatC(chartLabels.GetItem(j)->ConcatTo(sbuff), UTF8STRC("s")) - sbuff);
			retImg->GetTextSize(f, {sbuff, i}, sz);
			iVal = (UInt32)Double2Int32(sz[0]);
			if (iVal > xAxis)
			{
				xAxis = iVal;
			}
			chartLabels.GetItem(j)->Release();
		}

		retImg->DelFont(f);
		eng->DeleteImage(retImg);
		retImg = 0;

		freqs = allFreqs;
		i = 0;
		j = timeRes;
		while (i <= j)
		{
			currSample = (UOSInt)(sampleCnt * i / j);
			audio->ReadSample(currSample - (fftSize >> 1), fftSize, (UInt8*)samples);

			if (fmt.bitpersample == 16)
			{
				fft.ForwardBits(samples, freqs, Math::FFTCalc::ST_I16, fmt.nChannels, 1.0);
			}
			else if (fmt.bitpersample == 24)
			{
				fft.ForwardBits(samples, freqs, Math::FFTCalc::ST_I24, fmt.nChannels, 1.0);
			}
			if (i == 0)
			{
				l = 0;
				while (l < fftSize)
				{
					maxFreq[l] = freqs[l];
					minFreq[l] = freqs[l];
					l++;
				}
			}
			else
			{
				l = 0;
				while (l < fftSize)
				{
					if (freqs[l] > maxFreq[l])
					{
						maxFreq[l] = freqs[l];
					}
					if (freqs[l] < minFreq[l])
					{
						minFreq[l] = freqs[l];
					}
					l++;
				}
			}
			freqs += fftSize;
			i++;
		}

		Double minVal;
		Double maxVal;
		Double thisMin;
		Media::CS::TransferParam tranParam(Media::CS::TRANT_sRGB, 2.2);
		Media::CS::TransferFunc *trans = Media::CS::TransferFunc::CreateFunc(&tranParam);
		UOSInt sbpl;
		UInt8 *imgPtr;
		UInt8 v;
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		retImg = eng->CreateImage32((timeRes + 4) + yAxis + ihFontSize, (fftSize >> 1) + 4 + ihFontSize + xAxis, Media::AT_NO_ALPHA);
		retImg->SetColorProfile(&color);
		sbpl = retImg->GetImgBpl();
		b = retImg->NewBrushARGB(0xffffffff);
		retImg->DrawRect(0, 0, UOSInt2Double(retImg->GetWidth()), UOSInt2Double(retImg->GetHeight()), 0, b);
		retImg->DelBrush(b);

		minVal = minFreq[0];
		maxVal = maxFreq[0];
		i = 0;
		j = fftSize;
		while (i < j)
		{
			if (minFreq[i] < minVal)
			{
				minVal = minFreq[i];
			}
			if (maxFreq[i] > maxVal)
			{
				maxVal = maxFreq[i];
			}
			i++;
		}

		UInt32 *lut = MemAlloc(UInt32, 16385);
		Double imaxVal = 16384.0 / maxVal;
		i = 0;
		j = 16385;
		while (i < j)
		{
			v = (UInt8)(255 - (UInt8)Double2Int32(255.0 * trans->ForwardTransfer(UOSInt2Double(i) / 16384.0)));
			lut[i] = 0xff000000 | (UInt32)(v << 16) | (UInt32)(v << 8) | v;
			i++;
		}

		Bool revOrder;
		UInt8 *bmpBits = retImg->GetImgBits(&revOrder);
		freqs = allFreqs;
		if (revOrder)
		{
			i = 0;
			j = timeRes;
			while (i <= j)
			{
				imgPtr = bmpBits + sbpl * (xAxis + 4) + (i + yAxis + 3) * 4;
				k = fftSize >> 1;

				thisMin = freqs[0];
				l = k;
				while (l-- > 0)
				{
					if (freqs[l] < thisMin)
					{
						thisMin = freqs[l];
					}
				}


				l = 0;
				while (l < k)
				{
					*(UInt32*)imgPtr = lut[Double2Int32(freqs[l] * imaxVal)];

					imgPtr += sbpl;
					l++;
				}

				freqs += fftSize;
				i++;
			}
		}
		else
		{
			i = 0;
			j = timeRes;
			while (i <= j)
			{
				imgPtr = bmpBits + sbpl * (xAxis + 4) + (i + yAxis + 3) * 4;
				k = fftSize >> 1;

				thisMin = freqs[0];
				l = k;
				while (l-- > 0)
				{
					if (freqs[l] < thisMin)
					{
						thisMin = freqs[l];
					}
				}


				l = 0;
				while (l < k)
				{
					*(UInt32*)imgPtr = lut[Double2Int32(freqs[l] * imaxVal)];

					imgPtr += sbpl;
					l++;
				}

				freqs += fftSize;
				i++;
			}
		}
		DEL_CLASS(trans);
		MemFree(lut);

		p = retImg->NewPenARGB(0xff000000, 1, 0, 0);
		b = retImg->NewBrushARGB(0xff000000);
		f = retImg->NewFontPx(CSTR("Arial"), fontSizePx, Media::DrawEngine::DFS_ANTIALIAS, 0);
		retImg->DrawLine(yAxis + 3, ihFontSize, yAxis + 3, ihFontSize + dfftSize * 0.5, p);
		retImg->DrawLine(yAxis + 3, ihFontSize + dfftSize * 0.5, yAxis + 4 + UOSInt2Double(timeRes), ihFontSize + dfftSize * 0.5, p);

		chartPos.Clear();
		chartLabels.Clear();
		Data::IChart::CalScaleMarkDbl(&chartPos, &chartLabels, 0, fmt.frequency * 0.0005, dfftSize * 0.5, fontSizePx, "0", 1, 0);
		j = chartLabels.GetCount();
		while (j-- > 0)
		{
			i = (UOSInt)(Text::StrConcatC(chartLabels.GetItem(j)->ConcatTo(sbuff), UTF8STRC("kHz")) - sbuff);
			retImg->GetTextSize(f, {sbuff, i}, sz);
			k = (UInt32)Double2Int32(sz[0]);
			retImg->DrawString(yAxis - UOSInt2Double(k), dfftSize * 0.5 - chartPos.GetItem(j), {sbuff, i}, f, b);
			retImg->DrawLine(yAxis, dfftSize * 0.5 - chartPos.GetItem(j) + ihFontSize, yAxis + 4, UOSInt2Double(fftSize >> 1) - chartPos.GetItem(j) + ihFontSize, p);

			chartLabels.GetItem(j)->Release();
		}

		chartPos.Clear();
		chartLabels.Clear();
		Data::IChart::CalScaleMarkDbl(&chartPos, &chartLabels, 0, UInt64_Double(sampleCnt) / (Double)fmt.frequency, UOSInt2Double(timeRes), fontSizePx, "0.#", 1, 0);
		j = chartLabels.GetCount();
		while (j-- > 0)
		{
			i = (UOSInt)(Text::StrConcatC(chartLabels.GetItem(j)->ConcatTo(sbuff), UTF8STRC("s")) - sbuff);
			retImg->GetTextSize(f, {sbuff, i}, sz);
			k = (UInt32)Double2Int32(sz[0]);
			retImg->DrawStringRot(yAxis + 4 + chartPos.GetItem(j) + ihFontSize, ihFontSize + dfftSize * 0.5 + 4, {sbuff, i}, f, b, 270.0);
			retImg->DrawLine(yAxis + 4 + chartPos.GetItem(j), ihFontSize + dfftSize * 0.5, yAxis + 4 + chartPos.GetItem(j), ihFontSize + dfftSize * 0.5 + 4, p);

			chartLabels.GetItem(j)->Release();
		}
		retImg->DelBrush(b);
		retImg->DelPen(p);
		retImg->DelFont(f);

		MemFree(maxFreq);
		MemFree(minFreq);
		MemFree(samples);
		MemFreeA(allFreqs);
	}
	return retImg;
}
