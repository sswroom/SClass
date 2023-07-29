#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Math/LanczosFilter.h"
#include "Math/Math.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include <float.h>

void LanczosResampler(Double *srcBuff, UOSInt srcSize, Double *destBuff, UOSInt destSize, UOSInt nTap, Double sampleOfst)
{
	if (destSize == srcSize)
		return;
	Math::LanczosFilter lanczos(nTap);
	if (destSize > srcSize)
	{
		UOSInt i;
		UOSInt j;
		OSInt n;
		Double sum;
		Double pos;
		Double tval;
		Double lv;
		Double v;

		i = 0;
		while (i < destSize)
		{
			pos = (UOSInt2Double(i) + 0.5) * UOSInt2Double(srcSize);
			pos = pos / UOSInt2Double(destSize) + sampleOfst;
			n = (OSInt)Math_Fix(pos - (UOSInt2Double(nTap) * 0.5 - 0.5));//2.5);
			pos = (OSInt2Double(n) + 0.5 - pos);

			sum = 0;
			tval = 0;

			j = 0;
			while (j < nTap)
			{
				if (n < 0)
				{
					v = srcBuff[0];
				}
				else if((UOSInt)n >= srcSize)
				{
					v = srcBuff[srcSize - 1];
				}
				else
				{
					v = srcBuff[n];
				}
				lv = lanczos.Weight(pos);
				sum += lv;
				tval += v * lv;

				pos += 1;
				n += 1;
				j++;
			}

			destBuff[i] = tval / sum;
			i++;
		}
	}
	else
	{
		UOSInt i;
		UOSInt j;
		OSInt n;
		UOSInt effTap = (UOSInt)((nTap * (srcSize) + (destSize - 1)) / destSize);
		Double sum;
		Double pos;
		Double tval;
		Double lv;
		Double v;
//		Double phase;

		i = 0;
		while (i < destSize)
		{
			pos = (UOSInt2Double(i) - UOSInt2Double(nTap / 2) + 0.5) * UOSInt2Double(srcSize) / UOSInt2Double(destSize) + 0.5;
			n = (OSInt)Math_Fix(pos + sampleOfst);

			sum = 0;
			tval = 0;

			j = 0;
			while (j < effTap)
			{
//				phase = (OSInt2Double(n) + 0.5) * UOSInt2Double(destSize);
//				phase /= UOSInt2Double(srcSize);
//				phase -= (UOSInt2Double(i) + 0.5);

				if (n < 0)
				{
					v = srcBuff[0];
				}
				else if((UOSInt)n >= srcSize)
				{
					v = srcBuff[srcSize - 1];
				}
				else
				{
					v = srcBuff[n];
				}
				lv = lanczos.Weight(pos);
				sum += lv;
				tval += v * lv;

				n += 1;
				j++;
			}

			destBuff[i] = tval / sum;
			i++;
		}
	}
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Double inpVal[5];
	Double outVal[20];
	Double t;
	OSInt i;
	Text::StringBuilderUTF8 sb;
	IO::ConsoleWriter console;
	Manage::HiResClock clk;
	inpVal[0] = 1;
	inpVal[1] = 2;
	inpVal[2] = 3;
	inpVal[3] = 4;
	inpVal[4] = 5;

	clk.Start();
	LanczosResampler(inpVal, 5, outVal, 20, 6, 0);
	t = clk.GetTimeDiff();
	sb.AppendC(UTF8STRC("{"));
	i = 0;
	while (i < 20)
	{
		if (i > 0)
		{
			sb.AppendC(UTF8STRC(", "));
		}
		sb.AppendDouble(outVal[i]);
		i++;
	}
	sb.AppendC(UTF8STRC("}"));
	console.WriteLineC(sb.ToString(), sb.GetLength());
	sb.ClearStr();
	sb.AppendC(UTF8STRC("t = "));
	sb.AppendDouble(t);
	console.WriteLineC(sb.ToString(), sb.GetLength());
	return 0;
}
