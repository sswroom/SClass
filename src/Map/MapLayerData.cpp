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
#define FILEBUFFER(name) IO::MemFileBuffer(name)

Map::MapLayerData::MapLayerData(const UTF8Char *filePath)
{
	UTF8Char fileName[256];
	UTF8Char *str;

	str = Text::StrConcat(fileName, filePath);
	this->cipFile = 0;
	this->cixFile = 0;
	this->ciuFile = 0;
	this->blkFile = 0;

	Text::StrConcat(str, (const UTF8Char*)".cip");
	NEW_CLASS(this->cipFileObj, FILEBUFFER(fileName));
	this->cipFile = this->cipFileObj->GetPointer();

	Text::StrConcat(str, (const UTF8Char*)".cix");
	NEW_CLASS(this->cixFileObj, FILEBUFFER(fileName));
	this->cixFile = this->cixFileObj->GetPointer();

	Text::StrConcat(str, (const UTF8Char*)".ciu");
	NEW_CLASS(this->ciuFileObj, FILEBUFFER(fileName));
	this->ciuFile = this->ciuFileObj->GetPointer();

	Text::StrConcat(str, (const UTF8Char*)".blk");
	NEW_CLASS(this->blkFileObj, FILEBUFFER(fileName));
	this->blkFile = this->blkFileObj->GetPointer();
}

Map::MapLayerData::~MapLayerData()
{
	DEL_CLASS(this->cipFileObj);
	DEL_CLASS(this->cixFileObj);
	DEL_CLASS(this->ciuFileObj);
	DEL_CLASS(this->blkFileObj);
}

Bool Map::MapLayerData::IsError()
{
	return this->cixFile == 0 || this->ciuFile == 0 || this->cipFile == 0 || this->blkFile == 0;
}

UTF8Char *Map::MapLayerData::GetPGLabelD(UTF8Char *buff, Double xposD, Double yposD)
{
	Double xpos = xposD * 200000.0;
	Double ypos = yposD * 200000.0;
	Int32 blkx;
	Int32 blky;
	Int32 blkCnt;
	Int32 blkScale;
	Int32 i;
	Int32 j;
	Int32 k;
	Int32 l;
	Int32 *sfile = (Int32*)this->ciuFile;
	Int32 *databuff;
	UInt8 *strRec;
	Int32 *hdr;
	Int32 ofst;
	if (this->cixFile == 0 || this->ciuFile == 0 || this->cipFile == 0 || this->blkFile == 0)
		return 0;
	blkCnt = *sfile++;
	blkScale = *sfile++;
	blkx = (Int32)(xpos / blkScale);
	blky = (Int32)(ypos / blkScale);

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
	if (j & 0x80000000)
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
				strRec = &this->ciuFile[ofst];
				if (strRec[4] == 0)
				{
					ofst += 5;
					continue;
				}
				ofst += 5 + strRec[4];

				hdr = (Int32*)&this->cixFile[((*(Int32*)strRec) << 3) + 4];
				if (hdr[0] == *(Int32*)strRec)
				{
					Int32 *pfile = (Int32*)&this->cipFile[hdr[1]];
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
									if ((lastY > ypos) ^ (firstY > ypos))
									{
										if ((lastX - (lastX - firstX) * (lastY - ypos) / (lastY - firstY)) > xpos)
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
							if ((lastY > ypos) ^ (lastPolyPoint[1] > ypos))
							{
								if ((lastX - (lastX - *lastPolyPoint) * (lastY - ypos) / (lastY - lastPolyPoint[1])) > xpos)
									n++;
							}
						}
						lastX = lastPolyPoint[0];
						lastY = lastPolyPoint[1];
						lastPolyPoint += 2;
						o++;
					}

					if ((lastY > ypos) ^ (firstY > ypos))
					{
						if ((lastX - (lastX - firstX) * (lastY - ypos) / (lastY - firstY)) > xpos)
							n++;
					}

					if (n & 1)
					{
						buff = Text::StrUTF16_UTF8(buff, (UTF16Char*)&strRec[5], strRec[4] >> 1);
						*buff = 0;
						return buff;
					}
				}
				else
				{
				}
			}
		}
		k++;
	}
	return 0;
}

