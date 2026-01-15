#include "Stdafx.h"
#include "MyMemory.h"

extern "C"
{

void IVTCFilter_CalcField(UInt8 *oddPtr, UInt8 *evenPtr, UIntOS w, UIntOS h, UInt32 *fieldStats)
{
	UIntOS w2 = w << 1;
	UIntOS hLeft;
	UIntOS wLeft;
	UInt32 v1;
	UInt32 v2;
	UInt32 v3;
	UInt32 v4;
	UInt32 v5;
	UInt32 fieldDiff = 0;
	UInt32 fieldMDiff = 0;
	UInt32 fieldCnt = 0;
	UInt32 field2Diff = 0;
	UInt32 field2MDiff = 0;
	UInt32 field2Cnt = 0;
//	field3Cnt = 0;
//	field4Cnt = 0;

	hLeft = h - 2;
	while (hLeft > 0)
	{
		hLeft -= 2;
		wLeft = w;
		while (wLeft-- > 0)
		{
			v1 = oddPtr[0];
			v2 = oddPtr[w2];
			v3 = evenPtr[w];

			if (v1 >= v2)
			{
				if (v3 > v1)
				{
					v4 = v3 - v1;
					fieldDiff += v4;
					fieldCnt++;
					if (v4 > fieldMDiff)
					{
						fieldMDiff = v4;
					}
					if (v4 > v1 - v2)
					{
						v5 = v4 - (v1 - v2);
						field2Diff += v5;
						field2Cnt++;
						if (v5 > field2MDiff)
						{
							field2MDiff = v5;
						}
/*						if (v5 > 16)
						{
							field3Cnt++;
						}
						if (v5 > 32)
						{
							field4Cnt++;
						}*/
					}
				}
				else if (v3 < v2)
				{
					v4 = v2 - v3;
					fieldDiff += v4;
					fieldCnt++;
					if (v4 > fieldMDiff)
					{
						fieldMDiff = v4;
					}
					if (v4 > v1 - v2)
					{
						v5 = v4 - (v1 - v2);
						field2Diff += v5;
						field2Cnt++;
						if (v5 > field2MDiff)
						{
							field2MDiff = v5;
						}
/*						if (v5 > 16)
						{
							field3Cnt++;
						}
						if (v5 > 32)
						{
							field4Cnt++;
						}*/
					}
				}
			}
			else
			{
				if (v3 < v1)
				{
					v4 = v1 - v3;
					fieldDiff += v4;
					fieldCnt++;
					if (v4 > fieldMDiff)
					{
						fieldMDiff = v4;
					}
					if (v4 > v2 - v1)
					{
						v5 = v4 - (v2 - v1);
						field2Diff += v5;
						field2Cnt++;
						if (v5 > field2MDiff)
						{
							field2MDiff = v5;
						}
/*						if (v5 > 16)
						{
							field3Cnt++;
						}
						if (v5 > 32)
						{
							field4Cnt++;
						}*/
					}
				}
				else if (v3 > v2)
				{
					v4 = v3 - v2;
					fieldDiff += v4;
					fieldCnt++;
					if (v4 > fieldMDiff)
					{
						fieldMDiff = v4;
					}
					if (v4 > v2 - v1)
					{
						v5 = v4 - (v2 - v1);
						field2Diff += v5;
						field2Cnt++;
						if (v5 > field2MDiff)
						{
							field2MDiff = v5;
						}
/*						if (v5 > 16)
						{
							field3Cnt++;
						}
						if (v5 > 32)
						{
							field4Cnt++;
						}*/
					}
				}
			}

			v1 = evenPtr[w];
			v2 = evenPtr[w + w2];
			v3 = oddPtr[w2];

			if (v1 >= v2)
			{
				if (v3 > v1)
				{
					v4 = v3 - v1;
					fieldDiff += v4;
					fieldCnt++;
					if (v4 > fieldMDiff)
					{
						fieldMDiff = v4;
					}
					if (v4 > v1 - v2)
					{
						v5 = v4 - (v1 - v2);
						field2Diff += v5;
						field2Cnt++;
						if (v5 > field2MDiff)
						{
							field2MDiff = v5;
						}
/*						if (v5 > 16)
						{
							field3Cnt++;
						}
						if (v5 > 32)
						{
							field4Cnt++;
						}*/
					}
				}
				else if (v3 < v2)
				{
					v4 = v2 - v3;
					fieldDiff += v4;
					fieldCnt++;
					if (v4 > fieldMDiff)
					{
						fieldMDiff = v4;
					}
					if (v4 > v1 - v2)
					{
						v5 = v4 - (v1 - v2);
						field2Diff += v5;
						field2Cnt++;
						if (v5 > field2MDiff)
						{
							field2MDiff = v5;
						}
/*						if (v5 > 16)
						{
							field3Cnt++;
						}
						if (v5 > 32)
						{
							field4Cnt++;
						}*/
					}
				}
			}
			else
			{
				if (v3 < v1)
				{
					v4 = v1 - v3;
					fieldDiff += v4;
					fieldCnt++;
					if (v4 > fieldMDiff)
					{
						fieldMDiff = v4;
					}
					if (v4 > v2 - v1)
					{
						v5 = v4 - (v2 - v1);
						field2Diff += v5;
						field2Cnt++;
						if (v5 > field2MDiff)
						{
							field2MDiff = v5;
						}
/*						if (v5 > 16)
						{
							field3Cnt++;
						}*/
/*						if (v5 > 32)
						{
							field4Cnt++;
						}*/
					}
				}
				else if (v3 > v2)
				{
					v4 = v3 - v2;
					fieldDiff += v4;
					fieldCnt++;
					if (v4 > fieldMDiff)
					{
						fieldMDiff = v4;
					}
					if (v4 > v2 - v1)
					{
						v5 = v4 - (v2 - v1);
						field2Diff += v5;
						field2Cnt++;
						if (v5 > field2MDiff)
						{
							field2MDiff = v5;
						}
/*						if (v5 > 16)
						{
							field3Cnt++;
						}
						if (v5 > 32)
						{
							field4Cnt++;
						}*/
					}
				}
			}
			oddPtr++;
			evenPtr++;
		}

		oddPtr += w;
		evenPtr += w;
	}
	fieldStats[0] = fieldDiff;
	fieldStats[1] = fieldCnt;
	fieldStats[2] = field2Diff;
	fieldStats[3] = field2Cnt;
	fieldStats[4] = fieldMDiff;
	fieldStats[5] = field2MDiff;
}

void IVTCFilter_CalcFieldP(UInt8 *framePtr, UIntOS w, UIntOS h, UInt32 *fieldStats)
{
	UIntOS wLeft;
	UInt32 v1;
	UInt32 v2;
	UInt32 v3;
	UInt32 v4;
	UInt32 v5;
	UIntOS w2 = w << 1;
	UInt32 fieldDiff = 0;
	UInt32 fieldMDiff = 0;
	UInt32 fieldCnt = 0;
	UInt32 field2Diff = 0;
	UInt32 field2MDiff = 0;
	UInt32 field2Cnt = 0;
//	field3Cnt = 0;
//	field4Cnt = 0;

	wLeft = w * (h - 2);
	while (wLeft-- > 0)
	{
		v1 = framePtr[0];
		v2 = framePtr[w2];
		v3 = framePtr[w];

		if (v1 >= v2)
		{
			if (v3 > v1)
			{
				v4 = v3 - v1;
				fieldDiff += v4;
				fieldCnt++;
				if (v4 > fieldMDiff)
				{
					fieldMDiff = v4;
				}
				if (v4 > v1 - v2)
				{
					v5 = v4 - (v1 - v2);
					field2Diff += v5;
					field2Cnt++;
					if (v5 > field2MDiff)
					{
						field2MDiff = v5;
					}
/*					if (v5 > 16)
					{
						field3Cnt++;
					}
					if (v5 > 32)
					{
						field4Cnt++;
					}*/
				}
			}
			else if (v3 < v2)
			{
				v4 = v2 - v3;
				fieldDiff += v4;
				fieldCnt++;
				if (v4 > fieldMDiff)
				{
					fieldMDiff = v4;
				}
				if (v4 > v1 - v2)
				{
					v5 = v4 - (v1 - v2);
					field2Diff += v5;
					field2Cnt++;
					if (v5 > field2MDiff)
					{
						field2MDiff = v5;
					}
/*					if (v5 > 16)
					{
						field3Cnt++;
					}
					if (v5 > 32)
					{
						field4Cnt++;
					}*/
				}
			}
		}
		else
		{
			if (v3 < v1)
			{
				v4 = v1 - v3;
				fieldDiff += v4;
				fieldCnt++;
				if (v4 > fieldMDiff)
				{
					fieldMDiff = v4;
				}
				if (v4 > v2 - v1)
				{
					v5 = v4 - (v2 - v1);
					field2Diff += v5;
					field2Cnt++;
					if (v5 > field2MDiff)
					{
						field2MDiff = v5;
					}
/*					if (v5 > 16)
					{
						field3Cnt++;
					}
					if (v5 > 32)
					{
						field4Cnt++;
					}*/
				}
			}
			else if (v3 > v2)
			{
				v4 = v3 - v2;
				fieldDiff += v4;
				fieldCnt++;
				if (v4 > fieldMDiff)
				{
					fieldMDiff = v4;
				}
				if (v4 > v2 - v1)
				{
					v5 = v4 - (v2 - v1);
					field2Diff += v5;
					field2Cnt++;
					if (v5 > field2MDiff)
					{
						field2MDiff = v5;
					}
/*					if (v5 > 16)
					{
						field3Cnt++;
					}
					if (v5 > 32)
					{
						field4Cnt++;
					}*/
				}
			}
		}

		framePtr++;
	}
	fieldStats[0] = fieldDiff;
	fieldStats[1] = fieldCnt;
	fieldStats[2] = field2Diff;
	fieldStats[3] = field2Cnt;
	fieldStats[4] = fieldMDiff;
	fieldStats[5] = field2MDiff;
}

}
