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