UTF8Char *Map::MapLayerData::GetPLLabelD(UTF8Char *buff, Double xposD, Double yposD, Double *xposOut, Double *yposOut)
{
	Double xpos = xposD * 200000.0;
	Double ypos = yposD * 200000.0;
	Int32 blkCnt;
	Int32 blkScale;
	Int32 blkx;
	Int32 blky;
	Int32 blkx1;
	Int32 blky1;
	Int32 blkx2;
	Int32 blky2;
	OSInt buffSize = 0;
	Int32 *sfile = (Int32*)this->ciuFile;
	Int32 *databuff;
	UInt8 *strRec;
	Int32 ofst;
	Int32 i;
	Int32 j;
	Int32 k = 0;

	if (this->cixFile == 0 || this->ciuFile == 0 || this->cipFile == 0 || this->blkFile == 0)
		return 0;
	blkCnt = *sfile++;
	blkScale = *sfile++;
	blkx = (Int32)(xpos / blkScale);
	blky = (Int32)(ypos / blkScale);
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


	Int32 foundRec;
	Int32 currFound;
	Double dist = 63781370;
	foundRec = 0;
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
				strRec = &this->ciuFile[ofst];

				if (strRec[4] == 0)
				{
					ofst += 5;
					continue;
				}
				ofst += 5 + strRec[4];

				databuff = (Int32*)&this->cixFile[((*(UInt32*)strRec) << 3) + 4];
				pBuff = (Int32*)&this->cipFile[databuff[1]];
				nParts = pBuff[1];
				parts = &pBuff[2];
				nPoints = parts[nParts];
				points = &parts[nParts + 1];

				i = nParts;
				j = nPoints;

				currFound = 0;
				Double calBase;
				Double calH;
				Double calW;
				Double calX = 0;
				Double calY = 0;
				Double calD;
				Double calPtX = 0.0;
				Double calPtY = 0.0;
				Int32 part;

				while (i--)
				{
					part = parts[i];
					j--;
					while (j-- > part)
					{
						calH = points[(j << 1) + 1] - (Double)points[(j << 1) + 3];
						calW = points[(j << 1) + 0] - (Double)points[(j << 1) + 2];

						if (calH == 0)
						{
							calX = (Double)xpos;
						}
						else
						{
							calX = (calBase = (calW * calW)) * xpos;
							calBase += calH * calH;
							calX += calH * calH * (points[(j << 1) + 0]);
							calX += (ypos - (Double)points[(j << 1) + 1]) * calH * calW;
							calX /= calBase;
						}

						if (calW == 0)
						{
							calY = (Double)ypos;
						}
						else
						{
							calY = ((calX - (Double)(points[(j << 1) + 0])) * calH / calW) + (Double)points[(j << 1) + 1];
						}

						if (calW < 0)
						{
							if (points[(j << 1) + 0] > calX)
								continue;
							if (points[(j << 1) + 2] < calX)
								continue;
						}
						else
						{
							if (points[(j << 1) + 0] < calX)
								continue;
							if (points[(j << 1) + 2] > calX)
								continue;
						}

						if (calH < 0)
						{
							if (points[(j << 1) + 1] > calY)
								continue;
							if (points[(j << 1) + 3] < calY)
								continue;
						}
						else
						{
							if (points[(j << 1) + 1] < calY)
								continue;
							if (points[(j << 1) + 3] > calY)
								continue;
						}

						calH = ypos - calY;
						calW = xpos - calX;
						calD = calW * calW + calH * calH;
						if (calD < dist)
						{
							currFound = 1;
							dist = (UInt32)calD;
							calPtX = calX;
							calPtY = calY;
						}
					}
				}
				j = nPoints;
				while (j-- > 0)
				{
					calW = xpos - points[(j << 1) + 0];
					calH = ypos - points[(j << 1) + 1];
					calD = calW * calW + calH * calH;
					if (calD < dist)
					{
						currFound = 1;
						dist = (UInt32)calD;
						calPtX = calX;
						calPtY = calY;
					}
				}
				if (currFound)
				{
					foundRec = 1;
					buffSize = Text::StrUTF16_UTF8(buff, (UTF16Char*)&strRec[5], strRec[4] >> 1) - buff;
					buff[buffSize] = 0;
					if (xposOut)
					{
						*xposOut = calPtX / 200000.0;
						*yposOut = calPtY / 200000.0;
					}
				}
			}
		}
		k += 4;
	}

	if (foundRec)
	{
		return &buff[buffSize];
	}
	return 0;
}
