#include "Stdafx.h"
#include "DB/CSVFile.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Media/ProfiledResizer.h"
#include "Media/Batch/BatchSaveJPEG.h"
#include "Media/Batch/BatchSaveJPEGSize.h"
#include "Media/Batch/BatchSavePNG.h"
#include "Media/Batch/BatchSaveTIFF.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

void Media::ProfiledResizer::ReleaseProfile(Media::ProfiledResizer::ResizeProfile *profile)
{
	Text::StrDelNew(profile->profileName);
	Text::StrDelNew(profile->suffix);
	SDEL_TEXT(profile->watermark);
	MemFree(profile);
}

Media::ProfiledResizer::ProfiledResizer(Parser::ParserList *parsers, Media::ColorManagerSess *colorSess, Media::DrawEngine *deng)
{
	this->currProfile = (UOSInt)-1;
	this->saver = 0;
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(watermarker, Media::Batch::BatchWatermarker(deng, 0));
	NEW_CLASS(resizer, Media::Resizer::LanczosResizerLR_C32(14, 14, &destProfile, colorSess, Media::AT_NO_ALPHA, 0.0, Media::PF_B8G8R8A8));
	NEW_CLASS(bresizer, Media::Batch::BatchResizer(resizer, watermarker));
	NEW_CLASS(limiter, Media::Batch::BatchLimiter(bresizer));
	NEW_CLASS(conv, Media::Batch::BatchToLRGB(&srcProfile, &destProfile, limiter));
	NEW_CLASS(loader, Media::Batch::BatchLoader(parsers, conv));

	NEW_CLASS(profiles, Data::ArrayList<ResizeProfile*>());
}

Media::ProfiledResizer::~ProfiledResizer()
{
	UOSInt i;
	while (loader->IsProcessing())
	{
		Sync::Thread::Sleep(100);
	}

	i = profiles->GetCount();
	while (i-- > 0)
	{
		ReleaseProfile(this->profiles->GetItem(i));
	}
	DEL_CLASS(this->profiles);
	DEL_CLASS(this->loader);
	DEL_CLASS(this->conv);
	DEL_CLASS(this->limiter);
	DEL_CLASS(this->bresizer);
	DEL_CLASS(this->resizer);
	DEL_CLASS(this->watermarker);
	SDEL_CLASS(this->saver);
}

UOSInt Media::ProfiledResizer::GetProfileCount()
{
	return this->profiles->GetCount();
}

UOSInt Media::ProfiledResizer::GetCurrProfileIndex()
{
	return this->currProfile;
}

const Media::ProfiledResizer::ResizeProfile *Media::ProfiledResizer::GetCurrProfile()
{
	return this->profiles->GetItem(this->currProfile);
}

void Media::ProfiledResizer::SetCurrentProfile(UOSInt index)
{
	if (index >= this->profiles->GetCount())
		return;
	if (index == this->currProfile)
		return;

	while (loader->IsProcessing())
	{
		Sync::Thread::Sleep(100);
	}
	this->currProfile = index;
	const ResizeProfile *profile = this->profiles->GetItem(index);
	bresizer->ClearTargetSizes();
	if (profile->sizeType == ST_MAXSIZE)
	{
		bresizer->AddTargetSize(profile->targetSizeX, profile->targetSizeY, profile->suffix);
	}
	else if (profile->sizeType == ST_DPI)
	{
		bresizer->AddTargetDPI(profile->targetSizeX, profile->targetSizeY, profile->suffix);
	}
	this->watermarker->SetWatermark(profile->watermark);
	if (this->saver)
	{
		DEL_CLASS(this->saver);
	}
	this->saver = 0;
	if (profile->outType == OT_TIFF)
	{
		NEW_CLASS(this->saver, Media::Batch::BatchSaveTIFF(false));
	}
	else if (profile->outType == OT_JPEGQUALITY)
	{
		NEW_CLASS(this->saver, Media::Batch::BatchSaveJPEG(profile->outParam));
	}
	else if (profile->outType == OT_JPEGSIZE)
	{
		NEW_CLASS(this->saver, Media::Batch::BatchSaveJPEGSize(profile->outParam));
	}
	else if (profile->outType == OT_PNG)
	{
		NEW_CLASS(this->saver, Media::Batch::BatchSavePNG());
	}
	this->watermarker->SetHandler(this->saver);
}

