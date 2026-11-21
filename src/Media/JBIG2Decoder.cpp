#include "Stdafx.h"
#include "Data/ArrayListNative.hpp"
#include "Data/ByteBuffer.h"
#include "Media/CCITTFaxDecoder.h"
#include "Media/JBIG2Decoder.h"

Bool Media::JBIG2Decoder::IsJBIG2File(UnsafeArray<const UInt8> data, UIntOS dataSize)
{
	static const UInt8 signature[] = {0x97, 0x4a, 0x42, 0x32, 0x0d, 0x0a, 0x1a, 0x0a};
	return dataSize >= sizeof(signature) && memcmp(data.Ptr(), signature, sizeof(signature)) == 0;
}

Bool Media::JBIG2Decoder::ParseSegments(Data::ByteArrayR dataBuff, Data::ArrayListNative<JBIG2Segment> &segments)
{
	UnsafeArray<const UInt8> data = dataBuff.Arr();
	UIntOS dataSize = dataBuff.GetSize();
	UIntOS pos = 0;
	if (IsJBIG2File(data, dataSize))
	{
		if (dataSize < 9)
			return false;
		UInt8 fileFlags = data[8];
		pos = 9;
		if ((fileFlags & 2) == 0)
		{
			if (pos + 4 > dataSize)
				return false;
			pos += 4;
		}
		if ((fileFlags & 1) == 0)
			return false;
	}

	while (pos < dataSize)
	{
		if (dataSize - pos < 6)
			return false;
		UInt32 segNum = ReadMUInt32(&data[pos]);
		pos += 4;
		UInt8 flags = data[pos++];
		UInt8 refFlags = data[pos++];
		UInt32 referredCount = refFlags >> 5;
		if (referredCount == 7)
		{
			if (dataSize - pos < 4)
				return false;
			referredCount = ReadMUInt32(&data[pos]) >> 3;
			pos += 4;
			if (referredCount > 1000000)
				return false;
			UIntOS retentionSize = ((UIntOS)referredCount + 1 + 7) >> 3;
			if (retentionSize > dataSize - pos)
				return false;
			pos += retentionSize;
		}
		else
		{
			refFlags = (UInt8)(refFlags & 0x1f);
		}

		UIntOS refNumSize = segNum <= 256 ? 1 : segNum <= 65536 ? 2 : 4;
		UIntOS refsSize = (UIntOS)referredCount * refNumSize;
		if (refsSize > dataSize - pos)
			return false;
		pos += refsSize;

		UInt32 pageNum;
		if ((flags & 0x40) != 0)
		{
			if (dataSize - pos < 4)
				return false;
			pageNum = ReadMUInt32(&data[pos]);
			pos += 4;
		}
		else
		{
			if (pos >= dataSize)
				return false;
			pageNum = data[pos++];
		}
		if (dataSize - pos < 4)
			return false;
		UInt32 segDataSize = ReadMUInt32(&data[pos]);
		pos += 4;
		if (segDataSize == 0xffffffffU)
			return false;
		if ((UIntOS)segDataSize > dataSize - pos)
			return false;
		JBIG2Segment seg;
		seg.number = segNum;
		seg.type = (UInt8)(flags & 0x3f);
		seg.page = pageNum;
		seg.data = data + pos;
		seg.dataSize = segDataSize;
		segments.Add(seg);
		pos += segDataSize;
		if (pos > dataSize)
			return false;
	}
	return true;
}

