#include "Stdafx.h"
#include "DB/CSVFile.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Media/ProfiledResizer.h"
#include "Media/Batch/BatchSaveJPEG.h"
#include "Media/Batch/BatchSaveJPEGSize.h"
#include "Media/Batch/BatchSavePNG.h"
#include "Media/Batch/BatchSaveTIFF.h"
#include "Media/Batch/BatchSaveWebP.h"
#include "Sync/SimpleThread.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

void __stdcall Media::ProfiledResizer::ReleaseProfile(NN<Media::ProfiledResizer::ResizeProfile> profile)
{
	profile->profileName->Release();
	profile->suffix->Release();
	OPTSTR_DEL(profile->watermark);
	MemFreeNN(profile);
}

Media::ProfiledResizer::ProfiledResizer(NN<Parser::ParserList> parsers, Media::ColorManagerSess *colorSess, NN<Media::DrawEngine> deng)
{
	this->currProfile = (UOSInt)-1;
	this->saver = nullptr;
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_SRGB);
	NEW_CLASSNN(watermarker, Media::Batch::BatchWatermarker(deng, nullptr));
	NEW_CLASSNN(resizer, Media::Resizer::LanczosResizerLR_C32(14, 14, destProfile, colorSess, Media::AT_IGNORE_ALPHA, 0.0, Media::PF_B8G8R8A8));
	NEW_CLASSNN(bresizer, Media::Batch::BatchResizer(resizer, watermarker));
	NEW_CLASSNN(limiter, Media::Batch::BatchLimiter(bresizer));
	NEW_CLASSNN(conv, Media::Batch::BatchToLRGB(srcProfile, destProfile, limiter));
	NEW_CLASSNN(loader, Media::Batch::BatchLoader(parsers, conv));
}

Media::ProfiledResizer::~ProfiledResizer()
{
	while (loader->IsProcessing())
	{
		Sync::SimpleThread::Sleep(100);
	}

	this->profiles.FreeAll(ReleaseProfile);
	this->loader.Delete();
	this->conv.Delete();
	this->limiter.Delete();
	this->bresizer.Delete();
	this->resizer.Delete();
	this->watermarker.Delete();
	this->saver.Delete();
}

UOSInt Media::ProfiledResizer::GetProfileCount() const
{
	return this->profiles.GetCount();
}

UOSInt Media::ProfiledResizer::GetCurrProfileIndex()
{
	return this->currProfile;
}

Optional<const Media::ProfiledResizer::ResizeProfile> Media::ProfiledResizer::GetCurrProfile()
{
	return this->profiles.GetItem(this->currProfile);
}

void Media::ProfiledResizer::SetCurrentProfile(UOSInt index)
{
	if (index >= this->profiles.GetCount())
		return;
	if (index == this->currProfile)
		return;

	while (loader->IsProcessing())
	{
		Sync::SimpleThread::Sleep(100);
	}
	this->currProfile = index;
	NN<const ResizeProfile> profile = this->profiles.GetItemNoCheck(index);
	bresizer->ClearTargetSizes();
	if (profile->sizeType == ST_MAXSIZE)
	{
		bresizer->AddTargetSize(profile->targetSizeX, profile->targetSizeY, profile->suffix);
	}
	else if (profile->sizeType == ST_DPI)
	{
		bresizer->AddTargetDPI(profile->targetSizeX, profile->targetSizeY, profile->suffix);
	}
	this->watermarker->SetWatermark(OPTSTR_CSTR(profile->watermark));
	this->saver.Delete();
	if (profile->outType == OT_TIFF)
	{
		NEW_CLASSOPT(this->saver, Media::Batch::BatchSaveTIFF(false));
	}
	else if (profile->outType == OT_JPEGQUALITY)
	{
		NEW_CLASSOPT(this->saver, Media::Batch::BatchSaveJPEG(profile->outParam));
	}
	else if (profile->outType == OT_JPEGSIZE)
	{
		NEW_CLASSOPT(this->saver, Media::Batch::BatchSaveJPEGSize(profile->outParam));
	}
	else if (profile->outType == OT_PNG)
	{
		NEW_CLASSOPT(this->saver, Media::Batch::BatchSavePNG());
	}
	else if (profile->outType == OT_WEBPQUALITY)
	{
		NEW_CLASSOPT(this->saver, Media::Batch::BatchSaveWebP(profile->outParam));
	}
	this->watermarker->SetHandler(this->saver);
}

