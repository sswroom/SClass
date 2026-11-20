#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListNative.hpp"
#include "Media/CCITTFaxDecoder.h"

struct IO::FaxCode
{
	const Char *bits;
	UIntOS run;
};

Bool Media::CCITTFaxDecoder::ReadRun(IO::FaxBitReader &reader, Bool black, UIntOS width, UIntOS &run)
{
	return reader.ReadRun(black, width, run);
}

void Media::CCITTFaxDecoder::FillRun(UnsafeArray<UInt8> row, UIntOS start, UIntOS end, Bool black)
{
	if (end > start)
		memset(&row[start], black ? 0 : 255, end - start);
}

Bool Media::CCITTFaxDecoder::Decode1DLine(IO::FaxBitReader &reader, UnsafeArray<UInt8> row, UIntOS width)
{
	UIntOS x = 0;
	Bool black = false;
	while (x < width)
	{
		UIntOS run;
		if (!ReadRun(reader, black, width - x, run) || (run == 0 && black))
			return false;
		FillRun(row, x, x + run, black);
		x += run;
		black = !black;
	}
	return true;
}

Bool Media::CCITTFaxDecoder::Read2DMode(IO::FaxBitReader &reader, Int32 &mode)
{
	static const IO::FaxCode modes[] =
	{
		{"1",0},{"001",1},{"0001",2},{"010",3},{"011",4},{"000010",5},{"000011",6},{"0000010",7},{"0000011",8}
	};
	UIntOS codeCnt = sizeof(modes) / sizeof(modes[0]);
	UInt32 bit;
	Char code[8];
	UIntOS codeLen = 0;
	Bool hasPrefix;
	while (codeLen < 7)
	{
		if (!reader.ReadBit(bit))
			return false;
		code[codeLen++] = bit ? '1' : '0';
		hasPrefix = false;
		UIntOS i = 0;
		while (i < codeCnt)
		{
			UIntOS j = 0;
			while (j < codeLen && modes[i].bits[j] != 0 && code[j] == modes[i].bits[j])
				j++;
			if (j == codeLen)
			{
				if (modes[i].bits[j] == 0)
				{
					mode = (Int32)modes[i].run;
					return true;
				}
				hasPrefix = true;
			}
			i++;
		}
		if (!hasPrefix)
			return false;
	}
	return false;
}

void Media::CCITTFaxDecoder::GetTransitions(UnsafeArray<const UInt8> row, UIntOS width, Data::ArrayListNative<UIntOS> &transitions)
{
	Bool black = false;
	UIntOS x = 0;
	if (row[0] == 0)
	{
		transitions.Add(0);
		black = true;
	}
	x = 1;
	while (x < width)
	{
		Bool nextBlack = row[x] == 0;
		if (nextBlack != black)
		{
			transitions.Add(x);
			black = nextBlack;
		}
		x++;
	}
}