Bool Media::JBIG2Decoder::DecodeGenericBitmap(JBIG2ArithmeticDecoder &arith, UIntOS width, UIntOS height, const Int8 adaptive[8], Data::ByteBuffer &bitmap)
{
	if (width == 0 || height == 0 || width > INVALID_INDEX / height || width * height > 268435456)
	{
		printf("JBIG2Decoder: invalid generic bitmap dimensions %u x %u\r\n", (UInt32)width, (UInt32)height);
		return false;
	}
	bitmap.ChangeSizeAndIgnoreData(width * height);
	bitmap.Clear(0, bitmap.GetSize());
	Int32 xOffsets[16] = {-1,-2,-3,-4,0,2,1,0,-1,-2,0,0,1,0,-1,0};
	Int32 yOffsets[16] = {0,0,0,0,0,-1,-1,-1,-1,-1,0,0,-2,-2,-2,0};
	xOffsets[4] = adaptive[0];
	yOffsets[4] = adaptive[1];
	xOffsets[10] = adaptive[2];
	yOffsets[10] = adaptive[3];
	xOffsets[11] = adaptive[4];
	yOffsets[11] = adaptive[5];
	xOffsets[15] = adaptive[6];
	yOffsets[15] = adaptive[7];
	UIntOS y = 0;
	while (y < height)
	{
		UIntOS x = 0;
		while (x < width)
		{
			UIntOS context = 0;
			UIntOS bitIndex = 0;
			while (bitIndex < 16)
			{
				IntOS px = (IntOS)x + xOffsets[bitIndex];
				IntOS py = (IntOS)y + yOffsets[bitIndex];
				if (px >= 0 && py >= 0 && px < (IntOS)width && py < (IntOS)height)
					context |= (UIntOS)bitmap[(UIntOS)py * width + (UIntOS)px] << bitIndex;
				bitIndex++;
			}
			Int32 bit = arith.Decode(context);
			if (bit < 0)
			{
				printf("JBIG2Decoder: arithmetic data ended at pixel %u,%u\r\n", (UInt32)x, (UInt32)y);
				return false;
			}
			bitmap[y * width + x] = (UInt8)bit;
			x++;
		}
		y++;
	}
	return true;
}

Bool Media::JBIG2Decoder::DecodeSymbolDictionary(const JBIG2Segment &seg, Data::ByteBuffer &glyphData, Data::ArrayListNative<JBIG2Glyph> &glyphs)
{
	if (seg.dataSize < 18)
	{
		printf("JBIG2Decoder: truncated symbol dictionary header\r\n");
		return false;
	}
	UInt16 flags = (UInt16)(((UInt16)seg.data[0] << 8) | seg.data[1]);
	UInt8 templateId = (UInt8)((flags >> 2) & 3);
	UIntOS adaptSize = templateId == 0 ? 8 : 2;
	if (templateId != 0)
	{
		printf("JBIG2Decoder: only symbol dictionary template 0 is supported\r\n");
		return false;
	}
	UIntOS countsOfst = 2 + adaptSize;
	UIntOS numExported = ReadMUInt32(&seg.data[countsOfst]);
	UIntOS numNew = ReadMUInt32(&seg.data[countsOfst + 4]);
	if ((flags & 3) != 0 || numExported != numNew || numNew == 0 || numNew > 32768)
	{
		printf("JBIG2Decoder: symbol dictionary mode is unsupported\r\n");
		return false;
	}
	UIntOS arithmeticOfst = countsOfst + 8;
	if (arithmeticOfst >= seg.dataSize)
	{
		printf("JBIG2Decoder: truncated symbol dictionary\r\n");
		return false;
	}
	JBIG2ArithmeticDecoder arith(seg.data + arithmeticOfst, seg.dataSize - arithmeticOfst);
	Int8 adaptive[8];
	UIntOS i = 0;
	while (i < 8)
	{
		adaptive[i] = (Int8)seg.data[2 + i];
		i++;
	}
	Data::ByteBuffer bitmap;
	IntOS height = 0;
	UIntOS symbolIndex = 0;
	while (symbolIndex < numNew)
	{
		Int32 heightDelta;
		Bool outOfBounds;
		if (!arith.DecodeInteger(0, heightDelta, outOfBounds) || outOfBounds)
		{
			printf("JBIG2Decoder: failed to decode symbol height class\r\n");
			return false;
		}
		height += heightDelta;
		if (height <= 0 || height > 32768)
		{
			printf("JBIG2Decoder: invalid symbol height %u\r\n", (UInt32)height);
			return false;
		}
		IntOS symbolWidth = 0;
		Bool endOfHeightClass = false;
		while (symbolIndex < numNew && !endOfHeightClass)
		{
			Int32 widthDelta;
			if (!arith.DecodeInteger(512, widthDelta, outOfBounds))
			{
				printf("JBIG2Decoder: failed to decode symbol width\r\n");
				return false;
			}
			if (outOfBounds)
			{
				endOfHeightClass = true;
				continue;
			}
			symbolWidth += widthDelta;
			if (symbolWidth <= 0 || symbolWidth > 32768 ||
				(UIntOS)symbolWidth > INVALID_INDEX / (UIntOS)height ||
				(UIntOS)symbolWidth * (UIntOS)height > 268435456)
			{
				printf("JBIG2Decoder: invalid symbol width %u\r\n", (UInt32)symbolWidth);
				return false;
			}
			if (!DecodeGenericBitmap(arith, (UIntOS)symbolWidth, (UIntOS)height, adaptive, bitmap))
			{
				printf("JBIG2Decoder: generic bitmap entropy failure\r\n");
				return false;
			}
			if (glyphData.GetSize() > INVALID_INDEX - bitmap.GetSize())
				return false;
			JBIG2Glyph glyph;
			glyph.width = (UIntOS)symbolWidth;
			glyph.height = (UIntOS)height;
			glyph.offset = glyphData.GetSize();
			glyphData.ChangeSizeAndKeep(glyphData.GetSize() + bitmap.GetSize());
			glyphData.CopyFrom(glyph.offset, bitmap.AsByteArray());
			glyphs.Add(glyph);
			symbolIndex++;
		}
	}
	return glyphs.GetCount() == numNew;
}