Optional<const Media::ProfiledResizer::ResizeProfile> Media::ProfiledResizer::GetProfile(UOSInt index) const
{
	return this->profiles.GetItem(index);
}

NN<const Media::ProfiledResizer::ResizeProfile> Media::ProfiledResizer::GetProfileNoCheck(UOSInt index) const
{
	return this->profiles.GetItemNoCheck(index);
}

Bool Media::ProfiledResizer::AddProfile(Text::CStringNN profileName, Text::CStringNN suffix, UInt32 targetSizeX, UInt32 targetSizeY, OutputType outType, UInt32 outParam, Text::CString watermark, SizeType sizeType)
{
	NN<ResizeProfile> profile;
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
	else if (outType == OT_WEBPQUALITY)
	{
		if (outParam > 100)
			return false;
	}
	else
	{
		return false;
	}

	profile = MemAllocNN(ResizeProfile);
	profile->profileName = Text::String::New(profileName);
	profile->suffix = Text::String::New(suffix);
	profile->targetSizeX = targetSizeX;
	profile->targetSizeY = targetSizeY;
	profile->sizeType = sizeType;
	profile->outType = outType;
	profile->outParam = outParam;
	profile->watermark = Text::String::NewOrNull(watermark);
	this->profiles.Add(profile);
	return true;
}

Bool Media::ProfiledResizer::RemoveProfile(UOSInt index)
{
	if (index >= this->profiles.GetCount())
		return false;
	if (index == this->currProfile)
	{
		while (loader->IsProcessing())
		{
			Sync::SimpleThread::Sleep(100);
		}
	}
	ReleaseProfile(this->profiles.GetItemNoCheck(index));
	this->profiles.RemoveAt(index);
	if (index < this->currProfile)
	{
		this->currProfile--;
	}
	else if (index == this->currProfile)
	{
		if (this->profiles.GetCount() == 0)
		{
			this->currProfile = (UOSInt)-1;
			bresizer->ClearTargetSizes();
		}
		else if (this->profiles.GetCount() <= this->currProfile)
		{
			SetCurrentProfile(this->profiles.GetCount() - 1);
		}
		else
		{
			this->currProfile = (UOSInt)-1;
			SetCurrentProfile(index);
		}
	}
	return true;
}

void Media::ProfiledResizer::ProcessFile(Text::CStringNN fileName)
{
	this->loader->AddFileName(fileName);
}

Bool Media::ProfiledResizer::SaveProfile(Text::CStringNN fileName)
{
	UTF8Char sbuff[1024];
	UTF8Char sbuff2[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArrayOpt<const UTF8Char> cols[8];
	UOSInt i;
	UOSInt j;
	NN<ResizeProfile> profile;
	if (fileName.leng == 0)
	{
		IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("prof"));
		fileName = CSTRP(sbuff, sptr);
	}
	IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}

	Text::UTF8Writer writer(fs);
	i = 0;
	j = this->profiles.GetCount();
	while (i < j)
	{
		profile = this->profiles.GetItemNoCheck(i);
		cols[0] = UnsafeArray<const UTF8Char>(profile->profileName->v);
		cols[1] = UnsafeArray<const UTF8Char>(profile->suffix->v);
		cols[2] = sbuff2;
		sptr = Text::StrUInt32(sbuff2, profile->targetSizeX) + 1;
		cols[3] = UnsafeArray<const UTF8Char>(sptr);
		sptr = Text::StrUInt32(sptr, profile->targetSizeY) + 1;
		cols[4] = UnsafeArray<const UTF8Char>(sptr);
		sptr = Text::StrInt32(sptr, (Int32)profile->outType) + 1;
		cols[5] = UnsafeArray<const UTF8Char>(sptr);
		sptr = Text::StrUInt32(sptr, profile->outParam) + 1;
		NN<Text::String> watermark;
		if (profile->watermark.SetTo(watermark))
		{
			cols[6] = UnsafeArray<const UTF8Char>(watermark->v);
		}
		else
		{
			cols[6] = (const UTF8Char*)"";
		}
		cols[7] = UnsafeArray<const UTF8Char>(sptr);
		sptr = Text::StrInt32(sptr, (Int32)profile->sizeType) + 1;

		sptr = Text::StrCSVJoin(sbuff, cols, 8);
		writer.WriteLine(CSTRP(sbuff, sptr));

		i++;
	}
	return true;
}

