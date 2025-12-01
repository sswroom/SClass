#include "Stdafx.h"
#if defined(USE_OPENCL)
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Math/Math_C.h"
#include "Media/Resizer/LanczosResizerLR_C32_OCL.h"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include <CL/cl.h>

#define RGBTABLESIZE (65536 * 16)

const Char *clCode = ""
"__kernel void horizontal_filter(__global char *inPt, __global char *outPt, int width, int tap, __global long  *index, __global long *weight, int sstep, int dstep, __global char *rgbTable)	\r\n"
"{	\r\n"
"	int lineNum = get_global_id(0);	\r\n"
"	inPt = inPt + lineNum * sstep;	\r\n"
"	outPt = outPt + lineNum * dstep;	\r\n"
"		\r\n"
"	int ctap;	\r\n"
"	if (width & 1)	\r\n"
"	{	\r\n"
"		tap = tap >> 1;\r\n"
"		__global char *cweight = (__global char*)weight;\r\n"
"		while (width-- > 0)	\r\n"
"		{	\r\n"
"			ctap = tap;	\r\n"
"			int4 sum = (int4)(16384);	\r\n"
"			while (ctap-- > 0)	\r\n"
"			{	\r\n"
"				sum += convert_int4(*(__global short4*)&inPt[index[0]]) * convert_int4((short4)(*(__global short*)&cweight[0]));\r\n"
"				sum += convert_int4(*(__global short4*)&inPt[index[1]]) * convert_int4((short4)(*(__global short*)&cweight[2]));\r\n"
"				index += 2;	\r\n"
"				cweight += 16;\r\n"
"			}	\r\n"
"			*(__global short4*)outPt = convert_short4_sat(sum >> 15);	\r\n"
"			outPt += 8;	\r\n"
"		}	\r\n"
"	}	\r\n"
"	else if (tap == 6)	\r\n"
"	{	\r\n"
"		__global char *cweight = (__global char*)weight;\r\n"
"		while (width-- > 0)	\r\n"
"		{	\r\n"
"			int4 sum = (int4)(16384);\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[index[0]]) * convert_int4((short4)(*(__global short*)&cweight[0]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[index[1]]) * convert_int4((short4)(*(__global short*)&cweight[2]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[index[2]]) * convert_int4((short4)(*(__global short*)&cweight[16]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[index[3]]) * convert_int4((short4)(*(__global short*)&cweight[18]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[index[4]]) * convert_int4((short4)(*(__global short*)&cweight[32]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[index[5]]) * convert_int4((short4)(*(__global short*)&cweight[34]));\r\n"
"			*(__global short4*)outPt = convert_short4_sat(sum >> 15);	\r\n"
"			index += 6;	\r\n"
"			cweight += 48;\r\n"
"			outPt += 8;	\r\n"
"		}	\r\n"
"	}	\r\n"
"	else if (tap == 8)	\r\n"
"	{	\r\n"
"		int i;\r\n"
"		__global char *cweight = (__global char*)weight;\r\n"
"		width = width >> 1;	\r\n"
"		while (width-- > 0)	\r\n"
"		{	\r\n"
"			int4 sum = (int4)(16384);	\r\n"
"			i = *(__global int*)&cweight[0];	\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 0]) * convert_int4((short4)(*(__global short*)&cweight[16]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 8]) * convert_int4((short4)(*(__global short*)&cweight[18]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 16]) * convert_int4((short4)(*(__global short*)&cweight[32]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 24]) * convert_int4((short4)(*(__global short*)&cweight[34]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 32]) * convert_int4((short4)(*(__global short*)&cweight[48]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 40]) * convert_int4((short4)(*(__global short*)&cweight[50]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 48]) * convert_int4((short4)(*(__global short*)&cweight[64]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 56]) * convert_int4((short4)(*(__global short*)&cweight[66]));\r\n"
"			*(__global short4*)outPt = convert_short4_sat(sum >> 15);	\r\n"
"	\r\n"
"			i = *(__global int*)&cweight[8];\r\n"
"			sum = (int4)(16384);	\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 0]) * convert_int4((short4)(*(__global short*)&cweight[80]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 8]) * convert_int4((short4)(*(__global short*)&cweight[82]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 16]) * convert_int4((short4)(*(__global short*)&cweight[96]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 24]) * convert_int4((short4)(*(__global short*)&cweight[98]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 32]) * convert_int4((short4)(*(__global short*)&cweight[112]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 40]) * convert_int4((short4)(*(__global short*)&cweight[114]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 48]) * convert_int4((short4)(*(__global short*)&cweight[128]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 56]) * convert_int4((short4)(*(__global short*)&cweight[130]));\r\n"
"			*(__global short4*)&outPt[8] = convert_short4_sat(sum >> 15);	\r\n"
"			cweight += 144;\r\n"
"			outPt += 16;\r\n"
"		}	\r\n"
"	}	\r\n"
"	else if (tap == 16)	\r\n"
"	{	\r\n"
"		int i;\r\n"
"		__global char *cweight = (__global char*)weight;\r\n"
"		width = width >> 1;	\r\n"
"		while (width-- > 0)	\r\n"
"		{	\r\n"
"			int4 sum = (int4)(16384);	\r\n"
"			i = *(__global int*)&cweight[0];	\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 0]) * convert_int4((short4)(*(__global short*)&cweight[16]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 8]) * convert_int4((short4)(*(__global short*)&cweight[18]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 16]) * convert_int4((short4)(*(__global short*)&cweight[32]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 24]) * convert_int4((short4)(*(__global short*)&cweight[34]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 32]) * convert_int4((short4)(*(__global short*)&cweight[48]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 40]) * convert_int4((short4)(*(__global short*)&cweight[50]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 48]) * convert_int4((short4)(*(__global short*)&cweight[64]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 56]) * convert_int4((short4)(*(__global short*)&cweight[66]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 64]) * convert_int4((short4)(*(__global short*)&cweight[80]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 72]) * convert_int4((short4)(*(__global short*)&cweight[82]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 80]) * convert_int4((short4)(*(__global short*)&cweight[96]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 88]) * convert_int4((short4)(*(__global short*)&cweight[98]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 96]) * convert_int4((short4)(*(__global short*)&cweight[112]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 104]) * convert_int4((short4)(*(__global short*)&cweight[114]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 112]) * convert_int4((short4)(*(__global short*)&cweight[128]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 120]) * convert_int4((short4)(*(__global short*)&cweight[130]));\r\n"
"			*(__global short4*)outPt = convert_short4_sat(sum >> 15);	\r\n"
"	\r\n"
"			sum = (int4)(16384);	\r\n"
"			i = *(__global int*)&cweight[8];\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 0]) * convert_int4((short4)(*(__global short*)&cweight[144]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 8]) * convert_int4((short4)(*(__global short*)&cweight[146]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 16]) * convert_int4((short4)(*(__global short*)&cweight[160]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 24]) * convert_int4((short4)(*(__global short*)&cweight[162]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 32]) * convert_int4((short4)(*(__global short*)&cweight[176]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 40]) * convert_int4((short4)(*(__global short*)&cweight[178]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 48]) * convert_int4((short4)(*(__global short*)&cweight[192]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 56]) * convert_int4((short4)(*(__global short*)&cweight[194]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 64]) * convert_int4((short4)(*(__global short*)&cweight[208]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 72]) * convert_int4((short4)(*(__global short*)&cweight[210]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 80]) * convert_int4((short4)(*(__global short*)&cweight[224]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 88]) * convert_int4((short4)(*(__global short*)&cweight[226]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 96]) * convert_int4((short4)(*(__global short*)&cweight[240]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 104]) * convert_int4((short4)(*(__global short*)&cweight[242]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 112]) * convert_int4((short4)(*(__global short*)&cweight[256]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i + 120]) * convert_int4((short4)(*(__global short*)&cweight[258]));\r\n"
"			*(__global short4*)&outPt[8] = convert_short4_sat(sum >> 15);\r\n"
"			cweight += 272;\r\n"
"			outPt += 16;\r\n"
"		}\r\n"
"	}\r\n"
"	else\r\n"
"	{\r\n"
"		tap = tap >> 1;\r\n"
"		__global char *cweight = (__global char*)weight;\r\n"
"		while (width-- > 0)	\r\n"
"		{	\r\n"
"			ctap = tap;	\r\n"
"			int4 sum = (int4)(16384);\r\n"
"			while (ctap-- > 0)\r\n"
"			{\r\n"
"				sum += convert_int4(*(__global short4*)&inPt[index[0]]) * convert_int4((short4)(*(__global short*)&cweight[0]));\r\n"
"				sum += convert_int4(*(__global short4*)&inPt[index[1]]) * convert_int4((short4)(*(__global short*)&cweight[2]));\r\n"
"				index += 2;\r\n"
"				cweight += 16;\r\n"
"			}\r\n"
"			*(__global short4*)outPt = convert_short4_sat(sum >> 15);\r\n"
"			outPt += 8;\r\n"
"		}\r\n"
"	}\r\n"
"}\r\n"