const Media::ProfiledResizer::ResizeProfile *Media::ProfiledResizer::GetProfile(UOSInt index)
{
	return this->profiles->GetItem(index);
}

Bool Media::ProfiledResizer::AddProfile(const UTF8Char *profileName, const UTF8Char *suffix, UInt32 targetSizeX, UInt32 targetSizeY, OutputType outType, UInt32 outParam, const UTF8Char *watermark, SizeType sizeType)
{
	ResizeProfile *profile;
	if (outType == OT_TIFF)
	{
		
	}
	else if (outType == OT_JPEGQUALITY)
	{
		if (outParam > 100)
			return false;
	}
	else if (outType == OT_JPEGSIZE)
	{
		if (outParam <= 0 || outParam > 300)
			return false;
	}
	else if (outType == OT_PNG)
	{
		
	}
	else
	{
		return false;
	}

	profile = MemAlloc(ResizeProfile, 1);
	profile->profileName = Text::StrCopyNew(profileName);
	profile->suffix = Text::StrCopyNew(suffix);
	profile->targetSizeX = targetSizeX;
	profile->targetSizeY = targetSizeY;
	profile->sizeType = sizeType;
	profile->outType = outType;
	profile->outParam = outParam;
	if (watermark)
	{
		profile->watermark = Text::StrCopyNew(watermark);
	}
	else
	{
		profile->watermark = 0;
	}
	this->profiles->Add(profile);
	return true;
}

Bool Media::ProfiledResizer::RemoveProfile(UOSInt index)
{
	if (index >= this->profiles->GetCount())
		return false;
	if (index == this->currProfile)
	{
		while (loader->IsProcessing())
		{
			Sync::Thread::Sleep(100);
		}
	}
	ReleaseProfile(this->profiles->RemoveAt(index));
	if (index < this->currProfile)
	{
		this->currProfile--;
	}
	else if (index == this->currProfile)
	{
		if (this->profiles->GetCount() == 0)
		{
			this->currProfile = (UOSInt)-1;
			bresizer->ClearTargetSizes();
		}
		else if (this->profiles->GetCount() <= this->currProfile)
		{
			SetCurrentProfile(this->profiles->GetCount() - 1);
		}
		else
		{
			this->currProfile = (UOSInt)-1;
			SetCurrentProfile(index);
		}
	}
	return true;
}

void Media::ProfiledResizer::ProcessFile(const UTF8Char *fileName)
{
	this->loader->AddFileName(fileName);
}

Bool Media::ProfiledResizer::SaveProfile(Text::CString fileName)
{
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	UTF8Char sbuff[1024];
	UTF8Char sbuff2[256];
	UTF8Char *sptr;
	const UTF8Char *cols[8];
	UOSInt i;
	UOSInt j;
	ResizeProfile *profile;
	if (fileName.leng == 0)
	{
		IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::ReplaceExt(sbuff, (const UTF8Char*)"prof");
		NEW_CLASS(fs, IO::FileStream({sbuff, (UOSInt)(sptr - sbuff)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	}
	else
	{
		NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	}
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}

	NEW_CLASS(writer, Text::UTF8Writer(fs));
	i = 0;
	j = this->profiles->GetCount();
	while (i < j)
	{
		profile = this->profiles->GetItem(i);
		cols[0] = profile->profileName;
		cols[1] = profile->suffix;
		cols[2] = sbuff2;
		sptr = Text::StrUInt32(sbuff2, profile->targetSizeX) + 1;
		cols[3] = sptr;
		sptr = Text::StrUInt32(sptr, profile->targetSizeY) + 1;
		cols[4] = sptr;
		sptr = Text::StrInt32(sptr, (Int32)profile->outType) + 1;
		cols[5] = sptr;
		sptr = Text::StrUInt32(sptr, profile->outParam) + 1;
		if (profile->watermark)
		{
			cols[6] = profile->watermark;
		}
		else
		{
			cols[6] = (const UTF8Char*)"";
		}
		cols[7] = sptr;
		sptr = Text::StrInt32(sptr, (Int32)profile->sizeType) + 1;

		sptr = Text::StrCSVJoin(sbuff, cols, 8);
		writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));

		i++;
	}
	DEL_CLASS(writer);
	DEL_CLASS(fs);
	return true;
}

