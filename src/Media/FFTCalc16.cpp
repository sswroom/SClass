#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/FFTCalc16.h"
#include "Math/Math_C.h"

Media::FFTCalc16::FFTCalc16(UInt32 nPoints, UInt32 sampleRate)
{
	Int32 logPoints;
	UInt32 i;
	this->nPoints = nPoints;
	this->sampleRate = sampleRate;
	this->sqrtNPoints = Math_Sqrt(nPoints);

	logPoints = 0;
	i = nPoints - 1;
	while (i != 0)
	{
		logPoints++;
		i >>= 1;
	}
	//////////////////////////////////
}

Media::FFTCalc16::~FFTCalc16()
{
}

void Media::FFTCalc16::Transform(Int16 *data, UInt32 nPoints)
{
}

/*
#include "Fft.h"
#include "WinEx.h"
//------------------------------------
//  Reliable Software, (c) 1996-2002
//------------------------------------

// log (1) = 0, log(2) = 1, log(3) = 2, log(4) = 2 ...

const double PI = 2.0 * asin (1.0);

// points must be a power of 2

Fft::Fft (unsigned points, unsigned sampleRate)
	: _points (points), _sampleRate (sampleRate)
{
    _tape.resize (_points);
    _sqrtPoints = sqrt (static_cast<double> (_points));
    // calculate binary log
    _logPoints = 0;
    points--;
    while (points != 0)
    {
        points >>= 1;
        _logPoints++;
    }

    _aBitRev.resize (_points);
    _X.resize (_points);
    _W.resize (_logPoints + 1);
    // Precompute complex exponentials
    int _2_l = 2;
    for (unsigned l = 1; l <= _logPoints; l++)
    {
        _W[l].resize (_points);

        for (unsigned i = 0; i != _points; i++ )
        {
            double re =  cos (2. * PI * i / _2_l);
            double im = -sin (2. * PI * i / _2_l);
            _W[l][i] = Complex (re, im);
        }
        _2_l *= 2;
    }

    // set up bit reverse mapping
    int rev = 0;
    unsigned halfPoints = _points/2;
    for (unsigned i = 0; i < _points - 1; i++)
    {
        _aBitRev [i] = static_cast<unsigned> (rev);
        unsigned mask = halfPoints;
        // add 1 backwards
        while (rev >= static_cast<int> (mask))
        {
            rev -= mask; // turn off this bit
            mask >>= 1;
        }
        rev += mask;
    }
    _aBitRev [_points-1] = _points-1;
}

void Fft::DataIn (std::vector<int> const & data)
{
	if (data.size () > _points)
		throw Win::Exception ("Sample buffer larger than FFT capacity");
    // make space for samples at the end of tape
    // shifting previous samples towards the beginning
	// to           from
	// v-------------|
	// xxxxxxxxxxxxxxyyyyyyyyy
	// yyyyyyyyyoooooooooooooo
	// <- old -><- free tail->
	std::copy (_tape.begin () + data.size (), _tape.end (), _tape.begin ());
	std::copy (data.begin (), data.end (), _tape.begin () + (_points - data.size ()));

    // Initialize the FFT buffer
    for (unsigned i = 0; i != _points; ++i)
        PutAt (i, _tape [i]);
}

//----------------------------------------------------------------
//               0   1   2   3   4   5   6   7
//  level   1
//  step    1                                     0
//  increm  2                                   W 
//  j = 0        <--->   <--->   <--->   <--->   1
//  level   2
//  step    2
//  increm  4                                     0
//  j = 0        <------->       <------->      W      1
//  j = 1            <------->       <------->   2   W
//  level   3                                         2
//  step    4
//  increm  8                                     0
//  j = 0        <--------------->              W      1
//  j = 1            <--------------->           3   W      2
//  j = 2                <--------------->            3   W      3
//  j = 3                    <--------------->             3   W
//                                                              3
//----------------------------------------------------------------

void Fft::Transform (std::vector<int> const & data)
{
	DataIn (data);

    // step = 2 ^ (level-1)
    // increm = 2 ^ level;
    unsigned step = 1;
    for (unsigned level = 1; level <= _logPoints; level++)
    {
        unsigned increm = step * 2;
        for (unsigned j = 0; j < step; j++)
        {
            // U = exp ( - 2 PI j / 2 ^ level )
            Complex U = _W [level][j];
            for (unsigned i = j; i < _points; i += increm)
            {
                // butterfly
                Complex T = U;
                T *= _X [i+step];
                _X [i+step] = _X[i];
                _X [i+step] -= T;
                _X [i] += T;
            }
        }
        step *= 2;
    }
}
*/