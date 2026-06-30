#include "Stdafx.h"
#include "Core/Core.h"
#include "Core/ByteTool_C.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Math/FFTCalc.h"
#include "Math/Math_C.h"
#include "Text/StringBuilderUTF8.h"

#define SAMPLETYPE Int16
#define SAMPLETYPEI Math::FFTCalc::ST_I16

//#define SAMPLETYPE Int24
//#define SAMPLETYPEI Math::FFTCalc::ST_I24

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Double t0;
	Double t1;
	Double t2;
	Text::StringBuilderUTF8 sb;
	IO::ConsoleWriter console;
	Manage::HiResClock clk;
	UnsafeArray<UInt8> sampleBuff;
	UnsafeArray<Double> freq;
	NN<Math::FFTCalc> fft;
	UIntOS n = 1;
	UIntOS sampleCount;
	while (n < 26) //26
	{
		sampleCount = 1 << n;
		clk.Start();
		NEW_CLASSNN(fft, Math::FFTCalc(sampleCount, Math::FFTCalc::WT_BLACKMANN_HARRIS));
		t0 = clk.GetTimeDiff();

		sampleBuff = MemAllocArr(UInt8, sizeof(SAMPLETYPE) * sampleCount);
		freq = MemAllocArr(Double, sampleCount);
		clk.Start();
		MemClear(&sampleBuff[0], sizeof(SAMPLETYPE) * sampleCount);
		t1 = clk.GetTimeDiff();
		clk.Start();
		fft->ForwardBits(sampleBuff, freq, SAMPLETYPEI, 1, 1.0);
		t2 = clk.GetTimeDiff();
		MemFreeArr(sampleBuff);
		MemFreeArr(freq);

		sb.ClearStr();
		sb.Append(CSTR("N = "));
		sb.AppendUIntOS(n);
		sb.Append(CSTR(", t0 = "));
		sb.AppendDouble(t0);
		sb.Append(CSTR(", t1 = "));
		sb.AppendDouble(t1);
		sb.Append(CSTR(", t2 = "));
		sb.AppendDouble(t2);
		console.WriteLine(sb.ToCString());

		fft.Delete();
		n++;
	}
	return 0;
}
