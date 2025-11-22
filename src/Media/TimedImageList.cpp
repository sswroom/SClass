#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "Media/TimedImageList.h"

void Media::TimedImageList::ScanFile()
{
	NN<IO::FileStream> fs;
	if (this->fs.SetTo(fs))
	{
		UInt64 currOfst = 16;
		UInt8 indexBuff[32];
		this->flags &= ~2;
		this->changed = true;
		fs->SeekFromBeginning(16);
		while (fs->Read(BYTEARR(indexBuff)) == 32)
		{
			if (ReadUInt64(&indexBuff[16]) == currOfst + 32)
			{
				Int64 imgSize = ReadInt64(&indexBuff[24]);
				this->indexStm.Write(Data::ByteArrayR(indexBuff, 32));
				currOfst += 32 + (UInt64)imgSize;
				fs->SeekFromCurrent(imgSize);
			}
			else
			{
				break;
			}
		}
		fs->SeekFromBeginning(currOfst);
		this->currFileOfst = currOfst;
	}
}

Media::TimedImageList::TimedImageList(Text::CStringNN fileName)
{
	UInt8 hdr[16];
	NN<IO::FileStream> fs;
	NEW_CLASSNN(fs, IO::FileStream(fileName, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
	UInt64 fileSize = fs->GetLength();
	if (fileSize > 0 && fileSize < 16)
	{
		fs.Delete();
		this->fs = 0;
		return;
	}
	else if (fileSize == 0)
	{
		hdr[0] = 'S';
		hdr[1] = 'T';
		hdr[2] = 'i';
		hdr[3] = 'l';
		WriteInt32(&hdr[4], 0x1);
		WriteInt64(&hdr[8], 16);
		fs->Write(Data::ByteArrayR(hdr, 16));
		this->flags = 1;
		this->currFileOfst = 16;
		this->changed = false;
		this->fs = fs;
	}
	else
	{
		fs->SeekFromBeginning(0);
		fs->Read(BYTEARR(hdr));
		if (hdr[0] != 'S' || hdr[1] != 'T' || hdr[2] != 'i' || hdr[3] != 'l')
		{
			fs.Delete();
			this->fs = 0;
			return;
		}
		this->fs = fs;
		this->flags = ReadInt32(&hdr[4]);
		this->currFileOfst = ReadUInt64(&hdr[8]);
		if (this->flags & 2)
		{
			UInt64 indexSize = fileSize - this->currFileOfst;
			if (fileSize >= this->currFileOfst && (indexSize & 31) == 0)
			{
				Data::ByteBuffer indexBuff((UOSInt)indexSize);
				fs->SeekFromBeginning(this->currFileOfst);
				fs->Read(indexBuff);
				this->indexStm.Write(indexBuff);
				fs->SeekFromBeginning(this->currFileOfst);
			}
			else if (this->flags & 1)
			{
				ScanFile();
			}
			else
			{
				fs->SeekFromBeginning(16);
				this->currFileOfst = 16;
				this->changed = true;
			}
		}
		else if (this->flags & 1)
		{
			ScanFile();
		}
		else
		{
			fs->SeekFromBeginning(16);
			this->currFileOfst = 16;
			this->changed = true;
		}
	}
}

Media::TimedImageList::~TimedImageList()
{
	UInt8 hdr[16];
	NN<IO::FileStream> fs;
	if (this->fs.SetTo(fs))
	{
		if (this->changed)
		{
			UOSInt indexSize;
			UnsafeArray<UInt8> indexBuff;
			indexBuff = this->indexStm.GetBuff(indexSize);
			fs->Write(Data::ByteArrayR(indexBuff, indexSize));

			fs->SeekFromBeginning(0);
			hdr[0] = 'S';
			hdr[1] = 'T';
			hdr[2] = 'i';
			hdr[3] = 'l';
			WriteInt32(&hdr[4], this->flags | 2);
			WriteUInt64(&hdr[8], this->currFileOfst);
			fs->Write(Data::ByteArrayR(hdr, 16));
		}
		fs.Delete();
		this->fs = 0;
	}
}

Bool Media::TimedImageList::IsError()
{
	return this->fs.IsNull();
}

Bool Media::TimedImageList::AddImage(Int64 captureTimeTicks, UnsafeArray<const UInt8> imgBuff, UOSInt imgSize, ImageFormat imgFmt)
{
	NN<IO::FileStream> fs;
	if (!this->fs.SetTo(fs))
	{
		return false;
	}
	UInt8 indexBuff[32];
	Bool succ = true;
	if (!this->changed)
	{
		fs->SeekFromBeginning(0);
		this->flags &= ~2;
		indexBuff[0] = 'S';
		indexBuff[1] = 'T';
		indexBuff[2] = 'i';
		indexBuff[3] = 'l';
		WriteInt32(&indexBuff[4], this->flags);
		WriteUInt64(&indexBuff[8], this->currFileOfst);
		fs->Write(Data::ByteArrayR(indexBuff, 16));
		fs->SeekFromBeginning(this->currFileOfst);
		this->changed = true;
	}
	WriteInt64(&indexBuff[0], captureTimeTicks);
	WriteInt32(&indexBuff[8], 0); //flags
	WriteInt32(&indexBuff[12], imgFmt);
	WriteUInt64(&indexBuff[16], this->currFileOfst + 32);
	WriteUInt64(&indexBuff[24], imgSize);
	succ = succ && (fs->Write(Data::ByteArrayR(indexBuff, 32)) == 32);
	this->indexStm.Write(Data::ByteArrayR(indexBuff, 32));
	succ = succ && (fs->Write(Data::ByteArrayR(imgBuff, imgSize)) == imgSize);
	this->currFileOfst += 32 + imgSize;
	return succ;
}
