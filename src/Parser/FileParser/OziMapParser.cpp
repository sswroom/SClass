#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/StmData/FileData.h"
#include "Map/VectorLayer.h"
#include "Math/Math_C.h"
#include "Math/CSys/CoordinateSystemManager.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/ImageList.h"
#include "Media/ImagePreviewTool.h"
#include "Parser/ParserList.h"
#include "Parser/FileParser/OziMapParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/UTF8Reader.h"

Bool Parser::FileParser::OziMapParser::FitCorners(UnsafeArray<Double> ptXY, UInt32 nPts, Double imgW, Double imgH, Math::Coord2DDbl &tl, Math::Coord2DDbl &tr, Math::Coord2DDbl &br, Math::Coord2DDbl &bl)
{
	Double m[3][5] = {};
	UInt32 i = 0;
	while (i < nPts)
	{
		Double x = (ptXY[(i << 2)] - imgW * 0.5) / imgW;
		Double y = (ptXY[(i << 2) + 1] - imgH * 0.5) / imgH;
		Double values[3] = {x, y, 1};
		Double lon = ptXY[(i << 2) + 2];
		Double lat = ptXY[(i << 2) + 3];
		if (Math::IsNAN(x) || Math::IsNAN(y) || Math::IsNAN(lon) || Math::IsNAN(lat))
			return false;
		UInt32 row = 0;
		while (row < 3)
		{
			UInt32 col = 0;
			while (col < 3)
			{
				m[row][col] += values[row] * values[col];
				col++;
			}
			m[row][3] += values[row] * lon;
			m[row][4] += values[row] * lat;
			row++;
		}
		i++;
	}

	i = 0;
	while (i < 3)
	{
		UInt32 maxRow = i;
		UInt32 row = i + 1;
		while (row < 3)
		{
			if (fabs(m[row][i]) > fabs(m[maxRow][i]))
				maxRow = row;
			row++;
		}
		if (fabs(m[maxRow][i]) < 1e-12)
			return false;
		if (maxRow != i)
		{
			UInt32 col = 0;
			while (col < 5)
			{
				Double tmp = m[i][col];
				m[i][col] = m[maxRow][col];
				m[maxRow][col] = tmp;
				col++;
			}
		}
		Double pivot = m[i][i];
		UInt32 col = i;
		while (col < 5)
		{
			m[i][col] /= pivot;
			col++;
		}
		row = 0;
		while (row < 3)
		{
			if (row != i)
			{
				Double factor = m[row][i];
				col = i;
				while (col < 5)
				{
					m[row][col] -= factor * m[i][col];
					col++;
				}
			}
			row++;
		}
		i++;
	}

	Double lonCoef[3] = {m[0][3], m[1][3], m[2][3]};
	Double latCoef[3] = {m[0][4], m[1][4], m[2][4]};
	Double x = -0.5;
	Double y = -0.5;
	tl = Math::Coord2DDbl(lonCoef[0] * x + lonCoef[1] * y + lonCoef[2], latCoef[0] * x + latCoef[1] * y + latCoef[2]);
	x = 0.5;
	tr = Math::Coord2DDbl(lonCoef[0] * x + lonCoef[1] * y + lonCoef[2], latCoef[0] * x + latCoef[1] * y + latCoef[2]);
	y = 0.5;
	br = Math::Coord2DDbl(lonCoef[0] * x + lonCoef[1] * y + lonCoef[2], latCoef[0] * x + latCoef[1] * y + latCoef[2]);
	x = -0.5;
	bl = Math::Coord2DDbl(lonCoef[0] * x + lonCoef[1] * y + lonCoef[2], latCoef[0] * x + latCoef[1] * y + latCoef[2]);
	return !Math::IsNAN(tl.x) && !Math::IsNAN(tl.y) && !Math::IsNAN(tr.x) && !Math::IsNAN(tr.y) && !Math::IsNAN(br.x) && !Math::IsNAN(br.y) && !Math::IsNAN(bl.x) && !Math::IsNAN(bl.y);
}

Parser::FileParser::OziMapParser::OziMapParser()
{
	this->parsers = nullptr;
}

Parser::FileParser::OziMapParser::~OziMapParser()
{
}

Int32 Parser::FileParser::OziMapParser::GetName()
{
	return *(Int32*)"OMAP";
}

void Parser::FileParser::OziMapParser::SetParserList(Optional<Parser::ParserList> parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::OziMapParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.map"), CSTR("OziExplorer Map File"));
	}
}