"__kernel void vertical_filter(__global char *inPt, __global char *outPt, int width, int tap, __global long *index, __global long *weight, int sstep, int dstep, __global char *rgbTable)	\r\n"
"{	\r\n"
"	int lineNum = get_global_id(0);	\r\n"
"	outPt = outPt + lineNum * dstep;	\r\n"
"	if (tap == 6)\r\n"
"	{\r\n"
"		int4 sum;	\r\n"
"		short4 lc;	\r\n"
"		int c;	\r\n"
"		__global char *cweight = (__global char*)weight;\r\n"
"		cweight += lineNum * 80;	\r\n"
"		while (width-- > 0)	\r\n"
"		{	\r\n"
"			sum = (int4)(16384);\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[*(__global uint*)&cweight[0]]) * convert_int4((short4)(*(__global short*)&cweight[32]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[*(__global uint*)&cweight[4]]) * convert_int4((short4)(*(__global short*)&cweight[34]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[*(__global uint*)&cweight[8]]) * convert_int4((short4)(*(__global short*)&cweight[48]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[*(__global uint*)&cweight[12]]) * convert_int4((short4)(*(__global short*)&cweight[50]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[*(__global uint*)&cweight[16]]) * convert_int4((short4)(*(__global short*)&cweight[64]));\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[*(__global uint*)&cweight[20]]) * convert_int4((short4)(*(__global short*)&cweight[66]));\r\n"
"			lc = convert_short4_sat(sum >> 15);	\r\n"
"			c = *(__global int*)&rgbTable[0 + 4 * (ushort)lc.x];	\r\n"
"			c |= *(__global int*)&rgbTable[262144 + 4 * (ushort)lc.y];	\r\n"
"			c |= *(__global int*)&rgbTable[524288 + 4 * (ushort)lc.z];	\r\n"
"			c |= *(__global int*)&rgbTable[786432 + 4 * (ushort)lc.w];	\r\n"
"			*(__global int*)outPt = c;	\r\n"
"			outPt += 4;	\r\n"
"			inPt += 8;	\r\n"
"		}	\r\n"
"	}\r\n"
"	else\r\n"
"	{\r\n"
"		int4 sum;	\r\n"
"		short4 lc;	\r\n"
"		int c;\r\n"
"		int tmptap;\r\n"
"		__global char *cweight;\r\n"
"		__global long *cindex;\r\n"
"		index = index + tap * lineNum;\r\n"
"		weight = weight + tap * lineNum;\r\n"
"		tap = tap >> 1;\r\n"
"		while (width-- > 0)\r\n"
"		{\r\n"
"			tmptap = tap;\r\n"
"			cindex = index;\r\n"
"			cweight = (__global char*)weight;\r\n"
"			sum = (int4)(16384);	\r\n"
"			while (tmptap-- > 0)\r\n"
"			{\r\n"
"				sum += convert_int4(*(__global short4*)&inPt[cindex[0]]) * convert_int4((short4)(*(__global short*)&cweight[0]));\r\n"
"				sum += convert_int4(*(__global short4*)&inPt[cindex[1]]) * convert_int4((short4)(*(__global short*)&cweight[2]));\r\n"
"				cweight += 16;\r\n"
"				cindex += 2;\r\n"
"			}\r\n"
"			lc = convert_short4_sat(sum >> 15);	\r\n"
"			c = *(__global int*)&rgbTable[0 + 4 * (ushort)lc.x];	\r\n"
"			c |= *(__global int*)&rgbTable[262144 + 4 * (ushort)lc.y];	\r\n"
"			c |= *(__global int*)&rgbTable[524288 + 4 * (ushort)lc.z];	\r\n"
"			c |= *(__global int*)&rgbTable[786432 + 4 * (ushort)lc.w];	\r\n"
"			*(__global int*)outPt = c;	\r\n"
"			outPt += 4;\r\n"
"			inPt += 8;\r\n"
"		}\r\n"
"	}\r\n"
"}\r\n"

