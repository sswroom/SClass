#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/IAudioSource.h"
#include "Media/MediaFile.h"
#include "Exporter/WAVExporter.h"

Exporter::WAVExporter::WAVExporter()
{
	this->codePage = 65001;
}

Exporter::WAVExporter::~WAVExporter()
{
}

Int32 Exporter::WAVExporter::GetName()
{
	return *(Int32*)"WAVE";
}

IO::FileExporter::SupportType Exporter::WAVExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_VIDEO_PARSER)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	Media::MediaFile *file = (Media::MediaFile *)pobj;
	if (file->GetStream(1, 0) != 0)
		return IO::FileExporter::ST_NOT_SUPPORTED;
	Media::IMediaSource *stm = file->GetStream(0, 0);
	if (stm == 0)
		return IO::FileExporter::ST_NOT_SUPPORTED;
	if (stm->GetMediaType() != Media::MEDIA_TYPE_AUDIO)
		return IO::FileExporter::ST_NOT_SUPPORTED;
	return IO::FileExporter::ST_NORMAL_STREAM;
}

Bool Exporter::WAVExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"WAV File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.wav");
		return true;
	}
	return false;
}

void Exporter::WAVExporter::SetCodePage(Int32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::WAVExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_VIDEO_PARSER)
	{
		return false;
	}
	UInt8 *buff;
	Media::MediaFile *file = (Media::MediaFile *)pobj;
	if (file->GetStream(1, 0) != 0)
		return false;
	Media::IMediaSource *src = file->GetStream(0, 0);
	if (src == 0)
		return false;
	if (src->GetMediaType() != Media::MEDIA_TYPE_AUDIO)
		return false;

	Media::IAudioSource *audio = (Media::IAudioSource*)src;
	Media::AudioFormat format;
	audio->GetFormat(&format);
	Int64 fileSize = 0;
	Int64 headerSize = 0;
	Int64 initPos = stm->GetPosition();
	buff = MemAlloc(UInt8, 1048576);
	*(Int32*)&buff[0] = *(Int32*)"RIFF";
	*(UInt32*)&buff[4] = 0;
	*(Int32*)&buff[8] = *(Int32*)"WAVE";
	*(Int32*)&buff[12] = *(Int32*)"JUNK";
	*(UInt32*)&buff[16] = 28;
	MemClear(&buff[20], 28);
	*(Int32*)&buff[48] = *(Int32*)"fmt ";
	*(Int32*)&buff[52] = 18 + format.extraSize;
	*(UInt16*)&buff[56] = format.formatId;
	*(UInt16*)&buff[58] = format.nChannels;
	*(Int32*)&buff[60] = format.frequency;
	*(Int32*)&buff[64] = format.bitRate >> 3;
	*(UInt16*)&buff[68] = format.align;
	*(UInt16*)&buff[70] = format.bitpersample;
	*(UInt16*)&buff[72] = format.extraSize;
	if (format.extraSize > 0)
	{
		headerSize = 82 + format.extraSize;
		MemCopyNO(&buff[74], format.extra, format.extraSize);
		*(Int32*)&buff[(OSInt)(headerSize - 8)] = *(Int32*)"data";
		*(Int32*)&buff[(OSInt)(headerSize - 4)] = 0;
	}
	else
	{
		headerSize = 82;
		*(Int32*)&buff[74] = *(Int32*)"data";
		*(Int32*)&buff[78] = 0;
	}
	stm->Write(buff, (OSInt)headerSize);
	fileSize = headerSize;
	OSInt blockSize;
	Sync::Event *evt;
	NEW_CLASS(evt, Sync::Event(true, (const UTF8Char*)"Exporter.WAVExporter.evt"));
	if (audio->Start(evt, (Int32)(1048576 - headerSize)))
	{
		while ((blockSize = audio->ReadBlock(&buff[(OSInt)headerSize], (Int32)(1048576 - headerSize))) > 0)
		{
			stm->Write(&buff[(OSInt)headerSize], blockSize);
			fileSize += blockSize;
		}
		audio->Stop();
	}
	DEL_CLASS(evt);
	if (fileSize >= 0x100000000LL)
	{
		*(Int32*)&buff[12] = *(Int32*)"ds64";
		*(Int64*)&buff[20] = fileSize - 8;
		*(Int64*)&buff[28] = fileSize - headerSize;
		*(Int64*)&buff[36] = 0;
		*(Int32*)&buff[44] = 0;

		*(Int32*)&buff[4] = -1;
		*(Int32*)&buff[(OSInt)(headerSize - 4)] = -1;

		*(Int32*)&buff[0] = *(Int32*)"RF64";
	}
	else
	{
		*(UInt32*)&buff[4] = (UInt32)(fileSize - 8);
		*(UInt32*)&buff[(OSInt)(headerSize - 4)] = (UInt32)(fileSize - headerSize);
	}
	stm->Seek(IO::SeekableStream::ST_BEGIN, initPos);
	stm->Write(buff, (OSInt)headerSize);
	MemFree(buff);

	return true;
}
