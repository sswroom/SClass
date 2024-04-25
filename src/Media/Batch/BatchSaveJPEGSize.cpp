#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Media/Batch/BatchSaveJPEGSize.h"

Media::Batch::BatchSaveJPEGSize::BatchSaveJPEGSize(UInt32 sizePercent)
{
	if (sizePercent > 100)
		this->sizePercent = 100;
	else if (sizePercent < 1)
		this->sizePercent = 1;
	else
		this->sizePercent = sizePercent;
}

Media::Batch::BatchSaveJPEGSize::~BatchSaveJPEGSize()
{
}

void Media::Batch::BatchSaveJPEGSize::ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN targetId)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Optional<IO::FileExporter::ParamData> param;
	Int32 minIndex;
	UInt64 minSize;
	Int32 maxIndex;
	UInt64 maxSize;
	Int32 currIndex;
	UInt64 currSize;
	UInt64 targetSize;
	Media::RasterImage *img;

	sptr = fileId.ConcatTo(sbuff);
	sptr = Text::StrConcatC(targetId.ConcatTo(sptr), UTF8STRC(".jpg"));

	img = imgList->GetImage(0, 0);
	targetSize = (img->info.dispSize.CalcArea() * img->info.storeBPP >> 3) / this->sizePercent;

	IO::MemoryStream mstm;
	param = this->exporter.CreateParam(imgList);

	mstm.Clear();
	this->exporter.SetParamInt32(param, 0, 0);
	this->exporter.ExportFile(mstm, CSTRP(sbuff, sptr), imgList, param);
	minIndex = 0;
	minSize = mstm.GetLength();
	if (minSize > targetSize)
	{
		currIndex = minIndex;
	}
	else
	{
		mstm.Clear();
		this->exporter.SetParamInt32(param, 0, 100);
		this->exporter.ExportFile(mstm, CSTRP(sbuff, sptr), imgList, param);
		maxIndex = 100;
		maxSize = mstm.GetLength();
		if (maxSize < targetSize)
		{
			currIndex = maxIndex;
		}
		else
		{
			while (true)
			{
				currIndex = (maxIndex + minIndex) >> 1;
				if ((currIndex == minIndex) || (currIndex == maxIndex))
				{
					if (maxSize - targetSize > targetSize - minSize)
					{
						currIndex = minIndex;
					}
					else
					{
						currIndex = maxIndex;
					}
					break;
				}
				mstm.Clear();
				this->exporter.SetParamInt32(param, 0, currIndex);
				this->exporter.ExportFile(mstm, CSTRP(sbuff, sptr), imgList, param);
				currSize = mstm.GetLength();
				if (currSize > targetSize)
				{
					maxIndex = currIndex;
					maxSize = currSize;
				}
				else if (currSize < targetSize)
				{
					minIndex = currIndex;
					minSize = currSize;
				}
				else
				{
					break;
				}
			}
		}
	}

	{
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
		this->exporter.SetParamInt32(param, 0, currIndex);
		this->exporter.ExportFile(fs, CSTRP(sbuff, sptr), imgList, param);
	}
	this->exporter.DeleteParam(param);
}