"__kernel void vertical_filter_na(__global char *inPt, __global char *outPt, int width, int tap, __global long *index, __global long *weight, int sstep, int dstep, __global char *rgbTable)	\r\n"
"{	\r\n"
"	int lineNum = get_global_id(0);	\r\n"
"	outPt = outPt + lineNum * dstep;	\r\n"
"	if (tap == 6)\r\n"
"	{\r\n"
"		int4 sum;	\r\n"
"		short4 lc;	\r\n"
"		int c;	\r\n"
"		__global char *cweight = (__global char*)weight;\r\n"
"		cweight += lineNum * 80;	\r\n"
"		int4 w0 = convert_int4((short4)(*(__global short*)&cweight[32]));\r\n"
"		int4 w1 = convert_int4((short4)(*(__global short*)&cweight[34]));\r\n"
"		int4 w2 = convert_int4((short4)(*(__global short*)&cweight[48]));\r\n"
"		int4 w3 = convert_int4((short4)(*(__global short*)&cweight[50]));\r\n"
"		int4 w4 = convert_int4((short4)(*(__global short*)&cweight[64]));\r\n"
"		int4 w5 = convert_int4((short4)(*(__global short*)&cweight[66]));\r\n"
"		uint i0 = *(__global uint*)&cweight[0];\r\n"
"		uint i1 = *(__global uint*)&cweight[4];\r\n"
"		uint i2 = *(__global uint*)&cweight[8];\r\n"
"		uint i3 = *(__global uint*)&cweight[12];\r\n"
"		uint i4 = *(__global uint*)&cweight[16];\r\n"
"		uint i5 = *(__global uint*)&cweight[20];\r\n"
"		while (width-- > 0)	\r\n"
"		{	\r\n"
"			sum = (int4)(16384);\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i0]) * w0;\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i1]) * w1;\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i2]) * w2;\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i3]) * w3;\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i4]) * w4;\r\n"
"			sum += convert_int4(*(__global short4*)&inPt[i5]) * w5;\r\n"
"			lc = convert_short4_sat(sum >> 15);	\r\n"
"			c = *(__global int*)&rgbTable[0 + (((uint)(ushort)lc.x) << 2)];	\r\n"
"			c |= *(__global int*)&rgbTable[262144 + (((uint)(ushort)lc.y) << 2)];	\r\n"
"			c |= *(__global int*)&rgbTable[524288 + (((uint)(ushort)lc.z) << 2)];	\r\n"
"			*(__global int*)outPt = c;	\r\n"
"			outPt += 4;	\r\n"
"			inPt += 8;	\r\n"
"		}	\r\n"
"	}\r\n"
"	else\r\n"
"	{\r\n"
"		int4 sum;	\r\n"
"		short4 lc;	\r\n"
"		int c;\r\n"
"		int tmptap;\r\n"
"		__global char *cweight;\r\n"
"		__global long *cindex;\r\n"
"		index = index + tap * lineNum;\r\n"
"		weight = weight + tap * lineNum;\r\n"
"		tap = tap >> 1;\r\n"
"		while (width-- > 0)\r\n"
"		{\r\n"
"			tmptap = tap;\r\n"
"			cindex = index;\r\n"
"			cweight = (__global char*)weight;\r\n"
"			sum = (int4)(16384);	\r\n"
"			while (tmptap-- > 0)\r\n"
"			{\r\n"
"				sum += convert_int4(*(__global short4*)&inPt[cindex[0]]) * convert_int4((short4)(*(__global short*)&cweight[0]));\r\n"
"				sum += convert_int4(*(__global short4*)&inPt[cindex[1]]) * convert_int4((short4)(*(__global short*)&cweight[2]));\r\n"
"				cweight += 16;\r\n"
"				cindex += 2;\r\n"
"			}\r\n"
"			lc = convert_short4_sat(sum >> 15);	\r\n"
"			c = *(__global int*)&rgbTable[0 + 4 * (ushort)lc.x];	\r\n"
"			c |= *(__global int*)&rgbTable[262144 + 4 * (ushort)lc.y];	\r\n"
"			c |= *(__global int*)&rgbTable[524288 + 4 * (ushort)lc.z];	\r\n"
"			*(__global int*)outPt = c;	\r\n"
"			outPt += 4;\r\n"
"			inPt += 8;\r\n"
"		}\r\n"
"	}\r\n"
"}\r\n"