Bool Media::CCITTFaxDecoder::Decode2DLine(IO::FaxBitReader &reader, UnsafeArray<UInt8> row, UnsafeArray<const UInt8> refRow, UIntOS width)
{
	Data::ArrayListNative<UIntOS> transitions;
	GetTransitions(refRow, width, transitions);
	memset(row.Ptr(), 255, width);
	UIntOS a0 = 0;
	Bool black = false;
	Bool firstChange = true;
	UIntOS iterations = 0;
	UIntOS maxIterations = width <= (INVALID_INDEX - 16) / 2 ? width * 2 + 16 : INVALID_INDEX;
	while (a0 < width)
	{
		if (++iterations > maxIterations)
			return false;
		Int32 mode;
		if (!Read2DMode(reader, mode))
			return false;
		if (mode == 1)
		{
			UIntOS a1 = a0;
			if (!ReadRun(reader, black, width - a0, a1) || a1 > width - a0)
				return false;
			a1 += a0;
			UIntOS a2;
			if (!ReadRun(reader, !black, width - a1, a2) || a2 > width - a1)
				return false;
			a2 += a1;
			if (a2 == a0)
				return false;
			FillRun(row, a0, a1, black);
			FillRun(row, a1, a2, !black);
			a0 = a2;
		}
		else
		{
			UIntOS refIndex = 0;
			while (refIndex < transitions.GetCount())
			{
				UIntOS pos = transitions.GetItem(refIndex);
				Bool afterBlack = (refIndex & 1) == 0;
				if ((firstChange ? pos >= a0 : pos > a0) && afterBlack != black)
					break;
				refIndex++;
			}
			UIntOS b1 = refIndex < transitions.GetCount() ? transitions.GetItem(refIndex) : width + 1;
			UIntOS b2 = refIndex + 1 < transitions.GetCount() ? transitions.GetItem(refIndex + 1) : width + 1;
			if (mode == 2)
			{
				if (b2 <= a0)
					return false;
				FillRun(row, a0, b2 > width ? width : b2, black);
				a0 = b2;
			}
			else
			{
				Int32 delta = mode == 3 ? -1 : mode == 4 ? 1 : mode == 5 ? -2 : mode == 6 ? 2 : mode == 7 ? -3 : mode == 8 ? 3 : 0;
				IntOS a1Signed = (IntOS)b1 + delta;
				if (a1Signed < (IntOS)a0)
					return false;
				UIntOS a1 = (UIntOS)a1Signed;
				FillRun(row, a0, a1 > width ? width : a1, black);
				a0 = a1;
				black = !black;
			}
		}
		firstChange = false;
	}
	return true;
}

Media::CCITTFaxDecoder::CCITTFaxDecoder()
{
}

Media::CCITTFaxDecoder::~CCITTFaxDecoder()
{
}

Optional<Media::StaticImage> Media::CCITTFaxDecoder::DecodeImage(Data::ByteArrayR dataBuff, UIntOS width, UIntOS height, Int32 k, Bool endOfLine, Bool encodedByteAlign) const
{
	if (width == 0 || height == 0 || width > INVALID_INDEX / height || dataBuff.GetSize() > INVALID_INDEX / 8)
		return nullptr;
	IO::FaxBitReader reader(dataBuff);
	Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
	NN<Media::StaticImage> img;
	NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UIntOS>(width, height), 0, 8, Media::PixelFormat::PF_PAL_W8, width * height, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	img->InitGrayPal();
	UIntOS bpl = img->GetDataBpl();
	UnsafeArray<UInt8> data = img->data;
	memset(data.Ptr(), 255, img->info.byteSize);
	UIntOS y = 0;
	UIntOS group3LineCount = 0;
	while (y < height)
	{
		if (encodedByteAlign)
			reader.AlignByte();
		if (endOfLine && !reader.ReadEOL())
		{
			img.Delete();
			return nullptr;
		}
		Bool oneDimensional = k >= 0;
		if (k > 0)
		{
			UInt32 tag;
			if (!reader.ReadBit(tag))
			{
				img.Delete();
				return nullptr;
			}
			oneDimensional = tag != 0;
			if (!oneDimensional && (group3LineCount >= (UIntOS)k || y == 0))
			{
				img.Delete();
				return nullptr;
			}
		}
		UnsafeArray<UInt8> row = data + y * bpl;
		Bool succ;
		if (oneDimensional)
		{
			succ = Decode1DLine(reader, row, width);
			group3LineCount = 0;
		}
		else
		{
			UnsafeArray<const UInt8> refRow = y == 0 ? UnsafeArray<const UInt8>(data) : UnsafeArray<const UInt8>(data + (y - 1) * bpl);
			succ = Decode2DLine(reader, row, refRow, width);
			group3LineCount++;
		}
		if (!succ)
		{
			img.Delete();
			return nullptr;
		}
		y++;
	}
	return img;
}
