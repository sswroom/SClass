#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include <float.h>

Double LanczosFunc(Double phase, OSInt nTap)
{
	Double ret;
	Double aphase = Math::Abs(phase);
	
	if(aphase < DBL_EPSILON)
	{
		return 1.0;
	}

	if ((aphase * 2) >= nTap){
		return 0.0;
	}

	ret = Math::Sin(Math::PI * phase) * Math::Sin(Math::PI * phase / nTap * 2) / (Math::PI * Math::PI * phase * phase / nTap * 2);

	return ret;
}

void LanczosResampler(Double *srcBuff, OSInt srcSize, Double *destBuff, OSInt destSize, OSInt nTap, Double sampleOfst)
{
	if (destSize == srcSize)
		return;
	if (destSize > srcSize)
	{
		OSInt i, j, n;
		Double sum;
		Double pos;
		Double tval;
		Double lv;
		Double v;

		i = 0;
		while (i < destSize)
		{
			pos = (i + 0.5) * srcSize;
			pos = pos / destSize + sampleOfst;
			n = (OSInt)Math::Fix(pos - (nTap * 0.5 - 0.5));//2.5);
			pos = (n + 0.5 - pos);

			sum = 0;
			tval = 0;

			j = 0;
			while (j < nTap)
			{
				if (n < 0)
				{
					v = srcBuff[0];
				}
				else if(n >= srcSize)
				{
					v = srcBuff[srcSize - 1];
				}
				else
				{
					v = srcBuff[n];
				}
				lv = LanczosFunc(pos, nTap);
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
		OSInt i, j, n;
		OSInt effTap = (OSInt)Math::Fix((nTap * (srcSize) + (destSize - 1)) / destSize);
		Double sum;
		Double pos;
		Double tval;
		Double lv;
		Double v;
		Double phase;

		i = 0;
		while (i < destSize)
		{
			pos = (i - (nTap / 2) + 0.5) * srcSize / destSize + 0.5;
			n = (OSInt)Math::Fix(pos + sampleOfst);

			sum = 0;
			tval = 0;

			j = 0;
			while (j < effTap)
			{
				phase = (n + 0.5) * destSize;
				phase /= srcSize;
				phase -= (i + 0.5);

				if (n < 0)
				{
					v = srcBuff[0];
				}
				else if(n >= srcSize)
				{
					v = srcBuff[srcSize - 1];
				}
				else
				{
					v = srcBuff[n];
				}
				lv = LanczosFunc(pos, nTap);
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

Int32 MyMain(Core::IProgControl *progCtrl)
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
	sb.Append((const UTF8Char*)"{");
	i = 0;
	while (i < 20)
	{
		if (i > 0)
		{
			sb.Append((const UTF8Char*)", ");
		}
		Text::SBAppendF64(&sb, outVal[i]);
		i++;
	}
	sb.Append((const UTF8Char*)"}");
	console.WriteLine(sb.ToString());
	sb.ClearStr();
	sb.Append((const UTF8Char*)"t = ");
	Text::SBAppendF64(&sb, t);
	console.WriteLine(sb.ToString());
	return 0;
}
