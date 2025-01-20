#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/StreamData.h"
#include "Math/Unit/Distance.h"
#include "Media/FrameInfo.h"
#include "Media/ICCProfile.h"
#include "Media/ImageResizer.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Parser/ParserList.h"
#include "Parser/FileParser/BMPParser.h"

void BMPParser_ReadPal(Media::StaticImage *img, NN<IO::StreamData> fd, UOSInt palStart, Int32 palType, UOSInt colorUsed)
{
	UOSInt maxColor = (UOSInt)1 << img->info.storeBPP;
	if (palType == 0)
	{
		if (colorUsed <= 0 || colorUsed >= maxColor)
		{
			fd->GetRealData(palStart, maxColor * 4, Data::ByteArray(img->pal, maxColor * 4));
		}
		else
		{
			fd->GetRealData(palStart, colorUsed * 4, Data::ByteArray(img->pal, colorUsed * 4));
			MemClear(&img->pal[colorUsed * 4], (maxColor - colorUsed) * 4);
		}
		UOSInt i;
		i = maxColor;
		while (i-- > 0)
		{
			img->pal[(i << 2) + 3] = 0xff;
		}
	}
	else if (palType == 1)
	{
		UInt8 palBuff[768];
		UOSInt i;
		UOSInt j;
		fd->GetRealData(palStart, maxColor * 3, BYTEARR(palBuff));
		i = 0;
		j = 0;
		while (i < maxColor)
		{
			
			img->pal[(i << 2) + 0] = palBuff[j];
			img->pal[(i << 2) + 1] = palBuff[j+1];
			img->pal[(i << 2) + 2] = palBuff[j+2];
			img->pal[(i << 2) + 3] = 0xff;
			i++;
			j += 3;
		}
	}
}

Parser::FileParser::BMPParser::BMPParser()
{
	this->parsers = 0;
}

Parser::FileParser::BMPParser::~BMPParser()
{
}

Int32 Parser::FileParser::BMPParser::GetName()
{
	return *(Int32*)"BMPP";
}

void Parser::FileParser::BMPParser::SetParserList(Optional<Parser::ParserList> parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::BMPParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.bmp"), CSTR("Bitmap File"));
		selector->AddFilter(CSTR("*.dib"), CSTR("DIB File"));
	}
}

IO::ParserType Parser::FileParser::BMPParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::BMPParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt32 imgWidth;
	Int32 imgHeight;
	UInt32 bpp;
	UInt32 endPos;
//	Int32 imgPos;
	UInt32 biCompression = 0;
	UInt32 colorUsed = 0;
//	Int32 colorImportant = 0;
//	Int32 biSizeImage = 0;
	Int32 headerSize;
	Int32 palType = 0;
	Media::PixelFormat pf;
	Double hdpi = 72.0;
	Double vdpi = 72.0;
	Media::AlphaType atype = Media::AT_NO_ALPHA;
	UInt32 rBit = 0;
	UInt32 gBit = 0;
	UInt32 bBit = 0;
	UInt32 aBit = 0;
	OSInt rBitCnt = 0;
	OSInt gBitCnt = 0;
	OSInt bBitCnt = 0;
	OSInt aBitCnt = 0;
	UOSInt srcBpp = 0;
	OSInt bitDefault = false;
	Bool headerValid = true;

	Media::StaticImage *outImg = 0;
	NN<Media::StaticImage> nnimg;

	if (hdr.ReadNI16(0) != *(Int16*)"BM")
	{
		return 0;
	}

	headerSize = ReadInt32(&hdr[14]);
	if (headerSize == 12) //BITMAPCOREHEADER / OS21XBITMAPHEADER
	{
		imgWidth = ReadUInt16(&hdr[18]);
		imgHeight = ReadInt16(&hdr[20]);
		bpp = ReadUInt16(&hdr[24]);
		if (bpp == 16)
		{
			pf = Media::PF_LE_R5G5B5;
		}
		else
		{
			pf = Media::PixelFormatGetDef(0, bpp);
		}
		biCompression = 0;
		endPos = 26;
		palType = 1;
		atype = (bpp == 32)?Media::AT_ALPHA:Media::AT_NO_ALPHA;
	}
	else if (headerSize == 16)
	{
		imgWidth = ReadUInt32(&hdr[18]);
		imgHeight = ReadInt32(&hdr[22]);
		bpp = ReadUInt16(&hdr[28]);
		if (bpp == 16)
		{
			pf = Media::PF_LE_R5G5B5;
		}
		else
		{
			pf = Media::PixelFormatGetDef(0, bpp);
		}
		biCompression = 0;
		endPos = 30;
		palType = 0;
		atype = (bpp == 32)?Media::AT_ALPHA:Media::AT_NO_ALPHA;
	}
	else if (headerSize >= 40)
	{
		imgWidth = ReadUInt32(&hdr[18]);
		imgHeight = ReadInt32(&hdr[22]);
		bpp = ReadUInt16(&hdr[28]);
		biCompression = ReadUInt32(&hdr[30]);
		if (bpp == 16)
		{
			pf = Media::PF_LE_R5G5B5;
		}
		else
		{
			pf = Media::PixelFormatGetDef(0, bpp);
			atype = (bpp == 32)?Media::AT_ALPHA:Media::AT_NO_ALPHA;
		}
		endPos = 14 + ReadUInt32(&hdr[14]);
//		biSizeImage = ReadInt32(&hdr[34]);
		hdpi = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, ReadInt32(&hdr[38]));
		vdpi = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, ReadInt32(&hdr[42]));
		colorUsed = ReadUInt32(&hdr[46]);