void Media::JBIG2Decoder::CombineGlyph(Data::ByteBuffer &region, UIntOS regionWidth, UIntOS regionHeight, const Data::ByteBuffer &glyphData, const JBIG2Glyph &glyph, IntOS x, IntOS y, UInt8 op)
{
	IntOS gy = 0;
	while (gy < (IntOS)glyph.height)
	{
		IntOS dy = y + gy;
		if (dy >= 0 && dy < (IntOS)regionHeight)
		{
			IntOS gx = 0;
			while (gx < (IntOS)glyph.width)
			{
				IntOS dx = x + gx;
				if (dx >= 0 && dx < (IntOS)regionWidth)
				{
					UIntOS destIndex = (UIntOS)dy * regionWidth + (UIntOS)dx;
					UInt8 src = glyphData[glyph.offset + (UIntOS)gy * glyph.width + (UIntOS)gx];
					if (op == 0)
						region[destIndex] |= src;
					else if (op == 1)
						region[destIndex] &= src;
					else if (op == 2)
						region[destIndex] ^= src;
					else if (op == 3)
						region[destIndex] = (UInt8)~(region[destIndex] ^ src);
				}
				gx++;
			}
		}
		gy++;
	}
}

Bool Media::JBIG2Decoder::DecodeTextRegion(const JBIG2Segment &seg, Data::ByteBuffer &page, UIntOS pageWidth, UIntOS pageHeight,
	const Data::ByteBuffer &glyphData, const Data::ArrayListNative<JBIG2Glyph> &glyphs)
{
	if (seg.dataSize < 23 || glyphs.GetCount() == 0)
		return false;
	UnsafeArray<const UInt8> data = seg.data;
	UIntOS width = ReadMUInt32(&data[0]);
	UIntOS height = ReadMUInt32(&data[4]);
	Int32 regionX = ReadMInt32(&data[8]);
	Int32 regionY = ReadMInt32(&data[12]);
	UInt8 regionOp = (UInt8)(data[16] & 7);
	UInt16 flags = (UInt16)(((UInt16)data[17] << 8) | data[18]);
	UInt32 numInstances = ReadMUInt32(&data[19]);
	if (width == 0 || height == 0 || width > INVALID_INDEX / height || width * height > 268435456 ||
		(flags & 0x000f) != 0 || ((flags >> 4) & 7) > 1 || (flags & 0x0080) != 0 ||
		(flags & 0x8000) != 0 || numInstances > 1000000)
	{
		printf("JBIG2Decoder: unsupported text region mode\r\n");
		return false;
	}
	Int32 strips = 1 << ((flags >> 2) & 3);
	if (strips != 1)
	{
		printf("JBIG2Decoder: multiple text region strips are not supported\r\n");
		return false;
	}
	UIntOS codeLength = 0;
	while (((UIntOS)1 << codeLength) < glyphs.GetCount())
		codeLength++;
	JBIG2ArithmeticDecoder arith(data + 23, seg.dataSize - 23);
	Data::ByteBuffer region(width * height);
	region.Fill((UInt8)((flags >> 8) & 1));
	Int32 stripValue;
	Bool outOfBounds;
	if (!arith.DecodeInteger(0, stripValue, outOfBounds) || outOfBounds)
	{
		printf("JBIG2Decoder: failed initial text strip decode\r\n");
		return false;
	}
	IntOS stripT = -strips * stripValue;
	IntOS firstS = 0;
	IntOS currentS = 0;
	UInt32 instance = 0;
	while (instance < numInstances)
	{
		Int32 deltaT;
		if (!arith.DecodeInteger(0, deltaT, outOfBounds) || outOfBounds)
		{
			printf("JBIG2Decoder: failed text strip position decode\r\n");
			return false;
		}
		stripT += strips * deltaT;
		Bool firstSymbol = true;
		while (instance < numInstances)
		{
			if (firstSymbol)
			{
				Int32 deltaFirstS;
				if (!arith.DecodeInteger(512, deltaFirstS, outOfBounds) || outOfBounds)
				{
					printf("JBIG2Decoder: failed first text symbol position decode\r\n");
					return false;
				}
				firstS += deltaFirstS;
				currentS = firstS;
				firstSymbol = false;
			}
			else
			{
				Int32 deltaS;
				if (!arith.DecodeInteger(1024, deltaS, outOfBounds))
				{
					printf("JBIG2Decoder: failed text symbol position decode\r\n");
					return false;
				}
				if (outOfBounds)
					break;
				currentS += deltaS + ((flags & 0x7c00) >> 10);
				if (((flags & 0x7c00) >> 10) & 0x10)
					currentS -= 0x20;
			}
			UIntOS glyphIndex;
			if (!arith.DecodeSymbolId(codeLength, glyphIndex) || glyphIndex >= glyphs.GetCount())
			{
				printf("JBIG2Decoder: invalid text symbol ID at instance %u\r\n", instance);
				return false;
			}
			const JBIG2Glyph &glyph = glyphs.GetItemNoCheckRef(glyphIndex);
			CombineGlyph(region, width, height, glyphData, glyph, currentS, stripT, (UInt8)((flags >> 9) & 3));
			currentS += (IntOS)glyph.width - 1;
			instance++;
		}
	}
	CombineRegion(page, pageWidth, pageHeight, region, width, height, regionX, regionY, regionOp);
	return true;
}

