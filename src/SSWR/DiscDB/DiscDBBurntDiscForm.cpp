#include "Stdafx.h"
#include "Data/ArrayListStrUTF8.h"
#include "IO/Path.h"
#include "IO/Registry.h"
#include "SSWR/DiscDB/DiscDBBurntDiscForm.h"
#include "Text/MyStringW.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void SSWR::DiscDB::DiscDBBurntDiscForm::UpdateDiscId()
{
	Data::ArrayList<SSWR::DiscDB::DiscDBEnv::BurntDiscInfo *> discList;
	SSWR::DiscDB::DiscDBEnv::BurntDiscInfo *disc;
	this->lbDiscId->ClearItems();
	this->env->GetBurntDiscs(&discList);
	UOSInt i = 0;
	UOSInt j = discList.GetCount();
	while (i < j)
	{
		disc = discList.GetItem(i);
		this->lbDiscId->AddItem(disc->discId, disc);
		i++;
	}
}

void SSWR::DiscDB::DiscDBBurntDiscForm::UpdateBrand()
{
	UTF8Char sbuff[128];
	UOSInt i;
	UOSInt j;
	OSInt k;
	SSWR::DiscDB::DiscDBEnv::DiscTypeInfo *dType;
	Data::ArrayList<SSWR::DiscDB::DiscDBEnv::DiscTypeInfo *> dTypeList;
	Data::ArrayListStrUTF8 brandList;
	const UTF8Char *brand;
	this->lbBrand->ClearItems();
	this->env->GetDiscTypes(&dTypeList);
	i = 0;
	j = dTypeList.GetCount();
	while (i < j)
	{
		dType = dTypeList.GetItem(i);
		Text::StrToUpperC(sbuff, dType->brand->v, dType->brand->leng);
		k = brandList.SortedIndexOf(sbuff);
		if (k < 0)
		{
			brandList.Insert((UOSInt)~k, Text::StrCopyNew(sbuff));
		}
		i++;
	}
	i = 0;
	j = brandList.GetCount();
	while (i < j)
	{
		brand = brandList.GetItem(i);
		this->lbBrand->AddItem(brand, 0);
		Text::StrDelNew(brand);
		i++;
	}
}

void SSWR::DiscDB::DiscDBBurntDiscForm::UpdateAnimeName()
{
	UOSInt i;
	UOSInt j;
	OSInt k;
	SSWR::DiscDB::DiscDBEnv::DVDVideoInfo *dvdVideo;
	Data::ArrayList<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo *> dvdVideoList;
	Data::ArrayListStrUTF8 animeList;
	const UTF8Char *anime;
	this->cboDVDName->ClearItems();
	this->env->GetDVDVideos(&dvdVideoList);
	i = 0;
	j = dvdVideoList.GetCount();
	while (i < j)
	{
		dvdVideo = dvdVideoList.GetItem(i);
		k = animeList.SortedIndexOf(dvdVideo->anime);
		if (k < 0)
		{
			animeList.Insert((UOSInt)~k, Text::StrCopyNew(dvdVideo->anime));
		}
		i++;
	}
	i = 0;
	j = animeList.GetCount();
	while (i < j)
	{
		anime = animeList.GetItem(i);
		this->cboDVDName->AddItem(anime, 0);
		Text::StrDelNew(anime);
		i++;
	}
	this->cboDVDName->SetText((const UTF8Char*)"Non-Anime");
}

void SSWR::DiscDB::DiscDBBurntDiscForm::UpdateSeries()
{
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo*> dvdVideoList;
	SSWR::DiscDB::DiscDBEnv::DVDVideoInfo *dvdVideo;
	UOSInt i;
	UOSInt j;
	OSInt k;
	Data::ArrayListStrUTF8 seriesList;

	this->cboDVDName->GetText(&sb);
	this->cboSeries->ClearItems();
	this->env->GetDVDVideos(&dvdVideoList);
	i = 0;
	j = dvdVideoList.GetCount();
	while (i < j)
	{
		dvdVideo = dvdVideoList.GetItem(i);

		if (sb.Equals(dvdVideo->anime))
		{
			if (dvdVideo->series == 0 || dvdVideo->series[0] == 0)
			{
			}
			else
			{
				k = seriesList.SortedIndexOf(dvdVideo->series);
				if (k < 0)
				{
					seriesList.Insert((UOSInt)~k, dvdVideo->series);
				}
			}
		}
		i++;
	}

	i = 0;
	j = seriesList.GetCount();
	while (i < j)
	{
		this->cboSeries->AddItem(seriesList.GetItem(i), 0);
		i++;
	}
	this->UpdateVolume();
}

void SSWR::DiscDB::DiscDBBurntDiscForm::UpdateVolume()
{
	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbSeries;
	Data::ArrayList<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo*> dvdVideoList;
	SSWR::DiscDB::DiscDBEnv::DVDVideoInfo *dvdVideo;
	UOSInt i;
	UOSInt j;

	this->cboDVDName->GetText(&sbName);
	this->cboSeries->GetText(&sbSeries);
	this->cboVolume->ClearItems();
	this->env->GetDVDVideos(&dvdVideoList);
	i = 0;
	j = dvdVideoList.GetCount();
	while (i < j)
	{
		dvdVideo = dvdVideoList.GetItem(i);
		if (sbName.Equals(dvdVideo->anime))
		{
			if (dvdVideo->series && dvdVideo->series[0] != 0)
			{
				if (sbSeries.Equals(dvdVideo->series))
				{
					if (dvdVideo->volume)
					{
						this->cboVolume->AddItem(dvdVideo->volume, dvdVideo);
					}
					else
					{
						this->cboVolume->AddItem((const UTF8Char*)"", dvdVideo);
					}
				}
			}
		}
		i++;
	}
}

