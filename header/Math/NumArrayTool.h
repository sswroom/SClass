#ifndef _SM_MATH_NUMARRAYTOOL
#define _SM_MATH_NUMARRAYTOOL
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListUInt32.h"
#include "Data/Random.h"

namespace Math
{
	class NumArrayTool
	{
	public:
		static void GenerateNormalRandom(NN<Data::ArrayListDbl> out, NN<Data::Random> random, Double average, Double stddev, UIntOS count);
		static void GenerateExponentialRandom(NN<Data::ArrayListDbl> out, NN<Data::Random> random, Double scale, UIntOS count);
		template<class K> static void RandomChoice(NN<Data::ArrayListNative<K>> out, NN<Data::Random> random, NN<Data::ReadingList<K>> srcList, UIntOS count)
		{
			UIntOS c = srcList->GetCount();
			while (count-- > 0)
			{
				out->Add(srcList->GetItem((UInt32)random->NextInt32() % c));
			}
		}

		template<class K> static void CreateHistogramCount(UnsafeArray<K> data, UIntOS dataCount, NN<Data::ArrayListDbl> outX, NN<Data::ArrayListUInt32> outCount, UIntOS barCount)
		{
			K min = data[0];
			K max = min;
			UIntOS i = 1;
			while (i < dataCount)
			{
				if (data[i] < min) min = data[i];
				if (data[i] > max) max = data[i];
				i++;
			}
			Double dmin = (Double)min;
			Double dmax = (Double)max;
			Double interval = (dmax - dmin) / (Double)barCount;
			UnsafeArray<Double> valArr = MemAllocArr(Double, barCount + 1);
			UnsafeArray<UInt32> cntArr = MemAllocArr(UInt32, barCount + 1);
			i = 0;
			while (i < barCount)
			{
				cntArr[i] = 0;
				valArr[i] = dmin + interval * (Double)(i + 1);
				i++;
			}
			cntArr[barCount] = 0;
			i = 0;
			while (i < dataCount)
			{
				Double v = (Double)data[i];
				cntArr[(Int32)((v - dmin) / interval)]++;
				i++;
			}
			cntArr[barCount - 1] += cntArr[barCount];
			outX->AddRange(valArr, barCount);
			outCount->AddRange(cntArr, barCount);
			MemFreeArr(valArr);
			MemFreeArr(cntArr);
		}

		static void GaussianElimination(UnsafeArray<Double> A, UnsafeArray<Double> B, UnsafeArray<Double> coefficients, UIntOS n);
		static Bool PolyFit(NN<Data::ArrayListDbl> x, NN<Data::ArrayListDbl> y, NN<Data::ArrayListDbl> coeffs, UIntOS degree, NN<Data::ArrayListDbl> weight);
	};
}
#endif
