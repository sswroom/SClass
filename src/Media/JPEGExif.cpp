#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include "IO/Stream.h"
#include "Media/JPEGExif.h"
#include <memory.h>

#define BUFFSIZE 1024

void Media::JPEGExif::FreeExif(ExifValue *exif)
{
	if (exif->t == EXIF_TYPE_GROUP)
	{
		if (exif->s)
		{
			OSInt i;
			Data::ArrayList<ExifValue*> *exifArr = (Data::ArrayList<ExifValue*> *)exif->s;
			i = exifArr->GetCount();
			while (i-- > 0)
			{
				FreeExif((Media::JPEGExif::ExifValue*)exifArr->GetItem(i));
			}
			DEL_CLASS(exifArr);
		}
	}
	else
	{
		if (exif->s)
			MemFree(exif->s);
	}
	MemFree(exif);
}

Media::JPEGExif::ExifValue *Media::JPEGExif::DupExif(ExifValue *exif)
{
	Media::JPEGExif::ExifValue *nexif;
	nexif = MemAlloc(Media::JPEGExif::ExifValue, 1);
	nexif->id = exif->id;
	nexif->numerator = exif->numerator;
	nexif->denominator = exif->denominator;
	if (exif->s)
	{
		if (exif->t == Media::JPEGExif::EXIF_TYPE_BYTE || exif->t == Media::JPEGExif::EXIF_TYPE_UNK)
		{
			nexif->s = MemAlloc(Char, exif->numerator);
			MemCopyNO(nexif->s, exif->s, exif->numerator);
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_NUMARR)
		{
			nexif->s = MemAlloc(Char, exif->numerator);
			MemCopyNO(nexif->s, exif->s, exif->numerator * 8);
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_GROUP)
		{
			Int32 i;
			Data::ArrayList<ExifValue*> *tmpArr;
			Data::ArrayList<ExifValue*> *tmpArr2 = (Data::ArrayList<ExifValue*>*)exif->s;
			NEW_CLASS(tmpArr, Data::ArrayList<ExifValue*>());
			nexif->s = (Char*)tmpArr;
			i = 0;
			while (i < tmpArr2->GetCount())
			{
				tmpArr->Add(DupExif(tmpArr2->GetItem(i)));
			}
		}
		else
		{
			nexif->s = MemAlloc(Char, Text::StrCharCnt(exif->s) + 1);
			Text::StrConcat(nexif->s, exif->s);
		}
	}
	else
	{
		nexif->s = 0;
	}
	return nexif;
}

Media::JPEGExif::ExifValue *Media::JPEGExif::GetExif(ExifValue *grp, Int32 id)
{
	Data::ArrayList<ExifValue*> *exifArr;
	if (grp)
	{
		exifArr = (Data::ArrayList<ExifValue*>*)grp->s;
	}
	else
	{
		exifArr = this->exifs;
	}

	OSInt i = exifArr->GetCount();
	Media::JPEGExif::ExifValue *exif;
	while (i-- > 0)
	{
		exif = exifArr->GetItem(i);
		if (exif->id == id)
			return exif;
	}
	exif = MemAlloc(Media::JPEGExif::ExifValue, 1);
	exif->id = id;
	exif->numerator = 0;
	exif->denominator = 0;
	exif->s = 0;
	exifArr->Add(exif);
	return exif;
}

void Media::JPEGExif::CalExifSize(Data::ArrayList<ExifValue*> *exifArr, UOSInt *size, UOSInt *endOfst)
{
	UOSInt i = 6;
	UOSInt j = 6;
	UOSInt k;
	UOSInt l;
	UOSInt m;
	Media::JPEGExif::ExifValue *exif;

	k = exifArr->GetCount();
	while (k-- > 0)
	{
		exif = (Media::JPEGExif::ExifValue *)exifArr->GetItem(k);
		if (exif->t == Media::JPEGExif::EXIF_TYPE_NUM)
		{
			i += 12;
			j += 20;
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_STR)
		{
			l = Text::StrCharCnt(exif->s) + 1;
			i += 12;
			if (l <= 4)
				j += 12;
			else
				j += 12 + l;
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_BYTE)
		{
			i += 12;
			if (exif->numerator <= 4)
			{
				j += 12;
			}
			else
			{
				j += 12 + exif->numerator;
			}
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_UNK)
		{
			i += 12;
			if (exif->numerator <= 4)
			{
				j += 12;
			}
			else
			{
				j += 12 + exif->numerator;
			}
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_SHORT)
		{
			i += 12;
			j += 12;
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_LONG)
		{
			i += 12;
			j += 12;
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_NUMARR)
		{
			i += 12;
			j += 12 + 8 * exif->numerator;
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_GROUP)
		{
			i += 12;
			j += 12;
			CalExifSize((Data::ArrayList<ExifValue*>*)exif->s, &l, &m);
			i += m;
			j += l;
		}
	}
	*size = j;
	*endOfst = i;
}

