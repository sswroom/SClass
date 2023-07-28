#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/IComparable.h"
#include "Data/ArrayListCmp.h"
#include "Data/Sort/ArtificialQuickSortC.h"
#include "Media/ImageTo8Bit.h"

extern "C"
{
	Int32 ImageTo8Bit_CalDiff(Int32 col1, Int32 col2);
}

namespace Media
{
	class ColorStat : public Data::IComparable
	{
	public:
		Int32 color;
		UOSInt count;
		UOSInt index;

		ColorStat()
		{
			this->color = 0;
			this->count = 0;
			this->index = 0;
		}

		virtual ~ColorStat()
		{

		}

		virtual OSInt CompareTo(Data::IComparable *obj) const
		{
			UOSInt cnt = ((Media::ColorStat*)obj)->count;
			if (cnt > count)
			{
				return 1;
			}
			else if (cnt < count)
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}
	};
}

void Media::ImageTo8Bit::From32bpp(UInt8 *src, UInt8 *dest, UInt8 *palette, UOSInt width, UOSInt height, OSInt sbpl, OSInt dbpl)
{
	Data::ArrayListCmp *arr[256];
	UOSInt i;
	UOSInt j;
	UOSInt h;
	UOSInt l;
	UOSInt k;
	UInt8 *currPtr;
	UInt8 *currPtr2;
	UInt8 *ptr;
	UInt8 *ptr2;
	Data::ArrayListCmp *currArr;
	Data::ArrayListCmp *cArr;
	Media::ColorStat *cs;
	Media::ColorStat *cs2;
	UOSInt colorCnt = 0;

	i = 256;
	while (i-- > 0)
	{
		arr[i] = 0;
	}

	ptr = src;
	h = height;
	while (h-- > 0)
	{
		currPtr = ptr;
		j = width;
		while (j-- > 0)
		{
			currArr = arr[*currPtr];
			if (currArr == 0)
			{
				NEW_CLASS(currArr, Data::ArrayListCmp());
                arr[*currPtr] = currArr;
			}
			l = 0;
			k = currArr->GetCount();
			while (k-- > 0)
			{
				cs = (Media::ColorStat*) currArr->GetItem(k);
				if (cs->color == *(Int32*)currPtr)
				{
					l = 1;
					cs->count++;
					break;
				}
			}
			if (l == 0)
			{
				NEW_CLASS(cs, ColorStat());
				cs->color = *(Int32*)currPtr;
				cs->count = 1;
				currArr->Add(cs);
				colorCnt++;
				if (colorCnt >= 1024)
				{
					break;
				}
			}
			currPtr += 4;
		}
		if (colorCnt > 1024)
		{
			break;
		}
		ptr += sbpl;
	}

	if (colorCnt > 1024)
	{
		UOSInt rVal;
		UOSInt gVal;
		UOSInt bVal;
		UInt8 *cols;
		i = 256;
		while (i-- > 0)
		{
			currArr = arr[i];
			if (currArr)
			{
				j = currArr->GetCount();
				while (j-- > 0)
				{
					cs = (ColorStat*)currArr->GetItem(i);
					DEL_CLASS(cs);
				}
				DEL_CLASS(currArr);
			}
		}

		cols = palette;
		i = 0;
		while (i < 8)
		{
			rVal = (i << 5) | (i << 2) | (i >> 1);
			gVal = 0;
			j = 0;
			while (j < 4)
			{
				k = 0;
				while (k < 8)
				{
					bVal = (k << 5) | (k << 2) | (k >> 1);
					cols[0] = (UInt8)bVal;
					cols[1] = (UInt8)gVal;
					cols[2] = (UInt8)rVal;
					cols[3] = 0xff;
					cols += 4;
					k++;
				}
				gVal += 0x55;
				j++;
			}
			i++;
		}

		UInt8 r;
		UInt8 g;
		UInt8 b;
		ptr = src;
		ptr2 = dest;
		h = height;
		while (h-- > 0)
		{
			rVal = 0;
			gVal = 0;
			bVal = 0;
			currPtr = ptr;
			currPtr2 = ptr2;
			j = width;
			while (j-- > 0)
			{
				bVal += currPtr[0];
				gVal += currPtr[1];
				rVal += currPtr[2];
				if (rVal < 0)
				{
					r = 0;
				}
				else if (rVal > 255)
				{
					r = 7;
				}
				else
				{
					r = (UInt8)(rVal >> 5);
				}
				if (gVal < 0)
				{
					g = 0;
				}
				else if (gVal > 255)
				{
					g = 3;
				}
				else
				{
					g = (UInt8)(gVal >> 6);
				}
				if (bVal < 0)
				{
					b = 0;
				}
				else if (bVal > 255)
				{
					b = 7;
				}
				else
				{
					b = (UInt8)(bVal >> 5);
				}

				*currPtr2 = (UInt8)((r << 5) | (g << 3) | b);
				rVal = rVal - (UOSInt)((r << 5) | (r << 2) | (r >> 1));
				gVal = gVal - (UOSInt)((g << 6) | (g << 4) | (g << 2) | g);
				bVal = bVal - (UOSInt)((b << 5) | (b << 2) | (b >> 1));

				currPtr += 4;
				currPtr2++;
			}
			ptr += sbpl;
			ptr2 += dbpl;
		}
	}
	else
	{
		NEW_CLASS(cArr, Data::ArrayListCmp());
		i = 256;
		while (i-- > 0)
		{
			NotNullPtr<Data::ArrayListCmp> carr;
			if (carr.Set(arr[i]))
			{
				cArr->AddAll(carr);
			}
		}
		UOSInt arrSize;
		Data::IComparable **cmpArr = cArr->GetArray(&arrSize);
		ArtificialQuickSort_SortCmpO(cmpArr, 0, (OSInt)arrSize - 1);

		Int32 *cols = (Int32*)palette;
		if (cArr->GetCount() > 256)
		{
			OSInt colorProc = (OSInt)cArr->GetCount() - 256;
			Int32 currDiff;
			Int32 minDiff;
			UOSInt minIndex;
			i = 256;
			while (i-- > 0)
			{
				cs = (ColorStat*)cArr->GetItem(i);
				cs->index = i;
				cols[i] = cs->color;
			}

			j = 256;

			i = cArr->GetCount();
			while (i-- > 0)
			{
				cs = (ColorStat*)cArr->GetItem(i);
				minDiff = 64;
				minIndex = 0;
				l = 0;
				k = 0;
				while (k < j)
				{
					cs2 = (ColorStat*)cArr->GetItem(k);
					if ((currDiff = ImageTo8Bit_CalDiff(cs2->color, cs->color)) < 64)
					{
						l = 1;
						if (currDiff < minDiff)
						{
							minDiff = currDiff;
							minIndex = cs2->index;
						}
					}
					k++;
				}
				if (l != 0)
				{
					cs->index = minIndex;
					colorProc--;
				}
				else
				{
					k = cArr->GetCount();
					while (--k > i)
					{
						cs2 = (ColorStat*)cArr->GetItem(k);
						if ((currDiff = ImageTo8Bit_CalDiff(cs2->color, cs->color)) < 64)
						{
							l = 1;
							if (currDiff < minDiff)
							{
								minDiff = currDiff;
								minIndex = cs2->index;
							}
						}
					}

					if (l != 0)
					{
						cs->index = minIndex;
						colorProc--;
					}
					else
					{
						l = 1;
						cs->index = --j;
						cols[j] = cs->color;
					}
				}
				if (colorProc <= 0)
					break;
			}
		}
		else
		{
			i = cArr->GetCount();
			while (i-- > 0)
			{
				cs = (ColorStat*)cArr->GetItem(i);
				cols[i] = cs->color;
				cs->index = i;
			}
		}

		ptr = src;
		ptr2 = dest;
		h = height;
		while (h-- > 0)
		{
			currPtr = ptr;
			currPtr2 = ptr2;
			j = width;
			while (j-- > 0)
			{
				currArr = arr[*currPtr];
				l = 0;
				k = currArr->GetCount();
				while (k-- > 0)
				{
					cs = (ColorStat*)currArr->GetItem(k);
					if (cs->color == *(Int32*)currPtr)
					{
						l = 1;
						*currPtr2 = (UInt8)cs->index;
						break;
					}
				}
				if (l == 0)
				{
					*currPtr2 = 0;
				}
				currPtr += 4;
				currPtr2++;
			}
			ptr += sbpl;
			ptr2 += dbpl;
		}

		i = 256;
		while (i-- > 0)
		{
			currArr = arr[i];
			if (currArr)
			{
				DEL_CLASS(currArr);
			}
		}
		i = cArr->GetCount();
		while (i-- > 0)
		{
			cs = (ColorStat*)cArr->RemoveAt(i);
			DEL_CLASS(cs);
		}
		DEL_CLASS(cArr);
	}
}