//		colorImportant = ReadInt32(&hdr[50]);
		if (hdpi == 0 || vdpi == 0)
		{
			hdpi = 72.0;
			vdpi = 72.0;
		}
		else
		{
			Double par = hdpi / vdpi;
			if (par < 0.1 || par > 10)
			{
				hdpi = 72.0;
				vdpi = 72.0;
			}
		}
		if (biCompression == 6) //BI_ALPHABITFIELDS
		{
			biCompression = 3;
		}
		if (biCompression == 3) //BI_BITFIELDS
		{
			rBit = ReadUInt32(&hdr[54]);
			gBit = ReadUInt32(&hdr[58]);
			bBit = ReadUInt32(&hdr[62]);
			aBit = ReadUInt32(&hdr[66]);
			if ((rBit & gBit) != 0 || (rBit & bBit) != 0 || (rBit & aBit) != 0 || (gBit & bBit) != 0 || (gBit & aBit) != 0 || (bBit & aBit) != 0)
			{
				headerValid = false;
			}
			srcBpp = bpp;
			if (rBit == 0xff0000 && gBit == 0xff00 && bBit == 0xff && aBit == 0xff000000 && bpp == 32)
			{
				bitDefault = true;
			}
			else if (rBit == 0xff0000 && gBit == 0xff00 && bBit == 0xff && aBit == 0 && bpp == 32)
			{
				bitDefault = true;
				atype = Media::AT_NO_ALPHA;
			}
			else if (rBit == 0x3ff && gBit == 0xffc00 && bBit == 0x3ff00000 && aBit == 0xc0000000 && bpp == 32)
			{
				bitDefault = true;
				pf = Media::PF_LE_A2B10G10R10;
			}
			else if (rBit == 0x3ff && gBit == 0xffc00 && bBit == 0x3ff00000 && aBit == 0x00000000 && bpp == 32)
			{
				bitDefault = true;
				pf = Media::PF_LE_A2B10G10R10;
				atype = Media::AT_NO_ALPHA;
			}
			else if (rBit == 0xf800 && gBit == 0x7e0 && bBit == 0x1f && bpp == 16)
			{
				bitDefault = true;
				pf = Media::PF_LE_R5G6B5;
			}
			else if (aBit == 0 && rBit == 0x7c00 && gBit == 0x3e0 && bBit == 0x1f && bpp == 16)
			{
				bitDefault = true;
				pf = Media::PF_LE_R5G5B5;
			}
			else
			{
				UOSInt i = 32;
				UInt32 v = 1;
				while (i-- > 0)
				{
					if (rBit & v)
					{
						rBitCnt++;
					}
					if (gBit & v)
					{
						gBitCnt++;
					}
					if (bBit & v)
					{
						bBitCnt++;
					}
					if (aBit & v)
					{
						aBitCnt++;
					}
					v = v << 1;
				}

				if (rBitCnt <= 5 && gBitCnt <= 6 && bBitCnt <= 5 && aBitCnt == 0)
				{
					bpp = 16;
					pf = Media::PF_LE_R5G6B5;
				}
				else if (rBitCnt <= 8 && gBitCnt <= 8 && bBitCnt <= 8 && aBitCnt <= 8)
				{
					bpp = 32;
					pf = Media::PF_B8G8R8A8;
					if (aBitCnt > 0)
					{
						atype = Media::AT_ALPHA;
					}
				}
				else
				{
					bpp = 64;
					pf = Media::PF_LE_B16G16R16A16;
					if (aBitCnt > 0)
					{
						atype = Media::AT_ALPHA;
					}
				}
			}
		}
	}
	else
	{
		imgWidth = ReadUInt32(&hdr[18]);
		imgHeight = ReadInt32(&hdr[22]);
		bpp = ReadUInt16(&hdr[28]);
		if (bpp == 16)
		{
			pf = Media::PF_LE_R5G5B5;
		}
		else
		{
			pf = Media::PixelFormatGetDef(0, bpp);
			atype = (bpp == 32)?Media::AT_ALPHA:Media::AT_NO_ALPHA;
		}
		endPos = 14 + ReadUInt32(&hdr[14]);
	}