void Media::JPEGExif::GenExifBuff(UInt8 *buff, Data::ArrayList<ExifValue*> *exifArr, UOSInt *startOfst, UOSInt *otherOfst)
{
	UOSInt objCnt;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Media::JPEGExif::ExifValue *exif;

	objCnt = 0;
	k = *otherOfst;
	j = *startOfst + 2;
	i = 0;
	while (i < exifArr->GetCount())
	{
		exif = (Media::JPEGExif::ExifValue *)exifArr->GetItem(i);
		if (exif->t == Media::JPEGExif::EXIF_TYPE_NUM)
		{
			*(Int16*)&buff[j] = exif->id;
			*(Int16*)&buff[j + 2] = 5;
			*(Int32*)&buff[j + 4] = 1;
			*(Int32*)&buff[j + 8] = k;
			*(Int32*)&buff[k] = exif->numerator;
			*(Int32*)&buff[k + 4] = exif->denominator;
			j += 12;
			k += 8;
			objCnt++;
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_STR)
		{
			*(Int16*)&buff[j] = exif->id;
			*(Int16*)&buff[j + 2] = 2;
			*(Int32*)&buff[j + 4] = (Int32)Text::StrCharCnt(exif->s) + 1;
			if (*(Int32*)&buff[j + 4] <= 4)
			{
				*(Int32*)&buff[j + 8] = 0;
				Text::StrConcat((Char*)&buff[j + 8], exif->s);
				j += 12;
			}
			else
			{
				*(Int32*)&buff[j + 8] = k;
				Text::StrConcat((Char*)&buff[k], exif->s);
				k += *(Int32*)&buff[j + 4];
				j += 12;
			}
			objCnt++;
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_BYTE)
		{
			*(Int16*)&buff[j] = exif->id;
			*(Int16*)&buff[j + 2] = 1;
			*(Int32*)&buff[j + 4] = exif->numerator;
			if (*(Int32*)&buff[j + 4] <= 4)
			{
				*(Int32*)&buff[j + 8] = 0;
				MemCopyNO(&buff[j + 8], exif->s, exif->numerator);
				j += 12;
			}
			else
			{
				*(Int32*)&buff[j + 8] = k;
				MemCopyNO(&buff[k], exif->s, exif->numerator);
				k += *(Int32*)&buff[j + 4];
				j += 12;
			}
			objCnt++;
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_UNK)
		{
			*(Int16*)&buff[j] = exif->id;
			*(Int16*)&buff[j + 2] = 7;
			*(Int32*)&buff[j + 4] = exif->numerator;
			if (*(Int32*)&buff[j + 4] <= 4)
			{
				*(Int32*)&buff[j + 8] = 0;
				MemCopyNO(&buff[j + 8], exif->s, exif->numerator);
				j += 12;
			}
			else
			{
				*(Int32*)&buff[j + 8] = k;
				MemCopyNO(&buff[k], exif->s, exif->numerator);
				k += *(Int32*)&buff[j + 4];
				j += 12;
			}
			objCnt++;
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_SHORT)
		{
			*(Int16*)&buff[j] = exif->id;
			*(Int16*)&buff[j + 2] = 3;
			*(Int32*)&buff[j + 4] = 1;
			*(Int32*)&buff[j + 8] = exif->numerator;
			objCnt++;
			j += 12;
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_LONG)
		{
			*(Int16*)&buff[j] = exif->id;
			*(Int16*)&buff[j + 2] = 4;
			*(Int32*)&buff[j + 4] = 1;
			*(Int32*)&buff[j + 8] = exif->numerator;
			objCnt++;
			j += 12;
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_NUMARR)
		{
			*(Int16*)&buff[j] = exif->id;
			*(Int16*)&buff[j + 2] = 5;
			*(Int32*)&buff[j + 4] = exif->numerator;
			*(Int32*)&buff[j + 8] = k;
			MemCopyNO(&buff[k], exif->s, exif->numerator * 8);
			objCnt++;
			j += 12;
			k += exif->numerator * 8;
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_GROUP)
		{
			*(Int16*)&buff[j] = exif->id;
			*(Int16*)&buff[j + 2] = 4;
			*(Int32*)&buff[j + 4] = 1;
			exif->numerator = j + 8;
//			*(Int32*)&buff[j + 8] = k;
			j += 12;
			objCnt++;
		}
		i++;
	}
	*(Int32*)&buff[j] = 0;
	*(Int16*)&buff[*startOfst] = objCnt;
	j += 4;

	i = 0;
	while (i < exifArr->GetCount())
	{
		exif = (Media::JPEGExif::ExifValue *)exifArr->GetItem(i);
		if (exif->t == Media::JPEGExif::EXIF_TYPE_GROUP)
		{
			*(Int32*)&buff[exif->numerator] = j;
			GenExifBuff(buff, (Data::ArrayList<ExifValue*>*)exif->s, &j, &k);
		}
		i++;
	}
	*startOfst = j;
	*otherOfst = k;
}

