#include "Stdafx.h"
#include "Media/JBIG2ArithmeticDecoder.h"

const Media::JBIG2ArithmeticDecoder::JBIG2Qe Media::JBIG2ArithmeticDecoder::jbig2Qe[] =
{
	{0x5601,1,1,true},{0x3401,2,6,false},{0x1801,3,9,false},{0x0ac1,4,12,false},
	{0x0521,5,29,false},{0x0221,38,33,false},{0x5601,7,6,true},{0x5401,8,14,false},
	{0x4801,9,14,false},{0x3801,10,14,false},{0x3001,11,17,false},{0x2401,12,18,false},
	{0x1c01,13,20,false},{0x1601,29,21,false},{0x5601,15,14,true},{0x5401,16,14,false},
	{0x5101,17,15,false},{0x4801,18,16,false},{0x3801,19,17,false},{0x3401,20,18,false},
	{0x3001,21,19,false},{0x2801,22,19,false},{0x2401,23,20,false},{0x2201,24,21,false},
	{0x1c01,25,22,false},{0x1801,26,23,false},{0x1601,27,24,false},{0x1401,28,25,false},
	{0x1201,29,26,false},{0x1101,30,27,false},{0x0ac1,31,28,false},{0x09c1,32,29,false},
	{0x08a1,33,30,false},{0x0521,34,31,false},{0x0441,35,32,false},{0x02a1,36,33,false},
	{0x0221,37,34,false},{0x0141,38,35,false},{0x0111,39,36,false},{0x0085,40,37,false},
	{0x0049,41,38,false},{0x0025,42,39,false},{0x0015,43,40,false},{0x0009,44,41,false},
	{0x0005,45,42,false},{0x0001,45,43,false},{0x5601,46,46,false}
};

Bool Media::JBIG2ArithmeticDecoder::ByteIn()
{
	if (previousByte == 0xff)
	{
		if (pos >= dataSize)
		{
			ct = 8;
			return true;
		}
		UInt8 next = data[pos];
		if (next > 0x8f)
		{
			ct = 8;
		}
		else
		{
			c += 0xfe00 - ((UInt32)next << 9);
			previousByte = next;
			pos++;
			ct = 7;
		}
	}
	else
	{
		if (pos >= dataSize)
		{
			c += 0xff00;
			previousByte = 0xff;
			ct = 8;
			return true;
		}
		UInt8 next = data[pos++];
		c += 0xff00 - ((UInt32)next << 8);
		previousByte = next;
		ct = 8;
	}
	return true;
}

Media::JBIG2ArithmeticDecoder::JBIG2ArithmeticDecoder(UnsafeArray<const UInt8> data, UIntOS dataSize) : data(data), dataSize(dataSize), pos(1), a(0x8000), c(0), ct(0), previousByte(dataSize == 0 ? 0 : data[0]), contexts(65536 + 8 * 512 + 32768)
{
	contexts.Clear(0, contexts.GetCount());
	if (dataSize > 0)
		c = ((UInt32)(UInt8)~data[0]) << 16;
	if (ByteIn())
	{
		c <<= 7;
		ct -= 7;
	}
}

Int32 Media::JBIG2ArithmeticDecoder::Decode(UIntOS cx)
{
	if (cx >= contexts.GetCount() || dataSize == 0)
		return -1;
	UInt8 &state = contexts[cx];
	const JBIG2Qe &q = jbig2Qe[state & 0x7f];
	a -= q.qe;
	UInt32 chigh = c >> 16;
	UInt32 bit;
	if (chigh < a)
	{
		if ((a & 0x8000) == 0)
		{
			if (a < q.qe)
			{
				bit = 1U - (state >> 7);
				state = (UInt8)((state & 0x80) | q.nlps);
				if (q.switchMPS)
					state ^= 0x80;
			}
			else
			{
				bit = state >> 7;
				state = (UInt8)((state & 0x80) | q.nmps);
			}
			do
			{
				if (ct == 0 && !ByteIn())
					return -1;
				a <<= 1;
				c <<= 1;
				ct--;
			} while ((a & 0x8000) == 0);
			return (Int32)bit;
		}
		return (Int32)(state >> 7);
	}
	c -= a << 16;
	if (a < q.qe)
	{
		a = q.qe;
		bit = state >> 7;
		state = (UInt8)((state & 0x80) | q.nmps);
	}
	else
	{
		a = q.qe;
		bit = 1U - (state >> 7);
		state = (UInt8)((state & 0x80) | q.nlps);
		if (q.switchMPS)
			state ^= 0x80;
	}
	do
	{
		if (ct == 0 && !ByteIn())
			return -1;
		a <<= 1;
		c <<= 1;
		ct--;
	} while ((a & 0x8000) == 0);
	return (Int32)bit;
}

