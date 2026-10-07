#include "Stdafx.h"
#include "Math/NumArrayTool.h"

void Math::NumArrayTool::GenerateNormalRandom(NN<Data::ArrayListDbl> out, NN<Data::Random> random, Double average, Double stddev, UIntOS count)
{
	while (count-- > 0)
	{
		Double u = 1 - random->NextDouble();
		Double v = random->NextDouble();
		Double z = Math_Sqrt( -2.0 * Math_Ln( u ) ) * Math_Cos( 2.0 * Math::PI * v );
		out->Add(average + stddev * z);
	}
}

// https://numpy.org/doc/2.1/reference/random/generated/numpy.random.exponential.html
void Math::NumArrayTool::GenerateExponentialRandom(NN<Data::ArrayListDbl> out, NN<Data::Random> random, Double scale, UIntOS count)
{
	Double inverseOfRate = -scale;
	while (count-- > 0)
	{
		out->Add(inverseOfRate * Math_Ln(1 - random->NextDouble()));
	}
}

void Math::NumArrayTool::GaussianElimination(UnsafeArray<Double> A, UnsafeArray<Double> B, UnsafeArray<Double> coefficients, UIntOS n)
{
	UnsafeArray<Double> tempA = MemAllocArr(Double, n);
	UIntOS k;
	UIntOS j;
	UIntOS i = 0;
    while (i < n)
	{
        UIntOS max_row = i;
		k = i + 1;
        while (k < n)
		{
            if (fabs(A[k * n + i]) > fabs(A[max_row * n + i])) max_row = k;
			k++;
        }
        if (max_row != i) {
            MemCopyNO(tempA.Ptr(), &A[i * n], sizeof(Double) * n);
            MemCopyNO(&A[i * n], &A[max_row * n], sizeof(Double) * n);
            MemCopyNO(&A[max_row * n], tempA.Ptr(), sizeof(Double) * n);
            Double temp_B = B[i]; B[i] = B[max_row]; B[max_row] = temp_B;
        }
        k = i + 1;
        while (k < n)
		{
            Double factor = A[k * n + i] / A[i * n + i];
			j = i;
            while (j < n)
			{
				A[k * n + j] -= factor * A[i * n + j];
				j++;
			}
            B[k] -= factor * B[i];
			k++;
        }
		i++;
    }
	i = n;
    while (i-- > 0)
	{
        coefficients[i] = B[i];
		j = i + 1;
        while (j < n)
		{
			coefficients[i] -= A[i * n + j] * coefficients[j];
			j++;
		}
        coefficients[i] /= A[i * n + i];
    }
	MemFreeArr(tempA);
}

Bool Math::NumArrayTool::PolyFit(NN<Data::ArrayListDbl> x, NN<Data::ArrayListDbl> y, NN<Data::ArrayListDbl> coeffs, UIntOS degree, NN<Data::ArrayListDbl> weight)
{
	UIntOS numPoints = x->GetCount();
	if (y->GetCount() != numPoints)
		return false;
	if (weight->GetCount() != numPoints)
		return false;
	UIntOS n = degree + 1;
	UIntOS i;
	UIntOS j;
	UIntOS p;
	UIntOS k;
	UnsafeArray<Double> A = MemAllocArr(Double, n * n);
	UnsafeArray<Double> B = MemAllocArr(Double, n);
	UnsafeArray<Double> coefficients = MemAllocArr(Double, n);

	i = 0;
	while (i < n)
	{
		UIntOS pow_i = degree - i;
		j = 0;
		while (j < n)
		{
			UIntOS pow_j = degree - j;
			k = i * n + j;
			A[k] = 0;
			p = 0;
			while (p < numPoints)
			{
				// If w is provided, use w[p]^2 to match NumPy's least-squares implementation
				//Double weight_sq = (w != NULL) ? (w[p] * w[p]) : 1.0;
				Double weight_sq = weight->GetItem(p) * weight->GetItem(p);
				A[k] += weight_sq * Math_Pow(x->GetItem(p), pow_i + pow_j);
				p++;
			}
			j++;
		}
		B[i] = 0;
		p = 0;
		while (p < numPoints)
		{
			//Double weight_sq = (w != NULL) ? (w[p] * w[p]) : 1.0;
			Double weight_sq = weight->GetItem(p) * weight->GetItem(p);
			B[i] += weight_sq * y->GetItem(p) * Math_Pow(x->GetItem(p), pow_i);
			p++;
		}
		i++;
	}

	GaussianElimination(A, B, coefficients, n);
	coeffs->AddRange(coefficients, n);

	MemFreeArr(A);
	MemFreeArr(B);
	MemFreeArr(coefficients);

	return true;
}
