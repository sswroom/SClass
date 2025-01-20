#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/MemFileBuffer.h"
#include "IO/ViewFileBuffer.h"
#include "Map/MapLayerData.h"
#include "Math/Math.h"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

//#define FILEBUFFER(name) IO::ViewFileBuffer(name)
#define FILEBUFFER(name, nameLen) IO::MemFileBuffer(Text::CStringNN(name, nameLen))

Map::MapLayerData::MapLayerData(Text::CStringNN filePath)
{
	UTF8Char fileName[256];
	UnsafeArray<UTF8Char> str;
	UnsafeArray<UTF8Char> sptrEnd;

	str = filePath.ConcatTo(fileName);
	this->cipFile = 0;
	this->cixFile = 0;
	this->ciuFile = 0;
	this->blkFile = 0;

	sptrEnd = Text::StrConcatC(str, UTF8STRC(".cip"));
	NEW_CLASSNN(this->cipFileObj, FILEBUFFER(fileName, (UOSInt)(sptrEnd - fileName)));
	this->cipFile = this->cipFileObj->GetPointer();

	sptrEnd = Text::StrConcatC(str, UTF8STRC(".cix"));
	NEW_CLASSNN(this->cixFileObj, FILEBUFFER(fileName, (UOSInt)(sptrEnd - fileName)));
	this->cixFile = this->cixFileObj->GetPointer();

	sptrEnd = Text::StrConcatC(str, UTF8STRC(".ciu"));
	NEW_CLASSNN(this->ciuFileObj, FILEBUFFER(fileName, (UOSInt)(sptrEnd - fileName)));
	this->ciuFile = this->ciuFileObj->GetPointer();

	sptrEnd = Text::StrConcatC(str, UTF8STRC(".blk"));
	NEW_CLASSNN(this->blkFileObj, FILEBUFFER(fileName, (UOSInt)(sptrEnd - fileName)));
	this->blkFile = this->blkFileObj->GetPointer();
}

Map::MapLayerData::~MapLayerData()
{
	this->cipFileObj.Delete();
	this->cixFileObj.Delete();
	this->ciuFileObj.Delete();
	this->blkFileObj.Delete();
}

Bool Map::MapLayerData::IsError() const
{
	return this->cixFile == 0 || this->ciuFile == 0 || this->cipFile == 0 || this->blkFile == 0;
}

