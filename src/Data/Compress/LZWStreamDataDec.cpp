#include "Stdafx.h"
#include "Data/Compress/LZWStreamDataDec.h"
#include "IO/BitReaderMSB.h"
#include "IO/StreamDataStream.h"

Bool Data::Compress::LZWStreamDataDec::Decode(NN<IO::StreamData> data, NN<IO::Stream> output, UIntOS earlyChange)
{
	IO::StreamDataStream stream(data);
	return Decode(stream, output, earlyChange);
}

Bool Data::Compress::LZWStreamDataDec::Decode(NN<IO::Stream> stream, NN<IO::Stream> output, UIntOS earlyChange)
{
	IO::BitReaderMSB reader(stream);
	UInt16 prefixes[4096];
	UInt8 suffixes[4096];
	UInt8 stack[4097];
	UIntOS nextCode = 258;
	UIntOS codeSize = 9;
	UInt32 previous = 0;
	Bool hasPrevious = false;
	UInt32 code;
	while (reader.ReadBits(code, codeSize))
	{
		if (code == 256)
		{
			nextCode = 258;
			codeSize = 9;
			hasPrevious = false;
			continue;
		}
		if (code == 257)
			return true;
		if (code > nextCode || (code == nextCode && (!hasPrevious || nextCode == 4096)) ||
			(!hasPrevious && code >= 256))
			return false;

		UInt32 current = code == nextCode ? previous : code;
		UIntOS count = 0;
		while (current >= 258)
		{
			if (current >= nextCode || count >= 4095)
				return false;
			stack[count++] = suffixes[current];
			current = prefixes[current];
		}
		if (current >= 256)
			return false;
		UInt8 first = (UInt8)current;
		stack[count++] = first;
		UIntOS i = 0;
		while (i < count / 2)
		{
			UInt8 tmp = stack[i];
			stack[i] = stack[count - 1 - i];
			stack[count - 1 - i] = tmp;
			i++;
		}
		if (code == nextCode)
			stack[count++] = first;
		if (output->Write(Data::ByteArrayR(stack, count)) != count)
			return false;
		if (hasPrevious && nextCode < 4096)
		{
			prefixes[nextCode] = (UInt16)previous;
			suffixes[nextCode] = first;
			nextCode++;
			if (codeSize < 12 && nextCode + earlyChange == ((UIntOS)1 << codeSize))
				codeSize++;
		}
		previous = code;
		hasPrevious = true;
	}
	return false;
}
