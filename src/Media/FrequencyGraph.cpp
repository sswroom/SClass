#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListStringNN.h"
#include "Data/ByteBuffer.h"
#include "Data/Chart.h"
#include "Math/FFT.h"
#include "Math/Math.h"
#include "Media/FrequencyGraph.h"
#include "Media/CS/TransferFunc.h"
#include "Text/MyString.h"

Optional<Media::DrawImage> Media::FrequencyGraph::CreateGraph(NN<Media::DrawEngine> eng, NN<Media::AudioSource> audio, UOSInt fftSize, UOSInt timeRes, Math::FFTCalc::WindowType wtype, Double fontSizePx)
{
	if (!audio->SupportSampleRead())
	{
		return 0;
	}

	UTF8Char sbuff[32];
	Media::DrawImage *retImg = 0;
	NN<Media::DrawImage> tmpImg;
	NN<Media::DrawPen> p;
	NN<Media::DrawBrush> b;
	NN<Media::DrawFont> f;
	Data::ArrayListDbl chartPos;
	Data::ArrayListStringNN chartLabels;
	UInt32 ihFontSize = (UInt32)Double2Int32(fontSizePx * 0.5);
	UInt32 yAxis;
	UInt32 xAxis;
	Math::Size2DDbl sz;
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
	Data::ArrayIterator<NN<Text::String>> it;
	NN<Text::String> s;
	audio->GetFormat(fmt);
	if (fmt.formatId == 1)
	{
		UInt64 sampleCnt = (UInt64)audio->GetSampleCount();
		Data::ByteBuffer samples(fftSize * fmt.nChannels * fmt.bitpersample >> 3);

		if (!eng->CreateImage32(Math::Size2D<UOSInt>(32, 32), Media::AT_ALPHA_ALL_FF).SetTo(tmpImg))
		{
			return 0;
		}
		allFreqs = MemAllocA(Double, fftSize * (timeRes + 1));
		maxFreq = MemAlloc(Double, fftSize);
		minFreq = MemAlloc(Double, fftSize);

		f = tmpImg->NewFontPx(CSTR("Arial"), fontSizePx, Media::DrawEngine::DFS_NORMAL, 0);
		Data::Chart::CalScaleMarkDbl(chartPos, chartLabels, 0, fmt.frequency * 0.0005, dfftSize * 0.5, fontSizePx, "0", 1, 0);
		yAxis = 0;
		it = chartLabels.Iterator();
		while (it.HasNext())
		{
			s = it.Next();
			i = (UOSInt)(Text::StrConcatC(s->ConcatTo(sbuff), UTF8STRC("kHz")) - sbuff);
			sz = tmpImg->GetTextSize(f, {sbuff, i});
			iVal = (UInt32)Double2Int32(sz.x);
			if (iVal > yAxis)
			{
				yAxis = iVal;
			}
			s->Release();
		}

		chartPos.Clear();
		chartLabels.Clear();
		Data::Chart::CalScaleMarkDbl(chartPos, chartLabels, 0, (Double)sampleCnt / (Double)fmt.frequency, UOSInt2Double(timeRes), fontSizePx, "0.#", 1, 0);
		xAxis = 0;
		it = chartLabels.Iterator();
		while (it.HasNext())
		{
			s = it.Next();
			i = (UOSInt)(Text::StrConcatC(s->ConcatTo(sbuff), UTF8STRC("s")) - sbuff);
			sz = tmpImg->GetTextSize(f, {sbuff, i});
			iVal = (UInt32)Double2Int32(sz.x);
			if (iVal > xAxis)
			{
				xAxis = iVal;
			}
			s->Release();
		}

		tmpImg->DelFont(f);
		eng->DeleteImage(tmpImg);

		freqs = allFreqs;
		i = 0;
		j = timeRes;
		while (i <= j)
		{
			currSample = (UOSInt)(sampleCnt * i / j);
			audio->ReadSample(currSample - (fftSize >> 1), fftSize, samples);

			if (fmt.bitpersample == 16)
			{
				fft.ForwardBits(samples.Arr().Ptr(), freqs, Math::FFTCalc::ST_I16, fmt.nChannels, 1.0);
			}
			else if (fmt.bitpersample == 24)
			{
				fft.ForwardBits(samples.Arr().Ptr(), freqs, Math::FFTCalc::ST_I24, fmt.nChannels, 1.0);
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
		NN<Media::CS::TransferFunc> trans = Media::CS::TransferFunc::CreateFunc(tranParam);
		UOSInt sbpl;
		UnsafeArray<UInt8> imgPtr;
		UInt8 v;
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		if (eng->CreateImage32(Math::Size2D<UOSInt>((timeRes + 4) + yAxis + ihFontSize, (fftSize >> 1) + 4 + ihFontSize + xAxis), Media::AT_ALPHA_ALL_FF).SetTo(tmpImg))
		{
			retImg = tmpImg.Ptr();
			tmpImg->SetColorProfile(color);
			sbpl = tmpImg->GetImgBpl();
			b = tmpImg->NewBrushARGB(0xffffffff);
			tmpImg->DrawRect(Math::Coord2DDbl(0, 0), tmpImg->GetSize().ToDouble(), 0, b);
			tmpImg->DelBrush(b);

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
			UnsafeArray<UInt8> bmpBits;
			if (tmpImg->GetImgBits(revOrder).SetTo(bmpBits))
			{
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
							*(UInt32*)imgPtr.Ptr() = lut[Double2Int32(freqs[l] * imaxVal)];

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
							*(UInt32*)imgPtr.Ptr() = lut[Double2Int32(freqs[l] * imaxVal)];

							imgPtr += sbpl;
							l++;
						}

						freqs += fftSize;
						i++;
					}
				}
			}
			trans.Delete();
			MemFree(lut);

			p = tmpImg->NewPenARGB(0xff000000, 1, 0, 0);
			b = tmpImg->NewBrushARGB(0xff000000);
			f = tmpImg->NewFontPx(CSTR("Arial"), fontSizePx, Media::DrawEngine::DFS_ANTIALIAS, 0);
			tmpImg->DrawLine(yAxis + 3, ihFontSize, yAxis + 3, ihFontSize + dfftSize * 0.5, p);
			tmpImg->DrawLine(yAxis + 3, ihFontSize + dfftSize * 0.5, yAxis + 4 + UOSInt2Double(timeRes), ihFontSize + dfftSize * 0.5, p);

			chartPos.Clear();
			chartLabels.Clear();
			Data::Chart::CalScaleMarkDbl(chartPos, chartLabels, 0, fmt.frequency * 0.0005, dfftSize * 0.5, fontSizePx, "0", 1, 0);
			it = chartLabels.Iterator();
			while (it.HasNext())
			{
				s = it.Next();
				i = (UOSInt)(Text::StrConcatC(s->ConcatTo(sbuff), UTF8STRC("kHz")) - sbuff);
				sz = tmpImg->GetTextSize(f, {sbuff, i});
				k = (UInt32)Double2Int32(sz.x);
				tmpImg->DrawString(Math::Coord2DDbl(yAxis - UOSInt2Double(k), dfftSize * 0.5 - chartPos.GetItem(j)), {sbuff, i}, f, b);
				tmpImg->DrawLine(yAxis, dfftSize * 0.5 - chartPos.GetItem(j) + ihFontSize, yAxis + 4, UOSInt2Double(fftSize >> 1) - chartPos.GetItem(j) + ihFontSize, p);

				s->Release();
			}

			chartPos.Clear();
			chartLabels.Clear();
			Data::Chart::CalScaleMarkDbl(chartPos, chartLabels, 0, UInt64_Double(sampleCnt) / (Double)fmt.frequency, UOSInt2Double(timeRes), fontSizePx, "0.#", 1, 0);
			it = chartLabels.Iterator();
			while (it.HasNext())
			{
				s = it.Next();
				i = (UOSInt)(Text::StrConcatC(s->ConcatTo(sbuff), UTF8STRC("s")) - sbuff);
				sz = tmpImg->GetTextSize(f, {sbuff, i});
				k = (UInt32)Double2Int32(sz.x);
				tmpImg->DrawStringRot(Math::Coord2DDbl(yAxis + 4 + chartPos.GetItem(j) + ihFontSize, ihFontSize + dfftSize * 0.5 + 4), {sbuff, i}, f, b, 270.0);
				tmpImg->DrawLine(yAxis + 4 + chartPos.GetItem(j), ihFontSize + dfftSize * 0.5, yAxis + 4 + chartPos.GetItem(j), ihFontSize + dfftSize * 0.5 + 4, p);

				s->Release();
			}
			tmpImg->DelBrush(b);
			tmpImg->DelPen(p);
			tmpImg->DelFont(f);
		}

		MemFree(maxFreq);
		MemFree(minFreq);
		MemFreeA(allFreqs);
	}
	return retImg;
}