Bool SSWR::DiscDB::DiscDBBurntDiscForm::UpdateFileInfo()
{
	this->selectedFile->cate = ((SSWR::DiscDB::DiscDBEnv::CategoryInfo*)this->cboCategory->GetSelectedItem())->id;
	this->BurntFileUpdateVideo(this->selectedFile);

	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbSeries;
	Text::StringBuilderUTF8 sbVolume;
	Int32 maxId = -1;
	Int32 volume;
	UOSInt i;
	UOSInt j;
	this->cboDVDName->GetText(&sbName);
	this->cboSeries->GetText(&sbSeries);
	this->cboVolume->GetText(&sbVolume);
	if (sbName.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error found in DVD Name", (const UTF8Char*)"Burnt Disc", this);
		return false;
	}
	if (sbVolume.GetLength() == 0)
	{
		volume = 0;
	}
	else if (!sbVolume.ToInt32(&volume))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error found in volume number", (const UTF8Char*)"Burnt Disc", this);
		return false;
	}
	Data::ArrayList<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo *> dvdVideoList;
	SSWR::DiscDB::DiscDBEnv::DVDVideoInfo *dvdVideo;
	const UTF8Char *s;
	this->env->GetDVDVideos(&dvdVideoList);
	i = 0;
	j = dvdVideoList.GetCount();
	while (i < j)
	{
		dvdVideo = dvdVideoList.GetItem(i);
		if (dvdVideo->videoId > maxId)
		{
			maxId = dvdVideo->videoId;
		}
		if (sbName.Equals(dvdVideo->anime))
		{
			if (dvdVideo->series == 0)
			{
				s = (const UTF8Char*)"";
			}
			else
			{
				s = dvdVideo->series;
			}

			if (sbSeries.Equals(s))
			{
				if (dvdVideo->volume == 0 || dvdVideo->volume[0] == 0)
				{
					if (volume == 0)
					{
						this->selectedFile->videoId = dvdVideo->videoId;
						return true;
					}
				}
				else
				{
					if (Text::StrToInt32(dvdVideo->volume) == volume)
					{
						this->selectedFile->videoId = dvdVideo->videoId;
						return true;
					}
				}
			}
		}

		i++;
	}

	if (UI::MessageDialog::ShowYesNoDialog((const UTF8Char*)"Do you want to add new anime", (const UTF8Char*)"Question", this))
	{
		Text::StringBuilderUTF8 sbDVDType;
		this->cboDVDType->GetText(&sbDVDType);
		maxId = this->env->NewDVDVideo(sbName.ToString(), sbSeries.ToString(), sbVolume.ToString(), sbDVDType.ToString());
		if (maxId >= 0)
		{
			this->selectedFile->videoId = maxId;
			this->UpdateAnimeName();
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in adding new anime", (const UTF8Char*)"Burnt Disc", this);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void SSWR::DiscDB::DiscDBBurntDiscForm::UpdateType()
{
	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbSeries;
	Text::StringBuilderUTF8 sbVolume;
	Data::ArrayList<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo*> dvdVideoList;
	SSWR::DiscDB::DiscDBEnv::DVDVideoInfo *dvdVideo;
	UOSInt i;
	UOSInt j;
	const UTF8Char *s;

	this->cboDVDName->GetText(&sbName);
	this->cboSeries->GetText(&sbSeries);
	this->cboVolume->GetText(&sbVolume);
	this->env->GetDVDVideos(&dvdVideoList);
	i = 0;
	j = dvdVideoList.GetCount();
	while (i < j)
	{
		dvdVideo = dvdVideoList.GetItem(i);
		if (sbName.Equals(dvdVideo->anime))
		{
			if (dvdVideo->series)
			{
				s = dvdVideo->series;
			}
			else
			{
				s = (const UTF8Char*)"";
			}
			if (sbSeries.Equals(s))
			{
				if (dvdVideo->volume)
				{
					s = dvdVideo->volume;
				}
				else
				{
					s = (const UTF8Char*)"";
				}
				if (sbVolume.Equals(s))
				{
					this->cboDVDType->SetText(dvdVideo->dvdType);
				}
			}
		}
		i++;
	}
}

void SSWR::DiscDB::DiscDBBurntDiscForm::SetVideoField(Int32 videoId)
{
	const SSWR::DiscDB::DiscDBEnv::DVDVideoInfo *dvdVideo = this->env->GetDVDVideo(videoId);
	if (dvdVideo)
	{
		this->cboDVDName->SetText(dvdVideo->anime);
		if (dvdVideo->series)
		{
			this->cboSeries->SetText(dvdVideo->series);
		}
		else
		{
			this->cboSeries->SetText((const UTF8Char*)"");
		}

		if (dvdVideo->volume)
		{
			this->cboVolume->SetText(dvdVideo->volume);
		}
		else
		{
			this->cboVolume->SetText((const UTF8Char*)"");
		}
		this->cboDVDType->SetText(dvdVideo->dvdType);
	}
	else
	{
		this->cboDVDName->SetText((const UTF8Char*)"Non-Anime");
		this->cboSeries->SetText((const UTF8Char*)"");
		this->cboVolume->SetText((const UTF8Char*)"");
		this->cboDVDType->SetText((const UTF8Char*)"ROM5");
	}
}

UInt64 SSWR::DiscDB::DiscDBBurntDiscForm::SearchSubDir(const UTF8Char *absPath, const UTF8Char *relPath, UInt64 maxSize)
{
	UInt64 size;
	UInt64 currSize = 0;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	BurntFile *file;
	UOSInt i = Text::StrCharCnt(relPath);
	sptr = Text::StrConcat(sbuff, absPath);
	relPath = sptr - i;
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(sbuff, (UOSInt)(sptr2 - sbuff));
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, &size))
		{
			if (size + currSize > maxSize)
				break;
			if (pt == IO::Path::PathType::File)
			{
				file = this->BurntFileNew(sptr, &relPath[1], size);
				this->lbFileName->AddItem(&relPath[1], file);
				this->fileList->Add(file);
				currSize += size;
			}
			else if (pt == IO::Path::PathType::Directory && sptr[0] != '.')
			{
				size = this->SearchSubDir(sbuff, relPath, maxSize - currSize);
				currSize += size;
			}
		}
		IO::Path::FindFileClose(sess);
	}
	return currSize;
}

void SSWR::DiscDB::DiscDBBurntDiscForm::BurntFileUpdateVideo(BurntFile *file)
{
	file->video = false;
	if (Text::StrEquals(file->cate, (const UTF8Char*)"ISO"))
	{
		file->video = true;
	}
	else if (Text::StrEquals(file->cate, (const UTF8Char*)"AC"))
	{
		if (Text::StrIndexOfICase(file->fname, (const UTF8Char*)"DVD") != INVALID_INDEX)
		{
			file->video = true;
		}
	}
}