"__kernel void collapse(__global char *inPt, __global char *outPt, int width, int sstep, int dstep, __global char *rgbTable)	\r\n"
"{	\r\n"
"	int lineNum = get_global_id(0);	\r\n"
"	uint c;	\r\n"
"	inPt = inPt + lineNum * sstep;	\r\n"
"	outPt = outPt + lineNum * dstep;	\r\n"
"	while (width-- > 0)	\r\n"
"	{	\r\n"
"		c = *(__global uint*)&rgbTable[0 + (4 * ((__global ushort*)inPt)[0])];	\r\n"
"		c |= *(__global uint*)&rgbTable[262144 + (4 * ((__global ushort*)inPt)[1])];	\r\n"
"		c |= *(__global uint*)&rgbTable[524288 + (4 * ((__global ushort*)inPt)[2])];	\r\n"
"		c |= *(__global uint*)&rgbTable[786432 + (4 * ((__global ushort*)inPt)[3])];	\r\n"
"		*(__global uint*)outPt = c;	\r\n"
"		inPt += 8;	\r\n"
"		outPt += 4;	\r\n"
"	}	\r\n"
"}	\r\n"


"__kernel void collapse_na(__global char *inPt, __global char *outPt, int width, int sstep, int dstep, __global char *rgbTable)	\r\n"
"{	\r\n"
"	int lineNum = get_global_id(0);	\r\n"
"	uint c;	\r\n"
"	inPt = inPt + lineNum * sstep;	\r\n"
"	outPt = outPt + lineNum * dstep;	\r\n"
"	while (width-- > 0)	\r\n"
"	{	\r\n"
"		c = *(__global uint*)&rgbTable[0 + 4 * ((__global ushort*)inPt)[0]];	\r\n"
"		c |= *(__global uint*)&rgbTable[262144 + 4 * ((__global ushort*)inPt)[1]];	\r\n"
"		c |= *(__global uint*)&rgbTable[524288 + 4 * ((__global ushort*)inPt)[2]];	\r\n"
"		*(__global uint*)outPt = c;	\r\n"
"		inPt += 8;	\r\n"
"		outPt += 4;	\r\n"
"	}	\r\n"
"}	\r\n"
;

typedef struct
{
	OSInt tap;
	OSInt indexSize;
	cl_mem index;
	OSInt weightSize;
	cl_mem weight;
} FilterParam;

struct Media::Resizer::LanczosResizerLR_C32_OCL::ClassData
{
	OSInt hTotCount;
	Double hTotTime;
	OSInt vTotCount;
	Double vTotTime;

	Bool inited;
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue commandQueue;
	cl_program program;
	cl_kernel hFilterKernel;
	cl_kernel vFilterKernel;
	cl_kernel vFilterNAKernel;
	cl_kernel collapseKernel;
	cl_kernel collapseNAKernel;
	cl_mem rgbTable;
	cl_mem buffObj;
	OSInt buffSize;
};

Media::Resizer::LanczosResizerLR_C32_OCL::LanczosResizerLR_C32_OCL()
{
	NN<ClassData> data = MemAllocNN(ClassData);
	this->clsData = data;
	data->hTotCount = 0;
	data->hTotTime = 0;
	data->vTotCount = 0;
	data->vTotTime = 0;

	cl_uint numPlatforms;
	data->inited = false;
	cl_int status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (status != CL_SUCCESS)
	{
		return;
	}
	if (numPlatforms == 0)
	{
		return;
	}
	cl_uint	numDevices = 0;
	cl_uint tmpIndex;
	cl_platform_id* platforms = MemAlloc(cl_platform_id, numPlatforms);
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);

	Bool found = false;
	tmpIndex = 0;
	while (tmpIndex < numPlatforms)
	{
		cl_device_id *devices;
		status = clGetDeviceIDs(platforms[tmpIndex], CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);	
		if (numDevices != 0)
		{
			devices = MemAlloc(cl_device_id, numDevices);
			data->platform = platforms[tmpIndex];
			status = clGetDeviceIDs(data->platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
			data->device = devices[0];
			MemFree(devices);
			found = true;
			break;
		}
		tmpIndex++;
	}
	if (!found)
	{
		cl_device_id *devices;
		data->platform = platforms[0];
		status = clGetDeviceIDs(data->platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);	
		devices = MemAlloc(cl_device_id, numDevices);
		status = clGetDeviceIDs(data->platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);
		data->device = devices[0];
		MemFree(devices);
	}
	MemFree(platforms);

/*	cl_uint maxComputeUnits;
	size_t maxWorkGroupSize;
	Char deviceName[128];
	Char vendor[128];
	Char version[128];
	status = clGetDeviceInfo(data->device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, 0);
	status = clGetDeviceInfo(data->device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, 0);
	status = clGetDeviceInfo(data->device, CL_DEVICE_NAME, sizeof(deviceName), deviceName, 0);
	status = clGetDeviceInfo(data->device, CL_DEVICE_VENDOR, sizeof(vendor), vendor, 0);
	status = clGetDeviceInfo(data->device, CL_DEVICE_VERSION, sizeof(version), version, 0);*/

	data->context = clCreateContext(0, 1, &data->device, 0, 0, 0);
	data->commandQueue = clCreateCommandQueue(data->context, data->device, 0, 0);
	size_t sourceSize[1];
	sourceSize[0] = Text::StrCharCnt(clCode);
	data->program = clCreateProgramWithSource(data->context, 1, &clCode, sourceSize, 0);
	status = clBuildProgram(data->program, 1, &data->device, 0, 0, 0);
	if (status == -11)
	{
		size_t buffSize = 65536;
		Char *cBuff = MemAlloc(Char, buffSize);
		status = clGetProgramBuildInfo(data->program, data->device, CL_PROGRAM_BUILD_LOG, buffSize, cBuff, &buffSize);
		cBuff[buffSize] = 0;
	}
	data->hFilterKernel = clCreateKernel(data->program, "horizontal_filter", 0);
	data->vFilterKernel = clCreateKernel(data->program, "vertical_filter", 0);
	data->vFilterNAKernel = clCreateKernel(data->program, "vertical_filter_na", 0);
	data->collapseKernel = clCreateKernel(data->program, "collapse", 0);
	data->collapseNAKernel = clCreateKernel(data->program, "collapse_na", 0);
	data->rgbTable = clCreateBuffer(data->context, CL_MEM_READ_ONLY, RGBTABLESIZE, 0, 0);
	data->buffSize = 0;
	data->inited = true;
}

