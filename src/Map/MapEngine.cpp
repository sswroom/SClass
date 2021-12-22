#include "Stdafx.h"
#ifdef _WIN64
#undef _WIN64
#endif

#include <stdio.h>
#include <windows.h>
#include <math.h>
//#include <memory.h>

#include "MyMemory.h"
#include "Data/IComparable.h"
#include "Data/Sort/QuickBubbleSort.h"
#include "Sync/Event.h"
#include "IO/Stream.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Text/Encoding.h"
#include "Media/DrawEngine.h"
#include "Media/GDEngine.h"
#include "Media/GDIEngine.h"
#include "Map/MapEngine.h"

#define GENGINE Media::GDIEngine

#define Xdpi 72
#define Ydpi 72
#define BOTTOMBAR 0
#define PI 3.1415926535898

#define SFLG_ROTATE 1
#define SFLG_SMART 2
#define SFLG_ALIGN 4

#define LBLMINDIST 150

#define MakeFileName(fname, fileName, suffix) {WChar *cTmp1 = fname; const WChar *cTmp2 = fileName; while ((*cTmp1++ = *cTmp2++) != 0); cTmp2 = suffix; cTmp1--;while ((*cTmp1++=*cTmp2++) != 0);}
#define cardNo 0
#define FileRef FILE *
#ifndef _WIN32_WCE
	#include <share.h>
#endif
#define Err Bool
#define vfsModeRead L"rb"
#define VFSFileSeek(file, origin, pos) fseek(file, pos, origin)
#define vfsOriginBeginning SEEK_SET
#define vfsOriginCurrent SEEK_CUR

Bool VFSFileOpen(Int32 cn, WChar *fName, const WChar *mode, FileRef *ptr)
{
#ifndef _WIN32_WCE
	*(ptr) = _wfsopen(fName, mode, _SH_DENYNO);
	if (*ptr == 0)
	{
		*ptr = 0;
	}
#else
	*(ptr) = _wfopen(fName, mode);
#endif
	return *ptr == 0;
}

Bool VFSFileRead(FileRef file, UInt32 size, void *ptr, UInt32 *outSize)
{
	OSInt retSize;
	if (outSize != 0)
		retSize = (*((size_t*)outSize) = (UInt32)fread(ptr, 1, size, file));
	else
		retSize = fread(ptr, 1, size, file);
	return retSize == 0;
}

void VFSFileClose(FileRef file)
{
	fclose(file);
}


typedef struct
{
	Int32 fontType;
	Int32 size;
	Int32 thickness;
	Int32 color;
	Media::DrawFont *font;
	void *other;
} MyFont;
/*
fontType:
0 - Normal Font
1 - Font background
2 - Font border
3 - Bitmap
4 - Draw Thick Font
*/

typedef struct
{
	Int32 lineType;
	Int32 width;
	Int32 color;
	Int32 nStyle;
	UInt8* styles;
	Media::DrawPen *pen;
} MyLine;
/*
lineType:
0 - solid line, nStyle and styles are not using
1 - solid line width is scale based
2 - user style lines
*/

typedef struct
{
	Int32 nStyles;
	void *styles;
} MyStyle;

typedef struct
{
	WChar *label;
	Int32 xPos;
	Int32 yPos;
	Int32 fontStyle;
	Int32 scaleW;
	Int32 scaleH;
	Int32 priority;
	Int32 totalSize;
	Int32 currSize;
	Int32 nPoints;
	Int32 shapeType;
	Int32 *points;
	Int32 flags;
} MyLabel;

typedef struct
{
	Int32 canDraw;
	Int32 scnWidth;
	Int32 scnHeight;
	Int32 mapPosX;
	Int32 mapPosY;
	Int32 mapScale;

	Int32 left;
	Int32 top;
	Int32 right;
	Int32 bottom;
	Int32 ScaleWidth;
	Int32 ScaleHeight;

	Int32 nMapFonts;
	MyStyle* mapFonts;
	Int32 nMapLines;
	MyStyle* mapLines;
	Int32 nMapLabels;
	Int32 currLabels;
	MyLabel* mapLabels;

	Media::DrawImage *img;
	Media::DrawEngine *eng;
} MapVars;

void ReleaseLine(MapVars *map, MyStyle *style)
{
	MyLine *lines = (MyLine*)style->styles;
	if (lines)
	{
		Int32 i = style->nStyles;
		while (i--)
		{
			if (lines[i].styles)
				MemFree(lines[i].styles);
			if (lines[i].pen)
			{
				map->img->DelPen(lines[i].pen);
			}
		}
		MemFree(lines);
	}
	style->styles = 0;
}

void ReleaseFont(MapVars *map, MyStyle *style)
{
	MyFont *fonts = (MyFont*)style->styles;
	if (fonts)
	{
		Int32 i = style->nStyles;
		while (i--)
		{
			if (fonts[i].font)
			{
				map->img->DelFont(fonts[i].font);
			}
			if (fonts[i].other)
			{
				if (fonts[i].fontType == 0)
				{
					map->img->DelBrush((Media::DrawBrush*)fonts[i].other);
				}
				else if (fonts[i].fontType == 1)
				{
					map->img->DelBrush((Media::DrawBrush*)fonts[i].other);
				}
				else if (fonts[i].fontType == 2)
				{
					map->img->DelPen((Media::DrawPen*)fonts[i].other);
				}
				else if (fonts[i].fontType == 3)
				{
					map->img->DelBrush((Media::DrawBrush*)fonts[i].other);
				}
				else if (fonts[i].fontType == 4)
				{
					map->img->DelBrush((Media::DrawBrush*)fonts[i].other);
				}
			}
		}
		MemFree(fonts);
	}
	style->styles = 0;
}

void ReleaseLabel(MapVars *map, MyLabel *label)
{
	if (label->label)
		MemFree(label->label);
	if (label->points)
		MemFree(label->points);
	label->label = 0;
	label->points = 0;
}

Int32 LabelOverlapped(Int32 *points, Int32 nPoints, Int32 tlx, Int32 tly, Int32 brx, Int32 bry)
{
	while (nPoints--)
	{
		if (points[(nPoints << 2) + 0] < brx && points[(nPoints << 2) + 2] > tlx && points[(nPoints << 2) + 1] < bry && points[(nPoints << 2) + 3] > tly) return 1;
	}

	return 0;
}

Int32 NewLabel(MapVars *map, Int32 priority)
{
	Int32 minPriority;
	Int32 i;
	Int32 j;
	Int32 k;

	if (map->currLabels >= map->nMapLabels)
	{
		i = map->currLabels;
		minPriority = map->mapLabels[0].priority;
		j = -1;
		k = map->mapLabels[0].totalSize;
		while (i--)
		{
			if (map->mapLabels[i].priority < minPriority)
			{
				minPriority = map->mapLabels[i].priority;
				j = i;
				k = map->mapLabels[i].totalSize;
			}
			else if (map->mapLabels[i].priority == minPriority)
			{
				if (map->mapLabels[i].totalSize < k)
				{
					j = i;
					k = map->mapLabels[i].totalSize;
				}
			}
		}
		if (j < 0)
			return j;
		if (map->mapLabels[j].label)
			MemFree(map->mapLabels[j].label);
		if (map->mapLabels[j].points)
			MemFree(map->mapLabels[j].points);
		map->mapLabels[j].label = 0;
		map->mapLabels[j].points = 0;
		map->mapLabels[j].priority = priority;
		return j;
	}
	else
	{
		i = map->currLabels++;
		map->mapLabels[i].priority = priority;
		map->mapLabels[i].label = 0;
		map->mapLabels[i].points = 0;
		return i;
	}
}

