#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Media/Batch/BatchSaveJPEGSize.h"

Media::Batch::BatchSaveJPEGSize::BatchSaveJPEGSize(UInt32 sizePercent)
{
	NEW_CLASS(exporter, Exporter::GUIJPGExporter());
	NEW_CLASS(mut, Sync::Mutex());
	if (sizePercent > 100)
		this->sizePercent = 100;
	else if (sizePercent < 1)
		this->sizePercent = 1;
	else
		this->sizePercent = sizePercent;
}

Media::Batch::BatchSaveJPEGSize::~BatchSaveJPEGSize()
{
	DEL_CLASS(mut);
	DEL_CLASS(exporter);
}

void Media::Batch::BatchSaveJPEGSize::ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *targetId)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::MemoryStream *mstm;
	IO::FileStream *fs;
	void *param;
	Int32 minIndex;
	UInt64 minSize;
	Int32 maxIndex;
	UInt64 maxSize;
	Int32 currIndex;
	UInt64 currSize;
	UInt64 targetSize;
	Media::Image *img;

	sptr = Text::StrConcat(sbuff, fileId);
	Text::StrConcat(Text::StrConcat(sptr, targetId), (const UTF8Char*)".jpg");

	img = imgList->GetImage(0, 0);
	targetSize = (img->info->dispWidth * img->info->dispHeight * img->info->storeBPP >> 3) / this->sizePercent;

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"Media.Batch.BatchSaveJPEGSize"));
	param = exporter->CreateParam(imgList);

	mstm->Clear();
	exporter->SetParamInt32(param, 0, 0);
	exporter->ExportFile(mstm, sbuff, imgList, param);
	minIndex = 0;
	minSize = mstm->GetLength();
	if (minSize > targetSize)
	{
		currIndex = minIndex;
	}
	else
	{
		mstm->Clear();
		exporter->SetParamInt32(param, 0, 100);
		exporter->ExportFile(mstm, sbuff, imgList, param);
		maxIndex = 100;
		maxSize = mstm->GetLength();
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
				mstm->Clear();
				exporter->SetParamInt32(param, 0, currIndex);
				exporter->ExportFile(mstm, sbuff, imgList, param);
				currSize = mstm->GetLength();
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

	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
	exporter->SetParamInt32(param, 0, currIndex);
	exporter->ExportFile(fs, sbuff, imgList, param);
	DEL_CLASS(fs);
	exporter->DeleteParam(param);
	DEL_CLASS(mstm);

}