Media::Resizer::LanczosResizerLR_C32_OCL::~LanczosResizerLR_C32_OCL()
{
	NN<ClassData> data = this->clsData;
	if (data->inited)
	{
		cl_int status;
		status = clReleaseMemObject(data->rgbTable);
		if (data->buffSize > 0)
		{
			status = clReleaseMemObject(data->buffObj);
			data->buffSize = 0;
		}
		status = clReleaseKernel(data->hFilterKernel);
		status = clReleaseKernel(data->vFilterKernel);
		status = clReleaseKernel(data->vFilterNAKernel);
		status = clReleaseKernel(data->collapseKernel);
		status = clReleaseKernel(data->collapseNAKernel);
		status = clReleaseProgram(data->program);
		status = clReleaseCommandQueue(data->commandQueue);
		status = clReleaseContext(data->context);
	}
	MemFree(data);
}

void Media::Resizer::LanczosResizerLR_C32_OCL::DoHorizontalVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt sheight, UOSInt dheight, NN<HoriFilter> hFilter, NN<VertFilter> vFilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType)
{
	NN<ClassData> data = this->clsData;
	NN<FilterParam> hparam = NN<FilterParam>::ConvertFrom(hFilter);
	NN<FilterParam> vparam = NN<FilterParam>::ConvertFrom(vFilter);

	if (data->buffSize < (sheight * dwidth << 3))
	{
		cl_int status;
		if (data->buffSize > 0)
		{
			status = clReleaseMemObject(data->buffObj);
		}
		data->buffSize = (sheight * dwidth << 3);
		data->buffObj = clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->buffSize, 0, 0);
	}
	Manage::HiResClock clk;
	cl_mem input;
	cl_mem output;
	cl_int status;
	OSInt inputSize = sheight * sstep;
	OSInt outputSize = dheight * dstep;
	OSInt buffStep = dwidth << 3;
//	input = clCreateBuffer(data->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, inputSize, inPt, 0);
//	output = clCreateBuffer(data->context, CL_MEM_WRITE_ONLY, outputSize, 0, 0);
	input = clCreateBuffer(data->context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, inputSize, inPt, 0);
	output = clCreateBuffer(data->context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, outputSize, outPt, 0);

	status = clSetKernelArg(data->hFilterKernel, 0, sizeof(cl_mem), &input);
	status = clSetKernelArg(data->hFilterKernel, 1, sizeof(cl_mem), &data->buffObj);
	status = clSetKernelArg(data->hFilterKernel, 2, sizeof(Int32), &dwidth);
	status = clSetKernelArg(data->hFilterKernel, 3, sizeof(Int32), &hparam->tap);
	if (hparam->indexSize > 0)
	{
		status = clSetKernelArg(data->hFilterKernel, 4, sizeof(cl_mem), &hparam->index);
	}
	else
	{
		status = clSetKernelArg(data->hFilterKernel, 4, sizeof(cl_mem), 0);
	}
	status = clSetKernelArg(data->hFilterKernel, 5, sizeof(cl_mem), &hparam->weight);
	status = clSetKernelArg(data->hFilterKernel, 6, sizeof(Int32), &sstep);
	status = clSetKernelArg(data->hFilterKernel, 7, sizeof(Int32), &buffStep);
	status = clSetKernelArg(data->hFilterKernel, 8, sizeof(cl_mem), &data->rgbTable);
	status = clEnqueueNDRangeKernel(data->commandQueue, data->hFilterKernel, 1, NULL, (const size_t*)&sheight, 0, 0, 0, 0);
	clFinish(data->commandQueue);
	data->hTotTime = clk.GetTimeDiff();
	data->hTotCount = 1;


	clk.Start();
	if (srcAlphaType == Media::AT_NO_ALPHA)
	{
		status = clSetKernelArg(data->vFilterNAKernel, 0, sizeof(cl_mem), &data->buffObj);
		status = clSetKernelArg(data->vFilterNAKernel, 1, sizeof(cl_mem), &output);
		status = clSetKernelArg(data->vFilterNAKernel, 2, sizeof(Int32), &dwidth);
		status = clSetKernelArg(data->vFilterNAKernel, 3, sizeof(Int32), &vparam->tap);
		if (vparam->indexSize > 0)
		{
			status = clSetKernelArg(data->vFilterNAKernel, 4, sizeof(cl_mem), &vparam->index);
		}
		else
		{
			status = clSetKernelArg(data->vFilterNAKernel, 4, sizeof(cl_mem), 0);
		}
		status = clSetKernelArg(data->vFilterNAKernel, 5, sizeof(cl_mem), &vparam->weight);
		status = clSetKernelArg(data->vFilterNAKernel, 6, sizeof(Int32), &buffStep);
		status = clSetKernelArg(data->vFilterNAKernel, 7, sizeof(Int32), &dstep);
		status = clSetKernelArg(data->vFilterNAKernel, 8, sizeof(cl_mem), &data->rgbTable);
		status = clEnqueueNDRangeKernel(data->commandQueue, data->vFilterNAKernel, 1, NULL, (const size_t*)&dheight, 0, 0, 0, 0);
	}
	else
	{
		status = clSetKernelArg(data->vFilterKernel, 0, sizeof(cl_mem), &data->buffObj);
		status = clSetKernelArg(data->vFilterKernel, 1, sizeof(cl_mem), &output);
		status = clSetKernelArg(data->vFilterKernel, 2, sizeof(Int32), &dwidth);
		status = clSetKernelArg(data->vFilterKernel, 3, sizeof(Int32), &vparam->tap);
		if (vparam->indexSize > 0)
		{
			status = clSetKernelArg(data->vFilterKernel, 4, sizeof(cl_mem), &vparam->index);
		}
		else
		{
			status = clSetKernelArg(data->vFilterKernel, 4, sizeof(cl_mem), 0);
		}
		status = clSetKernelArg(data->vFilterKernel, 5, sizeof(cl_mem), &vparam->weight);
		status = clSetKernelArg(data->vFilterKernel, 6, sizeof(Int32), &buffStep);
		status = clSetKernelArg(data->vFilterKernel, 7, sizeof(Int32), &dstep);
		status = clSetKernelArg(data->vFilterKernel, 8, sizeof(cl_mem), &data->rgbTable);
		clEnqueueNDRangeKernel(data->commandQueue, data->vFilterKernel, 1, NULL, (const size_t*)&dheight, 0, 0, 0, 0);
	}
