#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/IChart.h"
#include "Math/FFT.h"
#include "Math/Math.h"
#include "Media/FrequencyGraph.h"
#include "Media/CS/TransferFunc.h"
#include "Text/MyString.h"

Media::DrawImage *Media::FrequencyGraph::CreateGraph(Media::DrawEngine *eng, Media::IAudioSource *audio, OSInt fftSize, OSInt timeRes, Math::FFTCalc::WindowType wtype, Double fHeight)
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
	Data::ArrayList<const UTF8Char *> chartLabels;
	Int32 ihFontH = Math::Double2Int32(fHeight * 0.5);
	Int32 yAxis;
	Int32 xAxis;
	Double sz[2];
	OSInt currSample;
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	Double *allFreqs;
	Double *freqs;
	Double *maxFreq;
	Double *minFreq;
	Media::AudioFormat fmt;
	Math::FFTCalc fft(fftSize, wtype);
	audio->GetFormat(&fmt);
	if (fmt.formatId == 1)
	{
		Int64 sampleCnt = audio->GetSampleCount();
		UInt8 *samples = MemAlloc(UInt8, fftSize * fmt.nChannels * fmt.bitpersample >> 3);
		allFreqs = MemAllocA(Double, fftSize * (timeRes + 1));
		maxFreq = MemAlloc(Double, fftSize);
		minFreq = MemAlloc(Double, fftSize);

		retImg = eng->CreateImage32(32, 32, Media::AT_NO_ALPHA);
		f = retImg->NewFontH(L"Arial", fHeight, Media::DrawEngine::DFS_NORMAL, 0);
		Data::IChart::CalScaleMarkDbl(&chartPos, &chartLabels, 0, fmt.frequency * 0.0005, fftSize * 0.5, fHeight, "0", 1, 0);
		yAxis = 0;
		j = chartLabels.GetCount();
		while (j-- > 0)
		{
			i = Text::StrConcat(Text::StrConcat(sbuff, chartLabels.GetItem(j)), (const UTF8Char*)"kHz") - sbuff;
			retImg->GetTextSizeUTF8(f, sbuff, i, sz);
			k = Math::Double2Int32(sz[0]);
			if (k > yAxis)
			{
				yAxis = (Int32)k;
			}
			Text::StrDelNew(chartLabels.GetItem(j));
		}

		chartPos.Clear();
		chartLabels.Clear();
		Data::IChart::CalScaleMarkDbl(&chartPos, &chartLabels, 0, sampleCnt / (Double)fmt.frequency, Math::OSInt2Double(timeRes), fHeight, "0.#", 1, 0);
		xAxis = 0;
		j = chartLabels.GetCount();
		while (j-- > 0)
		{
			i = Text::StrConcat(Text::StrConcat(sbuff, chartLabels.GetItem(j)), (const UTF8Char*)"s") - sbuff;
			retImg->GetTextSizeUTF8(f, sbuff, i, sz);
			k = Math::Double2Int32(sz[0]);
			if (k > xAxis)
			{
				xAxis = (Int32)k;
			}
			Text::StrDelNew(chartLabels.GetItem(j));
		}

		retImg->DelFont(f);
		eng->DeleteImage(retImg);
		retImg = 0;

		freqs = allFreqs;
		i = 0;
		j = timeRes;
		while (i <= j)
		{
			currSample = (OSInt)(sampleCnt * (Int64)i / j);
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
		OSInt sbpl;
		UInt8 *imgPtr;
		UInt8 v;
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		retImg = eng->CreateImage32((Int32)(timeRes + 4) + yAxis + ihFontH, (Int32)(fftSize >> 1) + 4 + ihFontH + xAxis, Media::AT_NO_ALPHA);
		retImg->SetColorProfile(&color);
		sbpl = retImg->GetImgBpl();
		b = retImg->NewBrushARGB(0xffffffff);
		retImg->DrawRect(0, 0, Math::OSInt2Double(retImg->GetWidth()), Math::OSInt2Double(retImg->GetHeight()), 0, b);
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

		Int32 *lut = MemAlloc(Int32, 16385);
		Double imaxVal = 16384.0 / maxVal;
		i = 0;
		j = 16385;
		while (i < j)
		{
			v = 255 - (UInt8)Math::Double2Int32(255.0 * trans->ForwardTransfer(i / 16384.0));
			lut[i] = 0xff000000 | (v << 16) | (v << 8) | v;
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
					*(Int32*)imgPtr = lut[Math::Double2Int32(freqs[l] * imaxVal)];

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
					*(Int32*)imgPtr = lut[Math::Double2Int32(freqs[l] * imaxVal)];

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
		f = retImg->NewFontH(L"Arial", fHeight, Media::DrawEngine::DFS_ANTIALIAS, 0);
		retImg->DrawLine(yAxis + 3, ihFontH, yAxis + 3, ihFontH + fftSize * 0.5, p);
		retImg->DrawLine(yAxis + 3, ihFontH + fftSize * 0.5, yAxis + 4 + Math::OSInt2Double(timeRes), ihFontH + fftSize * 0.5, p);

		chartPos.Clear();
		chartLabels.Clear();
		Data::IChart::CalScaleMarkDbl(&chartPos, &chartLabels, 0, fmt.frequency * 0.0005, fftSize * 0.5, fHeight, "0", 1, 0);
		j = chartLabels.GetCount();
		while (j-- > 0)
		{
			i = Text::StrConcat(Text::StrConcat(sbuff, chartLabels.GetItem(j)), (const UTF8Char*)"kHz") - sbuff;
			retImg->GetTextSizeUTF8(f, sbuff, i, sz);
			k = Math::Double2Int32(sz[0]);
			retImg->DrawStringUTF8(yAxis - Math::OSInt2Double(k), (fftSize >> 1) - chartPos.GetItem(j), sbuff, f, b);
			retImg->DrawLine(yAxis, (fftSize >> 1) - chartPos.GetItem(j) + ihFontH, yAxis + 4, (fftSize >> 1) - chartPos.GetItem(j) + ihFontH, p);

			Text::StrDelNew(chartLabels.GetItem(j));
		}

		chartPos.Clear();
		chartLabels.Clear();
		Data::IChart::CalScaleMarkDbl(&chartPos, &chartLabels, 0, sampleCnt / (Double)fmt.frequency, Math::OSInt2Double(timeRes), fHeight, "0.#", 1, 0);
		j = chartLabels.GetCount();
		while (j-- > 0)
		{
			i = Text::StrConcat(Text::StrConcat(sbuff, chartLabels.GetItem(j)), (const UTF8Char*)"s") - sbuff;
			retImg->GetTextSizeUTF8(f, sbuff, i, sz);
			k = Math::Double2Int32(sz[0]);
			retImg->DrawStringRotUTF8(yAxis + 4 + chartPos.GetItem(j) + ihFontH, ihFontH + fftSize * 0.5 + 4, sbuff, f, b, 270.0);
			retImg->DrawLine(yAxis + 4 + chartPos.GetItem(j), ihFontH + fftSize * 0.5, yAxis + 4 + chartPos.GetItem(j), ihFontH + fftSize * 0.5 + 4, p);

			Text::StrDelNew(chartLabels.GetItem(j));
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