SSWR::DiscDB::DiscDBBurntDiscForm::BurntFile *SSWR::DiscDB::DiscDBBurntDiscForm::BurntFileNew(const UTF8Char *fileName, const UTF8Char *relPath, UInt64 fileSize)
{
	BurntFile *file;
	file = MemAlloc(BurntFile, 1);
	file->fname = Text::StrCopyNew(relPath);
	file->fSize = fileSize;
	file->videoId = 0;
	file->cate = (const UTF8Char*)"ISO";
	file->video = true;
	file->anime = 0;
	const WChar *wfileName = Text::StrToWCharNew(fileName);
	if (Text::StrIndexOfICase(fileName, (const UTF8Char*)"EAC") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"AC";
	}
	else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)".TS") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"MV";
		file->anime = this->MovieColsNew(fileName);
	}
	else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)"DVDISO") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"ISO";
	}
	else if (Text::StrIndexOf(wfileName, L"アプリ") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"APP";
	}
	else if (Text::StrIndexOf(wfileName, L"ラジオ") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"RA";
	}
	else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)".VOB") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"OP";
	}
	else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)"XBOX") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"XG";
	}
	else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)".AVI") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"MV";
		file->anime = this->MovieColsNew(fileName);
	}
	else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)".WMV") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"MV";
		file->anime = this->MovieColsNew(fileName);
	}
	else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)".MKV") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"MV";
		file->anime = this->MovieColsNew(fileName);
	}
	else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)".MP4") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"MV";
		file->anime = this->MovieColsNew(fileName);
	}
	else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)".MOV") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"MV";
		file->anime = this->MovieColsNew(fileName);
	}
	else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)"PS2") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"P2G";
	}
	else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)"WII") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"WII";
	}
	else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)"PS") != INVALID_INDEX)
	{
		file->cate = (const UTF8Char*)"PG";
	}
	else if (Text::StrIndexOf(wfileName, L"ゲーム") != INVALID_INDEX)
	{
		if (Text::StrIndexOfICase(fileName, (const UTF8Char*)"DVD") != INVALID_INDEX)
		{
			file->cate = (const UTF8Char*)"DG";
		}
		else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)"CCD") != INVALID_INDEX)
		{
			file->cate = (const UTF8Char*)"GC";
		}
		else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)"CD") != INVALID_INDEX)
		{
			file->cate = (const UTF8Char*)"GC";
		}
		else if (Text::StrIndexOfICase(fileName, (const UTF8Char*)"MDS") != INVALID_INDEX)
		{
			file->cate = (const UTF8Char*)"DG";
		}
		else
		{
			file->cate = (const UTF8Char*)"GC";
		}
	}
	this->BurntFileUpdateVideo(file);
	Text::StrDelNew(wfileName);
	return file;
}

void SSWR::DiscDB::DiscDBBurntDiscForm::BurntFileFree(BurntFile *file)
{
	Text::StrDelNew(file->fname);
	if (file->anime)
	{
		this->MovieColsFree(file->anime);
	}
	MemFree(file);
}