Bool Map::MapLayerData::GetPGLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OptOut<Math::Coord2DDbl> outCoord, UOSInt strIndex)
{
	Math::Coord2DDbl mapPos = coord * 200000.0;
	Int32 blkx;
	Int32 blky;
	Int32 blkCnt;
	Int32 blkScale;
	Int32 i;
	Int32 j;
	Int32 k;
	Int32 l;
	UnsafeArray<UInt8> ciuFile;
	UnsafeArray<UInt8> cixFile;
	UnsafeArray<UInt8> cipFile;
	UnsafeArray<Int32> sfile;
	Int32 *databuff;
	UInt8 *strRec;
	Int32 *hdr;
	Int32 ofst;
	if (!this->cixFile.SetTo(cixFile) || !this->ciuFile.SetTo(ciuFile) || !this->cipFile.SetTo(cipFile) || this->blkFile == 0)
		return false;
	sfile = UnsafeArray<Int32>::ConvertFrom(ciuFile);
	blkCnt = *sfile++;
	blkScale = *sfile++;
	blkx = (Int32)(mapPos.x / blkScale);
	blky = (Int32)(mapPos.y / blkScale);

	if (blkCnt > 10)
	{
		i = 0;
		j = blkCnt - 1;
		while (i <= j)
		{
			k = (i + j) >> 1;
			databuff = &sfile[k << 2];
			if (databuff[0] < blkx)
			{
				i = k + 1;
			}
			else if (databuff[0] > blkx)
			{
				j = k - 1;
			}
			else if (databuff[1] < blky)
			{
				i = k + 1;
			}
			else if (databuff[1] > blky)
			{
				j = k - 1;
			}
			else
			{
				j = k;
				break;
			}
		}
	}
	else
	{
		j = 0;
	}
	if (j < 0)
		j = 0;

	k = j;
	while (k < blkCnt)
	{
		databuff = &sfile[k << 2];
		if (*databuff > blkx)
			break;

		if ((databuff[1] == blky) && (databuff[0] >= blkx))
		{
			l = databuff[2];
			ofst = databuff[3];
			while (l-- > 0)
			{
				strRec = &ciuFile[ofst];
				if (strRec[4] == 0)
				{
					ofst += 5;
					continue;
				}
				ofst += 5 + strRec[4];

				hdr = (Int32*)&cixFile[((*(Int32*)strRec) << 3) + 4];
				if (hdr[0] == *(Int32*)strRec)
				{
					Int32 *pfile = (Int32*)&cipFile[hdr[1]];
					Int32 nParts = pfile[1];
					Int32 *parts = &pfile[2];
					Int32 numPoints;
					Int32 *points;
					pfile = &pfile[2+nParts];
					numPoints = pfile[0];
					points = &pfile[1];

					Int32 m,n,o;
					Int32 firstX = 0;
					Int32 firstY = 0;
					Int32 lastX = 0;
					Int32 lastY = 0;
					Int32 *lastPolyPoint = points;
					bool firstPt;

					n = 0;
					o = 0;
					m = 0;
					while (o < numPoints)
					{
						firstPt = false;
						if (m < nParts)
						{
							if (parts[m] <= (Int32)o)
							{
								if (o)
								{
									if ((lastY > mapPos.y) ^ (firstY > mapPos.y))
									{
										if ((lastX - (lastX - firstX) * (lastY - mapPos.y) / (lastY - firstY)) > mapPos.x)
											n++;
									}
								}

								firstX = lastPolyPoint[0];
								firstY = lastPolyPoint[1];
								m++;
								firstPt = true;
							}
						}
						if (!firstPt)
						{
							if ((lastY > mapPos.y) ^ (lastPolyPoint[1] > mapPos.y))
							{
								if ((lastX - (lastX - *lastPolyPoint) * (lastY - mapPos.y) / (lastY - lastPolyPoint[1])) > mapPos.x)
									n++;
							}
						}
						lastX = lastPolyPoint[0];
						lastY = lastPolyPoint[1];
						lastPolyPoint += 2;
						o++;
					}

					if ((lastY > mapPos.y) ^ (firstY > mapPos.y))
					{
						if ((lastX - (lastX - firstX) * (lastY - mapPos.y) / (lastY - firstY)) > mapPos.x)
							n++;
					}

					if (n & 1)
					{
						sb->AppendUTF16((UTF16Char*)&strRec[5], (UOSInt)strRec[4] >> 1);
						outCoord.Set(coord);
						return true;
					}
				}
				else
				{
				}
			}
		}
		k++;
	}
	return false;
}

