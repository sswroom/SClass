#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListNN.hpp"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include "IO/Stream.h"
#include "Media/JPEGExif.h"
#include <memory.h>

#define BUFFSIZE 1024

void Media::JPEGExif::FreeExif(NN<ExifValue> exif)
{
	UnsafeArray<UTF8Char> s;
	if (exif->t == EXIF_TYPE_GROUP)
	{
		if (exif->s.SetTo(s))
		{
			IntOS i;
			Data::ArrayListNN<ExifValue> *exifArr = (Data::ArrayListNN<ExifValue> *)s.Ptr();
			exifArr->FreeAll(FreeExif);
			DEL_CLASS(exifArr);
		}
	}
	else
	{
		if (exif->s.SetTo(s))
			MemFreeArr(s);
	}
	MemFreeNN(exif);
}

NN<Media::JPEGExif::ExifValue> Media::JPEGExif::DupExif(NN<ExifValue> exif)
{
	UnsafeArray<UTF8Char> s;
	UnsafeArray<UTF8Char> ns;
	NN<Media::JPEGExif::ExifValue> nexif;
	nexif = MemAllocNN(Media::JPEGExif::ExifValue);
	nexif->id = exif->id;
	nexif->numerator = exif->numerator;
	nexif->denominator = exif->denominator;
	if (exif->s.SetTo(s))
	{
		if (exif->t == Media::JPEGExif::EXIF_TYPE_BYTE || exif->t == Media::JPEGExif::EXIF_TYPE_UNK)
		{
			nexif->s = ns = MemAllocArr(UTF8Char, exif->numerator);
			MemCopyNO(&ns[0], &s[0], exif->numerator);
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_NUMARR)
		{
			nexif->s = ns = MemAllocArr(UTF8Char, exif->numerator);
			MemCopyNO(&ns[0], &s[0], exif->numerator * 8);
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_GROUP)
		{
			Int32 i;
			NN<Data::ArrayListNN<ExifValue>> tmpArr;
			Data::ArrayListNN<ExifValue> *tmpArr2 = (Data::ArrayListNN<ExifValue>*)exif->s.Ptr();
			NEW_CLASSNN(tmpArr, Data::ArrayListNN<ExifValue>());
			nexif->s = (UTF8Char*)tmpArr.Ptr();
			i = 0;
			while (i < tmpArr2->GetCount())
			{
				tmpArr->Add(DupExif(tmpArr2->GetItemNoCheck(i)));
				i++;
			}
		}
		else
		{
			nexif->s = ns = MemAllocArr(UTF8Char, Text::StrCharCnt(s) + 1);
			Text::StrConcat(ns, s);
		}
	}
	else
	{
		nexif->s = nullptr;
	}
	return nexif;
}

NN<Media::JPEGExif::ExifValue> Media::JPEGExif::GetExif(Optional<ExifValue> grp, Int32 id)
{
	NN<Media::JPEGExif::ExifValue> exif;
	Data::ArrayListNN<ExifValue> *exifArr;
	if (grp.SetTo(exif))
	{
		exifArr = (Data::ArrayListNN<ExifValue>*)exif->s.Ptr();
	}
	else
	{
		exifArr = &this->exifs;
	}

	UIntOS i = exifArr->GetCount();
	while (i-- > 0)
	{
		exif = exifArr->GetItemNoCheck(i);
		if (exif->id == id)
			return exif;
	}
	exif = MemAllocNN(Media::JPEGExif::ExifValue);
	exif->id = id;
	exif->numerator = 0;
	exif->denominator = 0;
	exif->s = nullptr;
	exifArr->Add(exif);
	return exif;
}