SSWR::DiscDB::DiscDBBurntDiscForm::MovieCols *SSWR::DiscDB::DiscDBBurntDiscForm::MovieColsNew(const UTF8Char *fileName)
{
	WChar fname[512];
	WChar *mainTitle;
	WChar chapterTitle[256];
	Text::StrUTF8_WChar(fname, fileName, 0);
	UOSInt i;
	UOSInt j;
	MovieCols *anime = MemAlloc(MovieCols, 1);
	anime->type = Text::StrCopyNew((const UTF8Char*)"TV");
	anime->remark = 0;
	anime->chapter = 0;
	anime->aspectRatio = 0;
	Text::StrReplace(fname, '_', ' ');
	chapterTitle[0] = 0;
	while (true)
	{
		j = 0;
		if (Text::StrStartsWith(fname, L"["))
		{
			i = Text::StrIndexOf(fname, L"]");
			if (i != INVALID_INDEX)
			{
				j = 1;
				fname[i] = 0;
				Text::StrDelNew(anime->type);
				anime->type = Text::StrToUTF8New(&fname[1]);
				Text::StrConcat(fname, &fname[i + 1]);
				Text::StrTrim(fname);
			}
		}
		if (Text::StrStartsWith(fname, L"【"))
		{
			i = Text::StrIndexOf(fname, L"】");
			if (i != INVALID_INDEX)
			{
				j = 1;
				fname[i] = 0;
				Text::StrDelNew(anime->type);
				anime->type = Text::StrToUTF8New(&fname[1]);
				Text::StrConcat(fname, &fname[i + 1]);
				Text::StrTrim(fname);
			}
		}
		if (Text::StrStartsWith(fname, L"〔"))
		{
			i = Text::StrIndexOf(fname, L"〕");
			if (i != INVALID_INDEX)
			{
				j = 1;
				fname[i] = 0;
				Text::StrDelNew(anime->type);
				anime->type = Text::StrToUTF8New(&fname[1]);
				Text::StrConcat(fname, &fname[i + 1]);
				Text::StrTrim(fname);
			}
		}
		if (Text::StrStartsWith(fname, L"("))
		{
			i = Text::StrIndexOf(fname, L")");
			if (i != INVALID_INDEX)
			{
				j = 1;
				fname[i] = 0;
				Text::StrDelNew(anime->type);
				anime->type = Text::StrToUTF8New(&fname[1]);
				Text::StrConcat(fname, &fname[i + 1]);
				Text::StrTrim(fname);
			}
		}
		if (Text::StrStartsWith(fname, L"（"))
		{
			i = Text::StrIndexOf(fname, L"）");
			if (i != INVALID_INDEX)
			{
				j = 1;
				fname[i] = 0;
				Text::StrDelNew(anime->type);
				anime->type = Text::StrToUTF8New(&fname[1]);
				Text::StrConcat(fname, &fname[i + 1]);
				Text::StrTrim(fname);
			}
		}
		if (j == 0)
		{
			break;
		}
	}

	if ((i = Text::StrIndexOf(fname, L"「")) != INVALID_INDEX)
	{
		mainTitle = fname;
		fname[i] = 0;
		Text::StrConcat(chapterTitle, &fname[i + 1]);
		if ((i = Text::StrIndexOf(chapterTitle, L"」")) != INVALID_INDEX)
		{
			anime->remark = Text::StrToUTF8New(&chapterTitle[i + 1]);
			chapterTitle[i] = 0;
		}
		else if ((i = Text::StrLastIndexOf(chapterTitle, L".")) != INVALID_INDEX)
		{
			chapterTitle[i] = 0;
			anime->remark = Text::StrCopyNew((const UTF8Char*)"");
		}
	}
	else if ((i = Text::StrIndexOf(fname, L"｢")) != INVALID_INDEX)
	{
		mainTitle = fname;
		fname[i] = 0;
		Text::StrConcat(chapterTitle, &fname[i + 1]);
		if ((i = Text::StrIndexOf(chapterTitle, L"｣")) != INVALID_INDEX)
		{
			anime->remark = Text::StrToUTF8New(&chapterTitle[i + 1]);
			chapterTitle[i] = 0;
		}
		else if ((i = Text::StrLastIndexOf(chapterTitle, L".")) != INVALID_INDEX)
		{
			chapterTitle[i] = 0;
			anime->remark = Text::StrCopyNew((const UTF8Char*)"");
		}
	}
	else if ((i = Text::StrIndexOf(fname, L"(")) != INVALID_INDEX)
	{
		anime->remark = Text::StrToUTF8New(&fname[i]);
		mainTitle = fname;
		fname[i] = 0;
	}
	else if ((i = Text::StrIndexOf(fname, L"（")) != INVALID_INDEX)
	{
		anime->remark = Text::StrToUTF8New(&fname[i]);
		mainTitle = fname;
		fname[i] = 0;
	}
	else if ((i = Text::StrIndexOf(fname, L"640")) != INVALID_INDEX)
	{
		anime->remark = Text::StrToUTF8New(&fname[i]);
		mainTitle = fname;
		fname[i] = 0;
	}
	else if ((i = Text::StrIndexOf(fname, L"848")) != INVALID_INDEX)
	{
		anime->remark = Text::StrToUTF8New(&fname[i]);
		mainTitle = fname;
		fname[i] = 0;
	}
	else
	{
		mainTitle = fname;
		anime->remark = Text::StrCopyNew((const UTF8Char*)"");
	}
	if ((i = Text::StrIndexOf(mainTitle, L"\\")) != INVALID_INDEX)
	{
		Text::StrConcat(mainTitle, &mainTitle[i + 1]);
	}
	i = Text::StrLastIndexOfChar(mainTitle, '.');
	if ((i != INVALID_INDEX && i == Text::StrCharCnt(mainTitle) - 4) || Text::StrEndsWithICase(mainTitle, L".TS"))
	{
		mainTitle[i] = 0;
	}
	i = (UOSInt)(Text::StrTrim(mainTitle) - mainTitle - 1);
	while (i-- > 0)
	{
		if ((mainTitle[i] >= 0x30 && mainTitle[i] <= 0x39 && mainTitle[i + 1] >= 0x30 && mainTitle[i + 1] <= 0x39) || (mainTitle[i] == 0x7B2C && mainTitle[i + 1] >= 0x30 && mainTitle[i + 1] <= 0x39)) //0x7b2c == '第'
		{
			if ((j = Text::StrIndexOf(&mainTitle[i + 2], L" ")) != INVALID_INDEX)
			{
				if (mainTitle[0] >= 0x30 && mainTitle[0] <= 0x39)
				{
				}
				else
				{
					if (chapterTitle[0] == 0)
					{
						mainTitle[j] = 0;
						j = (UOSInt)(Text::StrConcat(chapterTitle, &mainTitle[j + 1]) - chapterTitle);
						if (j > 3 && chapterTitle[j - 4] == '.')
						{
							chapterTitle[j - 4] = 0;
						}
					}
					else
					{
					}
				}
			}
			if (mainTitle[0] >= 0x30 && mainTitle[0] <= 0x39)
			{
				j = Text::StrIndexOf(mainTitle, L" ");
				i = Text::StrIndexOf(mainTitle, L"　");
				if (j != INVALID_INDEX && j > i)
				{
					i = j;
				}
				if (i == INVALID_INDEX)
				{
					anime->chapter = 0;
				}
				else
				{
					mainTitle[i] = 0;
					anime->chapter = Text::StrToUTF8New(mainTitle);
					Text::StrConcat(mainTitle, &mainTitle[i + 1]);
				}
			}
			else
			{
				j = Text::StrLastIndexOf(mainTitle, L" ");
				i = Text::StrLastIndexOf(mainTitle, L"　");
				if (j != INVALID_INDEX && j > i)
				{
					i = j;
				}
				if (j == INVALID_INDEX)
				{
					j = Text::StrIndexOf(mainTitle, L"0");
					if (j == INVALID_INDEX)
					{
						anime->chapter = 0;
					}
					else
					{
						anime->chapter = Text::StrToUTF8New(&mainTitle[j]);
						mainTitle[j] = 0;
					}
				}
				else
				{
					anime->chapter = Text::StrToUTF8New(&mainTitle[i + 1]);
					mainTitle[i] = 0;
				}
			}
			break;
		}
	}
	Text::StrTrim(mainTitle);
	anime->mainTitle = Text::StrToUTF8New(mainTitle);
	if (chapterTitle[0])
	{
		anime->chapterTitle = Text::StrToUTF8New(chapterTitle);
	}
	else
	{
		anime->chapterTitle = 0;
	}
	Text::StrUTF8_WChar(fname, anime->remark, 0);
	Text::StrDelNew(anime->remark);
	anime->remark = 0;
	if ((i = Text::StrIndexOf(fname, L"(")) != INVALID_INDEX && (j = Text::StrIndexOf(fname, L")")) != INVALID_INDEX)
	{
		fname[j] = 0;
		anime->remark = Text::StrToUTF8New(&fname[i + 1]);
	}
	else if ((i = Text::StrIndexOf(fname, L"（")) != INVALID_INDEX && (j = Text::StrIndexOf(fname, L"）")) != INVALID_INDEX)
	{
		fname[j] = 0;
		anime->remark = Text::StrToUTF8New(&fname[i + 1]);
	}
	else
	{
		UOSInt ui = Text::StrCharCnt(fname);
		if (ui > 3 && fname[ui - 4] == '.')
		{
			fname[ui - 4] = 0;
			anime->remark = Text::StrToUTF8New(fname);
		}
		else
		{
			anime->remark = 0;
		}
	}
	if (Text::StrEquals(anime->type, (const UTF8Char*)"TV"))
	{
		Text::StrUTF8_WChar(fname, anime->mainTitle, 0);
		if (Text::StrIndexOf(fname, L"ショウ") != INVALID_INDEX)
		{
			Text::StrDelNew(anime->type);
			anime->type = Text::StrCopyNew((const UTF8Char*)"Live");
		}
		else if (Text::StrIndexOf(fname, L"OVA") != INVALID_INDEX)
		{
			Text::StrDelNew(anime->type);
			anime->type = Text::StrCopyNew((const UTF8Char*)"OVA");
		}
		else if (Text::StrIndexOf(fname, L"劇場版") != INVALID_INDEX)
		{
			Text::StrDelNew(anime->type);
			anime->type = Text::StrCopyNew((const UTF8Char*)"Movie");
		}
	}
	if (Text::StrEndsWithICase(fileName, (const UTF8Char*)".WMV"))
	{
		anime->videoFormat = (const UTF8Char*)"WMV3";
		anime->width = 0;
		anime->height = 0;
		anime->fps = 0;
		anime->length = 0;
		anime->audioFormat = (const UTF8Char*)"WMA";
		anime->samplingRate = 0;
		anime->bitRate = 0;
	}
/*	else if (this->ParseAVI(fileName))
	{
	}*/
	else
	{
		anime->videoFormat = (const UTF8Char*)"UNK";
		anime->width = 0;
		anime->height = 0;
		anime->fps = 0;
		anime->length = 0;
		anime->audioFormat = (const UTF8Char*)"UNK";
		anime->samplingRate = 0;
		anime->bitRate = 0;
	}
	return anime;
}

