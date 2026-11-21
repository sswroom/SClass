#include "Stdafx.h"
#include "Data/ArrayListNative.hpp"
#include "Data/ByteBuffer.h"
#include "Media/CCITTFaxDecoder.h"
#include "Media/JBIG2Decoder.h"

namespace
{
	struct JBIG2Segment
	{
		UInt32 number;
		UInt8 type;
		UInt32 page;
		UnsafeArray<const UInt8> data;
		UIntOS dataSize;

		JBIG2Segment() = default;
		JBIG2Segment(std::nullptr_t)
		{
			number = 0;
			type = 0;
			page = 0;
			data = &type;
			dataSize = 0;
		}

		Bool operator==(const JBIG2Segment &other) const
		{
			return number == other.number && type == other.type && page == other.page && Text::StrEqualsC(data, dataSize, other.data, other.dataSize);
		}
	};

	struct JBIG2Qe
	{
		UInt16 qe;
		UInt8 nmps;
		UInt8 nlps;
		Bool switchMPS;
	};

	static const JBIG2Qe jbig2Qe[] =
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

	class JBIG2ArithmeticDecoder
	{
	private:
		UnsafeArray<const UInt8> data;
		UIntOS dataSize;
		UIntOS pos;
		UInt32 a;
		UInt32 c;
		UInt32 ct;
		UInt8 previousByte;
		Data::ByteBuffer contexts;

		Bool ByteIn()
		{
			if (previousByte == 0xff)
			{
				if (pos >= dataSize)
					return false;
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
					return false;
				UInt8 next = data[pos++];
				c += 0xff00 - ((UInt32)next << 8);
				previousByte = next;
				ct = 8;
			}
			return true;
		}

	public:
		JBIG2ArithmeticDecoder(UnsafeArray<const UInt8> data, UIntOS dataSize) : data(data), dataSize(dataSize), pos(1), a(0x8000), c(0), ct(0), previousByte(dataSize == 0 ? 0 : data[0]), contexts(65536 + 4 * 512)
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

		Int32 Decode(UIntOS cx)
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

		Bool DecodeInteger(UIntOS contextBase, Int32 &value)
		{
			contextBase += 65536;
			UIntOS prev = 1;
			Int32 sign = Decode(contextBase + prev);
			if (sign < 0)
				return false;
			prev = (prev << 1) | (UIntOS)sign;
			Int32 bit = Decode(contextBase + prev);
			if (bit < 0)
				return false;
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
					return false;
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
						return false;
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
							return false;
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
								return false;
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
					return false;
				prev = ((prev << 1) & 511) | (prev & 256) | (UIntOS)bit;
				v = (v << 1) | (UInt32)bit;
				i++;
			}
			Int64 result = (Int64)v + offset;
			if (sign)
				result = -result;
			if (result < -2147483647LL - 1 || result > 2147483647LL)
				return false;
			value = (Int32)result;
			return true;
		}
	};

	static Bool IsJBIG2File(UnsafeArray<const UInt8> data, UIntOS dataSize)
	{
		static const UInt8 signature[] = {0x97, 0x4a, 0x42, 0x32, 0x0d, 0x0a, 0x1a, 0x0a};
		return dataSize >= sizeof(signature) && memcmp(data.Ptr(), signature, sizeof(signature)) == 0;
	}

	static Bool ParseSegments(Data::ByteArrayR dataBuff, Data::ArrayListNative<JBIG2Segment> &segments)
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

	static Bool DecodeGenericBitmap(JBIG2ArithmeticDecoder &arith, UIntOS width, UIntOS height, const Int8 adaptive[8], Data::ByteBuffer &bitmap)
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

	static Bool DecodeSingleSymbolDictionary(const JBIG2Segment &seg, Data::ByteBuffer &bitmap, UIntOS &bitmapWidth, UIntOS &bitmapHeight)
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
		if ((flags & 3) != 0 || numExported != 1 || numNew != 1)
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
		Int32 heightDelta;
		Int32 widthDelta;
		if (!arith.DecodeInteger(0, heightDelta))
		{
			printf("JBIG2Decoder: failed to decode symbol height\r\n");
			return false;
		}
		if (heightDelta <= 0 || !arith.DecodeInteger(512, widthDelta) || widthDelta <= 0)
		{
			printf("JBIG2Decoder: invalid symbol dimensions h=%d w=%d\r\n", heightDelta, widthDelta);
			return false;
		}
		bitmapHeight = (UIntOS)heightDelta;
		bitmapWidth = (UIntOS)widthDelta;
		Int8 adaptive[8];
		UIntOS i = 0;
		while (i < 8)
		{
			adaptive[i] = (Int8)seg.data[2 + i];
			i++;
		}
		if (!DecodeGenericBitmap(arith, bitmapWidth, bitmapHeight, adaptive, bitmap))
		{
			printf("JBIG2Decoder: generic bitmap entropy failure\r\n");
			return false;
		}
		return true;
	}

	static void CombineRegion(Data::ByteBuffer &page, UIntOS pageWidth, UIntOS pageHeight, const Data::ByteBuffer &region, UIntOS regionWidth, UIntOS regionHeight, Int32 regionX, Int32 regionY, UInt8 op)
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

	static Optional<Media::StaticImage> DecodeJBIG2Image(Data::ByteArrayR dataBuff, UIntOS width, UIntOS height, Data::ByteArrayR globalData)
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
		Data::ByteBuffer symbolBitmap;
		UIntOS symbolWidth = 0;
		UIntOS symbolHeight = 0;
		Bool symbolDecoded = false;
		Bool pageFound = false;
		UIntOS i = 0;
		while (i < segments.GetCount())
		{
			JBIG2Segment &seg = segments.GetItemNoCheckRef(i);
			UnsafeArray<const UInt8> segData = seg.data;
			if (seg.type == 0)
			{
				if (symbolDecoded || !DecodeSingleSymbolDictionary(seg, symbolBitmap, symbolWidth, symbolHeight))
				{
					printf("JBIG2Decoder: unsupported symbol dictionary\r\n");
					goto error;
				}
				symbolDecoded = true;
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
				if (symbolDecoded && symbolWidth == pageWidth && symbolHeight == pageHeight)
				{
					page.ChangeSizeAndIgnoreData(symbolBitmap.GetSize());
					page.CopyFrom(symbolBitmap);
				}
				else
				{
					page.ChangeSizeAndIgnoreData(pageWidth * pageHeight);
					page.Fill(defaultPixel);
				}
				pageFound = true;
			}
			else if (seg.type == 6)
			{
				if (!symbolDecoded || symbolWidth != pageWidth || symbolHeight != pageHeight)
				{
					printf("JBIG2Decoder: text region composition is not supported\r\n");
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