IO::ParserType Parser::FileParser::OziMapParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::FileParser::OziMapParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	Optional<Text::String> fileName = nullptr;
	NN<Text::String> nnfileName;
	UnsafeArray<UTF8Char> tmpArr[6];
	Optional<Map::VectorLayer> lyr = nullptr;
	NN<Map::VectorLayer> nnlyr;
	Bool valid;
	NN<Parser::ParserList> parsers;

	if (!this->parsers.SetTo(parsers))
		return nullptr;

	if (!Text::StrEqualsC(&hdr[0], 34, UTF8STRC("OziExplorer Map Data File Version ")))
		return nullptr;

	IO::StreamDataStream stm(fd);
	Text::UTF8Reader reader(stm);

	valid = true;
	if (!reader.ReadLine(sbuff, 1024).SetTo(sptr) || !Text::StrStartsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("OziExplorer Map Data File Version ")))
	{
		valid = false;
	}
	if (reader.ReadLine(sbuff, 1024).SetTo(sptr))
	{
		fileName = Text::String::New(sbuff, (UIntOS)(sptr - sbuff));
	}
	else
	{
		valid = false;
	}
	reader.ReadLine(sbuff, 1024); //Full Path
	reader.ReadLine(sbuff, 1024); //?
	if (!reader.ReadLine(sbuff, 1024).SetTo(sptr) || !Text::StrStartsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("WGS 84")))
	{
		valid = false;
	}
	if (!reader.ReadLine(sbuff, 1024).SetTo(sptr) || !Text::StrStartsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("Reserved ")))
	{
		valid = false;
	}
	if (valid)
	{
		UInt32 currPt;
		UInt32 nPts = 0;
		Double *ptXY = 0;
		Int32 *ptStatus = 0;
		Double imgW = 0;
		Double imgH = 0;
		Math::Coord2DDbl tl;
		Math::Coord2DDbl tr;
		Math::Coord2DDbl br;
		Math::Coord2DDbl bl;
		while (reader.ReadLine(sbuff, 1024).SetTo(sptr))
		{
			if (Text::StrStartsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("MMPNUM,")))
			{
				if (ptXY == 0)
				{
					nPts = Text::StrToUInt32(&sbuff[7]);
					if (nPts > 0 && nPts <= 10)
					{
						ptXY = MemAlloc(Double, nPts << 2);
						ptStatus = MemAlloc(Int32, nPts);
						currPt = nPts;
						while (currPt-- > 0)
						{
							ptStatus[currPt] = 0;
						}
					}
				}
			}
			else if (Text::StrStartsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("MMPXY,")))
			{
				if (ptXY && Text::StrSplitTrim(tmpArr, 6, sbuff, ',') == 4)
				{
					currPt = Text::StrToUInt32(tmpArr[1]) - 1;
					if ((Int32)currPt >= 0 && currPt < nPts)
					{
						ptXY[(currPt << 2)] = Text::StrToDoubleOrNAN(tmpArr[2]);
						ptXY[(currPt << 2) + 1] = Text::StrToDoubleOrNAN(tmpArr[3]);
						ptStatus[currPt] |= 1;
					}
				}
			}
			else if (Text::StrStartsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("MMPLL,")))
			{
				if (ptXY && Text::StrSplitTrim(tmpArr, 6, sbuff, ',') == 4)
				{
					currPt = Text::StrToUInt32(tmpArr[1]) - 1;
					if ((Int32)currPt >= 0 && currPt <nPts)
					{
						ptXY[(currPt << 2) + 2] = Text::StrToDoubleOrNAN(tmpArr[2]);
						ptXY[(currPt << 2) + 3] = Text::StrToDoubleOrNAN(tmpArr[3]);
						ptStatus[currPt] |= 2;
					}
				}
			}
			else if (Text::StrStartsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("IWH,")))
			{
				if (Text::StrSplitTrim(tmpArr, 6, sbuff, ',') == 4)
				{
					imgW = Text::StrToDoubleOrNAN(tmpArr[2]);
					imgH = Text::StrToDoubleOrNAN(tmpArr[3]);
				}
			}
		}
		if (ptXY == 0)
		{
			valid = false;
		}
		else
		{
			currPt = nPts;
			while (currPt-- > 0)
			{
				if (ptStatus[currPt] != 3)
					valid = false;
			}
		}
		if (imgW == 0 || imgH == 0)
		{
			valid = false;
		}
		if (valid && !FitCorners(ptXY, nPts, imgW, imgH, tl, tr, br, bl))
		{
			valid = false;
		}

		if (valid && fileName.SetTo(nnfileName))
		{
			Optional<Media::ImageList> imgList = nullptr;
			NN<Media::ImageList> nnimgList;
			sptr = fd->GetFullFileName()->ConcatTo(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, nnfileName->ToCString());
			{
				IO::StmData::FileData imgFd(CSTRP(sbuff, sptr), false);
				imgList = Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(imgFd, IO::ParserType::ImageList));
			}

			if (imgList.SetTo(nnimgList))
			{
				NN<Math::Geometry::VectorImage> vimg;
				NN<Media::SharedImage> shimg;
				NN<Math::CSys::GeographicCoordinateSystem> gcs;
				if (Math::CSys::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CSys::CoordinateSystemManager::GCST_WGS84).SetTo(gcs))
				{
					Data::ArrayListNN<Media::StaticImage> prevList;
					Media::ImagePreviewTool::CreatePreviews(nnimgList, prevList, 640);
					NEW_CLASSNN(shimg, Media::SharedImage(nnimgList, prevList));
					NEW_CLASSNN(vimg, Math::Geometry::VectorImage(gcs->GetSRID(), shimg, Math::Coord2DDbl(0, 0), Math::Coord2DDbl(imgW, imgH), Math::Coord2DDbl(imgW, imgH), false, CSTRP(sbuff, sptr), 0, 0));
					vimg->SetCoordinates(tl, tr, br, bl);
					UIntOS i = Text::StrLastIndexOfCharC(sbuff, (UIntOS)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
					NN<Text::String> s = Text::String::New(&sbuff[i + 1], (UIntOS)(sptr - &sbuff[i + 1]));
					NEW_CLASSNN(nnlyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), gcs, s));
					lyr = nnlyr;
					s->Release();
					nnlyr->AddVector2(vimg, (Text::PString*)0);
				}
			}
		}
		if (ptXY)
		{
			MemFree(ptXY);
			MemFree(ptStatus);
		}
	}
	OPTSTR_DEL(fileName);
	return lyr;
}
