#include "Stdafx.h"
#include "Data/ArrayList.h"
#include "MyMemory.h"
#include "IO/StmData/FileData.h"
#include "IO/DataSegment.h"
#include <windows.h>
#include "Media/VideoFile.h"
#include <memory.h>

VideoFile::VideoFile(WChar *name, IO::DataSegment *vs, Data::ArrayList<AudioStream*> *audioList, AVIStream *avis, Char *chap)
{
	if (name == 0)
	{
		fname = 0;
	}
	else
	{
		WChar *src = name;
		while (*src++);
		WChar *dest = fname = MemAlloc(WChar, src - name);
		src = name;
		while ((*dest++ = *src++) != 0);
	}

	this->chap = 0;
	if (chap)
	{
		Int32 dataCnt = *(Int32*)chap;
		Char *src;
		WChar * wsrc;
		src = &chap[*(Int32*)&chap[dataCnt*8]];
		if (*(UInt8*)src == 0xff && ((UInt8*)src)[1] == 0xfe)
		{
			wsrc = (WChar*)src;
			while (*wsrc++);
			dataCnt = (Char*)wsrc - chap;
		}
		else
		{
			while (*src++);
			dataCnt = src - chap;
		}
		Char *dest = this->chap = MemAlloc(Char, dataCnt);
		src = chap;
		while (dataCnt--)
			*dest++ = *src++;
	}
	NEW_CLASS(vseqList, Data::ArrayList<IO::DataSegment*>());
	vseqList->Add(vs);
	astmList = audioList;

	strh = MemAlloc(STRH, 1);
	MemCopy(strh, &avis->strh, sizeof(STRH));
	strf = MemAlloc(Char, strfSize = avis->strfSize);
	MemCopy(strf, avis->strf, strfSize);
}

VideoFile::~VideoFile()
{
	if (chap)
	{
		MemFree(chap);
		chap = 0;
	}
	MemFree(strh);
	MemFree(strf);
	strh = 0;
	strf = 0;

	Int32 i = vseqList->GetCount();
	while (i--)
	{
		DEL_CLASS((IO::DataSegment*)vseqList->RemoveAt(i));
	}
	DEL_CLASS(vseqList);

	i = astmList->GetCount();
	while (i--)
	{
		DEL_CLASS((AudioStream*)astmList->RemoveAt(i));
	}
	DEL_CLASS(astmList);

	if (fname)
	{
		MemFree(fname);
		fname = 0;
	}
}

WChar *VideoFile::getName()
{
	return fname;
}