Bool Map::MapLayerData::GetPLLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OutParam<Math::Coord2DDbl> outCoord, UOSInt strIndex)
{
	Math::Coord2DDbl mapPos = coord * 200000.0;
	Int32 blkCnt;
	Int32 blkScale;
	Int32 blkx;
	Int32 blky;
	Int32 blkx1;
	Int32 blky1;
	Int32 blkx2;
	Int32 blky2;
	UnsafeArray<UInt8> ciuFile;
	UnsafeArray<UInt8> cixFile;
	UnsafeArray<UInt8> cipFile;
	UnsafeArray<Int32> sfile;
	Int32 *databuff;
	UInt8 *strRec;
	Int32 ofst;
	Int32 i;
	Int32 j;
	Int32 k = 0;

	if (!this->cixFile.SetTo(cixFile) || !this->ciuFile.SetTo(ciuFile) || !this->cipFile.SetTo(cipFile) || this->blkFile == 0)
		return false;
	sfile = UnsafeArray<Int32>::ConvertFrom(ciuFile);
	blkCnt = *sfile++;
	blkScale = *sfile++;
	blkx = (Int32)(mapPos.x / blkScale);
	blky = (Int32)(mapPos.y / blkScale);
	blkx1 = blkx - 1;
	blky1 = blky - 1;
	blkx2 = blkx + 1;
	blky2 = blky + 1;
	i = 0;
	j = blkCnt - 1;
	while (i <= j)
	{
		k = ((i + j) & 0x7ffffffe) << 1;
		if (sfile[k] < blkx1)
		{
			i = ((i + j) >> 1) + 1;
		}
		else if (sfile[k] > blkx1)
		{
			j = ((i + j) >> 1) - 1;
		}
		else if (sfile[k + 1] < blky1)
		{
			i = ((i + j) >> 1) + 1;
		}
		else if (sfile[k + 1] > blky1)
		{
			j = ((i + j) >> 1) - 1;
		}
		else
		{
			break;
		}
	}


	Bool foundRec = false;
	Bool currFound;
	Double dist = 63781370;
	while (k < (blkCnt << 2))
	{
		Int32 nRecs;
		nRecs = sfile[k + 2];
		ofst = sfile[k + 3];
		if (sfile[k] > blkx2)
			break;
		if ((sfile[k + 1] >= blky1) && (sfile[k + 1] <= blky2))
		{
			while (nRecs--)
			{
				Int32 nParts;
				Int32 nPoints;
				Int32 *pBuff;
				Int32 *parts;
				Int32 *points;
				strRec = &ciuFile[ofst];

				if (strRec[4] == 0)
				{
					ofst += 5;
					continue;
				}
				ofst += 5 + strRec[4];

				databuff = (Int32*)&cixFile[((*(UInt32*)strRec) << 3) + 4];
				pBuff = (Int32*)&cipFile[databuff[1]];
				nParts = pBuff[1];
				parts = &pBuff[2];
				nPoints = parts[nParts];
				points = &parts[nParts + 1];

				i = nParts;
				j = nPoints;

				currFound = false;
				Double calBase;
				Double xDiff;
				Double yDiff;
				Double calX = 0;
				Double calY = 0;
				Double calD;
				Double calPtX = 0.0;
				Double calPtY = 0.0;
				Double thisPtX;
				Double thisPtY;
				Double lastPtX;
				Double lastPtY;
				Int32 part;

				while (i--)
				{
					part = parts[i];
					j--;
					thisPtX = points[(j << 1) + 0];
					thisPtY = points[(j << 1) + 1];
					while (j-- > part)
					{
						lastPtX = thisPtX;
						lastPtY = thisPtY;
						thisPtX = points[(j << 1) + 0];
						thisPtY = points[(j << 1) + 1];
						xDiff = thisPtX - lastPtX;
						yDiff = thisPtY - lastPtY;

						if (thisPtY == lastPtY)
						{
							calX = mapPos.x;
						}
						else
						{
							calX = (calBase = (xDiff * xDiff)) * mapPos.x;
							calBase += yDiff * yDiff;
							calX += yDiff * yDiff * thisPtX;
							calX += (mapPos.y - thisPtY) * xDiff * yDiff;
							calX /= calBase;
						}

						if (thisPtX == lastPtX)
						{
							calY = mapPos.y;
						}
						else
						{
							calY = ((calX - thisPtX) * yDiff / xDiff) + thisPtY;
						}

						if (thisPtX < lastPtX)
						{
							if (thisPtX > calX)
								continue;
							if (lastPtX < calX)
								continue;
						}
						else
						{
							if (thisPtX < calX)
								continue;
							if (lastPtX > calX)
								continue;
						}

						if (thisPtY < lastPtY)
						{
							if (thisPtY > calY)
								continue;
							if (lastPtY < calY)
								continue;
						}
						else
						{
							if (thisPtY < calY)
								continue;
							if (lastPtY > calY)
								continue;
						}

						yDiff = mapPos.y - calY;
						xDiff = mapPos.x - calX;
						calD = xDiff * xDiff + yDiff * yDiff;
						if (calD < dist)
						{
							currFound = true;
							dist = calD;
							calPtX = calX;
							calPtY = calY;
						}
					}
				}
				j = nPoints;
				while (j-- > 0)
				{
					xDiff = mapPos.x - points[(j << 1) + 0];
					yDiff = mapPos.y - points[(j << 1) + 1];
					calD = xDiff * xDiff + yDiff * yDiff;
					if (calD < dist)
					{
						currFound = true;
						dist = calD;
						calPtX = calX;
						calPtY = calY;
					}
				}
				if (currFound)
				{
					foundRec = true;
					sb->ClearStr();
					sb->AppendUTF16((UTF16Char*)&strRec[5], (UOSInt)strRec[4] >> 1);
					outCoord.Set(Math::Coord2DDbl(calPtX / 200000.0, calPtY / 200000.0));
				}
			}
		}
		k += 4;
	}
	return foundRec;
}