void AddLabel(MapVars *map, WChar *label, Int32 lblSize, Int32 nPoint, Int32 *points, Int32 priority, Int32 recType, Int32 fontStyle, Int32 flags)
{
	Int32 size;
	Int32 visibleSize;

	Int32 i;
	Int32 j;

	WChar *src;
	WChar *dest;
	Int32 *pInt;

	Int32 scnX;
	Int32 scnY;
	Int32 found;
	if (recType == 1) //Point
	{
		found = 0;
		i = 0;
		while (i < map->currLabels)
		{
			if (recType == map->mapLabels[i].shapeType)
			{
				src = map->mapLabels[i].label;
				dest = label;
				j = (lblSize >> 1);
				while (j-- && *src)
					if (*src++ != *dest++)
						break;
				if (!(*src || (j >= 0)))
				{
					found = 1;

					pInt = points;
					j = nPoint;
					while (j--)
					{
						scnX = map->mapPosX - *pInt++;
						scnY = map->mapPosY - *pInt++;
						scnX = scnX * scnX + scnY * scnY;
						if (scnX < map->mapLabels[i].currSize)
						{
							map->mapLabels[i].xPos = pInt[-2];
							map->mapLabels[i].yPos = pInt[-1];
							map->mapLabels[i].fontStyle = fontStyle;
							map->mapLabels[i].scaleW = 0;
							map->mapLabels[i].scaleH = 0;
							map->mapLabels[i].currSize = scnX;
							if (priority > map->mapLabels[i].priority)
								map->mapLabels[i].priority = priority;
						}
						map->mapLabels[i].totalSize++;
					}
					break;
				}
			}
			i++;
		}

		if (found == 0)
		{
			i = -1;
			pInt = points;
			j = nPoint;
			while (j--)
			{
				if (pInt[0] >= map->left && pInt[0] < map->right && pInt[1] >= map->top && pInt[1] < map->bottom)
				{
					found = 1;

					i = NewLabel(map, priority);
					if (i < 0)
						return;

//					src = label;
//					while (*src++);
					dest = map->mapLabels[i].label = MemAlloc(WChar, (lblSize >> 1) + 1);
					src = label;
					j = (lblSize >> 1);
					while (j--)
						*dest++ = *src++;
					*dest++=0;

					map->mapLabels[i].xPos = pInt[0];
					map->mapLabels[i].yPos = pInt[1];
					map->mapLabels[i].fontStyle = fontStyle;
					map->mapLabels[i].scaleW = 0;
					map->mapLabels[i].scaleH = 0;
					map->mapLabels[i].priority = priority;

					scnX = map->mapPosX - pInt[0];
					scnY = map->mapPosY - pInt[1];
					scnX = scnX * scnX + scnY * scnY;

					map->mapLabels[i].currSize = scnX;
					map->mapLabels[i].totalSize = nPoint;
					map->mapLabels[i].nPoints = 0;
					map->mapLabels[i].shapeType = 1;
					map->mapLabels[i].points = 0;
					map->mapLabels[i].flags = flags;

					pInt = points;
					j = nPoint;
					while (j--)
					{
						scnX = map->mapPosX - *pInt++;
						scnY = map->mapPosY - *pInt++;
						scnX = scnX * scnX + scnY * scnY;
						if (scnX < map->mapLabels[i].currSize)
						{
							map->mapLabels[i].xPos = pInt[-2];
							map->mapLabels[i].yPos = pInt[-1];
							map->mapLabels[i].fontStyle = fontStyle;
							map->mapLabels[i].scaleW = 0;
							map->mapLabels[i].scaleH = 0;
							map->mapLabels[i].currSize = scnX;
							if (priority > map->mapLabels[i].priority)
								map->mapLabels[i].priority = priority;
						}
					}

					break;
				}
				pInt += 2;
			}
		}
	}
	else if (recType == 3) //lines
	{
		Int32 tmp;
		Int32 lastPtX;
		Int32 lastPtY = points[1];
		Int32 thisPtX;
		Int32 thisPtY;

		Int32 toUpdate;
//		lastPtIn = (lastPtX >= left && lastPtX < right && lastPtY >= top && lastPtY < bottom);

		visibleSize = 0;
		size = 0;

		i = 1;
		while (i < nPoint)
		{
			lastPtX = points[(i << 1) - 2];
			lastPtY = points[(i << 1) - 1];
			thisPtX = points[(i << 1) + 0];
			thisPtY = points[(i << 1) + 1];
			if (lastPtX > thisPtX)
			{
				tmp = lastPtX;
				lastPtX = thisPtX;
				thisPtX = tmp;
			}
			if (lastPtY > thisPtY)
			{
				tmp = lastPtY;
				lastPtY = thisPtY;
				thisPtY = tmp;
			}

			if ((thisPtY - lastPtY) > (thisPtX - lastPtX))
				size += thisPtY - lastPtY;
			else
				size += thisPtX - lastPtX;

			if (map->left < thisPtX && map->right > lastPtX && map->top < thisPtY && map->bottom > lastPtY)
			{
				if (map->left > lastPtX)
				{
					lastPtY += MulDiv(map->left - lastPtX, thisPtY - lastPtY, thisPtX - lastPtX);
					lastPtX = map->left;
				}
				if (map->top > lastPtY)
				{
					lastPtX += MulDiv(map->top - lastPtY, thisPtX - lastPtX, thisPtY - lastPtY);
					lastPtY = map->top;
				}
				if (map->right < thisPtX)
				{
					thisPtY += MulDiv(map->right - lastPtX, thisPtY - lastPtY, thisPtX - lastPtX);
					thisPtX = map->right;
				}
				if (map->bottom < thisPtY)
				{
					thisPtX += MulDiv(map->bottom - lastPtY, thisPtX - lastPtX, thisPtY - lastPtY);
					thisPtY = map->bottom;
				}

				if ((thisPtY - lastPtY) > (thisPtX - lastPtX))
					visibleSize += thisPtY - lastPtY;
				else
					visibleSize += thisPtX - lastPtX;

			}
			i++;
		}

		toUpdate = 0;
		found = 0;
		long totalSize = 0;
		long foundInd;
		i = 0;

		while (i < map->currLabels)
		{
			if (recType == map->mapLabels[i].shapeType)
			{
				src = map->mapLabels[i].label;
				dest = label;
				j = lblSize >> 1;
				while (j-- && *src)
					if (*src++ != *dest++)
						break;
				if (!(*src || (j >= 0)))
				{
					found++;

					if (totalSize == 0)
					{
						map->mapLabels[i].totalSize += size;
						totalSize = map->mapLabels[i].totalSize;
					}
					else
					{
						map->mapLabels[i].totalSize = totalSize;
					}


					if (priority > map->mapLabels[i].priority)
						map->mapLabels[i].priority = priority;
					else
						priority = map->mapLabels[i].priority;

					if (map->mapLabels[i].currSize < visibleSize)
					{
						toUpdate = 1;
					}

					if (found >= 2)
					{
					}
					else if (map->mapLabels[i].points[0] == points[(nPoint << 1) - 2] && map->mapLabels[i].points[1] == points[(nPoint << 1) - 1])
					{
						Int32 newSize = map->mapLabels[i].nPoints + nPoint - 1;
						Int32* newArr = MemAlloc(Int32, newSize << 1);
						Int32 k;
						Int32 l;
						l = 0;
						k = 0;
						while (k < map->mapLabels[i].nPoints)
						{
							newArr[l++] = map->mapLabels[i].points[k << 1];
							newArr[l++] = map->mapLabels[i].points[(k << 1) + 1];
							k++;
						}
						k = 1;
						while (k < nPoint)
						{
							newArr[l++] = points[k << 1];
							newArr[l++] = points[(k << 1) + 1];
							k++;
						}
						MemFree(map->mapLabels[i].points);
						map->mapLabels[i].points = newArr;
						map->mapLabels[i].nPoints = newSize;
						toUpdate = 1;
						foundInd = i;
					}
					else if (map->mapLabels[i].points[(map->mapLabels[i].nPoints << 1) - 2] == points[0] && map->mapLabels[i].points[(map->mapLabels[i].nPoints << 1) - 1] == points[1])
					{
						Int32 newSize = map->mapLabels[i].nPoints + nPoint - 1;
						Int32* newArr = MemAlloc(Int32, newSize << 1);
						Int32 k;
						Int32 l;
						l = 0;
						k = 0;
						while (k < nPoint)
						{
							newArr[l++] = points[k << 1];
							newArr[l++] = points[(k << 1) + 1];
							k++;
						}
						k = 1;
						while (k < map->mapLabels[i].nPoints)
						{
							newArr[l++] = map->mapLabels[i].points[k << 1];
							newArr[l++] = map->mapLabels[i].points[(k << 1) + 1];
							k++;
						}
						MemFree(map->mapLabels[i].points);
						map->mapLabels[i].points = newArr;
						map->mapLabels[i].nPoints = newSize;
						toUpdate = 1;
						foundInd = i;
					}
					else 
					{
						found = 0;
					}
				}
			}
			i++;
		}
		if (found)
			i = foundInd;
		if (found == 0 && visibleSize > 0)
		{
			i = NewLabel(map, priority);

			if (i < 0)
				return;

			map->mapLabels[i].shapeType = recType;
			map->mapLabels[i].fontStyle = fontStyle;
			map->mapLabels[i].totalSize = size;
			map->mapLabels[i].currSize = visibleSize;

			src = label;
			dest = map->mapLabels[i].label = MemAlloc(WChar, j = lblSize);
			j = j >> 1;
			while (j--)
				*dest++ = *src++;
			*dest = 0;

			map->mapLabels[i].points = 0;

			toUpdate = 1;
		}

		if (toUpdate)
		{
			/////////////////////////////////////////////////////////////////////////////////
			j = map->mapLabels[i].nPoints = nPoint;
			if (map->mapLabels[i].points)
				MemFree(map->mapLabels[i].points);
			map->mapLabels[i].points = pInt = MemAlloc(Int32, nPoint * 2);
			j = j << 1;
			long k = 0;
			while (k < j)
				*pInt++ = points[k++];

			visibleSize = visibleSize >> 1;

			j = 1;
			while (j < nPoint)
			{
				lastPtX = points[(j << 1) - 2];
				lastPtY = points[(j << 1) - 1];
				thisPtX = points[(j << 1) + 0];
				thisPtY = points[(j << 1) + 1];
				if (lastPtX > thisPtX)
				{
					tmp = lastPtX;
					lastPtX = thisPtX;
					thisPtX = tmp;
				}
				if (lastPtY > thisPtY)
				{
					tmp = lastPtY;
					lastPtY = thisPtY;
					thisPtY = tmp;
				}

				if (map->left < thisPtX && map->right > lastPtX && map->top < thisPtY && map->bottom > lastPtY)
				{
					lastPtX = points[(j << 1) - 2];
					lastPtY = points[(j << 1) - 1];
					thisPtX = points[(j << 1) + 0];
					thisPtY = points[(j << 1) + 1];
					if (map->left > lastPtX)
					{
						lastPtY += MulDiv(map->left - lastPtX, thisPtY - lastPtY, thisPtX - lastPtX);
						lastPtX = map->left;
					}
					else if (lastPtX > map->right)
					{
						lastPtY += MulDiv(map->right - lastPtX, thisPtY - lastPtY, thisPtX - lastPtX);
						lastPtX = map->right;
					}

					if (map->top > lastPtY)
					{
						lastPtX += MulDiv(map->top - lastPtY, thisPtX - lastPtX, thisPtY - lastPtY);
						lastPtY = map->top;
					}
					else if (lastPtY > map->bottom)
					{
						lastPtX += MulDiv(map->bottom - lastPtY, thisPtX - lastPtX, thisPtY - lastPtY);
						lastPtY = map->bottom;
					}

					if (thisPtX < map->left)
					{
						thisPtY += MulDiv(map->left - thisPtX, thisPtY - lastPtY, thisPtX - lastPtX);
						thisPtX = map->left;
					}
					else if (map->right < thisPtX)
					{
						thisPtY += MulDiv(map->right - thisPtX, thisPtY - lastPtY, thisPtX - lastPtX);
						thisPtX = map->right;
					}

					if (map->top > thisPtY)
					{
						thisPtX += MulDiv(map->top - thisPtY, thisPtX - lastPtX, thisPtY - lastPtY);
						thisPtY = map->top;
					}
					else if (map->bottom < thisPtY)
					{
						thisPtX += MulDiv(map->bottom - thisPtY, thisPtX - lastPtX, thisPtY - lastPtY);
						thisPtY = map->bottom;
					}

					Int32 diffX = (thisPtX - lastPtX);
					Int32 diffY = (thisPtY - lastPtY);
					if (diffX < 0) diffX = -diffX;
					if (diffY < 0) diffY = -diffY;

					if (diffY > diffX)
					{
						if (visibleSize > diffY)
						{
							visibleSize -= diffY;
						}
						else
						{
							if (flags & SFLG_ROTATE)
							{
								map->mapLabels[i].scaleW = points[(j << 1)] - points[(j << 1) - 2];
								map->mapLabels[i].scaleH = points[(j << 1) + 1] - points[(j << 1) - 1];
							}
							else
							{
								map->mapLabels[i].scaleW = 0;
								map->mapLabels[i].scaleH = 0;
							}

							map->mapLabels[i].flags = flags;

							if (points[(j << 1) + 1] > points[(j << 1) - 1])
							{
								thisPtY = lastPtY + visibleSize;
							}
							else
							{
								thisPtY = lastPtY - visibleSize;
							}
							thisPtX = points[(j << 1) - 2] + MulDiv(thisPtY - points[(j << 1) - 1], points[(j << 1)] - points[(j << 1) - 2], points[(j << 1) + 1] - points[(j << 1) - 1]);

							map->mapLabels[i].xPos = thisPtX;
							map->mapLabels[i].yPos = thisPtY;
							visibleSize = -1;
							break;
						}
					}
					else
					{
						if (visibleSize > diffX)
						{
							visibleSize -= diffX;
						}
						else
						{
							if (flags & SFLG_ROTATE)
							{
								map->mapLabels[i].scaleW = points[(j << 1)] - points[(j << 1) - 2];
								map->mapLabels[i].scaleH = points[(j << 1) + 1] - points[(j << 1) - 1];
							}
							else
							{
								map->mapLabels[i].scaleW = 0;
								map->mapLabels[i].scaleH = 0;
							}

							map->mapLabels[i].flags = flags;

							if (points[(j << 1)] > points[(j << 1) - 2])
							{
								thisPtX = lastPtX + visibleSize;
							}
							else
							{
								thisPtX = lastPtX - visibleSize;
							}
							thisPtY = points[(j << 1) + 1] + MulDiv(thisPtX - points[(j << 1)], points[(j << 1) + 1] - points[(j << 1) - 1], points[(j << 1)] - points[(j << 1) - 2]);

							map->mapLabels[i].xPos = thisPtX;
							map->mapLabels[i].yPos = thisPtY;
							visibleSize = -1;
							break;
						}
					}
				}
				j++;
			}
			if (visibleSize > 0)
			{
				j = nPoint - 1;
				if (flags & SFLG_ROTATE)
				{
					map->mapLabels[i].scaleW = points[(j << 1)] - points[(j << 1) - 2];
					map->mapLabels[i].scaleH = points[(j << 1) + 1] - points[(j << 1) - 1];
				}
				else
				{
					map->mapLabels[i].scaleW = 0;
					map->mapLabels[i].scaleH = 0;
				}

				map->mapLabels[i].flags = flags;

				if (points[(j << 1)] > points[(j << 1) - 2])
				{
					thisPtX = lastPtX + visibleSize;
				}
				else
				{
					thisPtX = lastPtX - visibleSize;
				}
				thisPtY = points[(j << 1) + 1] + MulDiv(thisPtX - points[(j << 1)], points[(j << 1) + 1] - points[(j << 1) - 1], points[(j << 1)] - points[(j << 1) - 2]);

				map->mapLabels[i].xPos = thisPtX;
				map->mapLabels[i].yPos = thisPtY;
				visibleSize = -1;
			}
		}
	}
	else if (recType == 5) //polygon
	{
		/*
Int32 size;
Int32 visibleSize;

Int32 i;
Int32 j;

WChar *src;
WChar *dest;
Int32 *pInt;

Int32 scnX;
Int32 scnY;
Int32 found;
*/
		if (nPoint <= 2)
			return;

		found = 0;
		i = 0;
		while (i < map->currLabels)
		{
			if (recType == map->mapLabels[i].shapeType)
			{
				src = map->mapLabels[i].label;
				dest = label;
				j = (lblSize >> 1);
				while (j-- && *src)
					if (*src++ != *dest++)
						break;
				if (!(*src || (j >= 0)))
				{
					found = 1;

		/*			pInt = points;
					j = nPoint;
					while (j--)
					{
						scnX = mapPosX - *pInt++;
						scnY = mapPosY - *pInt++;
						scnX = scnX * scnX + scnY * scnY;
						if (scnX < map->mapLabels[i].currSize)
						{
							map->mapLabels[i].xPos = pInt[-2];
							map->mapLabels[i].yPos = pInt[-1];
							map->mapLabels[i].fontStyle = fontStyle;
							map->mapLabels[i].scaleW = 0;
							map->mapLabels[i].scaleH = 0;
							map->mapLabels[i].currSize = scnX;
							if (priority > map->mapLabels[i].priority)
								map->mapLabels[i].priority = priority;
						}
						map->mapLabels[i].totalSize++;
					}*/
					break;
				}
			}
			i++;
		}


/* old
		if (found == 0)
		{
			i = -1;
			pInt = points;
			j = nPoint;
			__int64 totalX;
			__int64 totalY;
			totalX = 0;
			totalY = 0;
			Int32 centX;
			Int32 centY;

			while (j--)
			{
				totalX += pInt[0];
				totalY += pInt[1];

				pInt += 2;
			}
			centX = (Int32)(totalX / nPoint);
			centY = (Int32)(totalY / nPoint);

			if (centX >= left && centX < right && centY >= top && centY < bottom)
			{
				found = 1;

				i = newLabel(priority);
				if (i < 0)
					return;

				src = label;
				while (*src++);
				dest = map->mapLabels[i].label = (WChar*) MemPtrNew(((lblSize >> 1) + 1) * sizeof(label[0]));
				src = label;
				j = (lblSize >> 1);
				while (j--)
					*dest++ = *src++;
				*dest++=0;

				map->mapLabels[i].xPos = centX;
				map->mapLabels[i].yPos = centY;
				map->mapLabels[i].fontStyle = fontStyle;
				map->mapLabels[i].scaleW = 0;
				map->mapLabels[i].scaleH = 0;
				map->mapLabels[i].priority = priority;

				scnX = mapPosX - pInt[0];
				scnY = mapPosY - pInt[1];
				scnX = scnX * scnX + scnY * scnY;

				map->mapLabels[i].currSize = nPoint;
				map->mapLabels[i].totalSize = nPoint;
				map->mapLabels[i].nPoints = nPoint;
				map->mapLabels[i].shapeType = 5;
				map->mapLabels[i].points = 0;
				map->mapLabels[i].flags = flags;
			}

		}*/


		if (found == 0)
		{
			Int32 startIndex;
			Int32 lastX;
			Int32 lastY;
			Int32 lastTX;
			Int32 lastTY;
			Int32 thisX;
			Int32 thisY;
			Int32 thisTX;
			Int32 thisTY;
			Int32* outPts;
			Int32 outPtCnt;
			Int64 sum;
			Int64 sumX;
			Int64 sumY;

			Int32 crossList[8];
			Int32 crossCnt;
			Int32 inCnt;
			Int32 tmp;
			Int32 tmp2;
			outPts = MemAlloc(Int32, nPoint << 2);
			outPtCnt = 0;
			startIndex = -1;
			i = 0;
			while (i < nPoint)
			{
				lastX = points[i << 1];
				lastY = points[(i << 1) + 1];
				if (lastX >= map->left && lastX < map->right && lastY >= map->top && lastY < map->bottom)
				{
					lastTX = lastX;
					lastTY = lastY;
					startIndex = i;
					break;
				}
				i++;
			}
			if (startIndex == -1)
			{
				startIndex = 0;
				lastTX = lastX = points[0];
				lastTY = lastY = points[1];
				if (lastTX < map->left) lastTX = map->left;
				if (lastTX >= map->right) lastTX = map->right;
				if (lastTY < map->top) lastTY = map->top;
				if (lastTY >= map->bottom) lastTY = map->bottom;
			}
			outPts[(outPtCnt << 1)] = lastTX;
			outPts[(outPtCnt << 1)+1] = lastTY;
			outPtCnt++;

			Int32 prevTX = lastTX;
			Int32 prevTY = lastTY;
			i = (startIndex + 1) % nPoint;
			while (i != startIndex)
			{
				thisX = points[(i << 1)];
				thisY = points[(i << 1) + 1];

				crossCnt = 0;
				inCnt = 0;
				if ((lastX < map->left && thisX >= map->left) || (lastX >= map->left && thisX < map->left))
				{
					crossList[(crossCnt << 1)] = map->left;
					lastTY = crossList[(crossCnt << 1) + 1] = (lastY - thisY) * (map->left - lastX) / (lastX - thisX) + lastY;
					if (lastTY >= map->top && lastTY <= map->bottom)
						inCnt++;
					crossCnt++;
				}

				if ((lastX < map->right && thisX >= map->right) || (lastX >= map->right && thisX < map->right))
				{
					crossList[(crossCnt << 1)] = map->right;
					lastTY = crossList[(crossCnt << 1) + 1] = (lastY - thisY) * (map->right - lastX) / (lastX - thisX) + lastY;
					if (lastTY >= map->top && lastTY <= map->bottom)
						inCnt++;
					crossCnt++;
				}

				if ((lastY < map->top && thisY >= map->top) || (lastY >= map->top && thisY < map->top))
				{
					lastTX = crossList[(crossCnt << 1)] = (lastX - thisX) * (map->top - lastY) / (lastY - thisY) + lastX;
					crossList[(crossCnt << 1) + 1] = map->top;
					if (lastTX >= map->left && lastTX <= map->right)
						inCnt++;
					crossCnt++;
				}

				if ((lastY < map->bottom && thisY >= map->bottom) || (lastY >= map->bottom && thisY < map->bottom))
				{
					lastTX = crossList[(crossCnt << 1)] = (lastX - thisX) * (map->bottom - lastY) / (lastY - thisY) + lastX;
					crossList[(crossCnt << 1) + 1] = map->bottom;
					if (lastTX >= map->left && lastTX <= map->right)
						inCnt++;
					crossCnt++;
				}

				if (crossCnt == 0)
				{
					lastTX = thisX;
					lastTY = thisY;
					if (lastTX < map->left) lastTX = map->left;
					if (lastTX >= map->right) lastTX = map->right;
					if (lastTY < map->top) lastTY = map->top;
					if (lastTY >= map->bottom) lastTY = map->bottom;

					if ((prevTX != lastTX) || (prevTY != lastTY))
					{
						outPts[(outPtCnt << 1)] = lastTX;
						outPts[(outPtCnt << 1)+1] = lastTY;
						outPtCnt++;
						prevTX = lastTX;
						prevTY = lastTY;
					}
				}
				else if (crossCnt == 1)
				{
					lastTX = crossList[0];
					lastTY = crossList[1];
					if (lastTX < map->left) lastTX = map->left;
					if (lastTX >= map->right) lastTX = map->right;
					if (lastTY < map->top) lastTY = map->top;
					if (lastTY >= map->bottom) lastTY = map->bottom;

					if ((prevTX != lastTX) || (prevTY != lastTY))
					{
						outPts[(outPtCnt << 1)] = lastTX;
						outPts[(outPtCnt << 1)+1] = lastTY;
						outPtCnt++;
						prevTX = lastTX;
						prevTY = lastTY;
					}

					lastTX = thisX;
					lastTY = thisY;
					if (lastTX < map->left) lastTX = map->left;
					if (lastTX >= map->right) lastTX = map->right;
					if (lastTY < map->top) lastTY = map->top;
					if (lastTY >= map->bottom) lastTY = map->bottom;

					if ((prevTX != lastTX) || (prevTY != lastTY))
					{
						outPts[(outPtCnt << 1)] = lastTX;
						outPts[(outPtCnt << 1)+1] = lastTY;
						outPtCnt++;
						prevTX = lastTX;
						prevTY = lastTY;
					}

				}
				else
				{
					tmp2 = crossCnt;
					while (tmp2 > 0)
					{
						tmp = 1;
						while (tmp < tmp2)
						{
							lastTX = crossList[(tmp << 1) - 2] - lastX;
							lastTY = crossList[(tmp << 1) - 1] - lastY;

							thisTX = crossList[(tmp << 1) ] - lastX;
							thisTY = crossList[(tmp << 1) + 1] - lastY;

							if (((__int64)lastTX * lastTX + (__int64)lastTY * lastTY) > ((__int64)thisTX * thisTX + (__int64)thisTY + thisTY))
							{
								lastTX = crossList[(tmp << 1) - 2];
								lastTY = crossList[(tmp << 1) - 1];
								crossList[(tmp << 1) - 2] = crossList[(tmp << 1) ];
								crossList[(tmp << 1) - 1] = crossList[(tmp << 1) + 1];
								crossList[(tmp << 1)] = lastTX;
								crossList[(tmp << 1) + 1] = lastTY;
							}
							tmp++;
						}
						tmp2--;
					}
					if (inCnt == 0)
					{
						lastTX = crossList[0];
						lastTY = crossList[1];
						if (lastTX < map->left) lastTX = map->left;
						if (lastTX >= map->right) lastTX = map->right;
						if (lastTY < map->top) lastTY = map->top;
						if (lastTY >= map->bottom) lastTY = map->bottom;

						if ((prevTX != lastTX) || (prevTY != lastTY))
						{
							outPts[(outPtCnt << 1)] = lastTX;
							outPts[(outPtCnt << 1)+1] = lastTY;
							outPtCnt++;
							prevTX = lastTX;
							prevTY = lastTY;
						}


						lastTX = thisX;
						lastTY = thisY;
						if (lastTX < map->left) lastTX = map->left;
						if (lastTX >= map->right) lastTX = map->right;
						if (lastTY < map->top) lastTY = map->top;
						if (lastTY >= map->bottom) lastTY = map->bottom;

						if ((prevTX != lastTX) || (prevTY != lastTY))
						{
							outPts[(outPtCnt << 1)] = lastTX;
							outPts[(outPtCnt << 1)+1] = lastTY;
							outPtCnt++;
							prevTX = lastTX;
							prevTY = lastTY;
						}

					}
					else if (inCnt == 1)
					{
						tmp = 0;
						while (tmp < crossCnt)
						{
							lastTX = crossList[(tmp << 1)];
							lastTY = crossList[(tmp << 1) + 1];
							if (lastTX >= map->left && lastTX <= map->right && lastTY >= map->top && lastTY <= map->bottom)
							{
								if (lastTX < map->left) lastTX = map->left;
								if (lastTX >= map->right) lastTX = map->right;
								if (lastTY < map->top) lastTY = map->top;
								if (lastTY >= map->bottom) lastTY = map->bottom;

								if ((prevTX != lastTX) || (prevTY != lastTY))
								{
									outPts[(outPtCnt << 1)] = lastTX;
									outPts[(outPtCnt << 1)+1] = lastTY;
									outPtCnt++;
									prevTX = lastTX;
									prevTY = lastTY;
								}
								break;
							}
							tmp++;
						}

						lastTX = thisX;
						lastTY = thisY;
						if (lastTX < map->left) lastTX = map->left;
						if (lastTX >= map->right) lastTX = map->right;
						if (lastTY < map->top) lastTY = map->top;
						if (lastTY >= map->bottom) lastTY = map->bottom;

						if ((prevTX != lastTX) || (prevTY != lastTY))
						{
							outPts[(outPtCnt << 1)] = lastTX;
							outPts[(outPtCnt << 1)+1] = lastTY;
							outPtCnt++;
							prevTX = lastTX;
							prevTY = lastTY;
						}
					}
					else if (inCnt == 2)
					{
						tmp = 0;
						while (tmp < crossCnt)
						{
							lastTX = crossList[(tmp << 1)];
							lastTY = crossList[(tmp << 1) + 1];
							if (lastTX >= map->left && lastTX <= map->right && lastTY >= map->top && lastTY <= map->bottom)
							{
								if (lastTX < map->left) lastTX = map->left;
								if (lastTX >= map->right) lastTX = map->right;
								if (lastTY < map->top) lastTY = map->top;
								if (lastTY >= map->bottom) lastTY = map->bottom;

								if ((prevTX != lastTX) || (prevTY != lastTY))
								{
									outPts[(outPtCnt << 1)] = lastTX;
									outPts[(outPtCnt << 1)+1] = lastTY;
									outPtCnt++;
									prevTX = lastTX;
									prevTY = lastTY;
								}
							}
							tmp++;
						}
					}
					else
					{
						//MessageBox(NULL, "Error", "Internal", MB_OK);
					}
				}


				found = 1;
				while (found && (outPtCnt > 3))
				{
					found = 0;
					outPtCnt--;
					if ((outPts[((outPtCnt - 1) << 1)] == outPts[((outPtCnt - 2) << 1)]) && (outPts[((outPtCnt - 2) << 1)] == outPts[((outPtCnt - 3) << 1)]))
					{
						outPts[((outPtCnt - 2) << 1) + 1] = outPts[((outPtCnt - 1) << 1) + 1];
						outPts[((outPtCnt - 1) << 1)] = outPts[((outPtCnt - 0) << 1)];
						outPts[((outPtCnt - 1) << 1) + 1] = outPts[((outPtCnt - 0) << 1) + 1];
						outPtCnt--;
						found = 1;
					}
					if ((outPts[((outPtCnt - 1) << 1) + 1] == outPts[((outPtCnt - 2) << 1) + 1]) && (outPts[((outPtCnt - 2) << 1) + 1] == outPts[((outPtCnt - 3) << 1) + 1]))
					{
						outPts[((outPtCnt - 2) << 1)] = outPts[((outPtCnt - 1) << 1)];
						outPts[((outPtCnt - 1) << 1)] = outPts[((outPtCnt - 0) << 1)];
						outPts[((outPtCnt - 1) << 1) + 1] = outPts[((outPtCnt - 0) << 1) + 1];
						outPtCnt--;
						found = 1;
					}

					outPtCnt++;
					if ((outPts[((outPtCnt - 1) << 1)] == outPts[((outPtCnt - 2) << 1)]) && (outPts[((outPtCnt - 2) << 1)] == outPts[((outPtCnt - 3) << 1)]))
					{
						outPts[((outPtCnt - 2) << 1) + 1] = outPts[((outPtCnt - 1) << 1) + 1];
						outPtCnt--;
						found = 1;
					}
					if ((outPts[((outPtCnt - 1) << 1) + 1] == outPts[((outPtCnt - 2) << 1) + 1]) && (outPts[((outPtCnt - 2) << 1) + 1] == outPts[((outPtCnt - 3) << 1) + 1]))
					{
						outPts[((outPtCnt - 2) << 1)] = outPts[((outPtCnt - 1) << 1)];
						outPtCnt--;
						found = 1;
					}

				}

				lastX = thisX;
				lastY = thisY;

				i = (i + 1) % nPoint;
			}

			i = 0;
			sum = 0;
			sumX = sumY = 0;
			lastX = outPts[(outPtCnt << 1) - 2];
			lastY = outPts[(outPtCnt << 1) - 1];
			while (i < outPtCnt)
			{
				thisX = outPts[(i << 1)];
				thisY = outPts[(i << 1) + 1];

				sum += ((__int64)lastX * thisY) - ((__int64)lastY * thisX);
//				sumX += thisX;
//				sumY += thisY;

				lastX = thisX;
				lastY = thisY;
				i++;
			}
			if (sum != 0)
			{
				Int32 *finalPts;
				Int32 finalCnt;
				Int32 maxX;
				Int32 maxY;
				Int32 minX;
				Int32 minY;
				finalCnt = 0;
				finalPts = MemAlloc(Int32, outPtCnt << 1);
				sumX += maxX = minX = lastX = finalPts[0] = outPts[0];
				sumY += maxY = minY = lastY = finalPts[1] = outPts[1];
				finalCnt++;

				i = 2;
				while (i < outPtCnt)
				{
					thisX = outPts[(i << 1) - 2];
					thisY = outPts[(i << 1) - 1];
					if ((Int64)(outPts[(i << 1)] - lastX) * (lastY - thisY) == (Int64)(outPts[(i << 1) + 1] - lastY) * (lastX - thisX))
					{
						
					}
					else
					{
						sumX += finalPts[(finalCnt << 1)] = thisX;
						sumY += finalPts[(finalCnt << 1) + 1] = thisY;
						if (maxX < thisX)
							maxX = thisX;
						if (minX > thisX)
							minX = thisX;
						if (maxY < thisY)
							maxY = thisY;
						if (minY > thisY)
							minY = thisY;
						finalCnt++;
					}
					lastX = thisX;
					lastY = thisY;
					i++;
				}

				thisX = outPts[(outPtCnt << 1) - 2];
				thisY = outPts[(outPtCnt << 1) - 1];
				sumX += finalPts[(finalCnt << 1)] = thisX;
				sumY += finalPts[(finalCnt << 1) + 1] = thisY;
				finalCnt++;
				if (maxX < thisX)
					maxX = thisX;
				if (minX > thisX)
					minX = thisX;
				if (maxY < thisY)
					maxY = thisY;
				if (minY > thisY)
					minY = thisY;

				lastX = thisX;
				lastY = thisY;
				sum = 0;
				thisY = (maxY + minY) >> 1;
				i = 0;
				while (i < finalCnt)
				{
					thisTX = finalPts[(i << 1)];
					thisTY = finalPts[(i << 1) + 1];
					if ((lastY >= thisY && thisTY < thisY) || (thisTY >= thisY && lastY < thisY))
					{
						thisX = lastX + (thisY - lastY) * (thisTX - lastX) / (thisTY - lastY);
						if (sum == 0)
						{
							minX = thisX;
							maxX = thisX;
						}
						else
						{
							if (thisX > maxX)
								maxX = thisX;
							if (thisX < minX)
								minX = thisX;
						}
						sum = 1;
					}
					lastX = thisTX;
					lastY = thisTY;
					i++;
				}

				
				MemFree(outPts);
				outPts = finalPts;
				outPtCnt = finalCnt;
				
//				thisX = (long)(sumX / outPtCnt);
//				thisY = (long)(sumY / outPtCnt);
				thisX = (maxX + minX) >> 1;
				thisY = (maxY + minY) >> 1;

				i = NewLabel(map, priority);
				if (i < 0)
				{
					MemFree(outPts);
					return;
				}

				src = label;
				while (*src++);
				dest = map->mapLabels[i].label = MemAlloc(WChar, (lblSize >> 1) + 1);
				src = label;
				j = (lblSize >> 1);
				while (j--)
					*dest++ = *src++;
				*dest++=0;

				map->mapLabels[i].xPos = thisX;
				map->mapLabels[i].yPos = thisY;
				map->mapLabels[i].fontStyle = fontStyle;
				map->mapLabels[i].scaleW = 0;
				map->mapLabels[i].scaleH = 0;
				map->mapLabels[i].priority = priority;

				map->mapLabels[i].currSize = outPtCnt;
				map->mapLabels[i].totalSize = outPtCnt;
				map->mapLabels[i].nPoints = outPtCnt;
				map->mapLabels[i].shapeType = 5;
				if (map->mapLabels[i].points)
					MemFree(map->mapLabels[i].points);
				map->mapLabels[i].points = outPts;
				map->mapLabels[i].flags = flags;

/*				Int32 *tmpPts = (Int32*)MemPtrNew(outPtCnt << 3);
				i = 0;
				while (i < outPtCnt)
				{
					tmpPts[(i << 1)] = MulDiv(outPts[(i << 1)] - left, scnWidth, ScaleWidth);
					tmpPts[(i << 1)+1] = MulDiv(bottom - outPts[(i << 1)+1], scnHeight, ScaleHeight);
					
					i++;
				}
				Polygon(MapHDC, (POINT*)tmpPts, outPtCnt);
				MemPtrFree(tmpPts);*/
			}
			else
			{
				MemFree(outPts);
			}
		}















/*

		__int64 currVal = 0;
		Int32* currPts;
		Int32* ptVisible;
		Int32 xIn;
		Int32 yIn;
		Int32 xIn2;
		Int32 yIn2;

		Int32 currX;
		Int32 currY;

		i = nPoint - 1;
		pInt = points;
		while (i--)
		{
			currVal += Int32x32To64(pInt[0], pInt[3]) - Int32x32To64(pInt[1],pInt[2]);
			pInt += 2;
		}
		currVal += Int32x32To64(pInt[0], points[1]) - Int32x32To64(pInt[1],points[0]);
		size = (Int32)(currVal >> 1);

		pInt = currPts = (Int32*)MemPtrNew(nPoint * sizeof(Int32) * 4);

		scnX = points[0];
		scnY = points[1];
		j = 0;
		if (scnX < left)
			xIn = -1;
		else if (scnX >= right)
			xIn = 1;
		else
			xIn = 0;
		if (scnY < top)
			yIn = -1;
		else if (scnY >= bottom)
			yIn = 1;
		else
			yIn = 0;

		if ((xIn | yIn) == 0)
		{
			*pInt++ = points[0];
			*pInt++ = points[1];
		}

		i = nPoint;
		while (i--)
		{
			if (points[(i << 1)] < left)
				xIn2 = -1;
			else if (points[(i << 1)] >= right)
				xIn2 = 1;
			else
				xIn2 = 0;
			if (points[(i << 1) + 1] < top)
				yIn2 = -1;
			else if (points[(i << 1) + 1] >= bottom)
				yIn2 = 1;
			else
				yIn2 = 0;

			while ((yIn != yIn2) || (xIn != xIn2))
			{
				found = 0;
				if (xIn == xIn2)
				{
					if (yIn < 0)
					{
						currY = top;
						currX = scnX + MulDiv(currY - scnY, points[(i << 1)] - scnX, points[(i << 1) + 1] - scnY);
						if (currX < left)
							currX = left;
						if (currX > right)
							currX = right;

						*pInt++ = currX;
						*pInt++ = currY;

						yIn = 0;
					}
					else if (yIn > 0)
					{
						currY = bottom;
						currX = scnX + MulDiv(currY - scnY, points[(i << 1)] - scnX, points[(i << 1) + 1] - scnY);
						if (currX < left)
							currX = left;
						if (currX > right)
							currX = right;

						*pInt++ = currX;
						*pInt++ = currY;
						yIn = 0;
					}

					if (yIn != yIn2)
					{
						if (yIn2 < 0)
						{
							currY = top;
							currX = scnX + MulDiv(currY - scnY, points[(i << 1)] - scnX, points[(i << 1) + 1] - scnY);
							if (currX < left)
								currX = left;
							if (currX > right)
								currX = right;

							*pInt++ = currX;
							*pInt++ = currY;
						}
						else if (yIn2 > 0)
						{
							currY = bottom;
							currX = scnX + MulDiv(currY - scnY, points[(i << 1)] - scnX, points[(i << 1) + 1] - scnY);
							if (currX < left)
								currX = left;
							if (currX > right)
								currX = right;

							*pInt++ = currX;
							*pInt++ = currY;
						}

					}
				}
				else if (yIn == yIn2)
				{
					if (xIn < 0)
					{
						currX = left;
						currY = scnY + MulDiv(currX - scnX, points[(i << 1) + 1] - scnY, points[(i << 1)] - scnX);
						if (currY < top)
							currY = top;
						if (currY > bottom)
							currY = bottom;

						*pInt++ = currX;
						*pInt++ = currY;

						xIn = 0;
					}
					else if (xIn > 0)
					{
						currX = right;
						currY = scnY + MulDiv(currX - scnX, points[(i << 1) + 1] - scnY, points[(i << 1)] - scnX);
						if (currY < top)
							currY = top;
						if (currY > bottom)
							currY = bottom;

						*pInt++ = currX;
						*pInt++ = currY;

						xIn = 0;
					}

					if (xIn != xIn2)
					{
						if (xIn2 < 0)
						{
							currX = left;
							currY = scnY + MulDiv(currX - scnX, points[(i << 1) + 1] - scnY, points[(i << 1)] - scnX);
							if (currY < top)
								currY = top;
							if (currY > bottom)
								currY = bottom;

							*pInt++ = currX;
							*pInt++ = currY;
						}
						else if (xIn2 > 0)
						{
							currX = right;
							currY = scnY + MulDiv(currX - scnX, points[(i << 1) + 1] - scnY, points[(i << 1)] - scnX);
							if (currY < top)
								currY = top;
							if (currY > bottom)
								currY = bottom;

							*pInt++ = currX;
							*pInt++ = currY;
						}

					}
				}
				else
				{
					if (xIn > 0)
					{
						currX = right;
						currY = scnY + MulDiv(currX - scnX, points[(i << 1) + 1] - scnY, points[(i << 1)] - scnX);
						if (currY >= top && currY <= bottom)
						{
							*pInt++ = currX;
							*pInt++ = currY;
							xIn = 0;
							yIn = 0;
						}
					}
					else if (xIn < 0)
					{
						currX = left;
						currY = scnY + MulDiv(currX - scnX, points[(i << 1) + 1] - scnY, points[(i << 1)] - scnX);
						if (currY >= top && currY <= bottom)
						{
							*pInt++ = currX;
							*pInt++ = currY;
							xIn = 0;
							yIn = 0;
						}
					}
					if (yIn > 0)
					{
						currY = bottom;
						currX = scnX + MulDiv(currY - scnY, points[(i << 1)] - scnX, points[(i << 1) + 1] - scnY);
						if (currX >= left && currX <= right)
						{
							*pInt++ = currX;
							*pInt++ = currY;
							yIn = 0;
							xIn = 0;
						}
					}
					else if (yIn < 0)
					{
						currY = top;
						currX = scnX + MulDiv(currY - scnY, points[(i << 1)] - scnX, points[(i << 1) + 1] - scnY);
						if (currX >= left && currX <= right)
						{
							*pInt++ = currX;
							*pInt++ = currY;
							yIn = 0;
							xIn = 0;
						}
					}

					if (xIn || yIn)
					{
					}
///////////////////////////////////////////////////////////
				}*/

/*				if (yIn != yIn2)
				{
					if (yIn > 0)
					{
						currY = bottom;
						currX = scnX + MulDiv(currY - scnY, points[(i << 1)] - scnX, points[(i << 1) + 1] - scnY);
						if (found = (currX >= left) && (currX < right))
						{
							y
						}
					}
					else if (yIn < 0)
					{
						currY = top;
						currX = scnX + MulDiv(currY - scnY, points[(i << 1)] - scnX, points[(i << 1) + 1] - scnY);
						found = (currX >= left) && (currX < right);
					}
					else if (yIn
				}
				if (!found && (yIn < 0) && (yIn2 >= 0))
				{
				}
				if (!found && (xIn < 0) && (xIn2 >= 0))
				{
					currX = left;
					currY = scnY + MulDiv(currX - scnX, points[(i << 1) + 1] - scnY, points[(i << 1)] - scnX);
					found = (currY >= top) && (currY < bottom);
				}
				if (!found && (xIn < 0) && (yIn2 >= 0))
				{
					currX = right;
					currY = scnY + MulDiv(currX - scnX, points[(i << 1) + 1] - scnY, points[(i << 1)] - scnX);
					found = (currY >= top) && (currY < bottom);
				}*/

			
/*			}

			if ((yIn2 | xIn2) == 0)
			{
				*pInt++ = points[(i << 1)];
				*pInt++ = points[(i << 1) + 1];
			}
			xIn = xIn2;
			yIn = yIn2;
			scnX = points[(i << 1)];
			scnY = points[(i << 1) + 1];
		}



		i = nPoint - 1;
		while (i--)
		{
			
		}*/
		
	}
}