Bool Media::ProfiledResizer::LoadProfile(Text::CStringNN fileName)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[32];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UInt32 targetSizeX;
	UInt32 targetSizeY;
	Int32 outType;
	UInt32 outParam;
	Int32 sizeType;
	Text::StringBuilderUTF8 sb;
	DB::CSVFile *csv;
	if (fileName.leng == 0)
	{
		IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("prof"));
		NEW_CLASS(csv, DB::CSVFile(CSTRP(sbuff, sptr), 65001));
	}
	else
	{
		NEW_CLASS(csv, DB::CSVFile(fileName, 65001));
	}
	csv->SetNoHeader(true);
	NN<DB::DBReader> r;
	if (csv->QueryTableData(nullptr, CSTR(""), nullptr, 0, 0, nullptr, nullptr).SetTo(r))
	{
		while (loader->IsProcessing())
		{
			Sync::SimpleThread::Sleep(100);
		}

		this->currProfile = (UOSInt)-1;
		bresizer->ClearTargetSizes();
		this->profiles.FreeAll(ReleaseProfile);

		while (r->ReadNext())
		{
			if (r->ColCount() == 6)
			{
				targetSizeX = (UInt32)r->GetInt32(2);
				targetSizeY = (UInt32)r->GetInt32(3);
				outType = r->GetInt32(4);
				outParam = (UInt32)r->GetInt32(5);
				sptr = r->GetStr(0, sbuff, sizeof(sbuff)).Or(sbuff);
				sptr2 = r->GetStr(1, sbuff2, sizeof(sbuff2)).Or(sbuff2);
				this->AddProfile(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2), targetSizeX, targetSizeY, (OutputType)outType, outParam, nullptr, ST_MAXSIZE);
			}
			else if (r->ColCount() == 7)
			{
				targetSizeX = (UInt32)r->GetInt32(2);
				targetSizeY = (UInt32)r->GetInt32(3);
				outType = r->GetInt32(4);
				outParam = (UInt32)r->GetInt32(5);
				sptr = r->GetStr(0, sbuff, sizeof(sbuff)).Or(sbuff);
				sptr2 = r->GetStr(1, sbuff2, sizeof(sbuff2)).Or(sbuff2);
				sb.ClearStr();
				r->GetStr(6, sb);
				this->AddProfile(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2), targetSizeX, targetSizeY, (OutputType)outType, outParam, sb.ToCString(), ST_MAXSIZE);
			}
			else if (r->ColCount() == 8)
			{
				targetSizeX = (UInt32)r->GetInt32(2);
				targetSizeY = (UInt32)r->GetInt32(3);
				outType = r->GetInt32(4);
				outParam = (UInt32)r->GetInt32(5);
				sptr = r->GetStr(0, sbuff, sizeof(sbuff)).Or(sbuff);
				sptr2 = r->GetStr(1, sbuff2, sizeof(sbuff2)).Or(sbuff2);
				sb.ClearStr();
				r->GetStr(6, sb);
				sizeType = r->GetInt32(7);
				this->AddProfile(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2), targetSizeX, targetSizeY, (OutputType)outType, outParam, sb.ToCString(), (SizeType)sizeType);
			}
		}
		csv->CloseReader(r);
	}
	DEL_CLASS(csv);
	return false;
}