//	clEnqueueReadBuffer(data->commandQueue, output, CL_TRUE, 0, outputSize, outPt, 0, 0, 0);
	clFinish(data->commandQueue);
	clReleaseMemObject(output);
	clReleaseMemObject(input);
	data->vTotTime = clk.GetTimeDiff();
	data->vTotCount = 1;
}

void Media::Resizer::LanczosResizerLR_C32_OCL::DoHorizontalFilterCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt sheight, NN<HoriFilter> hFilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType)
{
	NN<ClassData> data = this->clsData;
	NN<FilterParam> hparam = NN<FilterParam>::ConvertFrom(hFilter);

	if (data->buffSize < (sheight * dwidth << 3))
	{
		cl_int status;
		if (data->buffSize > 0)
		{
			status = clReleaseMemObject(data->buffObj);
		}
		data->buffSize = (sheight * dwidth << 3);
		data->buffObj = clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->buffSize, 0, 0);
	}
	Manage::HiResClock clk;
	cl_mem input;
	cl_mem output;
	OSInt inputSize = sheight * sstep;
	OSInt outputSize = sheight * dstep;
	OSInt buffStep = dwidth << 3;
//	input = clCreateBuffer(data->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, inputSize, inPt, 0);
//	output = clCreateBuffer(data->context, CL_MEM_WRITE_ONLY, outputSize, 0, 0);
	input = clCreateBuffer(data->context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, inputSize, inPt, 0);
	output = clCreateBuffer(data->context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, outputSize, outPt, 0);

	clSetKernelArg(data->hFilterKernel, 0, sizeof(cl_mem), &input);
	clSetKernelArg(data->hFilterKernel, 1, sizeof(cl_mem), &data->buffObj);
	clSetKernelArg(data->hFilterKernel, 2, sizeof(Int32), &dwidth);
	clSetKernelArg(data->hFilterKernel, 3, sizeof(Int32), &hparam->tap);
	if (hparam->indexSize > 0)
	{
		clSetKernelArg(data->hFilterKernel, 4, sizeof(cl_mem), &hparam->index);
	}
	else
	{
		clSetKernelArg(data->hFilterKernel, 4, sizeof(cl_mem), 0);
	}
	clSetKernelArg(data->hFilterKernel, 5, sizeof(cl_mem), &hparam->weight);
	clSetKernelArg(data->hFilterKernel, 6, sizeof(Int32), &sstep);
	clSetKernelArg(data->hFilterKernel, 7, sizeof(Int32), &buffStep);
	clSetKernelArg(data->hFilterKernel, 8, sizeof(cl_mem), &data->rgbTable);
	clEnqueueNDRangeKernel(data->commandQueue, data->hFilterKernel, 1, NULL, (const size_t*)&sheight, 0, 0, 0, 0);
	data->hTotTime = clk.GetTimeDiff();
	data->hTotCount = 1;

	if (srcAlphaType == Media::AT_NO_ALPHA)
	{
		clSetKernelArg(data->collapseNAKernel, 0, sizeof(cl_mem), &data->buffObj);
		clSetKernelArg(data->collapseNAKernel, 1, sizeof(cl_mem), &output);
		clSetKernelArg(data->collapseNAKernel, 2, sizeof(Int32), &dwidth);
		clSetKernelArg(data->collapseNAKernel, 3, sizeof(Int32), &buffStep);
		clSetKernelArg(data->collapseNAKernel, 4, sizeof(Int32), &dstep);
		clSetKernelArg(data->collapseNAKernel, 5, sizeof(cl_mem), &data->rgbTable);
		clEnqueueNDRangeKernel(data->commandQueue, data->collapseNAKernel, 1, NULL, (const size_t*)&sheight, 0, 0, 0, 0);
	}
	else
	{
		clSetKernelArg(data->collapseKernel, 0, sizeof(cl_mem), &data->buffObj);
		clSetKernelArg(data->collapseKernel, 1, sizeof(cl_mem), &output);
		clSetKernelArg(data->collapseKernel, 2, sizeof(Int32), &dwidth);
		clSetKernelArg(data->collapseKernel, 3, sizeof(Int32), &buffStep);
		clSetKernelArg(data->collapseKernel, 4, sizeof(Int32), &dstep);
		clSetKernelArg(data->collapseKernel, 5, sizeof(cl_mem), &data->rgbTable);
		clEnqueueNDRangeKernel(data->commandQueue, data->collapseKernel, 1, NULL, (const size_t*)&sheight, 0, 0, 0, 0);
	}