void SwapLabel(MyLabel *mapLabels, Int32 index, Int32 index2)
{
	MyLabel l;

	l.label = mapLabels[index].label;
	l.xPos = mapLabels[index].xPos;
	l.yPos = mapLabels[index].yPos;
	l.fontStyle = mapLabels[index].fontStyle;
	l.scaleW = mapLabels[index].scaleW;
	l.scaleH = mapLabels[index].scaleH;
	l.priority = mapLabels[index].priority;
	l.totalSize = mapLabels[index].totalSize;
	l.currSize = mapLabels[index].currSize;
	l.nPoints = mapLabels[index].nPoints;
	l.shapeType = mapLabels[index].shapeType;
	l.points = mapLabels[index].points;
	l.flags = mapLabels[index].flags;
	
	mapLabels[index].label = mapLabels[index2].label;
	mapLabels[index].xPos = mapLabels[index2].xPos;
	mapLabels[index].yPos = mapLabels[index2].yPos;
	mapLabels[index].fontStyle = mapLabels[index2].fontStyle;
	mapLabels[index].scaleW = mapLabels[index2].scaleW;
	mapLabels[index].scaleH = mapLabels[index2].scaleH;
	mapLabels[index].priority = mapLabels[index2].priority;
	mapLabels[index].totalSize = mapLabels[index2].totalSize;
	mapLabels[index].currSize = mapLabels[index2].currSize;
	mapLabels[index].nPoints = mapLabels[index2].nPoints;
	mapLabels[index].shapeType = mapLabels[index2].shapeType;
	mapLabels[index].points = mapLabels[index2].points;
	mapLabels[index].flags = mapLabels[index2].flags;

	mapLabels[index2].label = l.label;
	mapLabels[index2].xPos = l.xPos;
	mapLabels[index2].yPos = l.yPos;
	mapLabels[index2].fontStyle = l.fontStyle;
	mapLabels[index2].scaleW = l.scaleW;
	mapLabels[index2].scaleH = l.scaleH;
	mapLabels[index2].priority = l.priority;
	mapLabels[index2].totalSize = l.totalSize;
	mapLabels[index2].currSize = l.currSize;
	mapLabels[index2].nPoints = l.nPoints;
	mapLabels[index2].shapeType = l.shapeType;
	mapLabels[index2].points = l.points;
	mapLabels[index2].flags = l.flags;
}