void SSWR::DiscDB::DiscDBBurntDiscForm::MovieColsFree(MovieCols *anime)
{
	Text::StrDelNew(anime->type);
	Text::StrDelNew(anime->mainTitle);
	SDEL_TEXT(anime->chapter);
	SDEL_TEXT(anime->chapterTitle);
	SDEL_TEXT(anime->remark);
	SDEL_TEXT(anime->aspectRatio);

	MemFree(anime);
}


void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnBrowseClicked(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	UI::FileDialog *ofd;
	NEW_CLASS(ofd, UI::FileDialog(L"SSWR", L"DVDDB", L"Browse", true));
	if (ofd->ShowDialog(me->GetHandle()))
	{
		UOSInt i = Text::StrLastIndexOfChar(ofd->GetFileName(), IO::Path::PATH_SEPERATOR);
		Text::StringBuilderUTF8 sbBasePath;
		if (i != INVALID_INDEX)
		{
			sbBasePath.AppendC(ofd->GetFileName(), (UOSInt)i);
		}
		me->lbFileName->ClearItems();
		UOSInt j = me->fileList->GetCount();
		while (j-- > 0)
		{
			me->BurntFileFree(me->fileList->GetItem(j));
		}
		me->fileList->Clear();
		me->selectedFile = 0;
		me->pnlFile->SetEnabled(false);
		me->SearchSubDir(sbBasePath.ToString(), (const UTF8Char*)"", 50000000000LL);
		i = Text::StrLastIndexOfCharC(sbBasePath.ToString(), sbBasePath.GetLength(), IO::Path::PATH_SEPERATOR);
		me->txtDiscId->SetText(sbBasePath.ToString() + i + 1);
	}
	DEL_CLASS(ofd);
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnFileNameSelChg(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	me->pnlFile->SetEnabled(true);
	if (me->selectedFile)
	{
		if (!me->UpdateFileInfo())
		{
			BurntFile *tmpFile = me->selectedFile;
			me->selectedFile = 0;
			me->lbFileName->SetSelectedIndex(me->fileList->IndexOf(tmpFile));
			return;
		}
	}

	me->selectedFile = (BurntFile*)me->lbFileName->GetSelectedItem();
	if (me->selectedFile == 0)
	{
		me->pnlFile->SetEnabled(false);
		return;
	}
	Data::ArrayList<SSWR::DiscDB::DiscDBEnv::CategoryInfo *> cateList;
	UOSInt i;
	me->env->GetCategories(&cateList);
	i = cateList.GetCount();
	while (i-- > 0)
	{
		if (Text::StrEquals(me->selectedFile->cate, cateList.GetItem(i)->id))
		{
			me->cboCategory->SetSelectedIndex(i);
			break;
		}
	}

	if (me->selectedFile->video && me->selectedFile->videoId == 0)
	{
		UOSInt animeLen = 0;
		UOSInt seriesLen = 0;
		UOSInt len;
		const UTF8Char *anime = (const UTF8Char*)"";
		const UTF8Char *series = (const UTF8Char*)"";
		UOSInt prefix = Text::StrIndexOf(me->selectedFile->fname, (const UTF8Char*)".part");
		UOSInt i;
		UOSInt j;

		if (prefix != INVALID_INDEX)
		{
			Text::StringBuilderUTF8 sbBegText;
			BurntFile *bFile;
			sbBegText.AppendC(me->selectedFile->fname, prefix);

			i = 0;
			j = me->fileList->GetCount();
			while (i < j)
			{
				bFile = me->fileList->GetItem(i);
				if (bFile->videoId != 0 && Text::StrStartsWith(bFile->fname, sbBegText.ToString()))
				{
					me->selectedFile->videoId = bFile->videoId;
					me->SetVideoField(bFile->videoId);
					return;
				}
				i++;
			}
		}

		if (me->selectedFile->videoId == 0)
		{
			Data::ArrayList<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo *> dvdVideoList;
			SSWR::DiscDB::DiscDBEnv::DVDVideoInfo *dvdVideo;
			me->env->GetDVDVideos(&dvdVideoList);
			i = 0;
			j = dvdVideoList.GetCount();
			while (i < j)
			{
				dvdVideo = dvdVideoList.GetItem(i);

				len = Text::StrCharCnt(dvdVideo->anime);
				if (animeLen < len)
				{
					if (Text::StrIndexOf(me->selectedFile->fname, dvdVideo->anime) != INVALID_INDEX)
					{
						anime = dvdVideo->anime;
						animeLen = len;
						series = (const UTF8Char*)"";
						seriesLen = 0;
					}
				}
				if (animeLen > 0)
				{
					if (Text::StrEquals(dvdVideo->anime, anime))
					{
						if (dvdVideo->series)
						{
							len = Text::StrCharCnt(dvdVideo->series);
							if (seriesLen < len)
							{
								if (Text::StrIndexOf(me->selectedFile->fname, dvdVideo->series) != INVALID_INDEX)
								{
									series = dvdVideo->series;
									seriesLen = len;
								}
							}
						}
					}
				}
				i++;
			}
			if (animeLen == 0)
			{
				me->cboDVDName->SetText((const UTF8Char*)"Non-Anime");
				me->cboSeries->SetText((const UTF8Char*)"");
				me->cboVolume->SetText((const UTF8Char*)"");
				me->cboDVDType->SetText((const UTF8Char*)"ROM5");
			}
			else
			{
				me->cboDVDName->SetText(anime);
				OnCboDVDNameTextChg(me);
				if (seriesLen == 0)
				{
					me->cboSeries->SetText((const UTF8Char*)"TV");
					series = anime;
				}
				else
				{
					me->cboSeries->SetText(series);
				}
				me->OnSeriesSelChg(me);
				Bool hasVol = false;
				const UTF8Char *fname = me->selectedFile->fname;
				i = Text::StrIndexOf(fname, series) + seriesLen;
				OSInt lastR = 0;
				UOSInt endOfst = Text::StrCharCnt(fname);
				UOSInt i2;
				if ((i2 = Text::StrIndexOfICase(fname, (const UTF8Char*)".PART")) != INVALID_INDEX)
				{
					endOfst = i2;
				}
				while (i < endOfst)
				{
					if (fname[i] >= 0x30 && fname[i] <= 0x39 && lastR < 2)
					{
						j = i + 1;
						while (j < endOfst)
						{
							if (fname[j] < 0x30 || fname[j] > 0x39)
							{
								break;
							}
							j++;
						}
						Text::StringBuilderUTF8 sb;
						sb.AppendC(&fname[i], j - i);
						me->cboVolume->SetText(sb.ToString());
						hasVol = true;
						break;
					}
					else if (fname[i] == 'r' || fname[i] == 'R')
					{
						lastR += 1;
					}
					else
					{
						lastR = 0;
					}
					i++;
				}
				if (!hasVol)
				{
					me->cboVolume->SetText((const UTF8Char*)"1");
				}
				me->cboDVDType->SetText((const UTF8Char*)"ROM5");
				me->UpdateType();
			}
		}
	}
	else
	{
		const SSWR::DiscDB::DiscDBEnv::DVDVideoInfo *dvdVideo = me->env->GetDVDVideo(me->selectedFile->videoId);
		me->cboDVDName->SetText(dvdVideo->anime);
		OnCboDVDNameTextChg(me);
		if (dvdVideo->series == 0)
		{
			me->cboSeries->SetText((const UTF8Char*)"");
		}
		else
		{
			me->cboSeries->SetText(dvdVideo->series);
		}
		me->OnSeriesSelChg(me);

		if (dvdVideo->volume == 0)
		{
			me->cboVolume->SetText((const UTF8Char*)"");
		}
		else
		{
			me->cboVolume->SetText(dvdVideo->volume);
		}
		me->cboDVDType->SetText(dvdVideo->dvdType);
	}
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnBrandSelChg(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	Data::ArrayList<const SSWR::DiscDB::DiscDBEnv::DiscTypeInfo*> discList;
	const SSWR::DiscDB::DiscDBEnv::DiscTypeInfo *discType;
	Text::String *s = me->lbBrand->GetSelectedItemTextNew();
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	me->env->GetDiscTypesByBrand(&discList, s->v, s->leng);
	s->Release();
	me->lbDVDName->ClearItems();
	i = 0;
	j = discList.GetCount();
	while (i < j)
	{
		discType = discList.GetItem(i);
		sptr = Text::StrConcatC(sbuff, UTF8STRC("MID: "));
		sptr = Text::StrConcat(sptr, discType->mid);
		sptr = Text::StrConcatC(sptr, UTF8STRC(", "));
		sptr = Text::StrConcat(sptr, discType->name);
		me->lbDVDName->AddItem(sbuff, (void*)discType);

		i++;
	}
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnDVDNameSelChg(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	const SSWR::DiscDB::DiscDBEnv::DiscTypeInfo *discType = (const SSWR::DiscDB::DiscDBEnv::DiscTypeInfo *)me->lbDVDName->GetSelectedItem();
	me->txtDiscType->SetText(discType->discTypeId);
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnCboDVDNameSelChg(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	me->UpdateSeries();
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnCboDVDNameTextChg(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	me->UpdateSeries();
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnSeriesSelChg(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	me->UpdateVolume();
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnFinishClicked(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	if (me->selectedFile)
	{
		if (!me->UpdateFileInfo())
		{
			return;
		}
	}
	if (me->fileList->GetCount() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please browse file first", (const UTF8Char*)"Burnt Disc", me);
		return;
	}
	Data::DateTime theDate;
	Text::StringBuilderUTF8 sbDate;
	Text::StringBuilderUTF8 sbDiscId;
	Text::StringBuilderUTF8 sbDVDId;
	me->txtDate->GetText(&sbDate);
	if (!theDate.SetValue(sbDate.ToString(), sbDate.GetLength()))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in parsing the date", (const UTF8Char*)"Burnt Disc", me);
		me->txtDate->Focus();
		return;
	}
	me->txtDiscType->GetText(&sbDiscId);
	if (me->env->GetDiscType(sbDiscId.ToString()) == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in the Disc Type", (const UTF8Char*)"Burnt Disc", me);
		me->txtDiscType->Focus();
		return;
	}
	me->txtDiscId->GetText(&sbDVDId);
	if (sbDVDId.GetLength() < 3)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Invalid Disc Id", (const UTF8Char*)"Burnt Disc", me);
		me->txtDiscId->Focus();
		return;
	}
	if (me->env->GetBurntDisc(sbDVDId.ToString()) != 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Disc Id already found in database", (const UTF8Char*)"Burnt Disc", me);
		me->txtDiscId->Focus();
		return;
	}
	const SSWR::DiscDB::DiscDBEnv::BurntDiscInfo *disc = me->env->NewBurntDisc(sbDVDId.ToString(), sbDiscId.ToString(), &theDate);
	if (disc)
	{
		BurntFile *file;
		UOSInt i;
		UOSInt j;
		i = 0;
		j = me->fileList->GetCount();
		while (i < j)
		{
			file = me->fileList->GetItem(i);
			me->env->NewBurntFile(disc->discId, i, file->fname, file->fSize, file->cate, file->videoId);
			if (file->anime)
			{
				me->env->NewMovies(disc->discId, i, file->anime->mainTitle, file->anime->type, file->anime->chapter, file->anime->chapterTitle, file->anime->videoFormat, file->anime->width, file->anime->height, file->anime->fps, file->anime->length, file->anime->audioFormat, file->anime->samplingRate, file->anime->bitRate, file->anime->aspectRatio, file->anime->remark);
			}
			i++;
		}

		IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"DVDDB");
		if (reg)
		{
			const WChar *wptr = Text::StrToWCharNew(sbDiscId.ToString());
			reg->SetValue(L"DiscType", wptr);
			Text::StrDelNew(wptr);
			IO::Registry::CloseRegistry(reg);
		}
		me->Close();
	}
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnVolumeSelChg(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	me->UpdateType();
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnRemoveFileClicked(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	UOSInt i = me->lbFileName->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->selectedFile = 0;
		me->lbFileName->RemoveItem(i);
		me->BurntFileFree(me->fileList->RemoveAt(i));
	}
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnCategorySelChg(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	SSWR::DiscDB::DiscDBEnv::CategoryInfo *cate = (SSWR::DiscDB::DiscDBEnv::CategoryInfo *)me->cboCategory->GetSelectedItem();
	if (cate)
	{
		if (Text::StrEquals(cate->id, (const UTF8Char*)"ISO"))
		{
			me->pnlDVDV->SetEnabled(true);
		}
		else if (Text::StrEquals(cate->id, (const UTF8Char*)"AC"))
		{
			BurntFile *file = (BurntFile*)me->lbFileName->GetSelectedItem();
			if (file && Text::StrIndexOfICase(file->fname, (const UTF8Char*)"DVD") != INVALID_INDEX)
			{
				me->pnlDVDV->SetEnabled(true);
			}
			else
			{
				me->pnlDVDV->SetEnabled(false);
			}
		}
		else
		{
			me->pnlDVDV->SetEnabled(false);
		}
	}
	else
	{
		me->pnlDVDV->SetEnabled(false);
	}
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnDiscIdTextChg(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtDiscId->GetText(&sb);
    if (sb.GetLength() <= 0)
	{
        me->lbDiscId->SetSelectedIndex(0);
        return;
	}
	sb.ToUpper();
	OSInt i = me->env->GetBurntDiscIndex(sb.ToString());
	if (i < 0)
	{
		i = ~i;
	}
    me->lbDiscId->SetSelectedIndex((UOSInt)i);
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnSectorSizeClicked(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	UOSInt i;
	UOSInt j;
	UInt64 size;
	UInt64 baseSize;
	Text::StringBuilderUTF8 sb;
	me->txtSectorSize->GetText(&sb);
	if (sb.GetLength() > 6)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Sector Size too long", (const UTF8Char*)"Burnt Disc", me);
		return;
	}
	i = Text::StrHex2UInt32C(sb.ToString());
	size = i * (UInt64)2048;
	baseSize = 0;
	i = 0;
	j = me->fileList->GetCount();
	while (i < j)
	{
		baseSize += me->fileList->GetItem(i)->fSize;
		if (size < baseSize)
		{
			while (j > i)
			{
				j--;
				me->lbFileName->RemoveItem(j);
				me->BurntFileFree(me->fileList->RemoveAt(j));
			}
			break;
		}
	}
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnAllFileClicked(void *userObj)
{
	SSWR::DiscDB::DiscDBBurntDiscForm *me = (SSWR::DiscDB::DiscDBBurntDiscForm*)userObj;
	if (me->selectedFile)
	{
		if (me->UpdateFileInfo())
		{
			BurntFile *file;
			UOSInt i;
			UOSInt j;
			i = 0;
			j = me->fileList->GetCount();
			while (i < j)
			{
				file = me->fileList->GetItem(i);
				if (file != me->selectedFile)
				{
					Text::StrDelNew(file->cate);
					file->cate = Text::StrCopyNew(me->selectedFile->cate);
					me->BurntFileUpdateVideo(file);
					file->videoId = me->selectedFile->videoId;
				}
				i++;
			}
		}
	}
}

SSWR::DiscDB::DiscDBBurntDiscForm::DiscDBBurntDiscForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::DiscDB::DiscDBEnv *env) : UI::GUIForm(parent, 632, 483, ui)
{
	this->SetText((const UTF8Char*)"Burnt Disc");
	this->SetFont(0, 0, 8.25, false);
	this->env = env;
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
	this->selectedFile = 0;
	NEW_CLASS(this->fileList, Data::ArrayList<BurntFile*>());

	NEW_CLASS(this->pnlTop, UI::GUIPanel(ui, this));
	this->pnlTop->SetRect(0, 0, 624, 121, false);
	this->pnlTop->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->pnlBurntDisc, UI::GUIPanel(ui, this->pnlTop));
	this->pnlBurntDisc->SetRect(0, 0, 472, 121, false);
	this->pnlBurntDisc->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this->pnlBurntDisc, (const UTF8Char*)"&Browse"));
	this->btnBrowse->SetRect(8, 8, 83, 25, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	NEW_CLASS(this->btnFinish, UI::GUIButton(ui, this->pnlBurntDisc, (const UTF8Char*)"&Finsih"));
	this->btnFinish->SetRect(8, 43, 83, 25, false);
	this->btnFinish->HandleButtonClick(OnFinishClicked, this);
	NEW_CLASS(this->btnNewDisc, UI::GUIButton(ui, this->pnlBurntDisc, (const UTF8Char*)"&New Disc Type"));
	this->btnNewDisc->SetRect(8, 78, 83, 25, false);
	NEW_CLASS(this->lblDiscId, UI::GUILabel(ui, this->pnlBurntDisc, (const UTF8Char*)"Disc ID"));
	this->lblDiscId->SetRect(96, 9, 56, 25, false);
	NEW_CLASS(this->txtDiscId, UI::GUITextBox(ui, this->pnlBurntDisc, (const UTF8Char*)""));
	this->txtDiscId->SetRect(160, 9, 96, 20, false);
	this->txtDiscId->HandleTextChanged(OnDiscIdTextChg, this);
	NEW_CLASS(this->lblDiscType, UI::GUILabel(ui, this->pnlBurntDisc, (const UTF8Char*)"Disc Type"));
	this->lblDiscType->SetRect(96, 43, 64, 25, false);
	NEW_CLASS(this->txtDiscType, UI::GUITextBox(ui, this->pnlBurntDisc, (const UTF8Char*)""));
	this->txtDiscType->SetRect(160, 43, 96, 20, false);
	NEW_CLASS(this->lblDate, UI::GUILabel(ui, this->pnlBurntDisc, (const UTF8Char*)"Date"));
	this->lblDate->SetRect(96, 78, 56, 25, false);
	NEW_CLASS(this->txtDate, UI::GUITextBox(ui, this->pnlBurntDisc, (const UTF8Char*)""));
	this->txtDate->SetRect(160, 78, 96, 20, false);
	NEW_CLASS(this->lbDiscId, UI::GUIListBox(ui, this->pnlBurntDisc, false));
	this->lbDiscId->SetRect(256, 0, 80, 121, false);
	NEW_CLASS(this->lbBrand, UI::GUIListBox(ui, this->pnlBurntDisc, false));
	this->lbBrand->SetRect(336, 0, 136, 121, false);
	this->lbBrand->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->lbBrand->HandleSelectionChange(OnBrandSelChg, this);
	NEW_CLASS(this->lbDVDName, UI::GUIListBox(ui, this->pnlTop, false));
	this->lbDVDName->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbDVDName->HandleSelectionChange(OnDVDNameSelChg, this);

	NEW_CLASS(this->pnlFile, UI::GUIPanel(ui, this));
	this->pnlFile->SetRect(416, 121, 208, 335, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_RIGHT);
	NEW_CLASS(this->lblCategory, UI::GUILabel(ui, this->pnlFile, (const UTF8Char*)"Category"));
	this->lblCategory->SetRect(8, 17, 72, 25, false);
	NEW_CLASS(this->cboCategory, UI::GUIComboBox(ui, this->pnlFile, false));
	this->cboCategory->SetRect(80, 17, 121, 21, false);
	this->cboCategory->HandleSelectionChange(OnCategorySelChg, this);
	NEW_CLASS(this->pnlDVDV, UI::GUIPanel(ui, this->pnlFile));
	this->pnlDVDV->SetRect(0, 43, 208, 165, false);
	NEW_CLASS(this->lblDVDName, UI::GUILabel(ui, this->pnlDVDV, (const UTF8Char*)"Name"));
	this->lblDVDName->SetRect(8, 17, 48, 25, false);
	NEW_CLASS(this->cboDVDName, UI::GUIComboBox(ui, this->pnlDVDV, true));
	this->cboDVDName->SetText((const UTF8Char*)"Non-Anime");
	this->cboDVDName->SetRect(64, 17, 121, 21, false);
	this->cboDVDName->HandleSelectionChange(OnCboDVDNameSelChg, this);
//	this->cboDVDName->HandleTextChange(OnCboDVDNameTextChg, this);
	NEW_CLASS(this->lblSeries, UI::GUILabel(ui, this->pnlDVDV, (const UTF8Char*)"Series"));
	this->lblSeries->SetRect(8, 52, 48, 25, false);
	NEW_CLASS(this->cboSeries, UI::GUIComboBox(ui, this->pnlDVDV, true));
	this->cboSeries->SetRect(64, 52, 121, 21, false);
	this->cboSeries->HandleSelectionChange(OnSeriesSelChg, this);
	NEW_CLASS(this->lblVolume, UI::GUILabel(ui, this->pnlDVDV, (const UTF8Char*)"Volume"));
	this->lblVolume->SetRect(8, 87, 48, 25, false);
	NEW_CLASS(this->cboVolume, UI::GUIComboBox(ui, this->pnlDVDV, true));
	this->cboVolume->SetRect(64, 87, 121, 21, false);
	this->cboVolume->HandleSelectionChange(OnVolumeSelChg, this);
	NEW_CLASS(this->lblDVDType, UI::GUILabel(ui, this->pnlDVDV, (const UTF8Char*)"Type"));
	this->lblDVDType->SetRect(8, 121, 48, 25, false);
	NEW_CLASS(this->cboDVDType, UI::GUIComboBox(ui, this->pnlDVDV, false));
	this->cboDVDType->SetRect(64, 121, 121, 21, false);
	this->cboDVDType->AddItem((const UTF8Char*)"ROM5", 0);
	this->cboDVDType->AddItem((const UTF8Char*)"ROM9", 0);
	NEW_CLASS(this->btnAllFile, UI::GUIButton(ui, this->pnlFile, (const UTF8Char*)"All File Same Category"));
	this->btnAllFile->SetRect(24, 217, 136, 25, false);
	this->btnAllFile->HandleButtonClick(OnAllFileClicked, this);
	NEW_CLASS(this->btnRemoveFile, UI::GUIButton(ui, this->pnlFile, (const UTF8Char*)"&Remove File"));
	this->btnRemoveFile->SetRect(24, 251, 88, 25, false);
	this->btnRemoveFile->HandleButtonClick(OnRemoveFileClicked, this);
	NEW_CLASS(this->btnBuildMovie, UI::GUIButton(ui, this->pnlFile, (const UTF8Char*)"Build Movie"));
	this->btnBuildMovie->SetRect(128, 251, 75, 25, false);
	NEW_CLASS(this->lblSectorSize, UI::GUILabel(ui, this->pnlFile, (const UTF8Char*)"Sector Size"));
	this->lblSectorSize->SetRect(16, 277, 72, 25, false);
	NEW_CLASS(this->txtSectorSize, UI::GUITextBox(ui, this->pnlFile, (const UTF8Char*)"231000"));
	this->txtSectorSize->SetRect(96, 277, 100, 20, false);
	NEW_CLASS(this->btnSectorSize, UI::GUIButton(ui, this->pnlFile, (const UTF8Char*)"Apply"));
	this->btnSectorSize->SetRect(120, 303, 75, 25, false);
	this->btnSectorSize->HandleButtonClick(OnSectorSizeClicked, this);
	NEW_CLASS(this->lbFileName, UI::GUIListBox(ui, this, false));
	this->lbFileName->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbFileName->HandleSelectionChange(OnFileNameSelChg, this);

	this->pnlFile->SetEnabled(false);

	UTF8Char sbuff[128];
	UOSInt i;
	UOSInt j;
	Data::DateTime dt;
	dt.SetCurrTime();
	dt.ToString(sbuff, "yyyy-MM-dd");
    this->txtDate->SetText(sbuff);

	Data::ArrayList<SSWR::DiscDB::DiscDBEnv::CategoryInfo*> cateList;
	SSWR::DiscDB::DiscDBEnv::CategoryInfo *cate;
	this->env->GetCategories(&cateList);
	i = 0;
	j = cateList.GetCount();
	while (i < j)
	{
		cate = cateList.GetItem(i);
		this->cboCategory->AddItem(cate->name, cate);
		i++;
	}
	this->UpdateDiscId();
	this->UpdateBrand();
	this->UpdateAnimeName();

	IO::Registry *reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"DVDDB");
	if (reg)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		WChar wbuff[64];
		if (reg->GetValueStr(L"DiscType", wbuff))
		{
			
			Text::StrWChar_UTF8(sbuff, wbuff);
			this->txtDiscType->SetText(sbuff);
			const SSWR::DiscDB::DiscDBEnv::DiscTypeInfo *discType = this->env->GetDiscType(sbuff);
			if (discType)
			{
				i = this->lbBrand->GetCount();
				while (i-- > 0)
				{
					if ((sptr = this->lbBrand->GetItemText(sbuff, i)) != 0)
					{
						if (discType->brand->Equals(sbuff, (UOSInt)(sptr - sbuff)))
						{
							this->lbBrand->SetSelectedIndex(i);
							break;
						}
					}
				}
			}
		}
	}
	this->btnBrowse->Focus();
}

SSWR::DiscDB::DiscDBBurntDiscForm::~DiscDBBurntDiscForm()
{
	UOSInt i = this->fileList->GetCount();
	while (i-- > 0)
	{
		this->BurntFileFree(this->fileList->GetItem(i));
	}
	DEL_CLASS(this->fileList);
}

void SSWR::DiscDB::DiscDBBurntDiscForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}