void Media::JPEGExif::CalExifSize(NN<Data::ArrayListNN<ExifValue>> exifArr, OutParam<UIntOS> size, OutParam<UIntOS> endOfst)
{
	UIntOS i = 6;
	UIntOS j = 6;
	UIntOS k;
	UIntOS l;
	UIntOS m;
	UnsafeArray<UTF8Char> s;
	NN<Media::JPEGExif::ExifValue> exif;

	k = exifArr->GetCount();
	while (k-- > 0)
	{
		exif = exifArr->GetItemNoCheck(k);
		if (exif->t == Media::JPEGExif::EXIF_TYPE_NUM)
		{
			i += 12;
			j += 20;
		}
		else if (exif->t == Media::JPEGExif::EXIF_TYPE_STR)
		{
			if (exif->s.SetTo(s))
				l = Text::StrCharCnt(s) + 1;
			else
				l = 1;
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
			CalExifSize(NN<Data::ArrayListNN<ExifValue>>::FromPtr((Data::ArrayListNN<ExifValue>*)exif->s.Ptr()), l, m);
			i += m;
			j += l;
		}
	}
	size.Set(j);
	endOfst.Set(i);
}

void Media::JPEGExif::GenExifBuff(UInt8 *buff, NN<Data::ArrayListNN<ExifValue>> exifArr, InOutParam<UIntOS> startOfst, InOutParam<UIntOS> otherOfst)
{
	UIntOS objCnt;
	UIntOS i;
	UIntOS j;
	UIntOS k;
	NN<Media::JPEGExif::ExifValue> exif;

	objCnt = 0;
	k = otherOfst.Get();
	j = startOfst.Get() + 2;
	i = 0;
	while (i < exifArr->GetCount())
	{
		exif = exifArr->GetItemNoCheck(i);
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
			UnsafeArray<UTF8Char> s;
			if (!exif->s.SetTo(s))
				s = U8STR("");
			*(Int16*)&buff[j] = exif->id;
			*(Int16*)&buff[j + 2] = 2;
			*(Int32*)&buff[j + 4] = (Int32)Text::StrCharCnt(s) + 1;
			if (*(Int32*)&buff[j + 4] <= 4)
			{
				*(Int32*)&buff[j + 8] = 0;
				Text::StrConcat((Char*)&buff[j + 8], s);
				j += 12;
			}
			else
			{
				*(Int32*)&buff[j + 8] = k;
				Text::StrConcat((Char*)&buff[k], s);
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
				MemCopyNO(&buff[j + 8], exif->s.Ptr(), exif->numerator);
				j += 12;
			}
			else
			{
				*(Int32*)&buff[j + 8] = k;
				MemCopyNO(&buff[k], exif->s.Ptr(), exif->numerator);
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
				MemCopyNO(&buff[j + 8], exif->s.Ptr(), exif->numerator);
				j += 12;
			}
			else
			{
				*(Int32*)&buff[j + 8] = k;
				MemCopyNO(&buff[k], exif->s.Ptr(), exif->numerator);
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
			MemCopyNO(&buff[k], exif->s.Ptr(), exif->numerator * 8);
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
	*(Int16*)&buff[startOfst.Get()] = objCnt;
	j += 4;

	i = 0;
	while (i < exifArr->GetCount())
	{
		exif = exifArr->GetItemNoCheck(i);
		if (exif->t == Media::JPEGExif::EXIF_TYPE_GROUP)
		{
			*(Int32*)&buff[exif->numerator] = j;
			GenExifBuff(buff, NN<Data::ArrayListNN<ExifValue>>::FromPtr((Data::ArrayListNN<ExifValue>*)exif->s.Ptr()), j, k);
		}
		i++;
	}
	startOfst.Set(j);
	otherOfst.Set(k);
}

Media::JPEGExif::JPEGExif()
{
}

Media::JPEGExif::~JPEGExif()
{
	this->exifs.FreeAll(FreeExif);
}

NN<Media::JPEGExif::ExifValue> Media::JPEGExif::AddExifGroup(Int32 id)
{
	NN<Media::JPEGExif::ExifValue> val;
	NN<Data::ArrayListNN<ExifValue>> exifArr;
	val = MemAllocNN(Media::JPEGExif::ExifValue);
	val->id = id;
	val->t = Media::JPEGExif::EXIF_TYPE_GROUP;
	NEW_CLASSNN(exifArr, Data::ArrayListNN<ExifValue>());
	val->s = (UTF8Char*)exifArr.Ptr();
	this->exifs.Add(val);
	return val;
}

void Media::JPEGExif::SetExif(Optional<Media::JPEGExif::ExifValue> grp, Int32 id, Int32 numerator, Int32 denominator)
{
	NN<Media::JPEGExif::ExifValue> val = GetExif(grp, id);
	UnsafeArray<UTF8Char> s;
	if (val->s.SetTo(s))
	{
		MemFreeArr(s);
		val->s = nullptr;
	}
	val->t = Media::JPEGExif::EXIF_TYPE_NUM;
	val->numerator = numerator;
	val->denominator = denominator;
}

void Media::JPEGExif::SetExif(Optional<Media::JPEGExif::ExifValue> grp, Int32 id, UnsafeArray<const UTF8Char> s)
{
	NN<Media::JPEGExif::ExifValue> val = GetExif(grp, id);
	UnsafeArray<UTF8Char> ns;
	if (val->s.SetTo(ns))
	{
		MemFreeArr(ns);
		val->s = nullptr;
	}
	val->t = Media::JPEGExif::EXIF_TYPE_STR;
	val->s = ns = MemAllocArr(UTF8Char, Text::StrCharCnt(s) + 1);
	Text::StrConcat(ns, s);
}

void Media::JPEGExif::SetExif(Optional<Media::JPEGExif::ExifValue> grp, Int32 id, UnsafeArray<const UInt8> s, Int32 size)
{
	NN<Media::JPEGExif::ExifValue> val = GetExif(grp, id);
	UnsafeArray<UTF8Char> ns;
	if (val->s.SetTo(ns))
	{
		MemFreeArr(ns);
		val->s = nullptr;
	}
	val->t = Media::JPEGExif::EXIF_TYPE_BYTE;
	val->s = ns = MemAllocArr(UTF8Char, size);
	val->numerator = size;
	MemCopyNO(&ns[0], &s[0], size);
}

void Media::JPEGExif::SetExifUnk(Optional<Media::JPEGExif::ExifValue> grp, Int32 id, UnsafeArray<const UInt8> s, Int32 size)
{
	NN<Media::JPEGExif::ExifValue> val = GetExif(grp, id);
	UnsafeArray<UTF8Char> ns;
	if (val->s.SetTo(ns))
	{
		MemFreeArr(ns);
		val->s = nullptr;
	}
	val->t = Media::JPEGExif::EXIF_TYPE_UNK;
	val->s = ns = MemAllocArr(UTF8Char, size);
	val->numerator = size;
	MemCopyNO(&ns[0], &s[0], size);
}

void Media::JPEGExif::SetExif(Optional<Media::JPEGExif::ExifValue> grp, Int32 id, UInt16 val)
{
	NN<Media::JPEGExif::ExifValue> exif = GetExif(grp, id);
	UnsafeArray<UTF8Char> ns;
	if (exif->s.SetTo(ns))
	{
		MemFreeArr(ns);
		exif->s = nullptr;
	}
	exif->t = Media::JPEGExif::EXIF_TYPE_SHORT;
	exif->numerator = val;
}

void Media::JPEGExif::SetExif(Optional<Media::JPEGExif::ExifValue> grp, Int32 id, UInt32 val)
{
	NN<Media::JPEGExif::ExifValue> exif = GetExif(grp, id);
	UnsafeArray<UTF8Char> ns;
	if (exif->s.SetTo(ns))
	{
		MemFreeArr(ns);
		exif->s = nullptr;
	}
	exif->t = Media::JPEGExif::EXIF_TYPE_LONG;
	exif->numerator = val;
}

void Media::JPEGExif::SetExif(Optional<Media::JPEGExif::ExifValue> grp, Int32 id, UnsafeArray<const Int32> val, Int32 cnt)
{
	NN<Media::JPEGExif::ExifValue> exif = GetExif(grp, id);
	UnsafeArray<UTF8Char> ns;
	if (exif->s.SetTo(ns))
	{
		MemFreeArr(ns);
		exif->s = nullptr;
	}
	exif->t = Media::JPEGExif::EXIF_TYPE_NUMARR;
	exif->numerator = cnt;
	exif->s = ns = MemAllocArr(UTF8Char, cnt * 8);
	MemCopyNO(&ns[0], &val[0], cnt * 8);
}

void Media::JPEGExif::DelExif(Optional<ExifValue> grp, Int32 id)
{
	NN<Media::JPEGExif::ExifValue> val = GetExif(grp, id);
	UnsafeArray<UTF8Char> ns;
	if (val->s.SetTo(ns))
	{
		MemFreeArr(ns);
		val->s = nullptr;
	}
	val->t = Media::JPEGExif::EXIF_TYPE_DEL;
}

Bool Media::JPEGExif::WriteExif(NN<IO::Stream> input, NN<IO::Stream> output)
{
	UInt8 hdr[4];
	UnsafeArray<UInt8> buff;
	UnsafeArray<UInt8> excont;
	UnsafeArray<UInt8> oexcont;
	Int32 oexsize;

	IntOS hdrSize;
	UIntOS i;
	UIntOS j;
	UIntOS k;
	UIntOS l;
	Bool found;

	input->Read(Data::ByteArray(hdr, 2));
	if (*(UInt16*)hdr != 0xD8FF)
		return false;

	output->Write(Data::ByteArrayR(hdr, 2));

	buff = MemAllocArr(UInt8, BUFFSIZE);
	found = false;

	while ((hdrSize = input->Read(Data::ByteArray(hdr, 4))) == 4)
	{
		if (hdr[0] != 0xff)
		{
			MemFreeArr(buff);
			return false;
		}
		if (hdr[1] == 0xdb)
		{
			break;
		}

		j = ((hdr[2] << 8) | hdr[3]) - 2;
		if (hdr[1] == 0xe1)
		{
			excont = MemAllocArr(UInt8, j);
			input->Read(Data::ByteArray(excont, j));
			if (*(Int32*)&excont[0] == *(Int32*)"Exif")
			{
				if (*(Int16*)&excont[6] == *(Int16*)"II")
				{
					found = true;
					oexcont = excont;
					oexsize = (Int32)j;
					hdrSize = input->Read(Data::ByteArray(hdr, 4));
					if (true)
						oexsize = 16;
					MemFreeArr(excont);
					break;
				}
				else
				{
					output->Write(Data::ByteArrayR(hdr, 4));
					output->Write(Data::ByteArrayR(excont, j));
					MemFreeArr(excont);
				}
			}
			else
			{
				output->Write(Data::ByteArrayR(hdr, 4));
				output->Write(Data::ByteArrayR(excont, j));
				MemFreeArr(excont);
			}
		}
		else
		{
			output->Write(Data::ByteArrayR(hdr, 4));
			while (j > BUFFSIZE)
			{
				k = input->Read(Data::ByteArray(buff, BUFFSIZE));
				if (k == 0)
				{
					break;
				}
				j -= k;
				output->Write(Data::ByteArrayR(buff, k));
			}
			if (j == 0)
			{

			}
			else if (j <= BUFFSIZE)
			{
				k = input->Read(Data::ByteArray(buff, j));
				output->Write(Data::ByteArrayR(buff, k));
			}
			else
			{
			}
		}
	}

	CalExifSize(this->exifs, j, i);
	if (found)
	{
		i += oexsize - 16;
		j += oexsize - 16;
	}
	excont = MemAllocArr(UInt8, j + 18);

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
	GenExifBuff(&excont[10], this->exifs, k, l);
	output->Write(Data::ByteArrayR(excont, j + 24));
	MemFreeArr(excont);

	output->Write(Data::ByteArrayR(hdr, hdrSize));

	while (true)
	{
		k = input->Read(Data::ByteArray(buff, BUFFSIZE));
		if (k == 0)
			break;
		output->Write(Data::ByteArrayR(buff, k));
	}
	MemFreeArr(buff);
	return true;
}