Bool Media::ProfiledResizer::LoadProfile(const UTF8Char *fileName)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[32];
	UInt32 targetSizeX;
	UInt32 targetSizeY;
	Int32 outType;
	UInt32 outParam;
	Int32 sizeType;
	Text::StringBuilderUTF8 sb;
	DB::CSVFile *csv;
	if (fileName == 0)
	{
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::ReplaceExt(sbuff, (const UTF8Char*)"prof");
		NEW_CLASS(csv, DB::CSVFile(sbuff, 65001));
	}
	else
	{
		NEW_CLASS(csv, DB::CSVFile(fileName, 65001));
	}
	csv->SetNoHeader(true);
	DB::DBReader *r = csv->GetTableData(0, 0, 0, 0, 0, 0);
	if (r)
	{
		UOSInt i;
		while (loader->IsProcessing())
		{
			Sync::Thread::Sleep(100);
		}

		this->currProfile = (UOSInt)-1;
		bresizer->ClearTargetSizes();
		i = profiles->GetCount();
		while (i-- > 0)
		{
			ReleaseProfile(this->profiles->RemoveAt(i));
		}

		while (r->ReadNext())
		{
			if (r->ColCount() == 6)
			{
				targetSizeX = (UInt32)r->GetInt32(2);
				targetSizeY = (UInt32)r->GetInt32(3);
				outType = r->GetInt32(4);
				outParam = (UInt32)r->GetInt32(5);
				r->GetStr(0, sbuff, sizeof(sbuff));
				r->GetStr(1, sbuff2, sizeof(sbuff2));
				this->AddProfile(sbuff, sbuff2, targetSizeX, targetSizeY, (OutputType)outType, outParam, 0, ST_MAXSIZE);
			}
			else if (r->ColCount() == 7)
			{
				targetSizeX = (UInt32)r->GetInt32(2);
				targetSizeY = (UInt32)r->GetInt32(3);
				outType = r->GetInt32(4);
				outParam = (UInt32)r->GetInt32(5);
				r->GetStr(0, sbuff, sizeof(sbuff));
				r->GetStr(1, sbuff2, sizeof(sbuff2));
				sb.ClearStr();
				r->GetStr(6, &sb);
				this->AddProfile(sbuff, sbuff2, targetSizeX, targetSizeY, (OutputType)outType, outParam, sb.ToString(), ST_MAXSIZE);
			}
			else if (r->ColCount() == 8)
			{
				targetSizeX = (UInt32)r->GetInt32(2);
				targetSizeY = (UInt32)r->GetInt32(3);
				outType = r->GetInt32(4);
				outParam = (UInt32)r->GetInt32(5);
				r->GetStr(0, sbuff, sizeof(sbuff));
				r->GetStr(1, sbuff2, sizeof(sbuff2));
				sb.ClearStr();
				r->GetStr(6, &sb);
				sizeType = r->GetInt32(7);
				this->AddProfile(sbuff, sbuff2, targetSizeX, targetSizeY, (OutputType)outType, outParam, sb.ToString(), (SizeType)sizeType);
			}
		}
		csv->CloseReader(r);
	}
	DEL_CLASS(csv);
	return false;
}