Media::JPEGExif::JPEGExif()
{
	NEW_CLASS(this->exifs, Data::ArrayList<ExifValue*>());
}

Media::JPEGExif::~JPEGExif()
{
	OSInt i = this->exifs->GetCount();
	while (i-- > 0)
	{
		FreeExif((Media::JPEGExif::ExifValue*)this->exifs->GetItem(i));
	}
	DEL_CLASS(this->exifs);
}

Media::JPEGExif::ExifValue *Media::JPEGExif::AddExifGroup(Int32 id)
{
	Media::JPEGExif::ExifValue *val;
	Data::ArrayList<ExifValue*> *exifArr;
	val = MemAlloc(Media::JPEGExif::ExifValue, 1);
	val->id = id;
	val->t = Media::JPEGExif::EXIF_TYPE_GROUP;
	NEW_CLASS(exifArr, Data::ArrayList<ExifValue*>());
	val->s = (Char*)exifArr;
	this->exifs->Add(val);
	return val;
}

void Media::JPEGExif::SetExif(Media::JPEGExif::ExifValue *grp, Int32 id, Int32 numerator, Int32 denominator)
{
	Media::JPEGExif::ExifValue *val = GetExif(grp, id);
	if (val->s)
	{
		MemFree(val->s);
		val->s = 0;
	}
	val->t = Media::JPEGExif::EXIF_TYPE_NUM;
	val->numerator = numerator;
	val->denominator = denominator;
}

void Media::JPEGExif::SetExif(Media::JPEGExif::ExifValue *grp, Int32 id, const Char *s)
{
	Media::JPEGExif::ExifValue *val = GetExif(grp, id);
	if (val->s)
	{
		MemFree(val->s);
		val->s = 0;
	}
	val->t = Media::JPEGExif::EXIF_TYPE_STR;
	val->s = MemAlloc(Char, Text::StrCharCnt(s) + 1);
	Text::StrConcat(val->s, s);
}

void Media::JPEGExif::SetExif(Media::JPEGExif::ExifValue *grp, Int32 id, const UInt8 *s, Int32 size)
{
	Media::JPEGExif::ExifValue *val = GetExif(grp, id);
	if (val->s)
	{
		MemFree(val->s);
		val->s = 0;
	}
	val->t = Media::JPEGExif::EXIF_TYPE_BYTE;
	val->s = MemAlloc(Char, size);
	val->numerator = size;
	MemCopyNO(val->s, s, size);
}

void Media::JPEGExif::SetExifUnk(Media::JPEGExif::ExifValue *grp, Int32 id, const UInt8 *s, Int32 size)
{
	Media::JPEGExif::ExifValue *val = GetExif(grp, id);
	if (val->s)
	{
		MemFree(val->s);
		val->s = 0;
	}
	val->t = Media::JPEGExif::EXIF_TYPE_UNK;
	val->s = MemAlloc(Char, size);
	val->numerator = size;
	MemCopyNO(val->s, s, size);
}

void Media::JPEGExif::SetExif(Media::JPEGExif::ExifValue *grp, Int32 id, UInt16 val)
{
	Media::JPEGExif::ExifValue *exif = GetExif(grp, id);
	if (exif->s)
	{
		MemFree(exif->s);
		exif->s = 0;
	}
	exif->t = Media::JPEGExif::EXIF_TYPE_SHORT;
	exif->numerator = val;
}