void Media::JBIG2Decoder::CombineRegion(Data::ByteBuffer &page, UIntOS pageWidth, UIntOS pageHeight, const Data::ByteBuffer &region, UIntOS regionWidth, UIntOS regionHeight, Int32 regionX, Int32 regionY, UInt8 op)
{
	IntOS y = 0;
	while (y < (IntOS)regionHeight)
	{
		IntOS destY = (IntOS)regionY + y;
		if (destY >= 0 && destY < (IntOS)pageHeight)
		{
			IntOS x = 0;
			while (x < (IntOS)regionWidth)
			{
				IntOS destX = (IntOS)regionX + x;
				if (destX >= 0 && destX < (IntOS)pageWidth)
				{
					UIntOS destIndex = (UIntOS)destY * pageWidth + (UIntOS)destX;
					UInt8 src = region[(UIntOS)y * regionWidth + (UIntOS)x];
					if (op == 0)
						page[destIndex] |= src;
					else if (op == 1)
						page[destIndex] &= src;
					else if (op == 2)
						page[destIndex] ^= src;
					else if (op == 3)
						page[destIndex] = (UInt8)~(page[destIndex] ^ src);
					else if (op == 4)
						page[destIndex] = src;
					else
						page[destIndex] |= src;
				}
				x++;
			}
		}
		y++;
	}
}