//	imgPos = ReadUInt32(&hdr[10]);

	Bool inv = true;
	if (imgHeight < 0)
	{
		inv = false;
		imgHeight = -imgHeight;
	}
	UInt32 uimgHeight = (UInt32)imgHeight;
	if ((Int32)imgWidth < 0)
	{
		imgWidth = (UInt32)-(Int32)imgWidth;
	}
	if (biCompression == 0 && bpp != 1 && bpp != 2 && bpp != 4 && bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32 && bpp != 48 && bpp != 64)
	{
		UInt64 fileSize = fd->GetDataSize() - endPos;
		UOSInt lineSize = (UOSInt)(fileSize / (UInt32)imgHeight);
		if (lineSize >= imgWidth * 8)
		{
			bpp = 64;
			pf = Media::PF_LE_B16G16R16A16;
		}
		else if (lineSize >= imgWidth * 6)
		{
			bpp = 48;
			pf = Media::PF_LE_B16G16R16;
		}
		else if (lineSize >= imgWidth * 4)
		{
			bpp = 32;
			pf = Media::PF_B8G8R8A8;
		}
		else if (lineSize >= imgWidth * 3)
		{
			bpp = 24;
			pf = Media::PF_B8G8R8;
		}
		else if (lineSize >= imgWidth * 2)
		{
			bpp = 16;
			pf = Media::PF_LE_R5G5B5;
		}
		else if (lineSize >= imgWidth)
		{
			bpp = 8;
			pf = Media::PF_PAL_8;
		}
		else if (lineSize >= imgWidth >> 1)
		{
			bpp = 4;
			pf = Media::PF_PAL_4;
		}
		else if (lineSize >= imgWidth >> 2)
		{
			bpp = 2;
			pf = Media::PF_PAL_2;
		}
		else if (lineSize >= imgWidth >> 3)
		{
			bpp = 1;
			pf = Media::PF_PAL_1;
		}
		else
		{
			bpp = 0;
		}
	}
	if (biCompression == 4 || biCompression == 5) //BI_JPEG / BI_PNG)
	{
		Optional<IO::ParsedObject> pobj = 0;
		NN<Parser::ParserList> parsers;
		if (this->parsers.SetTo(parsers))
		{
			UInt32 currOfst = ReadUInt32(&hdr[10]);
			NN<IO::StreamData> innerFd = fd->GetPartialData(currOfst, fd->GetDataSize() - currOfst);
			pobj = parsers->ParseFile(innerFd);
			innerFd.Delete();
			return pobj;
		}
	}
	if (imgWidth > 0 && imgHeight > 0 && bpp != 0 && imgWidth <= 32768 && imgHeight <= 32768 && headerValid)
	{
		NEW_CLASS(outImg, Media::StaticImage(Math::Size2D<UOSInt>(imgWidth, uimgHeight), 0, bpp, pf, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, atype, Media::YCOFST_C_CENTER_LEFT));
		outImg->info.hdpi = hdpi;
		outImg->info.vdpi = vdpi;
		outImg->info.par2 = hdpi / vdpi;
		if (headerSize >= 124 && ReadInt32(&hdr[70]) == ReadInt32((const UInt8*)"DEBM")) //BITMAPV5HEADER
		{
			UInt32 imgDataSize = ReadUInt32(&hdr[126]);
			UInt32 iccSize = ReadUInt32(&hdr[130]);
			if (iccSize > 10 && imgDataSize + 14 + iccSize <= fd->GetDataSize())
			{
				Data::ByteBuffer iccBuff(iccSize);
				fd->GetRealData(imgDataSize + 14, iccSize, iccBuff);
				if (iccSize == ReadMUInt32(&iccBuff[0]))
				{
					NN<Media::ICCProfile> icc;
					if (Media::ICCProfile::Parse(iccBuff.WithSize(iccSize)).SetTo(icc))
					{
						icc->SetToColorProfile(outImg->info.color);
						icc.Delete();
					}
				}
			}
		}
		else if (headerSize >= 108 && ReadInt32(&hdr[70]) == ReadInt32((const UInt8*)"BGRs"))
		{
			outImg->info.color.SetCommonProfile(Media::ColorProfile::CPT_SRGB);
		}
		else if (headerSize >= 108 && ReadInt32(&hdr[70]) == 0)
		{
			Int32 rx = ReadInt32(&hdr[74]);
			Int32 ry = ReadInt32(&hdr[78]);
			Int32 rz = ReadInt32(&hdr[82]);
			Int32 gx = ReadInt32(&hdr[86]);
			Int32 gy = ReadInt32(&hdr[90]);
			Int32 gz = ReadInt32(&hdr[94]);
			Int32 bx = ReadInt32(&hdr[98]);
			Int32 by = ReadInt32(&hdr[102]);
			Int32 bz = ReadInt32(&hdr[106]);
			Int32 rg = ReadInt32(&hdr[110]);
			Int32 gg = ReadInt32(&hdr[114]);
			Int32 bg = ReadInt32(&hdr[118]);
			Bool valid = true;
			if (rx == 0) valid = false;
			if (ry == 0) valid = false;
			if (rz == 0) valid = false;
			if (gx == 0) valid = false;
			if (gy == 0) valid = false;
			if (gz == 0) valid = false;
			if (bx == 0) valid = false;
			if (by == 0) valid = false;
			if (bz == 0) valid = false;
			if (rg == 0) valid = false;
			if (gg == 0) valid = false;
			if (bg == 0) valid = false;
			if (valid)
			{
				Math::Vector3 xyzVec;
				xyzVec.val[0] = rx / (Double)0x40000000;
				xyzVec.val[1] = ry / (Double)0x40000000;
				xyzVec.val[2] = rz / (Double)0x40000000;
				outImg->info.color.primaries.r = Media::ColorProfile::ColorPrimaries::XYZToxyY(xyzVec).GetXY();

				xyzVec.val[0] = gx / (Double)0x40000000;
				xyzVec.val[1] = gy / (Double)0x40000000;
				xyzVec.val[2] = gz / (Double)0x40000000;
				outImg->info.color.primaries.g = Media::ColorProfile::ColorPrimaries::XYZToxyY(xyzVec).GetXY();

				xyzVec.val[0] = bx / (Double)0x40000000;
				xyzVec.val[1] = by / (Double)0x40000000;
				xyzVec.val[2] = bz / (Double)0x40000000;
				outImg->info.color.primaries.b = Media::ColorProfile::ColorPrimaries::XYZToxyY(xyzVec).GetXY();
				outImg->info.color.primaries.colorType = Media::ColorProfile::CT_CUSTOM;

				outImg->info.color.rtransfer.Set(Media::CS::TRANT_GAMMA, rg / 65536.0);
				outImg->info.color.gtransfer.Set(Media::CS::TRANT_GAMMA, gg / 65536.0);
				outImg->info.color.btransfer.Set(Media::CS::TRANT_GAMMA, bg / 65536.0);
			}
		}
	}
	if (outImg)
	{
		Data::ByteArray pBits = outImg->GetDataArray();
		UInt32 lineW;
		UInt32 lineW2;
		UInt32 currOfst;
		Int32 i;
		if (biCompression == 0 || (biCompression == 3 && bitDefault))
		{
			if (inv)
			{
				switch (bpp)
				{
				case 1:
					BMPParser_ReadPal(outImg, fd, endPos, palType, colorUsed);
					if (imgWidth & 7)
					{
						lineW = (imgWidth + 8 - (imgWidth & 7)) >> 3;
					}
					else
					{
						lineW = imgWidth >> 3;
					}
					lineW2 = lineW;
					currOfst = (lineW * uimgHeight) + ReadUInt32(&hdr[10]);
					while (uimgHeight-- > 0)
					{
						currOfst -= lineW;
						fd->GetRealData(currOfst, lineW2, pBits);
						pBits += lineW2;
					}
					break;
				case 2:
					BMPParser_ReadPal(outImg, fd, endPos, palType, colorUsed);
					if (imgWidth & 3)
					{
						lineW = (imgWidth + 4 - (imgWidth & 3)) >> 2;
					}
					else
					{
						lineW = imgWidth >> 2;
					}
					lineW2 = lineW;
					currOfst = (lineW * uimgHeight) + ReadUInt32(&hdr[10]);
					while (uimgHeight-- > 0)
					{
						currOfst -= lineW;
						fd->GetRealData(currOfst, lineW2, pBits);
						pBits += lineW2;
					}
					break;
				case 4:
					BMPParser_ReadPal(outImg, fd, endPos, palType, colorUsed);
					lineW = (imgWidth >> 1) + (imgWidth & 1);
					lineW2 = lineW;
					if (lineW & 3)
					{
						lineW = lineW + 4 - (lineW & 3);
					}
					currOfst = (lineW * uimgHeight) + ReadUInt32(&hdr[10]);
					while (uimgHeight-- > 0)
					{
						currOfst -= lineW;
						fd->GetRealData(currOfst, lineW2, pBits);
						pBits += lineW2;
					}
					break;
				case 8:
					BMPParser_ReadPal(outImg, fd, endPos, palType, colorUsed);
					lineW = imgWidth;
					if (lineW & 3)
					{
						lineW = lineW + 4 - (lineW & 3);
					}
					currOfst = (lineW * uimgHeight) + ReadUInt32(&hdr[10]);
					while (uimgHeight-- > 0)
					{
						currOfst -= lineW;
						fd->GetRealData(currOfst, imgWidth, pBits);
						pBits += imgWidth;
					}
					break;
				case 16:
					lineW = imgWidth << 1;
					if (lineW & 3)
					{
						lineW = lineW + 4 - (lineW & 3);
					}
					lineW2 = imgWidth << 1;
					currOfst = (lineW * uimgHeight) + ReadUInt32(&hdr[10]);
					while (uimgHeight-- > 0)
					{
						currOfst -= lineW;
						fd->GetRealData(currOfst, lineW2, pBits);
						pBits += lineW2;
					}
					break;
				case 24:
					lineW = imgWidth * 3;
					if (lineW & 3)
					{
						lineW = lineW + 4 - (lineW & 3);
					}
					lineW2 = imgWidth * 3;
					currOfst = (lineW * uimgHeight) + ReadUInt32(&hdr[10]);
					while (uimgHeight-- > 0)
					{
						currOfst -= lineW;
						fd->GetRealData(currOfst, lineW2, pBits);
						pBits += lineW2;
					}
					break;
				case 32:
					currOfst = ReadUInt32(&hdr[10]);
					pBits = pBits + imgWidth * uimgHeight * 4;
					while (uimgHeight-- > 0)
					{
						pBits -= imgWidth << 2;
						fd->GetRealData(currOfst, imgWidth << 2, pBits);
						currOfst += imgWidth << 2;
					}
					break;
				default:
					DEL_CLASS(outImg);
					outImg = 0;
					break;
				};
			}
			else
			{
				switch (bpp)
				{
				case 1:
					BMPParser_ReadPal(outImg, fd, endPos, palType, colorUsed);
					if (imgWidth & 7)
					{
						lineW = (imgWidth + 8 - (imgWidth & 7)) >> 3;
					}
					else
					{
						lineW = imgWidth >> 3;
					}
					lineW2 = lineW;
					currOfst = ReadUInt32(&hdr[10]);
					break;
				case 2:
					BMPParser_ReadPal(outImg, fd, endPos, palType, colorUsed);
					if (imgWidth & 3)
					{
						lineW = (imgWidth + 4 - (imgWidth & 3)) >> 2;
					}
					else
					{
						lineW = imgWidth >> 2;
					}
					lineW2 = lineW;
					currOfst = ReadUInt32(&hdr[10]);
					break;
				case 4:
					BMPParser_ReadPal(outImg, fd, endPos, palType, colorUsed);
					i = 16;
					while (i-- > 0)
					{
						outImg->pal[(i << 2) + 3] = 0xff;
					}
					lineW = (imgWidth >> 1) + (imgWidth & 1);
					lineW2 = lineW;
					if (lineW & 3)
					{
						lineW = lineW + 4 - (lineW & 3);
					}
					currOfst = ReadUInt32(&hdr[10]);
					break;
				case 8:
					BMPParser_ReadPal(outImg, fd, endPos, palType, colorUsed);
					lineW = imgWidth;
					lineW2 = lineW;
					if (lineW & 3)
					{
						lineW = lineW + 4 - (lineW & 3);
					}
					currOfst = ReadUInt32(&hdr[10]);
					break;
				case 16:
					lineW = imgWidth << 1;
					if (lineW & 3)
					{
						lineW = lineW + 4 - (lineW & 3);
					}
					lineW2 = imgWidth << 1;
					currOfst = ReadUInt32(&hdr[10]);
					break;
				case 24:
					lineW = imgWidth * 3;
					if (lineW & 3)
					{
						lineW = lineW + 4 - (lineW & 3);
					}
					lineW2 = imgWidth * 3;
					currOfst = ReadUInt32(&hdr[10]);
					break;
				case 32:
					lineW = imgWidth << 2;
					lineW2 = lineW;
					currOfst = ReadUInt32(&hdr[10]);
					break;
				default:
					DEL_CLASS(outImg);
					outImg = 0;
					break;
				};

				if (outImg)
				{
					UOSInt bpl = outImg->GetDataBpl();
					if (bpl != lineW)
					{
						while (uimgHeight-- > 0)
						{
							fd->GetRealData(currOfst, lineW, pBits);
							pBits += bpl;
							currOfst += lineW;
						}
					}
					else
					{
						fd->GetRealData(currOfst, uimgHeight * lineW, pBits);
					}
				}
			}
		}
		else if (biCompression == 1 && bpp == 8) // rle8
		{
			BMPParser_ReadPal(outImg, fd, endPos, palType, colorUsed);
			UOSInt dataSize = (UOSInt)(fd->GetDataSize() - ReadUInt32(&hdr[10]));
			Data::ByteBuffer rleData(dataSize);
			Data::ByteArray currPtr;
			OSInt dAdd = (OSInt)outImg->GetDataBpl();
			pBits.Clear(0, uimgHeight * (UOSInt)dAdd);
			fd->GetRealData(ReadUInt32(&hdr[10]), dataSize, rleData);
			UInt8 c;
			UInt8 v;
			UOSInt currX;
			UOSInt currY = 0;
			currPtr = rleData;
			currX = 0;
			if (inv)
			{
				pBits = pBits + (imgHeight - 1) * dAdd;
				dAdd = -dAdd;
			}
			if (dataSize & 1)
			{
				dataSize -= 1;
			}
			while (currY < uimgHeight && dataSize >= 2)
			{
				c = *currPtr++;
				if (c == 0)
				{
					c = *currPtr++;
					dataSize -= 2;
					if (c == 0)
					{
						currY++;
						currX = 0;
						pBits += dAdd;
					}
					else if (c == 1)
					{
						break;
					}
					else if (c == 2)
					{
						if (dataSize >= 2)
						{
							dataSize -= 2;
							c = *currPtr++;
							currX += c;
							c = *currPtr++;
							currY += c;
							pBits += dAdd * c;
						}
						else
						{
							break;
						}
					}
					else
					{
						if (dataSize < (UOSInt)c + 1)
						{
							c = (UInt8)dataSize;
						}
						while (c-- > 0)
						{
							v = *currPtr++;
							dataSize--;
							if (currX < imgWidth)
							{
								pBits[currX] = v;
							}
							currX++;
						}
						if (dataSize & 1)
						{
							currPtr++;
							dataSize--;
						}
					}
				}
				else
				{
					v = *currPtr++;
					dataSize -= 2;
					if (currX >= imgWidth)
					{
					}
					else if (c + currX >= imgWidth)
					{
						while (currX < imgWidth)
						{
							pBits[currX++] = v;
						}
					}
					else
					{
						while (c-- > 0)
						{
							pBits[currX++] = v;
						}
					}
				}
			}
		}
		else if (biCompression == 2 && bpp == 4) // rle4
		{
			BMPParser_ReadPal(outImg, fd, endPos, palType, colorUsed);
			UOSInt dataSize = (UOSInt)(fd->GetDataSize() - ReadUInt32(&hdr[10]));
			Data::ByteBuffer rleData(dataSize);
			OSInt dAdd = (OSInt)outImg->GetDataBpl() << 1;
			Data::ByteBuffer tmpData(uimgHeight * (UOSInt)dAdd);
			Data::ByteArray currPtr;
			tmpData.Clear(0, (UInt32)imgHeight * (UOSInt)dAdd);
			fd->GetRealData(ReadUInt32(&hdr[10]), dataSize, rleData);
			UInt8 c;
			UInt8 c2;
			UInt8 v;
			UOSInt currX;
			UOSInt currY = 0;
			currPtr = rleData;
			currX = 0;
			if (inv)
			{
				pBits = tmpData + (imgHeight - 1) * dAdd;
				dAdd = -dAdd;
			}
			else
			{
				pBits = tmpData;
			}
			if (dataSize & 1)
			{
				dataSize -= 1;
			}
			while (currY < uimgHeight && dataSize >= 2)
			{
				c = *currPtr++;
				if (c == 0)
				{
					c = *currPtr++;
					dataSize -= 2;
					if (c == 0)
					{
						currY++;
						currX = 0;
						pBits += dAdd;
					}
					else if (c == 1)
					{
						break;
					}
					else if (c == 2)
					{
						if (dataSize >= 2)
						{
							dataSize -= 2;
							c = *currPtr++;
							currX += c;
							c = *currPtr++;
							currY += c;
							pBits += dAdd * c;
						}
						else
						{
							break;
						}
					}
					else
					{
						c2 = (UInt8)(c >> 1);
						while (c2-- > 0 && dataSize > 0)
						{
							v = *currPtr++;
							dataSize--;
							if (currX < imgWidth)
							{
								pBits[currX] = (UInt8)(v >> 4);
							}
							currX++;
							if (currX < imgWidth)
							{
								pBits[currX] = (UInt8)(v & 15);
							}
							currX++;
						}
						if (dataSize > 0 && c & 1)
						{
							v = *currPtr++;
							dataSize--;
							if (currX < imgWidth)
							{
								pBits[currX] = (UInt8)(v >> 4);
							}
							currX++;
						}
						if (dataSize & 1)
						{
							currPtr++;
							dataSize--;
						}
					}
				}
				else
				{
					v = *currPtr++;
					dataSize -= 2;
					c2 = (UInt8)(c >> 1);
					while (c2-- > 0)
					{
						if (currX >= imgWidth)
						{
							break;
						}
						pBits[currX++] = (UInt8)(v >> 4);
						if (currX >= imgWidth)
						{
							break;
						}
						pBits[currX++] = v & 15;
					}
					if (c & 1)
					{
						if (currX < imgWidth)
						{
							pBits[currX] = (UInt8)(v >> 4);
						}
						currX++;
					}
				}
			}
			pBits = outImg->GetDataArray();
			rleData.ReplaceBy(tmpData);
			if (dAdd < 0)
			{
				dAdd = -dAdd;
			}
			dataSize = uimgHeight * (UOSInt)(dAdd >> 1);
			while (dataSize-- > 0)
			{
				*pBits++ = (UInt8)((rleData[0] << 4) | rleData[1]);
				rleData += 2;
			}
		}
		else if (biCompression == 3) //BitFields
		{
			UInt32 rVal;
			UInt32 gVal;
			UInt32 bVal;
			UInt32 aVal;
			UInt32 pxVal;
			OSInt dAdd = (OSInt)outImg->GetDataBpl();
			UOSInt currW;
			OSInt srcI;
			UOSInt destI;
			UInt32 bitVal;
			OSInt bitCnt;
			if (srcBpp == 16)
			{
				lineW = imgWidth << 1;
				if (lineW & 3)
				{
					lineW = lineW + 4 - (lineW & 3);
				}
				Data::ByteBuffer readBuff(lineW);
				currOfst = ReadUInt32(&hdr[10]);
				if (inv)
				{
					pBits = pBits + dAdd * (imgHeight - 1);
					dAdd = -dAdd;
				}
				while (imgHeight-- > 0)
				{
					fd->GetRealData(currOfst, lineW, readBuff);
					currOfst += lineW;

					currW = imgWidth;
					srcI = 0;
					destI = 0;
					while (currW-- > 0)
					{
						rVal = 0;
						gVal = 0;
						bVal = 0;
						aVal = 0;
						pxVal = ReadUInt16(&readBuff[srcI]);
						
						bitVal = 0x8000;
						bitCnt = 16;
						while (bitCnt-- > 0)
						{
							if (rBit & bitVal)
							{
								rVal = (rVal << 1) | ((pxVal >> bitCnt) & 1);
							}
							else if (gBit & bitVal)
							{
								gVal = (gVal << 1) | ((pxVal >> bitCnt) & 1);
							}
							else if (bBit & bitVal)
							{
								bVal = (bVal << 1) | ((pxVal >> bitCnt) & 1);
							}
							else if (aBit & bitVal)
							{
								aVal = (aVal << 1) | ((pxVal >> bitCnt) & 1);
							}
							bitVal = bitVal >> 1;
						}
						if (bpp == 16)
						{
							if (rBitCnt <= 0)
							{
								rVal = 0;
							}
							else if (rBitCnt == 1)
							{
								rVal = (rVal << 4) | (rVal << 3) | (rVal << 2) | (rVal << 1) | rVal;
							}
							else if (rBitCnt == 2)
							{
								rVal = (rVal << 3) | (rVal << 1) | (rVal >> 1);
							}
							else if (rBitCnt == 3)
							{
								rVal = (rVal << 2) | (rVal >> 1);
							}
							else if (rBitCnt == 4)
							{
								rVal = (rVal << 1) | (rVal >> 3);
							}
							if (gBitCnt <= 0)
							{
								gVal = 0;
							}
							else if (gBitCnt == 1)
							{
								gVal = (gVal << 2) | (gVal << 1) | gVal;
								gVal = (gVal << 3) | gVal;
							}
							else if (gBitCnt == 2)
							{
								gVal = (gVal << 4) | (gVal << 2) | gVal;
							}
							else if (gBitCnt == 3)
							{
								gVal = (gVal << 3) | gVal;
							}
							else if (gBitCnt == 4)
							{
								gVal = (gVal << 2) | (gVal >> 2);
							}
							else if (gBitCnt == 5)
							{
								gVal = (gVal << 1) | (gVal >> 4);
							}
							if (bBitCnt <= 0)
							{
								bVal = 0;
							}
							else if (bBitCnt == 1)
							{
								bVal = (bVal << 4) | (bVal << 3) | (bVal << 2) | (bVal << 1) | bVal;
							}
							else if (bBitCnt == 2)
							{
								bVal = (bVal << 3) | (bVal << 1) | (bVal >> 1);
							}
							else if (bBitCnt == 3)
							{
								bVal = (bVal << 2) | (bVal >> 1);
							}
							else if (bBitCnt == 4)
							{
								bVal = (bVal << 1) | (bVal >> 3);
							}
							pxVal = (rVal << 11) | (gVal << 5) | bVal;
							WriteInt16(&pBits[destI], pxVal);
						}
						else if (bpp == 32)
						{
							switch (rBitCnt)
							{
							case 0:
								rVal = 0;
								break;
							case 1:
								rVal = (rVal << 1) | rVal;
							case 2:
								rVal = (rVal << 2) | rVal;
							case 4:
								rVal = (rVal << 4) | rVal;
								break;
							case 3:
								rVal = (rVal << 5) | (rVal << 2) | (rVal >> 1);
								break;
							case 5:
								rVal = (rVal << 3) | (rVal >> 2);
								break;
							case 6:
								rVal = (rVal << 2) | (rVal >> 4);
								break;
							case 7:
								rVal = (rVal << 1) | (rVal >> 6);
								break;
							}
							switch (gBitCnt)
							{
							case 0:
								gVal = 0;
								break;
							case 1:
								gVal = (gVal << 1) | gVal;
							case 2:
								gVal = (gVal << 2) | gVal;
							case 4:
								gVal = (gVal << 4) | gVal;
								break;
							case 3:
								gVal = (gVal << 5) | (gVal << 2) | (gVal >> 1);
								break;
							case 5:
								gVal = (gVal << 3) | (gVal >> 2);
								break;
							case 6:
								gVal = (gVal << 2) | (gVal >> 4);
								break;
							case 7:
								gVal = (gVal << 1) | (gVal >> 6);
								break;
							}
							switch (bBitCnt)
							{
							case 0:
								bVal = 0;
								break;
							case 1:
								bVal = (bVal << 1) | bVal;
							case 2:
								bVal = (bVal << 2) | bVal;
							case 4:
								bVal = (bVal << 4) | bVal;
								break;
							case 3:
								bVal = (bVal << 5) | (bVal << 2) | (bVal >> 1);
								break;
							case 5:
								bVal = (bVal << 3) | (bVal >> 2);
								break;
							case 6:
								bVal = (bVal << 2) | (bVal >> 4);
								break;
							case 7:
								bVal = (bVal << 1) | (bVal >> 6);
								break;
							}
							switch (aBitCnt)
							{
							case 0:
								aVal = 0xff;
								break;
							case 1:
								aVal = (aVal << 1) | aVal;
							case 2:
								aVal = (aVal << 2) | aVal;
							case 4:
								aVal = (aVal << 4) | aVal;
								break;
							case 3:
								aVal = (aVal << 5) | (aVal << 2) | (aVal >> 1);
								break;
							case 5:
								aVal = (aVal << 3) | (aVal >> 2);
								break;
							case 6:
								aVal = (aVal << 2) | (aVal >> 4);
								break;
							case 7:
								aVal = (aVal << 1) | (aVal >> 6);
								break;
							}
							pxVal = (aVal << 24) | (rVal << 16) | (gVal << 8) | bVal;
							WriteUInt32(&pBits[destI], pxVal);
						}
						else if (bpp == 64)
						{
							switch (rBitCnt)
							{
							case 0:
								rVal = 0;
								break;
							case 1:
								rVal = (rVal << 1) | rVal;
							case 2:
								rVal = (rVal << 2) | rVal;
							case 4:
								rVal = (rVal << 4) | rVal;
							case 8:
								rVal = (rVal << 8) | rVal;
								break;
							case 3:
								rVal = (rVal << 13) | (rVal << 10) | (rVal << 7) | (rVal << 4) | (rVal << 1) | (rVal >> 2);
								break;
							case 5:
								rVal = (rVal << 11) | (rVal << 6) | (rVal << 1) | (rVal >> 4);
								break;
							case 6:
								rVal = (rVal << 10) | (rVal << 4) | (rVal >> 2);
								break;
							case 7:
								rVal = (rVal << 9) | (rVal << 2) | (rVal >> 5);
								break;
							case 9:
								rVal = (rVal << 7) | (rVal >> 2);
								break;
							case 10:
								rVal = (rVal << 6) | (rVal >> 4);
								break;
							case 11:
								rVal = (rVal << 5) | (rVal >> 6);
								break;
							case 12:
								rVal = (rVal << 4) | (rVal >> 8);
								break;
							case 13:
								rVal = (rVal << 3) | (rVal >> 10);
								break;
							case 14:
								rVal = (rVal << 2) | (rVal >> 12);
								break;
							case 15:
								rVal = (rVal << 1) | (rVal >> 14);
								break;
							case 16:
								break;
							default:
								rVal = rVal >> (rBitCnt - 16);
								break;
							}
							switch (gBitCnt)
							{
							case 0:
								gVal = 0;
								break;
							case 1:
								gVal = (gVal << 1) | gVal;
							case 2:
								gVal = (gVal << 2) | gVal;
							case 4:
								gVal = (gVal << 4) | gVal;
							case 8:
								gVal = (gVal << 8) | gVal;
								break;
							case 3:
								gVal = (gVal << 13) | (gVal << 10) | (gVal << 7) | (gVal << 4) | (gVal << 1) | (gVal >> 2);
								break;
							case 5:
								gVal = (gVal << 11) | (gVal << 6) | (gVal << 1) | (gVal >> 4);
								break;
							case 6:
								gVal = (gVal << 10) | (gVal << 4) | (gVal >> 2);
								break;
							case 7:
								gVal = (gVal << 9) | (gVal << 2) | (gVal >> 5);
								break;
							case 9:
								gVal = (gVal << 7) | (gVal >> 2);
								break;
							case 10:
								gVal = (gVal << 6) | (gVal >> 4);
								break;
							case 11:
								gVal = (gVal << 5) | (gVal >> 6);
								break;
							case 12:
								gVal = (gVal << 4) | (gVal >> 8);
								break;
							case 13:
								gVal = (gVal << 3) | (gVal >> 10);
								break;
							case 14:
								gVal = (gVal << 2) | (gVal >> 12);
								break;
							case 15:
								gVal = (gVal << 1) | (gVal >> 14);
								break;
							case 16:
								break;
							default:
								gVal = gVal >> (gBitCnt - 16);
								break;
							}
							switch (bBitCnt)
							{
							case 0:
								bVal = 0;
								break;
							case 1:
								bVal = (bVal << 1) | bVal;
							case 2:
								bVal = (bVal << 2) | bVal;
							case 4:
								bVal = (bVal << 4) | bVal;
							case 8:
								bVal = (bVal << 8) | bVal;
								break;
							case 3:
								bVal = (bVal << 13) | (bVal << 10) | (bVal << 7) | (bVal << 4) | (bVal << 1) | (bVal >> 2);
								break;
							case 5:
								bVal = (bVal << 11) | (bVal << 6) | (bVal << 1) | (bVal >> 4);
								break;
							case 6:
								bVal = (bVal << 10) | (bVal << 4) | (bVal >> 2);
								break;
							case 7:
								bVal = (bVal << 9) | (bVal << 2) | (bVal >> 5);
								break;
							case 9:
								bVal = (bVal << 7) | (bVal >> 2);
								break;
							case 10:
								bVal = (bVal << 6) | (bVal >> 4);
								break;
							case 11:
								bVal = (bVal << 5) | (bVal >> 6);
								break;
							case 12:
								bVal = (bVal << 4) | (bVal >> 8);
								break;
							case 13:
								bVal = (bVal << 3) | (bVal >> 10);
								break;
							case 14:
								bVal = (bVal << 2) | (bVal >> 12);
								break;
							case 15:
								bVal = (bVal << 1) | (bVal >> 14);
								break;
							case 16:
								break;
							default:
								bVal = bVal >> (bBitCnt - 16);
								break;
							}
							switch (aBitCnt)
							{
							case 0:
								aVal = 0xffff;
								break;
							case 1:
								aVal = (aVal << 1) | aVal;
							case 2:
								aVal = (aVal << 2) | aVal;
							case 4:
								aVal = (aVal << 4) | aVal;
							case 8:
								aVal = (aVal << 8) | aVal;
								break;
							case 3:
								aVal = (aVal << 13) | (aVal << 10) | (aVal << 7) | (aVal << 4) | (aVal << 1) | (aVal >> 2);
								break;
							case 5:
								aVal = (aVal << 11) | (aVal << 6) | (aVal << 1) | (aVal >> 4);
								break;
							case 6:
								aVal = (aVal << 10) | (aVal << 4) | (aVal >> 2);
								break;
							case 7:
								aVal = (aVal << 9) | (aVal << 2) | (aVal >> 5);
								break;
							case 9:
								aVal = (aVal << 7) | (aVal >> 2);
								break;
							case 10:
								aVal = (aVal << 6) | (aVal >> 4);
								break;
							case 11:
								aVal = (aVal << 5) | (aVal >> 6);
								break;
							case 12:
								aVal = (aVal << 4) | (aVal >> 8);
								break;
							case 13:
								aVal = (aVal << 3) | (aVal >> 10);
								break;
							case 14:
								aVal = (aVal << 2) | (aVal >> 12);
								break;
							case 15:
								aVal = (aVal << 1) | (aVal >> 14);
								break;
							case 16:
								break;
							default:
								aVal = aVal >> (aBitCnt - 16);
								break;
							}
							WriteInt16(&pBits[destI + 0], bVal);
							WriteInt16(&pBits[destI + 2], gVal);
							WriteInt16(&pBits[destI + 4], rVal);
							WriteInt16(&pBits[destI + 6], aVal);
						}

						srcI += 2;
						destI += bpp >> 3;
					}
					pBits += dAdd;
				}
			}
			else if (srcBpp == 32)
			{
				lineW = imgWidth << 2;
				Data::ByteBuffer readBuff(lineW);
				currOfst = ReadUInt32(&hdr[10]);
				if (inv)
				{
					pBits = pBits + dAdd * (imgHeight - 1);
					dAdd = -dAdd;
				}
				while (imgHeight-- > 0)
				{
					fd->GetRealData(currOfst, lineW, readBuff);
					currOfst += lineW;

					currW = imgWidth;
					srcI = 0;
					destI = 0;
					while (currW-- > 0)
					{
						rVal = 0;
						gVal = 0;
						bVal = 0;
						aVal = 0;
						pxVal = ReadUInt32(&readBuff[srcI]);
						
						bitVal = 0x80000000;
						bitCnt = 32;
						while (bitCnt-- > 0)
						{
							if (rBit & bitVal)
							{
								rVal = (rVal << 1) | ((pxVal >> bitCnt) & 1);
							}
							else if (gBit & bitVal)
							{
								gVal = (gVal << 1) | ((pxVal >> bitCnt) & 1);
							}
							else if (bBit & bitVal)
							{
								bVal = (bVal << 1) | ((pxVal >> bitCnt) & 1);
							}
							else if (aBit & bitVal)
							{
								aVal = (aVal << 1) | ((pxVal >> bitCnt) & 1);
							}
							bitVal = bitVal >> 1;
						}
						if (bpp == 16)
						{
							if (rBitCnt <= 0)
							{
								rVal = 0;
							}
							else if (rBitCnt == 1)
							{
								rVal = (rVal << 4) | (rVal << 3) | (rVal << 2) | (rVal << 1) | rVal;
							}
							else if (rBitCnt == 2)
							{
								rVal = (rVal << 3) | (rVal << 1) | (rVal >> 1);
							}
							else if (rBitCnt == 3)
							{
								rVal = (rVal << 2) | (rVal >> 1);
							}
							else if (rBitCnt == 4)
							{
								rVal = (rVal << 1) | (rVal >> 3);
							}
							if (gBitCnt <= 0)
							{
								gVal = 0;
							}
							else if (gBitCnt == 1)
							{
								gVal = (gVal << 2) | (gVal << 1) | gVal;
								gVal = (gVal << 3) | gVal;
							}
							else if (gBitCnt == 2)
							{
								gVal = (gVal << 4) | (gVal << 2) | gVal;
							}
							else if (gBitCnt == 3)
							{
								gVal = (gVal << 3) | gVal;
							}
							else if (gBitCnt == 4)
							{
								gVal = (gVal << 2) | (gVal >> 2);
							}
							else if (gBitCnt == 5)
							{
								gVal = (gVal << 1) | (gVal >> 4);
							}
							if (bBitCnt <= 0)
							{
								bVal = 0;
							}
							else if (bBitCnt == 1)
							{
								bVal = (bVal << 4) | (bVal << 3) | (bVal << 2) | (bVal << 1) | bVal;
							}
							else if (bBitCnt == 2)
							{
								bVal = (bVal << 3) | (bVal << 1) | (bVal >> 1);
							}
							else if (bBitCnt == 3)
							{
								bVal = (bVal << 2) | (bVal >> 1);
							}
							else if (bBitCnt == 4)
							{
								bVal = (bVal << 1) | (bVal >> 3);
							}
							pxVal = (rVal << 11) | (gVal << 5) | bVal;
							WriteInt16(&pBits[destI], pxVal);
						}
						else if (bpp == 32)
						{
							switch (rBitCnt)
							{
							case 0:
								rVal = 0;
								break;
							case 1:
								rVal = (rVal << 1) | rVal;
							case 2:
								rVal = (rVal << 2) | rVal;
							case 4:
								rVal = (rVal << 4) | rVal;
								break;
							case 3:
								rVal = (rVal << 5) | (rVal << 2) | (rVal >> 1);
								break;
							case 5:
								rVal = (rVal << 3) | (rVal >> 2);
								break;
							case 6:
								rVal = (rVal << 2) | (rVal >> 4);
								break;
							case 7:
								rVal = (rVal << 1) | (rVal >> 6);
								break;
							}
							switch (gBitCnt)
							{
							case 0:
								gVal = 0;
								break;
							case 1:
								gVal = (gVal << 1) | gVal;
							case 2:
								gVal = (gVal << 2) | gVal;
							case 4:
								gVal = (gVal << 4) | gVal;
								break;
							case 3:
								gVal = (gVal << 5) | (gVal << 2) | (gVal >> 1);
								break;
							case 5:
								gVal = (gVal << 3) | (gVal >> 2);
								break;
							case 6:
								gVal = (gVal << 2) | (gVal >> 4);
								break;
							case 7:
								gVal = (gVal << 1) | (gVal >> 6);
								break;
							}
							switch (bBitCnt)
							{
							case 0:
								bVal = 0;
								break;
							case 1:
								bVal = (bVal << 1) | bVal;
							case 2:
								bVal = (bVal << 2) | bVal;
							case 4:
								bVal = (bVal << 4) | bVal;
								break;
							case 3:
								bVal = (bVal << 5) | (bVal << 2) | (bVal >> 1);
								break;
							case 5:
								bVal = (bVal << 3) | (bVal >> 2);
								break;
							case 6:
								bVal = (bVal << 2) | (bVal >> 4);
								break;
							case 7:
								bVal = (bVal << 1) | (bVal >> 6);
								break;
							}
							switch (aBitCnt)
							{
							case 0:
								aVal = 0xff;
								break;
							case 1:
								aVal = (aVal << 1) | aVal;
							case 2:
								aVal = (aVal << 2) | aVal;
							case 4:
								aVal = (aVal << 4) | aVal;
								break;
							case 3:
								aVal = (aVal << 5) | (aVal << 2) | (aVal >> 1);
								break;
							case 5:
								aVal = (aVal << 3) | (aVal >> 2);
								break;
							case 6:
								aVal = (aVal << 2) | (aVal >> 4);
								break;
							case 7:
								aVal = (aVal << 1) | (aVal >> 6);
								break;
							}
							pxVal = (aVal << 24) | (rVal << 16) | (gVal << 8) | bVal;
							WriteUInt32(&pBits[destI], pxVal);
						}
						else if (bpp == 64)
						{
							switch (rBitCnt)
							{
							case 0:
								rVal = 0;
								break;
							case 1:
								rVal = (rVal << 1) | rVal;
							case 2:
								rVal = (rVal << 2) | rVal;
							case 4:
								rVal = (rVal << 4) | rVal;
							case 8:
								rVal = (rVal << 8) | rVal;
								break;
							case 3:
								rVal = (rVal << 13) | (rVal << 10) | (rVal << 7) | (rVal << 4) | (rVal << 1) | (rVal >> 2);
								break;
							case 5:
								rVal = (rVal << 11) | (rVal << 6) | (rVal << 1) | (rVal >> 4);
								break;
							case 6:
								rVal = (rVal << 10) | (rVal << 4) | (rVal >> 2);
								break;
							case 7:
								rVal = (rVal << 9) | (rVal << 2) | (rVal >> 5);
								break;
							case 9:
								rVal = (rVal << 7) | (rVal >> 2);
								break;
							case 10:
								rVal = (rVal << 6) | (rVal >> 4);
								break;
							case 11:
								rVal = (rVal << 5) | (rVal >> 6);
								break;
							case 12:
								rVal = (rVal << 4) | (rVal >> 8);
								break;
							case 13:
								rVal = (rVal << 3) | (rVal >> 10);
								break;
							case 14:
								rVal = (rVal << 2) | (rVal >> 12);
								break;
							case 15:
								rVal = (rVal << 1) | (rVal >> 14);
								break;
							case 16:
								break;
							default:
								rVal = rVal >> (rBitCnt - 16);
								break;
							}
							switch (gBitCnt)
							{
							case 0:
								gVal = 0;
								break;
							case 1:
								gVal = (gVal << 1) | gVal;
							case 2:
								gVal = (gVal << 2) | gVal;
							case 4:
								gVal = (gVal << 4) | gVal;
							case 8:
								gVal = (gVal << 8) | gVal;
								break;
							case 3:
								gVal = (gVal << 13) | (gVal << 10) | (gVal << 7) | (gVal << 4) | (gVal << 1) | (gVal >> 2);
								break;
							case 5:
								gVal = (gVal << 11) | (gVal << 6) | (gVal << 1) | (gVal >> 4);
								break;
							case 6:
								gVal = (gVal << 10) | (gVal << 4) | (gVal >> 2);
								break;
							case 7:
								gVal = (gVal << 9) | (gVal << 2) | (gVal >> 5);
								break;
							case 9:
								gVal = (gVal << 7) | (gVal >> 2);
								break;
							case 10:
								gVal = (gVal << 6) | (gVal >> 4);
								break;
							case 11:
								gVal = (gVal << 5) | (gVal >> 6);
								break;
							case 12:
								gVal = (gVal << 4) | (gVal >> 8);
								break;
							case 13:
								gVal = (gVal << 3) | (gVal >> 10);
								break;
							case 14:
								gVal = (gVal << 2) | (gVal >> 12);
								break;
							case 15:
								gVal = (gVal << 1) | (gVal >> 14);
								break;
							case 16:
								break;
							default:
								gVal = gVal >> (gBitCnt - 16);
								break;
							}
							switch (bBitCnt)
							{
							case 0:
								bVal = 0;
								break;
							case 1:
								bVal = (bVal << 1) | bVal;
							case 2:
								bVal = (bVal << 2) | bVal;
							case 4:
								bVal = (bVal << 4) | bVal;
							case 8:
								bVal = (bVal << 8) | bVal;
								break;
							case 3:
								bVal = (bVal << 13) | (bVal << 10) | (bVal << 7) | (bVal << 4) | (bVal << 1) | (bVal >> 2);
								break;
							case 5:
								bVal = (bVal << 11) | (bVal << 6) | (bVal << 1) | (bVal >> 4);
								break;
							case 6:
								bVal = (bVal << 10) | (bVal << 4) | (bVal >> 2);
								break;
							case 7:
								bVal = (bVal << 9) | (bVal << 2) | (bVal >> 5);
								break;
							case 9:
								bVal = (bVal << 7) | (bVal >> 2);
								break;
							case 10:
								bVal = (bVal << 6) | (bVal >> 4);
								break;
							case 11:
								bVal = (bVal << 5) | (bVal >> 6);
								break;
							case 12:
								bVal = (bVal << 4) | (bVal >> 8);
								break;
							case 13:
								bVal = (bVal << 3) | (bVal >> 10);
								break;
							case 14:
								bVal = (bVal << 2) | (bVal >> 12);
								break;
							case 15:
								bVal = (bVal << 1) | (bVal >> 14);
								break;
							case 16:
								break;
							default:
								bVal = bVal >> (bBitCnt - 16);
								break;
							}
							switch (aBitCnt)
							{
							case 0:
								aVal = 0xffff;
								break;
							case 1:
								aVal = (aVal << 1) | aVal;
							case 2:
								aVal = (aVal << 2) | aVal;
							case 4:
								aVal = (aVal << 4) | aVal;
							case 8:
								aVal = (aVal << 8) | aVal;
								break;
							case 3:
								aVal = (aVal << 13) | (aVal << 10) | (aVal << 7) | (aVal << 4) | (aVal << 1) | (aVal >> 2);
								break;
							case 5:
								aVal = (aVal << 11) | (aVal << 6) | (aVal << 1) | (aVal >> 4);
								break;
							case 6:
								aVal = (aVal << 10) | (aVal << 4) | (aVal >> 2);
								break;
							case 7:
								aVal = (aVal << 9) | (aVal << 2) | (aVal >> 5);
								break;
							case 9:
								aVal = (aVal << 7) | (aVal >> 2);
								break;
							case 10:
								aVal = (aVal << 6) | (aVal >> 4);
								break;
							case 11:
								aVal = (aVal << 5) | (aVal >> 6);
								break;
							case 12:
								aVal = (aVal << 4) | (aVal >> 8);
								break;
							case 13:
								aVal = (aVal << 3) | (aVal >> 10);
								break;
							case 14:
								aVal = (aVal << 2) | (aVal >> 12);
								break;
							case 15:
								aVal = (aVal << 1) | (aVal >> 14);
								break;
							case 16:
								break;
							default:
								aVal = aVal >> (aBitCnt - 16);
								break;
							}
							WriteInt16(&pBits[destI + 0], bVal);
							WriteInt16(&pBits[destI + 2], gVal);
							WriteInt16(&pBits[destI + 4], rVal);
							WriteInt16(&pBits[destI + 6], aVal);
						}

						srcI += 4;
						destI += bpp >> 3;
					}
					pBits += dAdd;
				}
			}
			else
			{
				DEL_CLASS(outImg);
				outImg = 0;
			}
		}
		else
		{
			DEL_CLASS(outImg);
			outImg = 0;
		}
	}

	if (nnimg.Set(outImg))
	{
		Media::ImageList *imgList;
		NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
		imgList->AddImage(nnimg, 0);
		return imgList;
	}
	else
	{
		return 0;
	}
}