//	clEnqueueReadBuffer(data->commandQueue, output, CL_TRUE, 0, outputSize, outPt, 0, 0, 0);
	clFinish(data->commandQueue);
	clReleaseMemObject(output);
	clReleaseMemObject(input);
}

void Media::Resizer::LanczosResizerLR_C32_OCL::DoVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt swidth, UOSInt sheight, UOSInt dheight, NN<VertFilter> vFilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType)
{
	NN<ClassData> data = this->clsData;
	NN<FilterParam> vparam = NN<FilterParam>::ConvertFrom(vFilter);
	Manage::HiResClock clk;
	cl_mem input;
	cl_mem output;
	OSInt inputSize = sheight * sstep;
	OSInt outputSize = dheight * dstep;
	input = clCreateBuffer(data->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, inputSize, inPt, 0);
	output = clCreateBuffer(data->context, CL_MEM_WRITE_ONLY, outputSize, 0, 0);
	if (srcAlphaType == Media::AT_NO_ALPHA)
	{
		clSetKernelArg(data->vFilterNAKernel, 0, sizeof(cl_mem), &input);
		clSetKernelArg(data->vFilterNAKernel, 1, sizeof(cl_mem), &output);
		clSetKernelArg(data->vFilterNAKernel, 2, sizeof(Int32), &swidth);
		clSetKernelArg(data->vFilterNAKernel, 3, sizeof(Int32), &vparam->tap);
		if (vparam->indexSize > 0)
		{
			clSetKernelArg(data->vFilterNAKernel, 4, sizeof(cl_mem), &vparam->index);
		}
		else
		{
			clSetKernelArg(data->vFilterNAKernel, 4, sizeof(cl_mem), 0);
		}
		clSetKernelArg(data->vFilterNAKernel, 5, sizeof(cl_mem), &vparam->weight);
		clSetKernelArg(data->vFilterNAKernel, 6, sizeof(Int32), &sstep);
		clSetKernelArg(data->vFilterNAKernel, 7, sizeof(Int32), &dstep);
		clSetKernelArg(data->vFilterNAKernel, 8, sizeof(cl_mem), &data->rgbTable);
		clEnqueueNDRangeKernel(data->commandQueue, data->vFilterNAKernel, 1, NULL, (const size_t*)&dheight, 0, 0, 0, 0);
	}
	else
	{
		clSetKernelArg(data->vFilterKernel, 0, sizeof(cl_mem), &input);
		clSetKernelArg(data->vFilterKernel, 1, sizeof(cl_mem), &output);
		clSetKernelArg(data->vFilterKernel, 2, sizeof(Int32), &swidth);
		clSetKernelArg(data->vFilterKernel, 3, sizeof(Int32), &vparam->tap);
		if (vparam->indexSize > 0)
		{
			clSetKernelArg(data->vFilterKernel, 4, sizeof(cl_mem), &vparam->index);
		}
		else
		{
			clSetKernelArg(data->vFilterKernel, 4, sizeof(cl_mem), 0);
		}
		clSetKernelArg(data->vFilterKernel, 5, sizeof(cl_mem), &vparam->weight);
		clSetKernelArg(data->vFilterKernel, 6, sizeof(Int32), &sstep);
		clSetKernelArg(data->vFilterKernel, 7, sizeof(Int32), &dstep);
		clSetKernelArg(data->vFilterKernel, 8, sizeof(cl_mem), &data->rgbTable);
		clEnqueueNDRangeKernel(data->commandQueue, data->vFilterKernel, 1, NULL, (const size_t*)&dheight, 0, 0, 0, 0);
	}
	clEnqueueReadBuffer(data->commandQueue, output, CL_TRUE, 0, outputSize, outPt, 0, 0, 0);
	clFinish(data->commandQueue);
	clReleaseMemObject(output);
	clReleaseMemObject(input);
	data->vTotTime = clk.GetTimeDiff();
	data->vTotCount = 1;
}