// lblSize in byte
void FntDrawChars(MapVars *map, WChar *label, Int32 lblSize, Int32 scnPosX, Int32 scnPosY, Int32 scaleW, Int32 scaleH, Int32 fontStyle, Int32 align)
{
	Int32 size[2];
	UInt16 absH;
	MyFont* fonts;
	Int32 fntCount;
	Int32 i;
	Int32 maxSize;
	Int32 maxIndex;

	if (fontStyle < 0)
		return;
	if (fontStyle >= map->nMapFonts)
		return;
	if (scaleW < 0)
	{
		scaleW = -scaleW;
		scaleH = -scaleH;
	}


	fonts = (MyFont*) map->mapFonts[fontStyle].styles;
	fntCount = map->mapFonts[fontStyle].nStyles;

	maxSize = 0;
	maxIndex = 0;
	i = 0;
	while (i < fntCount)
	{
		if (fonts[i].fontType == 0)
		{
			if (maxSize < fonts[i].size)
			{
				maxSize = fonts[i].size;
				maxIndex = i;
			}
		}
		i++;
	}

	if (maxSize == 0)
	{
		return;
	}
	
	map->img->GetTextSize(fonts[maxIndex].font, label, lblSize >> 1, size);

	if (scaleH == 0) //scaleW == 0 && scaleH == 0)
	{
		RECT rc;
		i = 0;
		while (i < fntCount)
		{
			if (fonts[i].fontType == 1)
			{
				Media::DrawPen *p = 0;
				Media::DrawBrush *b = 0;
				rc.left = scnPosX - ((size[0] + fonts[i].size) >> 1);
				rc.right = rc.left + size[0] + fonts[i].size;
				rc.top = scnPosY - ((size[1] + fonts[i].size) >> 1);
				rc.bottom = rc.top + size[1] + fonts[i].size;

				b = (Media::DrawBrush*)fonts[i].other;
				if (i < fntCount - 1)
					if (fonts[i+1].fontType == 2 && fonts[i+1].size == fonts[i].size)
					{
						p = (Media::DrawPen*)fonts[++i].other;
						rc.right += fonts[i].thickness >> 1;
						rc.bottom += fonts[i].thickness >> 1;
					}

				map->img->DrawRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, p, b);
			}
			else if (fonts[i].fontType == 2)
			{
				Int32 pt[10];
				rc.left = scnPosX - ((size[0] + fonts[i].size) >> 1);
				rc.right = rc.left + size[0] + fonts[i].size;
				rc.top = scnPosY - ((size[1] + fonts[i].size) >> 1);
				rc.bottom = rc.top + size[1] + fonts[i].size;

				pt[0] = rc.left;
				pt[1] = rc.top;
				pt[2] = rc.right;
				pt[3] = rc.top;
				pt[4] = rc.right;
				pt[5] = rc.bottom;
				pt[6] = rc.left;
				pt[7] = rc.bottom;
				pt[8] = pt[0];
				pt[9] = pt[1];
				
				map->img->DrawPolyline(pt, 5, (Media::DrawPen*)fonts[i].other);
			}
			else if (fonts[i].fontType == 0)
			{
				map->img->DrawStringW(scnPosX - (size[0]  >> 1), scnPosY - (size[0] >> 1), label, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
			}
			else if (fonts[i].fontType == 4)
			{
//				SelectObject(map->MapHDC, fonts[i].font);
//				SetTextColor(map->MapHDC, fonts[i].color);
//				SetBkMode(map->MapHDC, TRANSPARENT);
				Int32 j = fonts[i].thickness;
				Int32 k;
				Int32 px = scnPosX - (size[0] >> 1);
				Int32 py = scnPosY - (size[0] >> 1);
				while (j >= 0)
				{
					k = fonts[i].thickness - j;
					while (k >= 0)
					{
						if (j | k)
						{
							map->img->DrawStringW(px + j, py + k, label, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
							map->img->DrawStringW(px + j, py - k, label, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
							map->img->DrawStringW(px - j, py + k, label, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
							map->img->DrawStringW(px - j, py - k, label, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
						}
						else
						{
							map->img->DrawStringW(px, py, label, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
						}
						k--;
					}
					j--;
				}
			}
			i++;
		}

		return;
	}


	if (scaleH < 0)
		absH = (UInt16)-scaleH;
	else
		absH = (UInt16)scaleH;

	double degD = atan(scaleH / (double)scaleW);

	long deg = (long) (atan(scaleH / (double)scaleW) * 1800 / PI);
	while (deg < 0)
		deg += 3600;

	Int32 lastScaleW = scaleW;
	Int32 lastScaleH = scaleH;

	i = 0;
	while (i < fntCount)
	{
		Media::DrawBrush *b;
		Media::DrawPen *p;

		scaleW = lastScaleW;
		scaleH = lastScaleH;
		Int32 pt[10];
		Int32 xPos;
		Int32 yPos;
		double sVal;
		double cVal;

		if (fonts[i].fontType == 1)
		{
			xPos = size[0] + fonts[i].size;
			yPos = size[1] + fonts[i].size;
			Int32 xs = (Int32) ((xPos >> 1) * (sVal = sin(degD)));
			Int32 ys = (Int32) ((yPos >> 1) * sVal);
			Int32 xc = (Int32) ((xPos >> 1) * (cVal = cos(degD)));
			Int32 yc = (Int32) ((yPos >> 1) * cVal);

			pt[0] = scnPosX - xc - ys;
			pt[1] = scnPosY + xs - yc;
			pt[2] = scnPosX + xc - ys;
			pt[3] = scnPosY - xs - yc;
			pt[4] = scnPosX + xc + ys;
			pt[5] = scnPosY - xs + yc;
			pt[6] = scnPosX - xc + ys;
			pt[7] = scnPosY + xs + yc;


			p = 0;
			b = (Media::DrawBrush*)fonts[i].other;

			if (i < fntCount - 1)
				if (fonts[i+1].fontType == 2 && fonts[i+1].size == fonts[i].size)
				{
					p = (Media::DrawPen*)fonts[++i].other;
				}
			map->img->DrawPolygon(pt, 4, p, b);
		}
		else if (fonts[i].fontType == 2)
		{
			xPos = size[0] + fonts[i].size;
			yPos = size[1] + fonts[i].size;
			Int32 xs = (Int32) ((xPos >> 1) * (sVal = sin(degD)));
			Int32 ys = (Int32) ((yPos >> 1) * sVal);
			Int32 xc = (Int32) ((xPos >> 1) * (cVal = cos(degD)));
			Int32 yc = (Int32) ((yPos >> 1) * cVal);

			pt[0] = scnPosX - xc - ys;
			pt[1] = scnPosY + xs - yc;
			pt[2] = scnPosX + xc - ys;
			pt[3] = scnPosY - xs - yc;
			pt[4] = scnPosX + xc + ys;
			pt[5] = scnPosY - xs + yc;
			pt[6] = scnPosX - xc + ys;
			pt[7] = scnPosY + xs + yc;
			pt[8] = pt[0];
			pt[9] = pt[1];

			map->img->DrawPolyline(pt, 5, (Media::DrawPen*)fonts[i].other);
		}
		else if (fonts[i].fontType == 0 || fonts[i].fontType == 4)
		{
			if (align)
			{
				Int32 currX = 0;
				Int32 currY = 0;
				Int32 startX;
				Int32 startY;
				Int32 tmp;
				Int32 type;
				Int32 szThis[2];
				map->img->GetTextSize(fonts[i].font, label, lblSize >> 1, szThis);

				if ((szThis[0] * absH) < ((szThis[1] * lblSize * scaleW) >> 1))
				{
					scaleW = -scaleW;
					startX = scnPosX - (tmp = (szThis[0] >> 1));
					if (scaleW)
						startY = scnPosY - (szThis[1] >> 1) - (Int16)((Int32)tmp * (Int32)scaleH / (Int32)scaleW);
					else
						startY = scnPosY - (szThis[1] >> 1);
					type = 0;
				}
				else
				{
					scaleW = -scaleW;
					if (scaleH > 0)
					{
						startY = scnPosY - (tmp = ((szThis[1] * lblSize) >> 2));
						startX = scnPosX - (Int16)((Int32)tmp * (Int32)scaleW / (Int32)scaleH);
					}
					else if (scaleH)
					{
						scaleW = -scaleW;
						scaleH = -scaleH;
						startY = scnPosY - (tmp = ((szThis[1] * lblSize) >> 2));
						startX = scnPosX - (Int16)((Int32)tmp * (Int32)scaleW / (Int32)scaleH);
					}
					else
					{
						startY = scnPosY - (tmp = ((szThis[1] * lblSize) >> 2));
						startX = scnPosX;
					}
					type = 1;
				}

				Int32 cHeight;
				if (scaleH < 0)
					cHeight = szThis[1];
				else
					cHeight = -szThis[1];

				currX = 0;
				currY = 0;

				Int32 cnt;
				WChar *lbl = label;
				cnt = lblSize >> 1;

				while (cnt--)
				{
					map->img->GetTextSize(fonts[i].font, lbl, 1, szThis);

					if (type)
					{
						if (fonts[i].fontType == 0)
						{
							WChar l[2];
							l[0] = lbl[0];
							l[1] = 0;
							map->img->DrawStringW(startX + currX - (szThis[0] >> 1), startY + currY, l, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
						}
						else
						{
							WChar l[2];
							Int32 j = fonts[i].thickness;
							Int32 k;
							Int32 px = startX + currX - (szThis[0] >> 1);
							Int32 py = startY + currY;
							l[0] = lbl[0];
							l[1] = 0;
							while (j >= 0)
							{
								k = fonts[i].thickness - j;
								while (k >= 0)
								{
									if (j | k)
									{
										map->img->DrawStringW(px + j, py + k, l, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
										map->img->DrawStringW(px + j, py - k, l, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
										map->img->DrawStringW(px - j, py + k, l, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
										map->img->DrawStringW(px - j, py - k, l, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
									}
									else
									{
										map->img->DrawStringW(px, py, l, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
									}
									k--;
								}
								j--;
							}
						}

						currY += szThis[1];

						if (scaleH)
							currX = (Int16)((Int32)currY * (Int32)scaleW / (Int32)scaleH);
					}
					else
					{
						if (fonts[i].fontType == 0)
						{
							WChar l[2];
							l[0] = lbl[0];
							l[1] = 0;
							map->img->DrawStringW(startX + currX, startY + currY, l, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
						}
						else
						{
							WChar l[2];
							Int32 j = fonts[i].thickness;
							Int32 k;
							Int32 px = startX + currX;
							Int32 py = startY + currY;
							l[0] = lbl[0];
							l[1] = 0;
							while (j >= 0)
							{
								k = fonts[i].thickness - j;
								while (k >= 0)
								{
									if (j | k)
									{
										map->img->DrawStringW(px + j, py + k, l, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
										map->img->DrawStringW(px + j, py - k, l, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
										map->img->DrawStringW(px - j, py + k, l, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
										map->img->DrawStringW(px - j, py - k, l, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
									}
									else
									{
										map->img->DrawStringW(px, py, l, fonts[i].font, (Media::DrawBrush*)fonts[i].other);
									}
									k--;
								}
								j--;
							}
						}

						currX += szThis[0];
						if (scaleW)
							currY = (Int16)((Int32)currX * (Int32)scaleH / (Int32)scaleW);
					}
					lbl += 1;
				}

			}
			else
			{
				map->img->DrawStringRotW(scnPosX, scnPosY, label, fonts[i].font, (Media::DrawBrush*)fonts[i].other, (Int32)(degD * 180 / PI));

				if (fonts[i].fontType == 0)
				{
					map->img->DrawStringRotW(scnPosX, scnPosY, label, fonts[i].font, (Media::DrawBrush*)fonts[i].other, (Int32)(degD * 180 / PI));
				}
				else if (fonts[i].fontType == 4)
				{
					Int32 j = fonts[i].thickness;
					Int32 k;
					Int32 px = scnPosX;
					Int32 py = scnPosY;
					while (j >= 0)
					{
						k = fonts[i].thickness - j;
						while (k >= 0)
						{
							if (j | k)
							{
								map->img->DrawStringRotW(px + j, py + k, label, fonts[i].font, (Media::DrawBrush*)fonts[i].other, (Int32)(degD * 180 / PI));
								map->img->DrawStringRotW(px + j, py - k, label, fonts[i].font, (Media::DrawBrush*)fonts[i].other, (Int32)(degD * 180 / PI));
								map->img->DrawStringRotW(px - j, py + k, label, fonts[i].font, (Media::DrawBrush*)fonts[i].other, (Int32)(degD * 180 / PI));
								map->img->DrawStringRotW(px - j, py - k, label, fonts[i].font, (Media::DrawBrush*)fonts[i].other, (Int32)(degD * 180 / PI));
							}
							else
							{
								map->img->DrawStringRotW(px, py, label, fonts[i].font, (Media::DrawBrush*)fonts[i].other, (Int32)(degD * 180 / PI));
							}
							k--;
						}
						j--;
					}
				}
			}
		}
		i++;
	}

}

void FntGetSize(MapVars *map, Int32 *size, WChar *label, Int32 lblSize, Int32 fontStyle, Int32 scaleW, Int32 scaleH)
{
	Int32 xSizeAdd = 0;
	Int32 ySizeAdd = 0;

	MyFont *fonts;
	Int32 fntCount;

	Int32 i;
	Int32 maxSize;
	Int32 maxIndex;

	if (lblSize == 0)
	{
		size[0] = 0;
		size[1] = 0;
		return;
	}

	fonts = (MyFont*)map->mapFonts[fontStyle].styles;
	fntCount = map->mapFonts[fontStyle].nStyles;

	maxSize = 0;
	maxIndex = 0;
	i = 0;
	while (i < fntCount)
	{
		if (fonts[i].fontType == 0)
		{
			if (maxSize < fonts[i].size)
			{
				maxSize = fonts[i].size;
				maxIndex = i;
			}
		}
		else if (fonts[i].fontType == 4)
		{
			if (maxSize < fonts[i].size + (fonts[i].thickness << 1))
			{
				maxSize = fonts[i].size + (fonts[i].thickness << 1);
				maxIndex = i;
			}
		}
		else if (fonts[i].fontType == 1 || fonts[i].fontType == 2)
		{
			if (xSizeAdd < (fonts[i].size + fonts[i].thickness))
			{
				xSizeAdd = (fonts[i].size + fonts[i].thickness);
			}
			if (ySizeAdd < (fonts[i].size + fonts[i].thickness))
			{
				ySizeAdd = (fonts[i].size + fonts[i].thickness);
			}
		}
		i++;
	}

	if (maxSize == 0)
	{
		return;
	}

//	SIZE szTmp;
//	SelectObject(map->MapHDC, fonts[maxIndex].font);
//	GetTextExtentPointW(map->MapHDC, label, lblSize >> 1, &szTmp);
	Int32 szTmp[2];
	map->img->GetTextSize(fonts[maxIndex].font, label, lblSize >> 1, szTmp);

	if (scaleH == 0)
	{
		size[0] = szTmp[0] + xSizeAdd;
		size[1] = szTmp[1] + ySizeAdd;

		return;
	}


	Int32 pt[8];

	if (scaleW < 0)
	{
		scaleW = -scaleW;
		scaleH = -scaleH;
	}
	double degD = atan(scaleH / (double)scaleW);
	Int32 xPos = szTmp[0] + xSizeAdd;
	Int32 yPos = szTmp[1] + ySizeAdd;
	double sVal;
	double cVal;
	Int32 xs = (Int32) ((xPos >> 1) * (sVal = sin(degD)));
	Int32 ys = (Int32) ((yPos >> 1) * sVal);
	Int32 xc = (Int32) ((xPos >> 1) * (cVal = cos(degD)));
	Int32 yc = (Int32) ((yPos >> 1) * cVal);

	pt[0] = -xc - ys;
	pt[1] = xs - yc;
	pt[2] = xc - ys;
	pt[3] = -xs - yc;
	pt[4] = xc + ys;
	pt[5] = -xs + yc;
	pt[6] = -xc + ys;
	pt[7] = xs + yc;

	Int32 maxX = pt[0];
	Int32 minX = pt[0];
	Int32 maxY = pt[1];
	Int32 minY = pt[1];
	if (pt[2] > maxX) maxX = pt[2];
	if (pt[2] < minX) minX = pt[2];
	if (pt[3] > maxY) maxY = pt[3];
	if (pt[3] < minY) minY = pt[3];
	if (pt[4] > maxX) maxX = pt[4];
	if (pt[4] < minX) minX = pt[4];
	if (pt[5] > maxY) maxY = pt[5];
	if (pt[5] < minY) minY = pt[5];
	if (pt[6] > maxX) maxX = pt[6];
	if (pt[6] < minX) minX = pt[6];
	if (pt[7] > maxY) maxY = pt[7];
	if (pt[7] < minY) minY = pt[7];
	size[0] = maxX - minX;
	size[1] = maxY - minY;
}

void MapAfterDraw(MapVars *map)
{
	if (map->canDraw == 0)
		return;

	Int32 i;
	Int32 j;
	WChar *dest;
	WChar *lastLbl = 0;

	if (map->currLabels)
	{
		Int32* points;
		Int32 currPt;
		Int32 szThis[2];

		Int32 tlx;
		Int32 tly;
		Int32 brx;
		Int32 bry;
		Int32 scnPtX;
		Int32 scnPtY;
		long thisPts[10];
		long thisCnt = 0;

		WChar *src;

		if (!(points = MemAlloc(Int32, map->currLabels * 20)))
			return;

		i = 0;
		j = map->currLabels;
		while (j--)
		{
			currPt = 0;
			i = 0;
			while (i < j)
			{
				if (map->mapLabels[i].priority < map->mapLabels[i + 1].priority)
				{
					SwapLabel(map->mapLabels, i, i + 1);
					currPt = 1;
				}
				else if (map->mapLabels[i].priority == map->mapLabels[i + 1].priority)
				{
					if (map->mapLabels[i].totalSize < map->mapLabels[i + 1].totalSize)
					{
						SwapLabel(map->mapLabels, i, i + 1);
						currPt = 1;
					}
				}
				i++;
			}

			if (currPt == 0)
			{
				break;
			}
		}

		currPt = 0;
		i = 0;
		while (i < map->currLabels)
		{
			src = map->mapLabels[i].label;
			while (*src++);

			FntGetSize(map, szThis, map->mapLabels[i].label, (Int32)((src - map->mapLabels[i].label - 1) * sizeof(src[0])), map->mapLabels[i].fontStyle, map->mapLabels[i].scaleW, map->mapLabels[i].scaleH);

			scnPtX = MulDiv(map->mapLabels[i].xPos - (Int32)map->left, map->scnWidth, map->ScaleWidth);
			scnPtY = MulDiv(map->bottom - map->mapLabels[i].yPos, map->scnHeight, map->ScaleHeight);
			
		//	map->mapLabels[i].shapeType = 0;
			if (map->mapLabels[i].shapeType == 1)
			{
				j = 1;
				if (j)
				{
					tlx = scnPtX + 1;
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] >> 1);
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - szThis[0] - 1;
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] >> 1);
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - (szThis[0] >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY - szThis[1] - 1;
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - (szThis[0] >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY + 1;
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}

				if (!j)
				{
					src = map->mapLabels[i].label;
					while (*src++);
					FntDrawChars(map, map->mapLabels[i].label, (Int32)((src - map->mapLabels[i].label - 1) * sizeof(src[0])),  (tlx + brx) >> 1, (tly + bry) >> 1, 0, 0, map->mapLabels[i].fontStyle, 0);

					points[(currPt << 2)] = tlx;
					points[(currPt << 2) + 1] = tly;
					points[(currPt << 2) + 2] = brx;
					points[(currPt << 2) + 3] = bry;
					currPt++;
				}
			}
			else if (map->mapLabels[i].shapeType == 3)
			{

				if (lastLbl)
				{
					src = map->mapLabels[i].label;
					dest = lastLbl;
					while (*src++)
					{
						if (src[-1] != *dest++)
						{
							break;
						}
					}
					if (src[-1] || *dest)
					{
						thisCnt = 0;
					}
				}
				else
				{
					thisCnt = 0;
				}

				if (thisCnt == 0)
				{
					if (lastLbl)
						MemFree(lastLbl);
					src = map->mapLabels[i].label;
					while (*src++);
					dest = lastLbl = MemAlloc(WChar, src - map->mapLabels[i].label);
					src = map->mapLabels[i].label;
					while (*src++)
						*dest++ = src[-1];
					*dest++ = 0;
				}
				else
				{
				//	MessageBoxW(NULL, L"Test", lastLbl, MB_OK);
				}

				j = 1;
				if (j)
				{
					tlx = scnPtX - (szThis[0] >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] >> 1);
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}

				if (!j && thisCnt < 10)
				{
					long m;
					long n = 1;
					long tmpV;
					m = thisCnt;
					while (n && m > 0)
					{
						n = 0;
						tmpV = thisPts[--m];
						if ((tmpV - LBLMINDIST) < bry && (tmpV + LBLMINDIST) > tly)
						{
							n++;
						}
						tmpV = thisPts[--m];
						if ((tmpV - LBLMINDIST) < brx && (tmpV + LBLMINDIST) > tlx)
						{
							n++;
						}
						n = (n != 2);
					}

					if (n)
					{
						src = map->mapLabels[i].label;
						while (*src++);
						FntDrawChars(map, map->mapLabels[i].label, (Int32)((src - map->mapLabels[i].label - 1) * sizeof(src[0])),  (tlx + brx) >> 1, (tly + bry) >> 1, map->mapLabels[i].scaleW, map->mapLabels[i].scaleH, map->mapLabels[i].fontStyle, map->mapLabels[i].flags & SFLG_ALIGN);

						points[(currPt << 2)] = tlx;
						points[(currPt << 2) + 1] = tly;
						points[(currPt << 2) + 2] = brx;
						points[(currPt << 2) + 3] = bry;
						currPt++;

						thisPts[thisCnt++] = (tlx + brx) >> 1;
						thisPts[thisCnt++] = (tly + bry) >> 1;
					}
					j = (thisCnt < 10);
				}

				if (j)
				{
					Int32 k;
					Int32 l;
					Int32 *ptInt = map->mapLabels[i].points;

					k = 0;
					l = map->mapLabels[i].nPoints - 1;
					while (j && k < l)
					{
						Int32 lastPtX;
						Int32 lastPtY;
						Int32 thisPtX;
						Int32 thisPtY;
						Int32 tmp;
						lastPtX = ptInt[0];
						lastPtY = ptInt[1];
						thisPtX = ptInt[2];
						thisPtY = ptInt[3];
						if (lastPtX > thisPtX)
						{
							tmp = lastPtX;
							lastPtX = thisPtX;
							thisPtX = tmp;
						}
						if (lastPtY > thisPtY)
						{
							tmp = lastPtY;
							lastPtY = thisPtY;
							thisPtY = tmp;
						}

						if (map->left < thisPtX && map->right > lastPtX && map->top < thisPtY && map->bottom > lastPtY)
						{
							lastPtX = ptInt[0];
							lastPtY = ptInt[1];
							thisPtX = ptInt[2];
							thisPtY = ptInt[3];
							if (map->left > lastPtX)
							{
								lastPtY += MulDiv(map->left - lastPtX, thisPtY - lastPtY, thisPtX - lastPtX);
								lastPtX = map->left;
							}
							else if (lastPtX > map->right)
							{
								lastPtY += MulDiv(map->right - lastPtX, thisPtY - lastPtY, thisPtX - lastPtX);
								lastPtX = map->right;
							}

							if (map->top > lastPtY)
							{
								lastPtX += MulDiv(map->top - lastPtY, thisPtX - lastPtX, thisPtY - lastPtY);
								lastPtY = map->top;
							}
							else if (lastPtY > map->bottom)
							{
								lastPtX += MulDiv(map->bottom - lastPtY, thisPtX - lastPtX, thisPtY - lastPtY);
								lastPtY = map->bottom;
							}

							if (thisPtX < map->left)
							{
								thisPtY += MulDiv(map->left - thisPtX, thisPtY - lastPtY, thisPtX - lastPtX);
								thisPtX = map->left;
							}
							else if (map->right < thisPtX)
							{
								thisPtY += MulDiv(map->right - thisPtX, thisPtY - lastPtY, thisPtX - lastPtX);
								thisPtX = map->right;
							}

							if (map->top > thisPtY)
							{
								thisPtX += MulDiv(map->top - thisPtY, thisPtX - lastPtX, thisPtY - lastPtY);
								thisPtY = map->top;
							}
							else if (map->bottom < thisPtY)
							{
								thisPtX += MulDiv(map->bottom - thisPtY, thisPtX - lastPtX, thisPtY - lastPtY);
								thisPtY = map->bottom;
							}

							map->mapLabels[i].scaleW = 0;
							map->mapLabels[i].scaleH = 0;

							scnPtX = (thisPtX + lastPtX) >> 1;
							scnPtY = (thisPtY + lastPtY) >> 1;
							if (map->mapLabels[i].flags & SFLG_ROTATE)
							{
								map->mapLabels[i].scaleW = ptInt[2] - ptInt[0];
								map->mapLabels[i].scaleH = ptInt[3] - ptInt[1];
							}

							scnPtX = MulDiv(scnPtX - (Int32)map->left, map->scnWidth, map->ScaleWidth);
							scnPtY = MulDiv(map->bottom - scnPtY, map->scnHeight, map->ScaleHeight);

							FntGetSize(map, szThis, map->mapLabels[i].label, (Int32)((src - map->mapLabels[i].label - 1) * sizeof(src[0])), map->mapLabels[i].fontStyle, map->mapLabels[i].scaleW, map->mapLabels[i].scaleH);

							tlx = scnPtX - (szThis[0] >> 1);
							brx = tlx + szThis[0];
							tly = scnPtY - (szThis[1] >> 1);
							bry = tly + szThis[1];

							j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
						}


						if (!j && thisCnt < 10)
						{
							long m;
							long n = 1;
							long tmpV;
							m = thisCnt;
							while (n && m > 0)
							{
								n = 0;
								tmpV = thisPts[--m];
								if ((tmpV - LBLMINDIST) < bry && (tmpV + LBLMINDIST) > tly)
								{
									n++;
								}
								tmpV = thisPts[--m];
								if ((tmpV - LBLMINDIST) < brx && (tmpV + LBLMINDIST) > tlx)
								{
									n++;
								}
								n = (n != 2);
							}

							if (n)
							{
								src = map->mapLabels[i].label;
								while (*src++);
								FntDrawChars(map, map->mapLabels[i].label, (Int32)((src - map->mapLabels[i].label - 1) * sizeof(src[0])),  (tlx + brx) >> 1, (tly + bry) >> 1, map->mapLabels[i].scaleW, map->mapLabels[i].scaleH, map->mapLabels[i].fontStyle, map->mapLabels[i].flags & SFLG_ALIGN);

								points[(currPt << 2)] = tlx;
								points[(currPt << 2) + 1] = tly;
								points[(currPt << 2) + 2] = brx;
								points[(currPt << 2) + 3] = bry;
								currPt++;

								thisPts[thisCnt++] = (tlx + brx) >> 1;
								thisPts[thisCnt++] = (tly + bry) >> 1;
							}
							j = (thisCnt < 10);
						}

						ptInt += 2;
						k++;
					}
				}

				if (!j && false)
				{
					src = map->mapLabels[i].label;
					while (*src++);
					FntDrawChars(map, map->mapLabels[i].label, (Int32)((src - map->mapLabels[i].label - 1) * sizeof(src[0])),  (tlx + brx) >> 1, (tly + bry) >> 1, map->mapLabels[i].scaleW, map->mapLabels[i].scaleH, map->mapLabels[i].fontStyle, map->mapLabels[i].flags & SFLG_ALIGN);

					points[(currPt << 2)] = tlx;
					points[(currPt << 2) + 1] = tly;
					points[(currPt << 2) + 2] = brx;
					points[(currPt << 2) + 3] = bry;
					currPt++;
				}
			}
			else if (map->mapLabels[i].shapeType == 5)
			{
				j = 1;
				if (j)
				{
					tlx = scnPtX  - (szThis[0] >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] >> 1);
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX + 1;
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] >> 1);
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - szThis[0] - 1;
					brx = tlx + szThis[0];
					tly = scnPtY - (szThis[1] >> 1);
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - (szThis[0] >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY - szThis[1] - 1;
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}
				if (j)
				{
					tlx = scnPtX - (szThis[0] >> 1);
					brx = tlx + szThis[0];
					tly = scnPtY + 1;
					bry = tly + szThis[1];

					j = LabelOverlapped(points, currPt, tlx, tly, brx, bry);
				}

				if (!j)
				{
					src = map->mapLabels[i].label;
					while (*src++);
					FntDrawChars(map, map->mapLabels[i].label, (Int32)((src - map->mapLabels[i].label - 1) * sizeof(src[0])),  (tlx + brx) >> 1, (tly + bry) >> 1, 0, 0, map->mapLabels[i].fontStyle, 0);

					points[(currPt << 2)] = tlx;
					points[(currPt << 2) + 1] = tly;
					points[(currPt << 2) + 2] = brx;
					points[(currPt << 2) + 3] = bry;
					currPt++;
				}
			}
			else
			{

			}
			
			i++;
		}

		MemFree(points);
	}


	i = map->nMapFonts;
	while (i--)
	{
		ReleaseFont(map, &map->mapFonts[i]);
	}

	i = map->nMapLines;
	while (i--)
	{
		ReleaseLine(map, &map->mapLines[i]);
	}

	i = map->nMapLabels;
	while (i--)
	{
		ReleaseLabel(map, &map->mapLabels[i]);
	}

	if (lastLbl)
	{
		MemFree(lastLbl);
		lastLbl = 0;
	}

	MemFree(map->mapFonts);
	MemFree(map->mapLines);
	MemFree(map->mapLabels);
	map->mapFonts = 0;
	map->mapLines = 0;
	map->mapLabels = 0;
	map->canDraw = 0;
}

void SelectPen(MapVars *map, MyLine *lines, Int32 index, Int32 maxScale)
{
	if (lines[index].lineType == 1)
	{
		Int32 i;
		i = (Int32)(maxScale / (double)map->mapScale);
		if (i <= 0)
			i = 1;
		i += lines[index].width;
		if (i != lines[index].nStyle)
		{
			if (lines[index].pen)
			{
				map->img->DelPen(lines[index].pen);
			}
			lines[index].pen = map->img->NewPenARGB(lines[index].color, lines[index].nStyle = i, 0, 0);
		}
	}
}



/*PHP_FUNCTION(mdraw_test)
{
	char *arg = NULL;
	int arg_len;
	char *retVal;
	int retSize;
	char utf8Buff[256];
	Encoding enc(65001);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	DrawEngine *eng;
	DrawPen *p;
	DrawBrush *b;
	DrawFont *f;

	NEW_CLASS(eng, GENGINE());


	DrawImage *img = eng->CreateImage32(320, 240);
	p = img->NewPenARGB(0xffffff00, 4, 0, 0);
	img->DrawLine(0, 0, 320, 240, p);
	img->DelPen(p);

	b = img->NewBrushARGB(0xffff0000);
	f = img->NewFont("MingLiU", 14, 0);
	//f = img->NewFont("Arial", 12, 0);
	utf8Buff[enc.ToBytes((UInt8*)utf8Buff, L"ABc��??�e�X�gDef", 12)] = 0;
	img->DrawStringRot(160, 120, utf8Buff, f, b, 45);
	img->DelFont(f);
	img->DelBrush(b);

	FileStream *stm;
	NEW_CLASS(stm, FileStream(L"Test.png", FileStream::FILE_MODE_CREATE));
	if (stm->IsError() == 0)
	{
		img->SavePng((Stream*)stm);
		retVal = "Saved";
		retSize = 5;
	}
	else
	{
		retVal = "Error";
		retSize = 5;
	}
	DEL_CLASS(stm);
	DEL_CLASS((GENGINE*)eng);

	RETURN_STRINGL(retVal, retSize, 1);
}*/

void *Map::MapEngine::Init(Int32 w, Int32 h, Int32 xPos, Int32 yPos, Int32 bgColor, Int32 scale, Int32 nLineStyle, Int32 nFontStyle, Int32 nLabels)
{
	MapVars *map = MemAlloc(MapVars, 1);

	map->canDraw = 1;
	map->scnWidth = w;
	map->scnHeight = h;
	map->mapPosX = xPos;
	map->mapPosY = yPos;
	map->mapScale = scale;
	NEW_CLASS(map->eng, GENGINE());
	map->img = map->eng->CreateImage32(w, h);

	Media::DrawBrush *brush = map->img->NewBrushARGB(bgColor);
	map->img->DrawRect(0, 0, w, h, 0, brush);
	map->img->DelBrush(brush);

	map->nMapFonts = nFontStyle;
	map->nMapLines = nLineStyle;
	map->nMapLabels = nLabels;
	map->currLabels = 0;
	map->mapFonts = MemAlloc(MyStyle, map->nMapFonts);
	map->mapLines = MemAlloc(MyStyle, map->nMapLines);
	map->mapLabels = MemAlloc(MyLabel, map->nMapLabels);

	Int32 i;
	i = map->nMapFonts;
	while (i--)
		map->mapFonts[i].styles = 0;

	i = map->nMapLines;
	while (i--)
		map->mapLines[i].styles = 0;

	i = map->nMapLabels;
	while (i--)
	{
		map->mapLabels[i].points = 0;
		map->mapLabels[i].label = 0;
	}

	map->left = (UInt32)MulDiv(MulDiv(map->scnWidth >> 1, map->mapScale, Xdpi), 100, 3937);
	map->top = (UInt32)MulDiv(MulDiv(map->scnHeight >> 1, map->mapScale, Ydpi), 100, 3937);

	map->right = map->mapPosX + map->left;
	map->bottom = map->mapPosY + map->top;
	map->left = map->mapPosX - map->left;
	map->top = map->mapPosY - map->top;
	map->ScaleWidth = (Int32)(map->right - map->left);
	map->ScaleHeight = (Int32)(map->bottom - map->top);

	return map;
}

void Map::MapEngine::InitLine(void *m, Int32 lineNum, Int32 nStyles)
{
	MapVars *map = (MapVars*)m;
	Int32 i;
	MyLine *lines;
	if (lineNum < 0)
		return;
	if (lineNum >= map->nMapLines)
		return;
	if (map->mapLines[lineNum].styles)
		ReleaseLine(map, &map->mapLines[lineNum]);
	map->mapLines[lineNum].styles = lines = MemAlloc(MyLine, nStyles);
	map->mapLines[lineNum].nStyles = nStyles;
	i = nStyles;
	while (i--)
	{
		lines[i].styles = 0;
		lines[i].pen = 0;
	}
}

void Map::MapEngine::InitFont(void *m, Int32 fontNum, Int32 nStyles)
{
	MapVars *map = (MapVars*)m;
	Int32 i;
	MyFont *fonts;
	if (fontNum < 0)
		return;
	if (fontNum >= map->nMapFonts)
		return;
	if (map->mapFonts[fontNum].styles)
	{
		ReleaseFont(map, &map->mapFonts[fontNum]);
	}
	map->mapFonts[fontNum].styles = fonts = MemAlloc(MyFont, nStyles);
	map->mapFonts[fontNum].nStyles = nStyles;
	i = nStyles;
	while (i--)
	{
		fonts[i].font = 0;
		fonts[i].other = 0;
	}
}

void Map::MapEngine::SetLine(void *m, Int32 lineNum, Int32 index, Int32 lineType, Int32 width, Int32 color, WChar *styles)
{
	MapVars *map = (MapVars*)m;
	Int32 i;
	MyLine *lines;
	WChar *tmp;
	OSInt nStyle;
	if (styles)
	{
		tmp = styles;
		while (*tmp++);
		tmp--;
		nStyle = tmp - styles;
	}
	else
	{
		nStyle = 0;
	}
	if (lineNum < 0)
		return;
	if (lineNum >= map->nMapLines)
		return;
	lines = (MyLine*)map->mapLines[lineNum].styles;
	if (index < 0)
		return;
	if (index >= map->mapLines[lineNum].nStyles)
		return;

	if (lines[index].pen)
	{
		map->img->DelPen(lines[index].pen);
	}
	if (lines[index].styles)
	{
		MemFree(lines[index].styles);
	}
	lines[index].pen = 0;
	lines[index].styles = 0;

	lines[index].lineType = lineType;
	lines[index].width = width;
	lines[index].color = color;
	if (lineType == 0)
	{
		lines[index].pen = map->img->NewPenARGB(color, width, 0, 0);
	}
	else if (lineType == 1)
	{
		lines[index].nStyle = lines[index].width;
		lines[index].pen = map->img->NewPenARGB(lines[index].color, lines[index].width, 0, 0);
	}
	else if (lineType == 2)
	{
		Int32 currVal;
		WChar *currCh;
		i = lines[index].nStyle = (Int32)nStyle;
		lines[index].styles = MemAlloc(UInt8, nStyle);
		currCh = styles;
		currVal = 0;
		i = 0;
		while (true)
		{
			if (*currCh >= 0x30 && *currCh <= 0x39)
			{
				currVal = currVal * 10 + *currCh - 48;
			}
			else
			{
				lines[index].styles[i++] = currVal;
				currVal = 0;
				if (*currCh == ',')
				{
				}
				else if (*currCh == 0)
				{
					break;
				}
				else
				{
					break;
				}
			}
			currCh++;
		}
		lines[index].nStyle = i;
		lines[index].pen = map->img->NewPenARGB(lines[index].color, lines[index].width = width, lines[index].styles, lines[index].nStyle);
	}
}

void Map::MapEngine::SetFont(void *m, Int32 fontNum, Int32 index, Int32 fontType, WChar *fontName, Int32 size, Int32 thickness, Int32 color)
{
	MapVars *map = (MapVars*)m;
	MyFont *fonts;
	if (fontNum < 0)
		return;
	if (fontNum >= map->nMapFonts)
		return;
	fonts = (MyFont*)map->mapFonts[fontNum].styles;
	if (index < 0)
		return;
	if (index >= map->mapFonts[fontNum].nStyles)
		return;

	if (fonts[index].font)
	{
		map->img->DelFont(fonts[index].font);
	}
	if (fonts[index].other)
	{
		if (fonts[index].fontType == 0)
		{
			map->img->DelBrush((Media::DrawBrush*)fonts[index].other);
		}
		else if (fonts[index].fontType == 1)
		{
			map->img->DelBrush((Media::DrawBrush*)fonts[index].other);
		}
		else if (fonts[index].fontType == 2)
		{
			map->img->DelPen((Media::DrawPen*)fonts[index].other);
		}
		else if (fonts[index].fontType == 3)
		{
			map->img->DelBrush((Media::DrawBrush*)fonts[index].other);
		}
		else if (fonts[index].fontType == 4)
		{
			map->img->DelBrush((Media::DrawBrush*)fonts[index].other);
		}
	}
		
	fonts[index].font = 0;
	fonts[index].other = 0;

	fonts[index].fontType = fontType;
	fonts[index].size = size;
	fonts[index].thickness = thickness;
	fonts[index].color = color;
	if (fontType == 0) //Font
	{
		Int32 s = 0;
		if (thickness > 0)
			s = s | DFS_BOLD;
		fonts[index].font = map->img->NewFontW(fontName, fonts[index].size, s);
		fonts[index].other = map->img->NewBrushARGB(fonts[index].color);
	}
	else if (fontType == 1) //Background
	{
		fonts[index].other = map->img->NewBrushARGB(fonts[index].color);
	}
	else if (fontType == 2) //Border
	{
		fonts[index].other = map->img->NewPenARGB(fonts[index].color, fonts[index].thickness, 0, 0);
	}
	else if (fontType == 4)
	{
		Int32 s = 0;
//		if (thickness > 0)
//			s = s | DFS_BOLD;
		fonts[index].font = map->img->NewFontW(fontName, fonts[index].size, s);
		fonts[index].other = map->img->NewBrushARGB(fonts[index].color);
	}
} //thickness = 0 - 1000

void Map::MapEngine::EndDraw(void *m)
{
	MapAfterDraw((MapVars*)m);
}

Bool Map::MapEngine::SaveImg(void *m, WChar *fileName, Int32 imgFormat)
{
	MapVars *map;
	IO::FileStream *stm;
	Int32 ret;

	map = (MapVars*)m;
	MapAfterDraw(map);

	NEW_CLASS(stm, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyRead));
	if (imgFormat == 1)
	{
		ret = map->img->SaveGIF(stm);
	}
	else if (imgFormat == 2)
	{
		ret = map->img->SaveJPG(stm);
	}
	else
	{
		ret = map->img->SavePng(stm);
	}
	DEL_CLASS(stm);

	return ret != 0;
}

Int32 Map::MapEngine::GetImg(void *m, UInt8 *buff, Int32 imgFormat)
{
	MapVars *map;
	IO::MemoryStream *stm;
	BOOL ret;
	Int32 stmSize;
	UInt8 *buffTmp;

	map = (MapVars*)m;
	MapAfterDraw(map);

	NEW_CLASS(stm, IO::MemoryStream());
	if (imgFormat == 1)
	{
		ret = map->img->SaveGIF(stm);
	}
	else if (imgFormat == 2)
	{
		ret = map->img->SaveJPG(stm);
	}
	else
	{
		ret = map->img->SavePng(stm);
	}
	buffTmp = stm->GetBuff(&stmSize);
	MemCopy(buff, buffTmp, stmSize);
	DEL_CLASS(stm);

	return stmSize;
}

void Map::MapEngine::Close(void *m)
{
	MapVars *map;

	map = (MapVars*)m;

	if (map)
	{
		MapAfterDraw(map);
		if (map->mapFonts)
		{
			MemFree(map->mapFonts);
			map->mapFonts = 0;
		}
		if (map->mapLines)
		{
			MemFree(map->mapLines);
			map->mapLines = 0;
		}
		if (map->mapLabels)
		{
			MemFree(map->mapLabels);
			map->mapLabels = 0;
		}
		if (map->eng)
		{
			map->eng->DeleteImage(map->img);
			map->img = 0;
			DEL_CLASS((GENGINE*)map->eng);
			map->eng = 0;
		}

		MemFree(map);
	}
	return;
}

void Map::MapEngine::DrawLine(void *m, WChar *fileName, Int32 maxScale, Int32 lineStyle, Bool *drawn)
{
	MapVars *map = (MapVars*)m;
	if (map->canDraw == 0)
		return;

	Int32 leftBlk;
	Int32 rightBlk;
	Int32 topBlk;
	Int32 bottomBlk;
	
	Int32 blkScale;

	Err err;
	FileRef file;
	FileRef pfile;

	Int32 hdr[3]; //Total RecordNo, Shape Type
	UInt32 contHdr[2]; //RecordID, NumberOfParts
	Int32 *parts;
	Int32 numPoints;
	Int32 *points;
	UInt32 readSize;

	UInt32 currPart;

	UInt32 *idxes = 0;
	UInt32 *idxesEnd = 0;
	UInt32 objCount;
	Int32 *ides;
	UInt32 i,j;
	Bool seeked;

	WChar fName[256];

	MakeFileName(fName, fileName, L".blk");
	err = VFSFileOpen(cardNo, fName, vfsModeRead, &file);
	if (err)
		return;

	// Read the useful blocks to Linked list - idxes and idxesEnd (Last element)
	VFSFileRead(file, 4, &currPart, &readSize);
	VFSFileRead(file, 4, &blkScale, &readSize);

	leftBlk = map->left / blkScale;
	rightBlk = map->right / blkScale;
	topBlk = map->top / blkScale;
	bottomBlk = map->bottom / blkScale;

	objCount = 0;
	i = 0;
	while (currPart--)
	{
		VFSFileRead(file, 12, hdr, &readSize);
		if ((leftBlk <= hdr[1]) && (rightBlk >= hdr[1]) && (topBlk <= hdr[2]) && (bottomBlk >= hdr[2]))
		{
			if (drawn)
				*drawn = true;
			i++;
			objCount += hdr[0];
			if (hdr[0])
			{
				if (idxesEnd)
				{
					idxesEnd[0] = (UInt32)(OSInt)MemAlloc(UInt32, hdr[0] + 2);
					idxesEnd = (UInt32*)(OSInt)idxesEnd[0];
					idxesEnd[0] = 0;
					idxesEnd[1] = hdr[0];
					VFSFileRead(file, hdr[0] << 2, &idxesEnd[2], &readSize);
				}
				else
				{
					idxesEnd = idxes = MemAlloc(UInt32, hdr[0] + 2);
					idxesEnd[0] = 0;
					idxesEnd[1] = hdr[0];
					VFSFileRead(file, hdr[0] << 2, &idxesEnd[2], &readSize);
				}
			}
		}
		else
		{
			VFSFileSeek(file, vfsOriginCurrent, hdr[0] << 2);
		}
	}
	VFSFileClose(file);

	if (objCount == 0)
		return;

	// count number of records

	ides = MemAlloc(Int32, objCount);
	i = 0;
	idxesEnd = idxes;
	while (idxesEnd)
	{
		j = 0;
		idxes = idxesEnd;
		while (j < idxes[1])
			ides[i++] = idxes[2 + j++];
		idxesEnd = (UInt32*)(OSInt)idxes[0];
		MemFree(idxes);
	}

	//ides contains all record id to be shown on map
	//qpBsort((UInt32*)ides, 0, objCount - 1);
	//bsort((UInt32*)ides, 0, objCount-1);
	Data::Sort::QuickBubbleSort::Sort((UInt32*)ides, 0, objCount - 1);

	j = map->mapLines[lineStyle].nStyles;
	MyLine *lines = (MyLine*)map->mapLines[lineStyle].styles;
	UInt32 k = 0;

	//////////////////////////////////////////
	while (k < j)
	{
		SelectPen(map, lines, k, maxScale);

		
		MakeFileName(fName, fileName, L".cix");
		err = VFSFileOpen(cardNo, fName, vfsModeRead, &file);
		if (err)
		{
			MemFree(ides);
			return;
		}

		MakeFileName(fName, fileName, L".cip");
		err = VFSFileOpen(cardNo, fName, vfsModeRead, &pfile);
		if (err)
		{
			MemFree(ides);
			return;
		}

		// Search the cix file for record ID
		VFSFileRead(file, 4, hdr, &readSize);
		seeked = false;

		//long hScale = 1024 * map->scnWidth / map->ScaleWidth;// / ScnWidth;
		//long vScale = 1024 * map->scnHeight / map->ScaleHeight;// / ScnHeight;


		i = 0;
		while (i < objCount)
		{
			if (VFSFileRead(file, 8, hdr, &readSize))
				break;
			if (readSize == 0)
				break;

			if (hdr[0] == ides[i])
			{
				seeked = false;

				i++;

				VFSFileSeek(pfile, vfsOriginBeginning, hdr[1]);
				VFSFileRead(pfile, 8, contHdr, &readSize);
				parts = MemAlloc(Int32, contHdr[1]);
				VFSFileRead(pfile, contHdr[1] << 2, parts, &readSize);
				VFSFileRead(pfile, 4, &numPoints, &readSize);

				currPart = 0;
				points = MemAlloc(Int32, (numPoints + 1) << 1);

				VFSFileRead(pfile, numPoints << 3, points, &readSize);

#ifndef HAS_ASM32
				long nPoints;// = pointInParts;

				currPart = 0;
				nPoints = 0;
				j = numPoints;
				Int32 *tmpVal = points;
				while (j--)
				{
					*tmpVal++ = MulDiv(*tmpVal - map->left, map->scnWidth, map->ScaleWidth);
					*tmpVal++ = MulDiv(map->bottom - *tmpVal, map->scnHeight, map->ScaleHeight);
				}

				while (currPart < (contHdr[1] - 1))
				{
					parts[currPart] = parts[currPart + 1] - parts[currPart];
					currPart++;
				}
				parts[currPart] = numPoints - parts[currPart];
#else
				_asm
				{
					mov ebx,map
					mov edi,points
					mov ecx,numPoints
					cld
dluilop1:
					mov eax,dword ptr [edi]
					sub eax,MapVars.left[ebx]
					imul MapVars.scnWidth[ebx]
					idiv MapVars.ScaleWidth[ebx]
					stosd
					mov eax,MapVars.bottom[ebx]
					sub eax,dword ptr [edi]
					imul MapVars.scnHeight[ebx]
					idiv MapVars.ScaleHeight[ebx]
					stosd
					dec ecx
					jnz dluilop1
					
					mov ecx,dword ptr contHdr[4]
					mov esi,parts
					dec ecx
					jz dluilop3
dluilop2:
					mov edx,dword ptr [esi]
					add esi,4
					mov eax,dword ptr [esi]
					sub eax,edx
					mov dword ptr [esi-4],eax
					dec ecx
					jnz dluilop2
dluilop3:
					mov eax,dword ptr [esi]
					mov edx,dword ptr numPoints
					sub edx,eax
					mov dword ptr [esi],edx
				}
#endif

				Int32 l = 0;
				UInt32 m = 0;
				while (m < contHdr[1])
				{
					map->img->DrawPolyline(&points[l], parts[m], lines[k].pen);
					l += parts[m++] << 1;
				}
				MemFree(points);
				MemFree(parts);
			}
			else
			{
				if (!seeked)
				{
					VFSFileSeek(file, vfsOriginBeginning, (ides[i] << 3) + 4);
					seeked = true;
				}
			}
			while (i < objCount)
			{
				if (hdr[0] >= ides[i])
				{
					i++;
				}
				else
				{
					break;
				}
			}
		}
		VFSFileClose(file);
		VFSFileClose(pfile);

		k+=1;
	}

	MemFree(ides);
}


void Map::MapEngine::FillLine(void *m, WChar *fileName, Int32 maxScale, Int32 lineStyle, Int32 fillColor, Bool *drawn)
{
	MapVars *map = (MapVars*)m;
	if (map->canDraw == 0)
		return;

	Int32 leftBlk;
	Int32 rightBlk;
	Int32 topBlk;
	Int32 bottomBlk;
	
	Int32 blkScale;

	Err err;
	FileRef file;
	FileRef pfile;

	Int32 hdr[3]; //Total RecordNo, Shape Type
	UInt32 contHdr[2]; //RecordID, NumberOfParts
	Int32 *parts;
	Int32 numPoints;
	Int32 *points;
	UInt32 readSize;

//	Boolean lastPtValid;
	UInt32 currPart;
//	Int32 *pointPos;
//	Int32 pointInParts;

	// Open block index file
	UInt32 *idxes = 0;
	UInt32 *idxesEnd = 0;
	UInt32 objCount;
	Int32 *ides;
	UInt32 i,j;
	Bool seeked;

	WChar fName[256];

//	char debug[256];
	MakeFileName(fName, fileName, L".blk");
	err = VFSFileOpen(cardNo, fName, vfsModeRead, &file);
	if (err)
		return;

	// Read the useful blocks to Linked list - idxes and idxesEnd (Last element)
	VFSFileRead(file, 4, &currPart, &readSize);
	VFSFileRead(file, 4, &blkScale, &readSize);

	leftBlk = map->left / blkScale;
	rightBlk = map->right / blkScale;
	topBlk = map->top / blkScale;
	bottomBlk = map->bottom / blkScale;

	LARGE_INTEGER liSt;
	LARGE_INTEGER liEd;
	LARGE_INTEGER liFreq;
	Double spd;

	QueryPerformanceCounter(&liSt);

	objCount = 0;
	i = 0;
	while (currPart-- > 0)
	{
		VFSFileRead(file, 12, hdr, &readSize);
		if ((leftBlk <= hdr[1]) && (rightBlk >= hdr[1]) && (topBlk <= hdr[2]) && (bottomBlk >= hdr[2]))
		{
			if (drawn)
				*drawn = true;

			i++;
			objCount += hdr[0];
			if (hdr[0])
			{
				if (idxesEnd)
				{
					idxesEnd[0] = (UInt32)(OSInt)MemAlloc(UInt32, hdr[0] + 2);
					idxesEnd = (UInt32*)(OSInt)idxesEnd[0];
					idxesEnd[0] = 0;
					idxesEnd[1] = hdr[0];
					VFSFileRead(file, hdr[0] << 2, &idxesEnd[2], &readSize);
				}
				else
				{
					idxesEnd = idxes = MemAlloc(UInt32, hdr[0] + 2);
					idxesEnd[0] = 0;
					idxesEnd[1] = hdr[0];
					VFSFileRead(file, hdr[0] << 2, &idxesEnd[2], &readSize);
				}
			}
		}
		else
		{
			VFSFileSeek(file, vfsOriginCurrent, hdr[0] << 2);
		}
	}
	VFSFileClose(file);
	QueryPerformanceCounter(&liEd);
	QueryPerformanceFrequency(&liFreq);
	spd = (liEd.QuadPart - liSt.QuadPart) / (Double)liFreq.QuadPart;

	if (objCount == 0)
		return;

	// count number of records

	ides = MemAlloc(Int32, objCount);
	i = 0;
	idxesEnd = idxes;
	while (idxesEnd)
	{
		j = 0;
		idxes = idxesEnd;
		while (j < idxes[1])
			ides[i++] = idxes[2 + j++];
		idxesEnd = (UInt32*)(OSInt)idxes[0];
		MemFree(idxes);
	}

	//ides contains all record id to be shown on map
	//qpBsort((UInt32*)ides, 0, objCount - 1);
	//bsort((UInt32*)ides, 0, objCount-1);
	Data::Sort::QuickBubbleSort::Sort((UInt32*)ides, 0, objCount - 1);

	MakeFileName(fName, fileName, L".cix");
	err = VFSFileOpen(cardNo, fName, vfsModeRead, &file);
	if (err)
	{
		MemFree(ides);
		return;
	}

	MakeFileName(fName, fileName, L".cip");
	err = VFSFileOpen(cardNo, fName, vfsModeRead, &pfile);
	if (err)
	{
		MemFree(ides);
		VFSFileClose(file);
		return;
	}

	// Search the cix file for record ID
	VFSFileRead(file, 4, hdr, &readSize);
	seeked = false;

	Media::DrawBrush *brush = map->img->NewBrushARGB(fillColor);

	//long hScale = 1024 * map->scnWidth / map->ScaleWidth;// / ScnWidth;
	//long vScale = 1024 * map->scnHeight / map->ScaleHeight;// / ScnHeight;

	i = 0;
	while (i < objCount)
	{
		if (VFSFileRead(file, 8, hdr, &readSize))
			break;
		if (readSize == 0)
			break;
		if (hdr[0] == ides[i])
		{
			seeked = false;

			i++;

			VFSFileSeek(pfile, vfsOriginBeginning, hdr[1]);
			VFSFileRead(pfile, 8, contHdr, &readSize);
			parts = MemAlloc(Int32, contHdr[1]);
			VFSFileRead(pfile, contHdr[1] << 2, parts, &readSize);
			VFSFileRead(pfile, 4, &numPoints, &readSize);

			if (numPoints == 1709)
			{
				currPart = 0;
			}

			currPart = 0;
			points = MemAlloc(Int32, (numPoints + 1) << 1);

			VFSFileRead(pfile, numPoints << 3, points, &readSize);

#ifndef HAS_ASM32
			long nPoints;// = pointInParts;

			currPart = 0;
			nPoints = 0;
			j = numPoints;
			Int32 *tmpVal = points;
			while (j--)
			{
				*tmpVal++ = MulDiv(*tmpVal - map->left, map->scnWidth, map->ScaleWidth);
				*tmpVal++ = MulDiv(map->bottom - *tmpVal, map->scnHeight, map->ScaleHeight);
			}

			while (currPart < (contHdr[1] - 1))
			{
				parts[currPart] = parts[currPart + 1] - parts[currPart];
				currPart++;
			}
			parts[currPart] = numPoints - parts[currPart];
#else
			_asm
			{
				mov ebx,map
				mov edi,points
				mov ecx,numPoints
				cld
fluilop1:
				mov eax,dword ptr [edi]
				sub eax,MapVars.left[ebx]
				imul MapVars.scnWidth[ebx]
				idiv MapVars.ScaleWidth[ebx]
				stosd
				mov eax,MapVars.bottom[ebx]
				sub eax,dword ptr [edi]
				imul MapVars.scnHeight[ebx]
				idiv MapVars.ScaleHeight[ebx]
				stosd
				dec ecx
				jnz fluilop1
				
				mov ecx,dword ptr contHdr[4]
				mov esi,parts
				dec ecx
				jz fluilop3
fluilop2:
				mov edx,dword ptr [esi]
				add esi,4
				mov eax,dword ptr [esi]
				sub eax,edx
				mov dword ptr [esi-4],eax
				dec ecx
				jnz fluilop2
fluilop3:
				mov eax,dword ptr [esi]
				mov edx,dword ptr numPoints
				sub edx,eax
				mov dword ptr [esi],edx
			}
#endif

			//////////////////////////////////////////
			j = map->mapLines[lineStyle].nStyles;
			MyLine *lines = (MyLine*)map->mapLines[lineStyle].styles;
			SelectPen(map, lines, 0, maxScale);
			map->img->DrawPolyPolygon(points, parts, contHdr[1], lines->pen, brush);
			UInt32 k = 1;
			while (k < j)
			{
				SelectPen(map, lines, k, maxScale);
				map->img->DrawPolyPolygon(points, parts, contHdr[1], lines->pen, 0);
				k+=1;
			}
			MemFree(points);
			MemFree(parts);
		}
		else
		{
			if (!seeked)
			{
				VFSFileSeek(file, vfsOriginBeginning, (ides[i] << 3) + 4);
				seeked = true;
			}
		}
		while (i < objCount)
		{
			if (hdr[0] >= ides[i])
			{
				i++;
			}
			else
			{
				break;
			}
		}
	}

	if (brush)
	{
		map->img->DelBrush(brush);
	}

	MemFree(ides);
	VFSFileClose(file);
	VFSFileClose(pfile);
}

void Map::MapEngine::DrawString(void *m, WChar *fileName, Int32 priority, Int32 fontStyle, Int32 flags, Bool *drawn)
{
	MapVars *map = (MapVars*)m;
	Int32 leftBlk;
	Int32 rightBlk;
	Int32 topBlk;
	Int32 bottomBlk;

	Int32 blkScale;

	Err err;
	FileRef file;
	FileRef pfile;
	FileRef sfile;

	UInt32 hdr[3]; //Total RecordNo, Shape Type
	UInt32 contHdr[2]; //RecordID, NumberOfParts
	UInt32 numPoints;
	Int32 *points;
	UInt32 readSize;
	UInt32 recType;

	Int32 lastPtX;
	Int32 lastPtY;
	Int32 currPtX;
	Int32 currPtY;
	Int64 tempPtX;
	Int64 tempPtY;
	Bool currPtValid;
	Int32 currPart;
	Int32 *pointPos;

	// Open block index file
//	UInt32 *idxes = 0;
//	UInt32 *idxesEnd = 0;
	UInt32 i,j,k,l;
	UInt32 blkCnt;
	char buff[128];
	WChar fName[256];


	MakeFileName(fName, fileName, L".cix");
	err = VFSFileOpen(cardNo, fName, vfsModeRead, &file);
	if (err)
	{
		return;
	}

	MakeFileName(fName, fileName, L".cip");
	err = VFSFileOpen(cardNo, fName, vfsModeRead, &pfile);
	if (err)
	{
		VFSFileClose(file);
		return;
	}

	MakeFileName(fName, fileName, L".ciu");
	err = VFSFileOpen(cardNo, fName, vfsModeRead, &sfile);
	if (err)
	{
		VFSFileClose(pfile);
		VFSFileClose(file);
		return;
	}

	VFSFileRead(sfile, 4, &blkCnt, 0);
	VFSFileRead(sfile, 4, &blkScale, 0);

	leftBlk = map->left / blkScale;
	rightBlk = map->right / blkScale;
	topBlk = map->top / blkScale;
	bottomBlk = map->bottom / blkScale;

	if (blkCnt > 10)
	{
		i = 0;
		j = blkCnt - 1;
		while (((Int32)i) <= ((Int32)j))
		{
			k = (i + j) >> 1;
			VFSFileSeek(sfile, vfsOriginBeginning, 8 + k * 16);
			VFSFileRead(sfile, 16, buff, 0);
			if (*(Int32*)buff < leftBlk)
			{
				i = k + 1;
			}
			else if (*(Int32*)buff > leftBlk)
			{
				j = k - 1;
			}
			else if (*(Int32*)&buff[4] < topBlk)
			{
				i = k + 1;
			}
			else if (*(Int32*)&buff[4] > topBlk)
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
//	GetHex(buff, j);
//	ErrFatalDisplay(buff);
	VFSFileRead(pfile, 4, &recType, 0);
	VFSFileRead(pfile, 4, &recType, 0);

	k = j;
	while (k < blkCnt)
	{
		VFSFileSeek(sfile, vfsOriginBeginning, 8 + k * 16);
		VFSFileRead(sfile, 16, buff, 0);
		if (*(Int32*)buff > rightBlk)
			break;

		if ((*(Int32*)&buff[4] >= topBlk) && (*(Int32*)&buff[4] <= bottomBlk) && (*(Int32*)buff >= leftBlk))
		{
			if (drawn)
				*drawn = true;

			l = *(UInt32*)&buff[8];
			VFSFileSeek(sfile, vfsOriginBeginning, *(UInt32*)&buff[12]);
			while (l--)
			{
				VFSFileRead(sfile, 5, buff, &readSize);
				if (readSize == 0)
					break;
				if (buff[4] == 0)
					continue;

				VFSFileSeek(file, vfsOriginBeginning, ((*(UInt32*)buff) << 3) + 4);
				VFSFileRead(file, 8, hdr, &readSize);
				if (hdr[0] == *(UInt32*)buff)
				{
					Int32 *parts;
					VFSFileSeek(pfile, vfsOriginBeginning, hdr[1]);
					VFSFileRead(pfile, 8, contHdr, &readSize);
					if (flags & SFLG_SMART)
					{
						parts = MemAlloc(Int32, contHdr[1] + 1);
						VFSFileRead(pfile, contHdr[1] << 2, parts, &readSize);
					}
					else
					{
						VFSFileSeek(pfile, vfsOriginCurrent, contHdr[1] << 2);
					}

					VFSFileRead(pfile, 4, &numPoints, &readSize);
					points = MemAlloc(Int32, numPoints << 1);
					VFSFileRead(pfile, numPoints << 3, points, &readSize);
					pointPos = points;
					lastPtX = 0;
					lastPtY = 0;

					if (flags & SFLG_SMART)
					{
						UInt32 k;
						Int32 maxSize;
						Int32 maxPos;
						parts[contHdr[1]] = numPoints;
						maxSize = 0;
						k = 0;
						while (k < contHdr[1])
						{
							if ((parts[k + 1] - parts[k]) > maxSize)
								maxSize = (parts[k + 1] - (maxPos = parts[k]));
							k++;
						}
						j = buff[4];
						VFSFileRead(sfile, j, buff, &readSize);
						AddLabel(map, (WChar*)buff, j,maxSize, &pointPos[maxPos << 1], priority, recType, fontStyle, flags);
						MemFree(parts);
					}
					else if (recType == 3)
					{
						Int32 scaleW;
						Int32 scaleH;

						if (numPoints & 1)
						{
							lastPtX = pointPos[numPoints - 1];
							lastPtY = pointPos[numPoints];

							scaleW = pointPos[numPoints + 1] - pointPos[numPoints - 3];
							scaleH = pointPos[numPoints + 2] - pointPos[numPoints - 2];
						}
						else
						{
							lastPtX = (pointPos[numPoints - 2] + pointPos[numPoints]) >> 1;
							lastPtY = (pointPos[numPoints - 1] + pointPos[numPoints + 1]) >> 1;

							scaleW = pointPos[numPoints] - pointPos[numPoints - 2];
							scaleH = pointPos[numPoints + 1] - pointPos[numPoints - 1];
						}

#ifndef HAS_ASM32
						currPtX = MulDiv(lastPtX - (Int32)map->left, map->scnWidth, map->ScaleWidth);
						currPtY = MulDiv((Int32)map->bottom - lastPtY, map->scnHeight, map->ScaleHeight);
#else
						_asm
						{
							mov edi,map
							mov eax,lastPtX
							sub eax,MapVars.left[edi]
							imul MapVars.scnWidth[edi]
							idiv MapVars.ScaleWidth[edi]
							mov currPtX,eax

							mov eax,MapVars.bottom[edi]
							sub eax,lastPtY
							imul MapVars.scnHeight[edi]
							idiv MapVars.ScaleHeight[edi]
							mov currPtY,eax
						}
#endif
						currPtValid = ((currPtX >= 0) && (currPtY >= 0) && (currPtX < (Int32)map->scnWidth) && (currPtY < (Int32)map->scnHeight - BOTTOMBAR));
						j = buff[4];
						VFSFileRead(sfile, j, buff, &readSize);

						if (currPtValid)
						{
							if ((flags & SFLG_ROTATE) == 0)
								scaleW = scaleH = 0;
							FntDrawChars(map, (WChar*)buff, j, currPtX, currPtY, scaleW, scaleH, fontStyle, flags & SFLG_ALIGN);

//							GrpDrawCharsW((unsigned short*)buff, j, currPtX - (FntCharsWidthW((unsigned short*)buff, j >> 1) >> 1), currPtY - 6, rct);
							//GrpDrawChars(buff, j, currPtX, currPtY, rct);
						}
					}
					else
					{
						Int32 scaleW;
						Int32 scaleH;
#ifndef HAS_ASM32
						i = numPoints;
						while (i--)
						{
							lastPtX += *pointPos++;
							lastPtY += *pointPos++;
						}
						currPart = 0;
						currPtX = MulDiv((lastPtX / numPoints) - (Int32)map->left, map->scnWidth, map->ScaleWidth);
						currPtY = MulDiv((Int32)map->bottom - (lastPtY / numPoints), map->scnHeight, map->ScaleHeight);
#else
						_asm
						{
							mov edi,map
							mov ecx,numPoints
							mov dword ptr tempPtX[0],0
							mov dword ptr tempPtX[4],0
							mov dword ptr tempPtY[0],0
							mov dword ptr tempPtY[4],0
							mov esi,pointPos
dsnwlop:
							mov eax,dword ptr [esi]
							cdq
							add dword ptr tempPtX[0],eax
							adc dword ptr tempPtX[4],edx

							mov eax,dword ptr [esi+4]
							cdq
							add dword ptr tempPtY[0],eax
							adc dword ptr tempPtY[4],edx

							add esi,8
							dec ecx
							jnz dsnwlop

							mov currPart, 0

							mov eax, dword ptr tempPtX[0]
							mov edx, dword ptr tempPtX[4]
							idiv numPoints
							sub eax,MapVars.left[edi]
							imul MapVars.scnWidth[edi]
							idiv MapVars.ScaleWidth[edi]
							mov currPtX, eax

							mov eax,dword ptr tempPtY[0]
							mov edx,dword ptr tempPtY[4]
							idiv numPoints
							mov ebx,MapVars.bottom[edi]
							sub ebx,eax
							mov eax,ebx
							imul MapVars.scnHeight[edi]
							idiv MapVars.ScaleHeight[edi]
							mov currPtY,eax
						}
#endif
						currPtValid = ((currPtX >= 0) && (currPtY >= 0) && (currPtX < (Int32)map->scnWidth) && (currPtY < (Int32)map->scnHeight - BOTTOMBAR));
						j = buff[4];
						VFSFileRead(sfile, j, buff, &readSize);

						if (currPtValid)
						{
//							if ((flags & SFLG_ROTATE) == 0)
							scaleW = scaleH = 0;
							FntDrawChars(map, (WChar*)buff, j, currPtX, currPtY, scaleW, scaleH, fontStyle, flags & SFLG_ALIGN);

//							GrpDrawCharsW((unsigned short*)buff, j, currPtX - (FntCharsWidthW((unsigned short*)buff, j >> 1) >> 1), currPtY - 6, rct);
							//GrpDrawChars(buff, j, currPtX, currPtY, rct);
						}
					}

					MemFree(points);
				}
				else
				{
//					ErrFatalDisplay("Error in the id");
					VFSFileSeek(sfile, vfsOriginCurrent, buff[4]);
				}
			}
		}
		k++;
	}

	VFSFileClose(sfile);
	VFSFileClose(file);
	VFSFileClose(pfile);
}

void Map::MapEngine::DrawPoints(void *m, WChar *fileName, WChar *imgFile, Bool *drawn)
{
	MapVars *map = (MapVars*)m;
	Int32 leftBlk;
	Int32 rightBlk;
	Int32 topBlk;
	Int32 bottomBlk;

	Int32 blkScale;

	Err err;
	FileRef file;
	FileRef pfile;
	FileRef sfile;

	Int32 hdr[3]; //Total RecordNo, Shape Type
	Int32 contHdr[2]; //RecordID, NumberOfParts
	Int32 numPoints;
	Int32 *points;
	UInt32 readSize;
	Int32 recType;

	Int32 lastPtX;
	Int32 lastPtY;
	Int32 currPtX;
	Int32 currPtY;
	Bool currPtValid;
	Int32 currPart;
	Int32 *pointPos;

	// Open block index file
//	Int32 *idxes = 0;
//	Int32 *idxesEnd = 0;
	Int32 i,j,k,l;
	Int32 blkCnt;
	char buff[128];
	WChar fName[256];
	Media::DrawImage *mbm;

//	if (x < XMIN)
//		x = XMIN;
//	else if (x > XMAX)
//		x = XMAX;
//	if (y < YMIN)
//		y = YMIN;
//	else if (y > YMAX)
//		y = YMAX;

	if (imgFile == 0)
		return;
	if (*imgFile == 0)
		return;
	MakeFileName(fName, fileName, L".cix");
	err = VFSFileOpen(cardNo, fName, vfsModeRead, &file);
	if (err)
	{
		return;
	}

	MakeFileName(fName, fileName, L".cip");
	err = VFSFileOpen(cardNo, fName, vfsModeRead, &pfile);
	if (err)
	{
		VFSFileClose(file);
		return;
	}

	MakeFileName(fName, fileName, L".ciu");
	err = VFSFileOpen(cardNo, fName, vfsModeRead, &sfile);
	if (err)
	{
		VFSFileClose(pfile);
		VFSFileClose(file);
		return;
	}

	if ((mbm = map->eng->LoadImageW(imgFile)) == 0)
	{
		VFSFileClose(sfile);
		VFSFileClose(pfile);
		VFSFileClose(file);
		return;
	}

	VFSFileRead(sfile, 4, &blkCnt, 0);
	VFSFileRead(sfile, 4, &blkScale, 0);

	leftBlk = map->left / blkScale;
	rightBlk = map->right / blkScale;
	topBlk = map->top / blkScale;
	bottomBlk = map->bottom / blkScale;

	if (blkCnt > 10)
	{
		i = 0;
		j = blkCnt - 1;
		while (((Int32)i) <= ((Int32)j))
		{
			k = (i + j) >> 1;
			VFSFileSeek(sfile, vfsOriginBeginning, 8 + k * 16);
			VFSFileRead(sfile, 16, buff, 0);
			if (*(Int32*)buff < leftBlk)
			{
				i = k + 1;
			}
			else if (*(Int32*)buff > leftBlk)
			{
				j = k - 1;
			}
			else if (*(Int32*)&buff[4] < topBlk)
			{
				i = k + 1;
			}
			else if (*(Int32*)&buff[4] > topBlk)
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
//	GetHex(buff, j);
//	ErrFatalDisplay(buff);
	VFSFileRead(pfile, 4, &recType, 0);
	VFSFileRead(pfile, 4, &recType, 0);

	k = j;
	while (k < blkCnt)
	{
		VFSFileSeek(sfile, vfsOriginBeginning, 8 + k * 16);
		VFSFileRead(sfile, 16, buff, 0);
		if (*(Int32*)buff > rightBlk)
			break;

		if ((*(Int32*)&buff[4] >= topBlk) && (*(Int32*)&buff[4] <= bottomBlk) && (*(Int32*)buff >= leftBlk))
		{
			if (drawn)
				*drawn = true;

			l = *(Int32*)&buff[8];
			VFSFileSeek(sfile, vfsOriginBeginning, *(Int32*)&buff[12]);
			while (l--)
			{
				VFSFileRead(sfile, 5, buff, &readSize);
				if (readSize == 0)
					break;
//				if (buff[4] == 0)
//					continue;

				VFSFileSeek(file, vfsOriginBeginning, ((*(Int32*)buff) << 3) + 4);
				VFSFileRead(file, 8, hdr, &readSize);
				if ((UInt32)hdr[0] == *(UInt32*)buff)
				{
					VFSFileSeek(pfile, vfsOriginBeginning, hdr[1]);
					VFSFileRead(pfile, 8, contHdr, &readSize);
					VFSFileSeek(pfile, vfsOriginCurrent, contHdr[1] << 2);
					VFSFileRead(pfile, 4, &numPoints, &readSize);
					points = MemAlloc(Int32, numPoints << 1);
					VFSFileRead(pfile, numPoints << 3, points, &readSize);
					pointPos = points;
					lastPtX = 0;
					lastPtY = 0;

					if (recType == 3)
					{
						if (numPoints & 1)
						{
							lastPtX = pointPos[numPoints - 1];
							lastPtY = pointPos[numPoints];
						}
						else
						{
							lastPtX = (pointPos[numPoints - 2] + pointPos[numPoints]) >> 1;
							lastPtY = (pointPos[numPoints - 1] + pointPos[numPoints + 1]) >> 1;
						}

#ifndef HAS_ASM32
						currPtX = MulDiv(lastPtX - map->left, map->scnWidth, map->ScaleWidth);
						currPtY = MulDiv(map->bottom - lastPtY, map->scnHeight, map->ScaleHeight);
#else
						_asm
						{
							mov edi,map
							mov eax,lastPtX
							sub eax,MapVars.left[edi]
							imul MapVars.scnWidth[edi]
							idiv MapVars.ScaleWidth[edi]
							mov currPtX,eax

							mov eax,MapVars.bottom[edi]
							sub eax,lastPtY
							imul MapVars.scnHeight[edi]
							idiv MapVars.ScaleHeight[edi]
							mov currPtY,eax
						}
#endif
						currPtValid = ((currPtX >= 0) && (currPtY >= 0) && (currPtX < (Int32)map->scnWidth) && (currPtY < (Int32)map->scnHeight - BOTTOMBAR));
						j = buff[4];
						VFSFileRead(sfile, j, buff, &readSize);

						if (currPtValid)
						{
							map->img->DrawImagePt(mbm, currPtX - (mbm->GetWidth() >> 1), currPtY - (mbm->GetHeight() >> 1));
						}
					}
					else
					{
#ifndef HAS_ASM32
						i = numPoints;
						while (i--)
						{
							lastPtX += *pointPos++;
							lastPtY += *pointPos++;
						}
						currPart = 0;
						currPtX = MulDiv((lastPtX / numPoints) - (Int32)map->left, map->scnWidth, map->ScaleWidth);
						currPtY = MulDiv((Int32)map->bottom - (lastPtY / numPoints), map->scnHeight, map->ScaleHeight);
#else
						long val1;
						long val2;
						long val3;
						long val4;
						_asm
						{
							mov edi,map
							mov edx,numPoints
							mov i,edx
							mov val1,0
							mov val2,0
							mov val3,0
							mov val4,0
							mov esi,pointPos
dsnwlop:
							mov eax,dword ptr [esi]
							cdq
							add val1,eax
							adc val2,edx

							mov eax,dword ptr [esi+4]
							cdq
							add val3,eax
							add val4,edx
							add esi,8
							dec i
							jnz dsnwlop

							mov currPart, 0

							mov eax,val1
							mov edx,val2
							idiv numPoints
							sub eax,MapVars.left[edi]
							imul MapVars.scnWidth[edi]
							idiv MapVars.ScaleWidth[edi]
							mov currPtX, eax

							mov eax,val3
							mov edx,val4
							idiv numPoints
							mov ebx,MapVars.bottom[edi]
							sub ebx,eax
							mov eax,ebx
							imul MapVars.scnHeight[edi]
							idiv MapVars.ScaleHeight[edi]
							mov currPtY,eax
						}
#endif
						currPtValid = ((currPtX >= 0) && (currPtY >= 0) && (currPtX < (Int32)map->scnWidth) && (currPtY < (Int32)map->scnHeight - BOTTOMBAR));
						j = buff[4];
						VFSFileRead(sfile, j, buff, &readSize);

						if (currPtValid)
						{
							map->img->DrawImagePt(mbm, currPtX - (mbm->GetWidth() >> 1), currPtY - (mbm->GetHeight() >> 1));
						}
					}

					MemFree(points);
				}
				else
				{
//					ErrFatalDisplay("Error in the id");
					VFSFileSeek(sfile, vfsOriginCurrent, buff[4]);
				}
			}
		}
		k++;
	}
	map->eng->DeleteImage(mbm);
	VFSFileClose(sfile);
	VFSFileClose(file);
	VFSFileClose(pfile);
}

void Map::MapEngine::DrawMarker(void *m, Int32 xpos, Int32 ypos, Int32 color)
{
	MapVars *map = (MapVars*)m;
	Int32 currPtX;
	Int32 currPtY;
	currPtX = MulDiv(xpos - map->left, map->scnWidth, map->ScaleWidth);
	currPtY = MulDiv(map->bottom - ypos, map->scnHeight, map->ScaleHeight);
	Media::DrawPen *p = map->img->NewPenARGB(color, 1, 0, 0);
	map->img->DrawLine(currPtX - 5, currPtY, currPtX + 6, currPtY, p);
	map->img->DrawLine(currPtX, currPtY - 5, currPtX, currPtY + 6, p);
	map->img->DelPen(p);
}

/*PHP_FUNCTION(map_get_bound)
{
	long m;
	Char *fileName;
	Int32 fileNameSize;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &m, &fileName, &fileNameSize) == FAILURE) {
		return;
	}
	MapVars *map = (MapVars*)m;
	char fName[256];
	Int32 blkCnt;
	Int32 blkScale;
	FileRef sfile;
	Int32 i;
	Int32 xMin;
	Int32 xMax;
	Int32 yMin;
	Int32 yMax;
	Err err;
	char buff[16];

	MakeFileName(fName, fileName, ".ciu");
	err = VFSFileOpen(cardNo, fName, vfsModeRead, &sfile);
	if (err)
	{
		RETURN_LONG(1);
	}

	VFSFileRead(sfile, 4, &blkCnt, 0);
	VFSFileRead(sfile, 4, &blkScale, 0);

	if (blkCnt <= 0)
	{
		VFSFileClose(sfile);
		RETURN_LONG(2);
	}

	i = 0;
	VFSFileSeek(sfile, vfsOriginBeginning, 8 + i * 16);
	VFSFileRead(sfile, 16, buff, 0);
	xMin = xMax = *(Int32*)buff;
	yMin = yMax = *(Int32*)&buff[4];
	while (i < blkCnt)
	{
		VFSFileSeek(sfile, vfsOriginBeginning, 8 + i * 16);
		VFSFileRead(sfile, 16, buff, 0);
		if (*(Int32*)buff < xMin)
		{
			xMin = *(Int32*)buff;
		}
		if (*(Int32*)buff > xMax)
		{
			xMax = *(Int32*)buff;
		}
		if (*(Int32*)&buff[4] < yMin)
		{
			yMin = *(Int32*)&buff[4];
		}
		if (*(Int32*)&buff[4] > yMax)
		{
			yMax = *(Int32*)&buff[4];
		}
		i += 1;
	}
	
	VFSFileClose(sfile);
	*x1 = xMin * blkScale;
	*x2 = (xMax + 1) * blkScale;
	*y1 = yMin * blkScale;
	*y2 = (yMax + 1) * blkScale;
	RETURN_LONG(0);
}*/