Optional<Media::StaticImage> Media::JBIG2Decoder::DecodeJBIG2Image(Data::ByteArrayR dataBuff, UIntOS width, UIntOS height, Data::ByteArrayR globalData)
{
	Data::ArrayListNative<JBIG2Segment> segments;
	if (globalData.GetSize() > 0 && !ParseSegments(globalData, segments))
		return nullptr;
	if (!ParseSegments(dataBuff, segments))
	{
		printf("JBIG2Decoder: invalid segment stream\r\n");
		return nullptr;
	}

	UIntOS pageWidth = 0;
	UIntOS pageHeight = 0;
	UInt8 defaultPixel = 0;
	Data::ByteBuffer page;
	Data::ByteBuffer glyphData;
	Data::ArrayListNative<JBIG2Glyph> glyphs;
	Bool pageFound = false;
	UIntOS i = 0;
	while (i < segments.GetCount())
	{
		JBIG2Segment &seg = segments.GetItemNoCheckRef(i);
		UnsafeArray<const UInt8> segData = seg.data;
		if (seg.type == 0)
		{
			if (glyphs.GetCount() != 0 || !DecodeSymbolDictionary(seg, glyphData, glyphs))
			{
				printf("JBIG2Decoder: unsupported symbol dictionary\r\n");
				goto error;
			}
		}
		else if (seg.type == 48)
		{
			if (seg.dataSize < 19)
				goto error;
			pageWidth = ReadMUInt32(&segData[0]);
			pageHeight = ReadMUInt32(&segData[4]);
			defaultPixel = (UInt8)((segData[16] >> 2) & 1);
			if (pageWidth == 0 || pageHeight == 0 || pageWidth > INVALID_INDEX / pageHeight ||
				(width != 0 && width != pageWidth) || (height != 0 && height != pageHeight))
				goto error;
			if (pageWidth * pageHeight > 268435456)
				goto error;
			page.ChangeSizeAndIgnoreData(pageWidth * pageHeight);
			page.Fill(defaultPixel);
			pageFound = true;
		}
		else if (seg.type == 6)
		{
			if (!pageFound || glyphs.GetCount() == 0 ||
				!DecodeTextRegion(seg, page, pageWidth, pageHeight, glyphData, glyphs))
			{
				printf("JBIG2Decoder: text region decoding failed\r\n");
				goto error;
			}
		}
		else if (seg.type == 38 || seg.type == 39)
		{
			if (!pageFound || seg.dataSize < 18)
				goto error;
			UIntOS regionWidth = ReadMUInt32(&segData[0]);
			UIntOS regionHeight = ReadMUInt32(&segData[4]);
			Int32 regionX = ReadMInt32(&segData[8]);
			Int32 regionY = ReadMInt32(&segData[12]);
			UInt8 regionOp = (UInt8)(segData[16] & 7);
			UInt8 regionFlags = segData[17];
			if (regionWidth == 0 || regionHeight == 0 || regionWidth > INVALID_INDEX / regionHeight ||
				regionWidth * regionHeight > 268435456)
				goto error;
			if ((regionFlags & 1) == 0)
			{
				printf("JBIG2Decoder: arithmetic-coded generic regions are not supported yet\r\n");
				goto error;
			}
			Media::CCITTFaxDecoder faxDecoder;
			NN<Media::StaticImage> regionImage;
			if (!faxDecoder.DecodeImage(Data::ByteArrayR(segData + 18, seg.dataSize - 18), regionWidth, regionHeight, -1).SetTo(regionImage))
			{
				printf("JBIG2Decoder: invalid MMR generic region\r\n");
				goto error;
			}
			Data::ByteBuffer region(regionWidth * regionHeight);
			UIntOS y = 0;
			UIntOS regionBpl = regionImage->GetDataBpl();
			while (y < regionHeight)
			{
				UIntOS x = 0;
				while (x < regionWidth)
				{
					region[y * regionWidth + x] = regionImage->data[y * regionBpl + x] == 0 ? 1 : 0;
					x++;
				}
				y++;
			}
			regionImage.Delete();
			CombineRegion(page, pageWidth, pageHeight, region, regionWidth, regionHeight, regionX, regionY, regionOp);
		}
		else if (seg.type != 49 && seg.type != 50 && seg.type != 51)
		{
			printf("JBIG2Decoder: segment type %u is not supported yet\r\n", (UInt32)seg.type);
			goto error;
		}
		i++;
	}

	if (!pageFound)
		goto error;
	{
		Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
		NN<Media::StaticImage> img;
		NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UIntOS>(pageWidth, pageHeight), 0, 8, Media::PixelFormat::PF_PAL_W8, pageWidth * pageHeight, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AlphaType::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
		img->InitGrayPal();
		UnsafeArray<UInt8> dest = img->data;
		UIntOS y = 0;
		while (y < pageHeight)
		{
			UIntOS x = 0;
			while (x < pageWidth)
			{
				dest[y * img->GetDataBpl() + x] = page[y * pageWidth + x] != 0 ? 0 : 255;
				x++;
			}
			y++;
		}
		return img;
	}

error:
	printf("JBIG2Decoder: unsupported or invalid segment at index %u\r\n", (UInt32)i);
	return nullptr;
}

Media::JBIG2Decoder::JBIG2Decoder()
{
}

Media::JBIG2Decoder::~JBIG2Decoder()
{
}

Optional<Media::StaticImage> Media::JBIG2Decoder::DecodeImage(Data::ByteArrayR dataBuff, UIntOS width, UIntOS height) const
{
	return DecodeJBIG2Image(dataBuff, width, height, Data::ByteArrayR(UnsafeArray<const UInt8>::FromPtrNoCheck(nullptr), 0));
}

Optional<Media::StaticImage> Media::JBIG2Decoder::DecodeImage(Data::ByteArrayR dataBuff, UIntOS width, UIntOS height, Data::ByteArrayR globalData) const
{
	return DecodeJBIG2Image(dataBuff, width, height, globalData);
}