void Media::Resizer::LanczosResizerLR_C32_OCL::DoCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt swidth, UOSInt sheight, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType)
{
	NN<ClassData> data = this->clsData;
	cl_mem input;
	cl_mem output;
	OSInt inputSize = sheight * sstep;
	OSInt outputSize = sheight * dstep;
	input = clCreateBuffer(data->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, inputSize, inPt, 0);
	output = clCreateBuffer(data->context, CL_MEM_WRITE_ONLY, outputSize, 0, 0);
	if (srcAlphaType == Media::AT_NO_ALPHA)
	{
		clSetKernelArg(data->collapseNAKernel, 0, sizeof(cl_mem), &input);
		clSetKernelArg(data->collapseNAKernel, 1, sizeof(cl_mem), &output);
		clSetKernelArg(data->collapseNAKernel, 2, sizeof(Int32), &swidth);
		clSetKernelArg(data->collapseNAKernel, 3, sizeof(Int32), &sstep);
		clSetKernelArg(data->collapseNAKernel, 4, sizeof(Int32), &dstep);
		clSetKernelArg(data->collapseNAKernel, 5, sizeof(cl_mem), &data->rgbTable);
		clEnqueueNDRangeKernel(data->commandQueue, data->collapseNAKernel, 1, NULL, (const size_t*)&sheight, 0, 0, 0, 0);
	}
	else
	{
		clSetKernelArg(data->collapseKernel, 0, sizeof(cl_mem), &input);
		clSetKernelArg(data->collapseKernel, 1, sizeof(cl_mem), &output);
		clSetKernelArg(data->collapseKernel, 2, sizeof(Int32), &swidth);
		clSetKernelArg(data->collapseKernel, 3, sizeof(Int32), &sstep);
		clSetKernelArg(data->collapseKernel, 4, sizeof(Int32), &dstep);
		clSetKernelArg(data->collapseKernel, 5, sizeof(cl_mem), &data->rgbTable);
		clEnqueueNDRangeKernel(data->commandQueue, data->collapseKernel, 1, NULL, (const size_t*)&sheight, 0, 0, 0, 0);
	}
	clEnqueueReadBuffer(data->commandQueue, output, CL_TRUE, 0, outputSize, outPt, 0, 0, 0);
	clFinish(data->commandQueue);
	clReleaseMemObject(output);
	clReleaseMemObject(input);
}

void Media::Resizer::LanczosResizerLR_C32_OCL::UpdateRGBTable(UnsafeArray<UInt8> rgbTable)
{
	NN<ClassData> data = this->clsData;
	cl_int status;
	status = clEnqueueWriteBuffer(data->commandQueue, data->rgbTable, CL_TRUE, 0, RGBTABLESIZE, rgbTable, 0, 0, 0);
}

NN<Media::Resizer::LanczosResizerLR_C32Action::HoriFilter> Media::Resizer::LanczosResizerLR_C32_OCL::CreateHoriFilter(OSInt htap, UnsafeArray<OSInt> hIndex, UnsafeArray<Int64> hWeight, OSInt length)
{
	NN<ClassData> data = this->clsData;
	NN<FilterParam> param = MemAllocNN(FilterParam);
	param->tap = htap;
	if ((length & 1) == 0 && (htap == 8 || htap == 16))
	{
		param->indexSize = 0;
		param->weightSize = sizeof(Int64) * length * (htap + 1);
		param->weight = clCreateBuffer(data->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, param->weightSize, hWeight, 0);
	}
	else
	{
		param->indexSize = sizeof(OSInt) * length * htap;
		param->weightSize = sizeof(Int64) * length * htap;
		param->index = clCreateBuffer(data->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, param->indexSize, hIndex, 0);
		param->weight = clCreateBuffer(data->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, param->weightSize, hWeight, 0);
	}
	return NN<HoriFilter>::ConvertFrom(param);
}

void Media::Resizer::LanczosResizerLR_C32_OCL::DestroyHoriFilter(NN<Media::Resizer::LanczosResizerLR_C32Action::HoriFilter> hfilter)
{
	NN<FilterParam> param = NN<FilterParam>::ConvertFrom(hfilter);
	if (param->indexSize > 0)
	{
		clReleaseMemObject(param->index);
	}
	clReleaseMemObject(param->weight);
	MemFreeNN(param);
}

NN<Media::Resizer::LanczosResizerLR_C32Action::VertFilter> Media::Resizer::LanczosResizerLR_C32_OCL::CreateVertFilter(OSInt vtap, UnsafeArray<OSInt> vIndex, UnsafeArray<Int64> vWeight, UOSInt length)
{
	NN<ClassData> data = this->clsData;
	NN<FilterParam> param = MemAllocNN(FilterParam);
	param->tap = vtap;
	if (vtap == 6)
	{
		param->indexSize = 0;
		param->weightSize = sizeof(Int64) * length * 10;
		param->weight = clCreateBuffer(data->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, param->weightSize, vWeight, 0);
	}
	else
	{
		param->indexSize = sizeof(OSInt) * length * vtap;
		param->weightSize = sizeof(Int64) * length * vtap;
		param->index = clCreateBuffer(data->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, param->indexSize, vIndex, 0);
		param->weight = clCreateBuffer(data->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, param->weightSize, vWeight, 0);
	}
	return NN<VertFilter>::ConvertFrom(param);
}

void Media::Resizer::LanczosResizerLR_C32_OCL::DestroyVertFilter(NN<Media::Resizer::LanczosResizerLR_C32Action::VertFilter> vfilter)
{
	NN<FilterParam> param = NN<FilterParam>::ConvertFrom(vfilter);
	if (param->indexSize > 0)
	{
		clReleaseMemObject(param->index);
	}
	clReleaseMemObject(param->weight);
	MemFreeNN(param);
}

Double Media::Resizer::LanczosResizerLR_C32_OCL::GetHAvgTime()
{
	NN<ClassData> data = this->clsData;
	if (data->hTotCount <= 0)
		return 0;
	else
		return data->hTotTime / data->hTotCount;
}

Double Media::Resizer::LanczosResizerLR_C32_OCL::GetVAvgTime()
{
	NN<ClassData> data = this->clsData;
	if (data->vTotCount <= 0)
		return 0;
	else
		return data->vTotTime / data->vTotCount;
}
#endif