void Media::JPEGExif::SetExif(Media::JPEGExif::ExifValue *grp, Int32 id, UInt32 val)
{
	Media::JPEGExif::ExifValue *exif = GetExif(grp, id);
	if (exif->s)
	{
		MemFree(exif->s);
		exif->s = 0;
	}
	exif->t = Media::JPEGExif::EXIF_TYPE_LONG;
	exif->numerator = val;
}

void Media::JPEGExif::SetExif(Media::JPEGExif::ExifValue *grp, Int32 id, const Int32 *val, Int32 cnt)
{
	Media::JPEGExif::ExifValue *exif = GetExif(grp, id);
	if (exif->s)
	{
		MemFree(exif->s);
		exif->s = 0;
	}
	exif->t = Media::JPEGExif::EXIF_TYPE_NUMARR;
	exif->numerator = cnt;
	exif->s = MemAlloc(Char, cnt * 8);
	MemCopyNO(exif->s, val, cnt * 8);
}

void Media::JPEGExif::DelExif(ExifValue *grp, Int32 id)
{
	Media::JPEGExif::ExifValue *val = GetExif(grp, id);
	if (val->s)
	{
		MemFree(val->s);
		val->s = 0;
	}
	val->t = Media::JPEGExif::EXIF_TYPE_DEL;
}

Bool Media::JPEGExif::WriteExif(IO::Stream *input, IO::Stream *output)
{
	UInt8 hdr[4];
	UInt8 *buff;
	UInt8 *excont;
	UInt8 *oexcont;
	Int32 oexsize;

	OSInt hdrSize;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Bool found;

	input->Read(hdr, 2);
	if (*(UInt16*)hdr != 0xD8FF)
		return false;

	output->Write(hdr, 2);

	buff = MemAlloc(UInt8, BUFFSIZE);
	found = false;

	while ((hdrSize = input->Read(hdr, 4)) == 4)
	{
		if (hdr[0] != 0xff)
		{
			MemFree(buff);
			return false;
		}
		if (hdr[1] == 0xdb)
		{
			break;
		}

		j = ((hdr[2] << 8) | hdr[3]) - 2;
		if (hdr[1] == 0xe1)
		{
			excont = MemAlloc(UInt8, j);
			input->Read(excont, j);
			if (*(Int32*)excont == *(Int32*)"Exif")
			{
				if (*(Int16*)&excont[6] == *(Int16*)"II")
				{
					found = true;
					oexcont = excont;
					oexsize = (Int32)j;
					hdrSize = input->Read(hdr, 4);
					if (true)
						oexsize = 16;
					MemFree(excont);
					break;
				}
				else
				{
					output->Write(hdr, 4);
					output->Write(excont, j);
					MemFree(excont);
				}
			}
			else
			{
				output->Write(hdr, 4);
				output->Write(excont, j);
				MemFree(excont);
			}
		}
		else
		{
			output->Write(hdr, 4);
			while (j > BUFFSIZE)
			{
				k = input->Read(buff, BUFFSIZE);
				if (k == 0)
				{
					break;
				}
				j -= k;
				output->Write(buff, k);
			}
			if (j == 0)
			{

			}
			else if (j <= BUFFSIZE)
			{
				k = input->Read(buff, j);
				output->Write(buff, k);
			}
			else
			{
			}
		}
	}

	CalExifSize(this->exifs, &j, &i);
	if (found)
	{
		i += oexsize - 16;
		j += oexsize - 16;
	}
	excont = MemAlloc(UInt8, j + 18);

	excont[0] = 0xff;
	excont[1] = 0xe1;
	excont[2] = (UInt8)(((j + 22) >> 8) & 0xff);
	excont[3] = (UInt8)((j + 22) & 0xff);
	*(Int32*)&excont[4] = *(Int32*)"Exif";
	*(Int16*)&excont[8] = 0;
	*(Int16*)&excont[10] = *(Int16*)"II";
	*(Int16*)&excont[12] = 42;
	*(Int32*)&excont[14] = 8;
	k = 8;
	l = i + 8;
	GenExifBuff(&excont[10], this->exifs, &k, &l);
	output->Write(excont, j + 24);
	MemFree(excont);

	output->Write(hdr, hdrSize);

	while (true)
	{
		k = input->Read(buff, BUFFSIZE);
		if (k == 0)
			break;
		output->Write(buff, k);
	}
	MemFree(buff);
	return true;
}