Bool Media::JBIG2ArithmeticDecoder::DecodeInteger(UIntOS contextBase, Int32 &value, Bool &outOfBounds)
{
	outOfBounds = false;
	contextBase += 65536;
	UIntOS prev = 1;
	Int32 sign = Decode(contextBase + prev);
	if (sign < 0)
	{
		printf("JBIG2 integer decode failed at sign context %u pos=%u\r\n", (UInt32)(contextBase + prev), (UInt32)pos);
		return false;
	}
	prev = (prev << 1) | (UIntOS)sign;
	Int32 bit = Decode(contextBase + prev);
	if (bit < 0)
	{
		printf("JBIG2 integer decode failed at prefix context %u pos=%u\r\n", (UInt32)(contextBase + prev), (UInt32)pos);
		return false;
	}
	prev = (prev << 1) | (UIntOS)bit;
	Int32 nTail;
	Int32 offset;
	if (!bit)
	{
		nTail = 2;
		offset = 0;
	}
	else
	{
		bit = Decode(contextBase + prev);
		if (bit < 0)
		{
			printf("JBIG2 integer decode failed at branch context %u pos=%u\r\n", (UInt32)(contextBase + prev), (UInt32)pos);
			return false;
		}
		prev = (prev << 1) | (UIntOS)bit;
		if (!bit)
		{
			nTail = 4;
			offset = 4;
		}
		else
		{
			bit = Decode(contextBase + prev);
			if (bit < 0)
			{
				printf("JBIG2 integer decode failed at branch2 context %u pos=%u\r\n", (UInt32)(contextBase + prev), (UInt32)pos);
				return false;
			}
			prev = (prev << 1) | (UIntOS)bit;
			if (!bit)
			{
				nTail = 6;
				offset = 20;
			}
			else
			{
				bit = Decode(contextBase + prev);
				if (bit < 0)
				{
					printf("JBIG2 integer decode failed at branch3 context %u pos=%u\r\n", (UInt32)(contextBase + prev), (UInt32)pos);
					return false;
				}
				prev = (prev << 1) | (UIntOS)bit;
				if (!bit)
				{
					nTail = 8;
					offset = 84;
				}
				else
				{
					bit = Decode(contextBase + prev);
					if (bit < 0)
					{
						printf("JBIG2 integer decode failed at branch4 context %u pos=%u\r\n", (UInt32)(contextBase + prev), (UInt32)pos);
						return false;
					}
					prev = (prev << 1) | (UIntOS)bit;
					if (bit)
					{
						nTail = 32;
						offset = 4436;
					}
					else
					{
						nTail = 12;
						offset = 340;
					}
				}
			}
		}
	}
	UInt32 v = 0;
	Int32 i = 0;
	while (i < nTail)
	{
		bit = Decode(contextBase + prev);
		if (bit < 0)
		{
			printf("JBIG2 integer decode failed at tail%u context %u pos=%u\r\n", (UInt32)i, (UInt32)(contextBase + prev), (UInt32)pos);
			return false;
		}
		prev = ((prev << 1) & 511) | (prev & 256) | (UIntOS)bit;
		v = (v << 1) | (UInt32)bit;
		i++;
	}
	UInt32 maxValue = 2147483647U - (UInt32)offset;
	Int64 result = (Int64)(v > maxValue ? 2147483647U : v + (UInt32)offset);
	if (sign)
		result = -result;
	value = (Int32)result;
	outOfBounds = sign != 0 && v == 0;
	return true;
}

Bool Media::JBIG2ArithmeticDecoder::DecodeSymbolId(UIntOS codeLength, UIntOS &id)
{
	UIntOS previous = 1;
	UIntOS i = 0;
	id = 0;
	while (i < codeLength)
	{
		Int32 bit = Decode(65536 + 8 * 512 + previous);
		if (bit < 0)
			return false;
		previous = (previous << 1) | (UIntOS)bit;
		id = (id << 1) | (UIntOS)bit;
		i++;
	}
	return true;
}
