#include "Stdafx.h"
#include "Data/ArrayListString.h"
#include "Data/ArrayListStringNN.h"
#include "IO/Path.h"
#include "IO/Registry.h"
#include "SSWR/DiscDB/DiscDBBurntDiscForm.h"
#include "Text/MyStringW.h"
#include "UI/GUIFileDialog.h"

void SSWR::DiscDB::DiscDBBurntDiscForm::UpdateDiscId()
{
	Data::ArrayListNN<SSWR::DiscDB::DiscDBEnv::BurntDiscInfo> discList;
	NN<SSWR::DiscDB::DiscDBEnv::BurntDiscInfo> disc;
	this->lbDiscId->ClearItems();
	this->env->GetBurntDiscs(discList);
	UOSInt i = 0;
	UOSInt j = discList.GetCount();
	while (i < j)
	{
		disc = discList.GetItemNoCheck(i);
		this->lbDiscId->AddItem(disc->discId->ToCString(), disc);
		i++;
	}
}

void SSWR::DiscDB::DiscDBBurntDiscForm::UpdateBrand()
{
	UTF8Char sbuff[128];
	UOSInt i;
	UOSInt j;
	OSInt k;
	NN<SSWR::DiscDB::DiscDBEnv::DiscTypeInfo> dType;
	Data::ArrayListNN<SSWR::DiscDB::DiscDBEnv::DiscTypeInfo> dTypeList;
	Data::ArrayListStrUTF8 brandList;
	UnsafeArray<const UTF8Char> brand;
	this->lbBrand->ClearItems();
	this->env->GetDiscTypes(dTypeList);
	i = 0;
	j = dTypeList.GetCount();
	while (i < j)
	{
		dType = dTypeList.GetItemNoCheck(i);
		Text::StrToUpperC(sbuff, dType->brand->v, dType->brand->leng);
		k = brandList.SortedIndexOf(sbuff);
		if (k < 0)
		{
			brandList.Insert((UOSInt)~k, Text::StrCopyNew(sbuff).Ptr());
		}
		i++;
	}
	i = 0;
	j = brandList.GetCount();
	while (i < j)
	{
		if (brandList.GetItem(i).SetTo(brand))
		{
			this->lbBrand->AddItem({brand, Text::StrCharCnt(brand)}, 0);
			Text::StrDelNew(brand);
		}
		i++;
	}
}

void SSWR::DiscDB::DiscDBBurntDiscForm::UpdateAnimeName()
{
	UOSInt i;
	UOSInt j;
	OSInt k;
	NN<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideo;
	Data::ArrayListNN<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideoList;
	Data::ArrayListStringNN animeList;
	NN<Text::String> anime;
	this->cboDVDName->ClearItems();
	this->env->GetDVDVideos(dvdVideoList);
	i = 0;
	j = dvdVideoList.GetCount();
	while (i < j)
	{
		dvdVideo = dvdVideoList.GetItemNoCheck(i);
		k = animeList.SortedIndexOf(dvdVideo->anime);
		if (k < 0)
		{
			animeList.Insert((UOSInt)~k, dvdVideo->anime->Clone());
		}
		i++;
	}
	Data::ArrayIterator<NN<Text::String>> it = animeList.Iterator();
	while (it.HasNext())
	{
		anime = it.Next();
		this->cboDVDName->AddItem(anime, 0);
		anime->Release();
		i++;
	}
	this->cboDVDName->SetText(CSTR("Non-Anime"));
}

void SSWR::DiscDB::DiscDBBurntDiscForm::UpdateSeries()
{
	Text::StringBuilderUTF8 sb;
	Data::ArrayListNN<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideoList;
	NN<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideo;
	UOSInt i;
	UOSInt j;
	OSInt k;
	Data::ArrayListString seriesList;
	NN<Text::String> s;

	this->cboDVDName->GetText(sb);
	this->cboSeries->ClearItems();
	this->env->GetDVDVideos(dvdVideoList);
	i = 0;
	j = dvdVideoList.GetCount();
	while (i < j)
	{
		dvdVideo = dvdVideoList.GetItemNoCheck(i);

		if (sb.Equals(dvdVideo->anime))
		{
			if (!dvdVideo->series.SetTo(s) || s->v[0] == 0)
			{
			}
			else
			{
				k = seriesList.SortedIndexOf(s.Ptr());
				if (k < 0)
				{
					seriesList.Insert((UOSInt)~k, s.Ptr());
				}
			}
		}
		i++;
	}

	i = 0;
	j = seriesList.GetCount();
	while (i < j)
	{
		this->cboSeries->AddItem(Text::String::OrEmpty(seriesList.GetItem(i)), 0);
		i++;
	}
	this->UpdateVolume();
}

void SSWR::DiscDB::DiscDBBurntDiscForm::UpdateVolume()
{
	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbSeries;
	Data::ArrayListNN<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideoList;
	NN<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideo;
	UOSInt i;
	UOSInt j;
	NN<Text::String> s;

	this->cboDVDName->GetText(sbName);
	this->cboSeries->GetText(sbSeries);
	this->cboVolume->ClearItems();
	this->env->GetDVDVideos(dvdVideoList);
	i = 0;
	j = dvdVideoList.GetCount();
	while (i < j)
	{
		dvdVideo = dvdVideoList.GetItemNoCheck(i);
		if (sbName.Equals(dvdVideo->anime))
		{
			if (dvdVideo->series.SetTo(s) && s->v[0] != 0)
			{
				if (s->Equals(sbSeries))
				{
					this->cboVolume->AddItem(Text::String::OrEmpty(dvdVideo->volume), dvdVideo);
				}
			}
		}
		i++;
	}
}

Bool SSWR::DiscDB::DiscDBBurntDiscForm::UpdateFileInfo(NN<BurntFile> selectedFile)
{
	selectedFile->cate = this->cboCategory->GetSelectedItem().GetNN<SSWR::DiscDB::DiscDBEnv::CategoryInfo>()->id->ToCString();
	this->BurntFileUpdateVideo(selectedFile);

	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbSeries;
	Text::StringBuilderUTF8 sbVolume;
	Int32 maxId = -1;
	Int32 volume;
	UOSInt i;
	UOSInt j;
	this->cboDVDName->GetText(sbName);
	this->cboSeries->GetText(sbSeries);
	this->cboVolume->GetText(sbVolume);
	if (sbName.GetLength() == 0)
	{
		this->ui->ShowMsgOK(CSTR("Error found in DVD Name"), CSTR("Burnt Disc"), this);
		return false;
	}
	if (sbVolume.GetLength() == 0)
	{
		volume = 0;
	}
	else if (!sbVolume.ToInt32(volume))
	{
		this->ui->ShowMsgOK(CSTR("Error found in volume number"), CSTR("Burnt Disc"), this);
		return false;
	}
	Data::ArrayListNN<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideoList;
	NN<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideo;
	NN<Text::String> s;
	NN<Text::String> vol;
	this->env->GetDVDVideos(dvdVideoList);
	i = 0;
	j = dvdVideoList.GetCount();
	while (i < j)
	{
		dvdVideo = dvdVideoList.GetItemNoCheck(i);
		if (dvdVideo->videoId > maxId)
		{
			maxId = dvdVideo->videoId;
		}
		if (sbName.Equals(dvdVideo->anime))
		{
			s = Text::String::OrEmpty(dvdVideo->series);
			if (sbSeries.Equals(s))
			{
				if (!dvdVideo->volume.SetTo(vol) || vol->v[0] == 0)
				{
					if (volume == 0)
					{
						selectedFile->videoId = dvdVideo->videoId;
						return true;
					}
				}
				else
				{
					if (vol->ToInt32() == volume)
					{
						selectedFile->videoId = dvdVideo->videoId;
						return true;
					}
				}
			}
		}

		i++;
	}

	if (this->ui->ShowMsgYesNo(CSTR("Do you want to add new anime"), CSTR("Question"), this))
	{
		Text::StringBuilderUTF8 sbDVDType;
		this->cboDVDType->GetText(sbDVDType);
		maxId = this->env->NewDVDVideo(sbName.ToString(), sbSeries.ToString(), sbVolume.ToString(), sbDVDType.ToString());
		if (maxId >= 0)
		{
			selectedFile->videoId = maxId;
			this->UpdateAnimeName();
			this->ui->ShowMsgOK(CSTR("Error in adding new anime"), CSTR("Burnt Disc"), this);
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
	Data::ArrayListNN<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideoList;
	NN<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideo;
	UOSInt i;
	UOSInt j;
	NN<Text::String> s;

	this->cboDVDName->GetText(sbName);
	this->cboSeries->GetText(sbSeries);
	this->cboVolume->GetText(sbVolume);
	this->env->GetDVDVideos(dvdVideoList);
	i = 0;
	j = dvdVideoList.GetCount();
	while (i < j)
	{
		dvdVideo = dvdVideoList.GetItemNoCheck(i);
		if (sbName.Equals(dvdVideo->anime))
		{
			s = Text::String::OrEmpty(dvdVideo->series);
			if (sbSeries.Equals(s))
			{
				s = Text::String::OrEmpty(dvdVideo->volume);
				if (sbVolume.Equals(s))
				{
					this->cboDVDType->SetText(dvdVideo->dvdType->ToCString());
				}
			}
		}
		i++;
	}
}

void SSWR::DiscDB::DiscDBBurntDiscForm::SetVideoField(Int32 videoId)
{
	NN<const SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideo;
	if (this->env->GetDVDVideo(videoId).SetTo(dvdVideo))
	{
		this->cboDVDName->SetText(dvdVideo->anime->ToCString());
		this->cboSeries->SetText(Text::String::OrEmpty(dvdVideo->series)->ToCString());
		this->cboVolume->SetText(Text::String::OrEmpty(dvdVideo->volume)->ToCString());
		this->cboDVDType->SetText(dvdVideo->dvdType->ToCString());
	}
	else
	{
		this->cboDVDName->SetText(CSTR("Non-Anime"));
		this->cboSeries->SetText(CSTR(""));
		this->cboVolume->SetText(CSTR(""));
		this->cboDVDType->SetText(CSTR("ROM5"));
	}
}

UInt64 SSWR::DiscDB::DiscDBBurntDiscForm::SearchSubDir(UnsafeArray<const UTF8Char> absPath, UnsafeArray<const UTF8Char> relPath, UInt64 maxSize)
{
	UInt64 size;
	UInt64 currSize = 0;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<IO::Path::FindFileSession> sess;
	IO::Path::PathType pt;
	NN<BurntFile> file;
	UOSInt i = Text::StrCharCnt(relPath);
	sptr = Text::StrConcat(sbuff, absPath);
	relPath = sptr - i;
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, size).SetTo(sptr2))
		{
			if (size + currSize > maxSize)
				break;
			if (pt == IO::Path::PathType::File)
			{
				file = this->BurntFileNew(CSTRP(sptr, sptr2), CSTRP(&relPath[1], UnsafeArray<const UTF8Char>(sptr2)), size);
				this->lbFileName->AddItem({&relPath[1], Text::StrCharCnt(&relPath[1])}, file);
				this->fileList.Add(file);
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

void SSWR::DiscDB::DiscDBBurntDiscForm::BurntFileUpdateVideo(NN<BurntFile> file)
{
	file->video = false;
	if (file->cate.Equals(UTF8STRC("ISO")))
	{
		file->video = true;
	}
	else if (file->cate.Equals(UTF8STRC("AC")))
	{
		if (file->fname->IndexOfICase(UTF8STRC("DVD")) != INVALID_INDEX)
		{
			file->video = true;
		}
	}
}

NN<SSWR::DiscDB::DiscDBBurntDiscForm::BurntFile> SSWR::DiscDB::DiscDBBurntDiscForm::BurntFileNew(Text::CStringNN fileName, Text::CStringNN relPath, UInt64 fileSize)
{
	NN<BurntFile> file;
	file = MemAllocNN(BurntFile);
	file->fname = Text::String::New(relPath);
	file->fSize = fileSize;
	file->videoId = 0;
	file->cate = CSTR("ISO");
	file->video = true;
	file->anime = 0;
	UnsafeArray<const WChar> wfileName = Text::StrToWCharNew(fileName.v);
	if (fileName.IndexOfICase(UTF8STRC("EAC")) != INVALID_INDEX)
	{
		file->cate = CSTR("AC");
	}
	else if (fileName.IndexOfICase(UTF8STRC(".TS")) != INVALID_INDEX)
	{
		file->cate = CSTR("MV");
		file->anime = this->MovieColsNew(fileName);
	}
	else if (fileName.IndexOfICase(UTF8STRC("DVDISO")) != INVALID_INDEX)
	{
		file->cate = CSTR("ISO");
	}
	else if (Text::StrIndexOfW(wfileName, L"アプリ") != INVALID_INDEX)
	{
		file->cate = CSTR("APP");
	}
	else if (Text::StrIndexOfW(wfileName, L"ラジオ") != INVALID_INDEX)
	{
		file->cate = CSTR("RA");
	}
	else if (fileName.IndexOfICase(UTF8STRC(".VOB")) != INVALID_INDEX)
	{
		file->cate = CSTR("OP");
	}
	else if (fileName.IndexOfICase(UTF8STRC("XBOX")) != INVALID_INDEX)
	{
		file->cate = CSTR("XG");
	}
	else if (fileName.IndexOfICase(UTF8STRC(".AVI")) != INVALID_INDEX)
	{
		file->cate = CSTR("MV");
		file->anime = this->MovieColsNew(fileName);
	}
	else if (fileName.IndexOfICase(UTF8STRC(".WMV")) != INVALID_INDEX)
	{
		file->cate = CSTR("MV");
		file->anime = this->MovieColsNew(fileName);
	}
	else if (fileName.IndexOfICase(UTF8STRC(".MKV")) != INVALID_INDEX)
	{
		file->cate = CSTR("MV");
		file->anime = this->MovieColsNew(fileName);
	}
	else if (fileName.IndexOfICase(UTF8STRC(".MP4")) != INVALID_INDEX)
	{
		file->cate = CSTR("MV");
		file->anime = this->MovieColsNew(fileName);
	}
	else if (fileName.IndexOfICase(UTF8STRC(".MOV")) != INVALID_INDEX)
	{
		file->cate = CSTR("MV");
		file->anime = this->MovieColsNew(fileName);
	}
	else if (fileName.IndexOfICase(UTF8STRC("PS2")) != INVALID_INDEX)
	{
		file->cate = CSTR("P2G");
	}
	else if (fileName.IndexOfICase(UTF8STRC("WII")) != INVALID_INDEX)
	{
		file->cate = CSTR("WII");
	}
	else if (fileName.IndexOfICase(UTF8STRC("PS")) != INVALID_INDEX)
	{
		file->cate = CSTR("PG");
	}
	else if (Text::StrIndexOfW(wfileName, L"ゲーム") != INVALID_INDEX)
	{
		if (fileName.IndexOfICase(UTF8STRC("DVD")) != INVALID_INDEX)
		{
			file->cate = CSTR("DG");
		}
		else if (fileName.IndexOfICase(UTF8STRC("CCD")) != INVALID_INDEX)
		{
			file->cate = CSTR("GC");
		}
		else if (fileName.IndexOfICase(UTF8STRC("CD")) != INVALID_INDEX)
		{
			file->cate = CSTR("GC");
		}
		else if (fileName.IndexOfICase(UTF8STRC("MDS")) != INVALID_INDEX)
		{
			file->cate = CSTR("DG");
		}
		else
		{
			file->cate = CSTR("GC");
		}
	}
	this->BurntFileUpdateVideo(file);
	Text::StrDelNew(wfileName);
	return file;
}

void SSWR::DiscDB::DiscDBBurntDiscForm::BurntFileFree(NN<BurntFile> file)
{
	file->fname->Release();
	if (file->anime)
	{
		MovieColsFree(file->anime);
	}
	MemFreeNN(file);
}

SSWR::DiscDB::DiscDBBurntDiscForm::MovieCols *SSWR::DiscDB::DiscDBBurntDiscForm::MovieColsNew(Text::CStringNN fileName)
{
	WChar fname[512];
	WChar *mainTitle;
	WChar chapterTitle[256];
	UnsafeArray<const UTF8Char> nns;
	Text::StrUTF8_WChar(fname, fileName.v, 0);
	UOSInt i;
	UOSInt j;
	MovieCols *anime = MemAlloc(MovieCols, 1);
	anime->type = Text::String::New(UTF8STRC("TV"));
	anime->remark = nullptr;
	anime->chapter = nullptr;
	anime->aspectRatio = nullptr;
	Text::StrReplaceW(fname, '_', ' ');
	chapterTitle[0] = 0;
	while (true)
	{
		j = 0;
		if (Text::StrStartsWith(fname, L"["))
		{
			i = Text::StrIndexOfW(fname, L"]");
			if (i != INVALID_INDEX)
			{
				j = 1;
				fname[i] = 0;
				anime->type->Release();
				anime->type = Text::String::NewNotNull(&fname[1]);
				Text::StrConcat(fname, &fname[i + 1]);
				Text::StrTrim(fname);
			}
		}
		if (Text::StrStartsWith(fname, L"【"))
		{
			i = Text::StrIndexOfW(fname, L"】");
			if (i != INVALID_INDEX)
			{
				j = 1;
				fname[i] = 0;
				anime->type->Release();
				anime->type = Text::String::NewNotNull(&fname[1]);
				Text::StrConcat(fname, &fname[i + 1]);
				Text::StrTrim(fname);
			}
		}
		if (Text::StrStartsWith(fname, L"〔"))
		{
			i = Text::StrIndexOfW(fname, L"〕");
			if (i != INVALID_INDEX)
			{
				j = 1;
				fname[i] = 0;
				anime->type->Release();
				anime->type = Text::String::NewNotNull(&fname[1]);
				Text::StrConcat(fname, &fname[i + 1]);
				Text::StrTrim(fname);
			}
		}
		if (Text::StrStartsWith(fname, L"("))
		{
			i = Text::StrIndexOfW(fname, L")");
			if (i != INVALID_INDEX)
			{
				j = 1;
				fname[i] = 0;
				anime->type->Release();
				anime->type = Text::String::NewNotNull(&fname[1]);
				Text::StrConcat(fname, &fname[i + 1]);
				Text::StrTrim(fname);
			}
		}
		if (Text::StrStartsWith(fname, L"（"))
		{
			i = Text::StrIndexOfW(fname, L"）");
			if (i != INVALID_INDEX)
			{
				j = 1;
				fname[i] = 0;
				anime->type->Release();
				anime->type = Text::String::NewNotNull(&fname[1]);
				Text::StrConcat(fname, &fname[i + 1]);
				Text::StrTrim(fname);
			}
		}
		if (j == 0)
		{
			break;
		}
	}

	if ((i = Text::StrIndexOfW(fname, L"「")) != INVALID_INDEX)
	{
		mainTitle = fname;
		fname[i] = 0;
		Text::StrConcat(chapterTitle, &fname[i + 1]);
		if ((i = Text::StrIndexOfW(chapterTitle, L"」")) != INVALID_INDEX)
		{
			anime->remark = Text::StrToUTF8New(&chapterTitle[i + 1]);
			chapterTitle[i] = 0;
		}
		else if ((i = Text::StrLastIndexOfW(chapterTitle, L".")) != INVALID_INDEX)
		{
			chapterTitle[i] = 0;
			anime->remark = Text::StrCopyNewC(UTF8STRC("")).Ptr();
		}
	}
	else if ((i = Text::StrIndexOfW(fname, L"｢")) != INVALID_INDEX)
	{
		mainTitle = fname;
		fname[i] = 0;
		Text::StrConcat(chapterTitle, &fname[i + 1]);
		if ((i = Text::StrIndexOfW(chapterTitle, L"｣")) != INVALID_INDEX)
		{
			anime->remark = Text::StrToUTF8New(&chapterTitle[i + 1]);
			chapterTitle[i] = 0;
		}
		else if ((i = Text::StrLastIndexOfW(chapterTitle, L".")) != INVALID_INDEX)
		{
			chapterTitle[i] = 0;
			anime->remark = Text::StrCopyNewC(UTF8STRC("")).Ptr();
		}
	}
	else if ((i = Text::StrIndexOfW(fname, L"(")) != INVALID_INDEX)
	{
		anime->remark = Text::StrToUTF8New(&fname[i]);
		mainTitle = fname;
		fname[i] = 0;
	}
	else if ((i = Text::StrIndexOfW(fname, L"（")) != INVALID_INDEX)
	{
		anime->remark = Text::StrToUTF8New(&fname[i]);
		mainTitle = fname;
		fname[i] = 0;
	}
	else if ((i = Text::StrIndexOfW(fname, L"640")) != INVALID_INDEX)
	{
		anime->remark = Text::StrToUTF8New(&fname[i]);
		mainTitle = fname;
		fname[i] = 0;
	}
	else if ((i = Text::StrIndexOfW(fname, L"848")) != INVALID_INDEX)
	{
		anime->remark = Text::StrToUTF8New(&fname[i]);
		mainTitle = fname;
		fname[i] = 0;
	}
	else
	{
		mainTitle = fname;
		anime->remark = Text::StrCopyNewC(UTF8STRC("")).Ptr();
	}
	if ((i = Text::StrIndexOfW(mainTitle, L"\\")) != INVALID_INDEX)
	{
		Text::StrConcat(mainTitle, &mainTitle[i + 1]);
	}
	i = Text::StrLastIndexOfCharW(mainTitle, '.');
	if ((i != INVALID_INDEX && i == Text::StrCharCnt(mainTitle) - 4) || Text::StrEndsWithICase(mainTitle, L".TS"))
	{
		mainTitle[i] = 0;
	}
	i = (UOSInt)(Text::StrTrim(mainTitle) - mainTitle - 1);
	while (i-- > 0)
	{
		if ((mainTitle[i] >= 0x30 && mainTitle[i] <= 0x39 && mainTitle[i + 1] >= 0x30 && mainTitle[i + 1] <= 0x39) || (mainTitle[i] == 0x7B2C && mainTitle[i + 1] >= 0x30 && mainTitle[i + 1] <= 0x39)) //0x7b2c == '第'
		{
			if ((j = Text::StrIndexOfW(&mainTitle[i + 2], L" ")) != INVALID_INDEX)
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
				j = Text::StrIndexOfW(mainTitle, L" ");
				i = Text::StrIndexOfW(mainTitle, L"　");
				if (j != INVALID_INDEX && j > i)
				{
					i = j;
				}
				if (i == INVALID_INDEX)
				{
					anime->chapter = nullptr;
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
				j = Text::StrLastIndexOfW(mainTitle, L" ");
				i = Text::StrLastIndexOfW(mainTitle, L"　");
				if (j != INVALID_INDEX && j > i)
				{
					i = j;
				}
				if (j == INVALID_INDEX)
				{
					j = Text::StrIndexOfW(mainTitle, L"0");
					if (j == INVALID_INDEX)
					{
						anime->chapter = nullptr;
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
		anime->chapterTitle = nullptr;
	}
	if (anime->remark.SetTo(nns))
	{
		Text::StrUTF8_WChar(fname, nns, 0);
		Text::StrDelNew(nns);
	}
	else
	{
		fname[0] = 0;

	}
	anime->remark = nullptr;
	if ((i = Text::StrIndexOfW(fname, L"(")) != INVALID_INDEX && (j = Text::StrIndexOfW(fname, L")")) != INVALID_INDEX)
	{
		fname[j] = 0;
		anime->remark = Text::StrToUTF8New(&fname[i + 1]);
	}
	else if ((i = Text::StrIndexOfW(fname, L"（")) != INVALID_INDEX && (j = Text::StrIndexOfW(fname, L"）")) != INVALID_INDEX)
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
			anime->remark = nullptr;
		}
	}
	if (anime->type->Equals(UTF8STRC("TV")))
	{
		Text::StrUTF8_WChar(fname, anime->mainTitle, 0);
		if (Text::StrIndexOfW(fname, L"ショウ") != INVALID_INDEX)
		{
			anime->type->Release();
			anime->type = Text::String::New(UTF8STRC("Live"));
		}
		else if (Text::StrIndexOfW(fname, L"OVA") != INVALID_INDEX)
		{
			anime->type->Release();
			anime->type = Text::String::New(UTF8STRC("OVA"));
		}
		else if (Text::StrIndexOfW(fname, L"劇場版") != INVALID_INDEX)
		{
			anime->type->Release();
			anime->type = Text::String::New(UTF8STRC("Movie"));
		}
	}
	if (fileName.EndsWithICase(UTF8STRC(".WMV")))
	{
		anime->videoFormat = CSTR("WMV3");
		anime->width = 0;
		anime->height = 0;
		anime->fps = 0;
		anime->length = 0;
		anime->audioFormat = CSTR("WMA");
		anime->samplingRate = 0;
		anime->bitRate = 0;
	}
/*	else if (this->ParseAVI(fileName))
	{
	}*/
	else
	{
		anime->videoFormat = CSTR("UNK");
		anime->width = 0;
		anime->height = 0;
		anime->fps = 0;
		anime->length = 0;
		anime->audioFormat = CSTR("UNK");
		anime->samplingRate = 0;
		anime->bitRate = 0;
	}
	return anime;
}

void SSWR::DiscDB::DiscDBBurntDiscForm::MovieColsFree(MovieCols *anime)
{
	anime->type->Release();
	Text::StrDelNew(anime->mainTitle);
	SDEL_TEXT(anime->chapter);
	SDEL_TEXT(anime->chapterTitle);
	SDEL_TEXT(anime->remark);
	SDEL_TEXT(anime->aspectRatio);

	MemFree(anime);
}


void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnBrowseClicked(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	NN<UI::GUIFileDialog> ofd = me->ui->NewFileDialog(L"SSWR", L"DVDDB", L"Browse", true);
	if (ofd->ShowDialog(me->GetHandle()))
	{
		UOSInt i = ofd->GetFileName()->LastIndexOf(IO::Path::PATH_SEPERATOR);
		Text::StringBuilderUTF8 sbBasePath;
		if (i != INVALID_INDEX)
		{
			sbBasePath.AppendC(ofd->GetFileName()->v, (UOSInt)i);
		}
		me->lbFileName->ClearItems();
		me->fileList.FreeAll(BurntFileFree);
		me->selectedFile = nullptr;
		me->pnlFile->SetEnabled(false);
		me->SearchSubDir(sbBasePath.ToString(), (const UTF8Char*)"", 50000000000LL);
		i = Text::StrLastIndexOfCharC(sbBasePath.ToString(), sbBasePath.GetLength(), IO::Path::PATH_SEPERATOR);
		me->txtDiscId->SetText(sbBasePath.ToCString().Substring(i + 1));
	}
	ofd.Delete();
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnFileNameSelChg(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	me->pnlFile->SetEnabled(true);
	NN<BurntFile> selectedFile;
	if (me->selectedFile.SetTo(selectedFile))
	{
		if (!me->UpdateFileInfo(selectedFile))
		{
			me->selectedFile = nullptr;
			me->lbFileName->SetSelectedIndex(me->fileList.IndexOf(selectedFile));
			return;
		}
	}

	me->selectedFile = me->lbFileName->GetSelectedItem().GetOpt<BurntFile>();
	if (!me->selectedFile.SetTo(selectedFile))
	{
		me->pnlFile->SetEnabled(false);
		return;
	}
	Data::ArrayListNN<SSWR::DiscDB::DiscDBEnv::CategoryInfo> cateList;
	UOSInt i;
	me->env->GetCategories(cateList);
	i = cateList.GetCount();
	while (i-- > 0)
	{
		if (selectedFile->cate.Equals(cateList.GetItemNoCheck(i)->id))
		{
			me->cboCategory->SetSelectedIndex(i);
			break;
		}
	}

	if (selectedFile->video && selectedFile->videoId == 0)
	{
		UOSInt animeLen = 0;
		UOSInt seriesLen = 0;
		UOSInt len;
		NN<Text::String> anime = Text::String::NewEmpty();
		NN<Text::String> series = Text::String::NewEmpty();
		UOSInt prefix = selectedFile->fname->IndexOf(UTF8STRC(".part"));
		UOSInt i;
		UOSInt j;

		if (prefix != INVALID_INDEX)
		{
			Text::StringBuilderUTF8 sbBegText;
			NN<BurntFile> bFile;
			sbBegText.AppendC(selectedFile->fname->v, prefix);

			i = 0;
			j = me->fileList.GetCount();
			while (i < j)
			{
				bFile = me->fileList.GetItemNoCheck(i);
				if (bFile->videoId != 0 && bFile->fname->StartsWith(sbBegText))
				{
					selectedFile->videoId = bFile->videoId;
					me->SetVideoField(bFile->videoId);
					return;
				}
				i++;
			}
		}

		if (selectedFile->videoId == 0)
		{
			Data::ArrayListNN<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideoList;
			NN<SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideo;
			NN<Text::String> s;
			me->env->GetDVDVideos(dvdVideoList);
			i = 0;
			j = dvdVideoList.GetCount();
			while (i < j)
			{
				dvdVideo = dvdVideoList.GetItemNoCheck(i);

				len = dvdVideo->anime->leng;
				if (animeLen < len)
				{
					if (selectedFile->fname->IndexOf(dvdVideo->anime->v, dvdVideo->anime->leng) != INVALID_INDEX)
					{
						anime = dvdVideo->anime;
						animeLen = len;
						series = Text::String::NewEmpty();
						seriesLen = 0;
					}
				}
				if (animeLen > 0)
				{
					if (dvdVideo->anime->Equals(anime))
					{
						if (dvdVideo->series.SetTo(s))
						{
							len = s->leng;
							if (seriesLen < len)
							{
								if (selectedFile->fname->IndexOf(s->ToCString()) != INVALID_INDEX)
								{
									series = s;
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
				me->cboDVDName->SetText(CSTR("Non-Anime"));
				me->cboSeries->SetText(CSTR(""));
				me->cboVolume->SetText(CSTR(""));
				me->cboDVDType->SetText(CSTR("ROM5"));
			}
			else
			{
				me->cboDVDName->SetText(anime->ToCString());
				OnCboDVDNameTextChg(me);
				if (seriesLen == 0)
				{
					me->cboSeries->SetText(CSTR("TV"));
					series = anime;
				}
				else
				{
					me->cboSeries->SetText(series->ToCString());
				}
				me->OnSeriesSelChg(me);
				Bool hasVol = false;
				NN<Text::String> fname = selectedFile->fname;
				i = fname->IndexOf(series) + seriesLen;
				OSInt lastR = 0;
				UOSInt endOfst = fname->leng;
				UOSInt i2;
				if ((i2 = fname->IndexOfICase(UTF8STRC(".PART"))) != INVALID_INDEX)
				{
					endOfst = i2;
				}
				while (i < endOfst)
				{
					if (fname->v[i] >= 0x30 && fname->v[i] <= 0x39 && lastR < 2)
					{
						j = i + 1;
						while (j < endOfst)
						{
							if (fname->v[j] < 0x30 || fname->v[j] > 0x39)
							{
								break;
							}
							j++;
						}
						Text::StringBuilderUTF8 sb;
						sb.AppendC(&fname->v[i], j - i);
						me->cboVolume->SetText(sb.ToCString());
						hasVol = true;
						break;
					}
					else if (fname->v[i] == 'r' || fname->v[i] == 'R')
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
					me->cboVolume->SetText(CSTR("1"));
				}
				me->cboDVDType->SetText(CSTR("ROM5"));
				me->UpdateType();
			}
		}
	}
	else
	{
		NN<const SSWR::DiscDB::DiscDBEnv::DVDVideoInfo> dvdVideo;
		if (me->env->GetDVDVideo(selectedFile->videoId).SetTo(dvdVideo))
		{
			me->cboDVDName->SetText(dvdVideo->anime->ToCString());
			OnCboDVDNameTextChg(me);
			me->cboSeries->SetText(Text::String::OrEmpty(dvdVideo->series)->ToCString());
			me->OnSeriesSelChg(me);

			me->cboVolume->SetText(Text::String::OrEmpty(dvdVideo->volume)->ToCString());
			me->cboDVDType->SetText(dvdVideo->dvdType->ToCString());
		}
	}
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnBrandSelChg(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	Data::ArrayListNN<const SSWR::DiscDB::DiscDBEnv::DiscTypeInfo> discList;
	NN<const SSWR::DiscDB::DiscDBEnv::DiscTypeInfo> discType;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	NN<Text::String> s;
	if (me->lbBrand->GetSelectedItemTextNew().SetTo(s))
	{
		me->env->GetDiscTypesByBrand(discList, s->v, s->leng);
		s->Release();
	}
	me->lbDVDName->ClearItems();
	i = 0;
	j = discList.GetCount();
	while (i < j)
	{
		discType = discList.GetItemNoCheck(i);
		sptr = Text::StrConcatC(sbuff, UTF8STRC("MID: "));
		sptr = Text::StrConcat(sptr, discType->mid.Or(U8STR("")));
		sptr = Text::StrConcatC(sptr, UTF8STRC(", "));
		sptr = Text::StrConcat(sptr, discType->name);
		me->lbDVDName->AddItem(CSTRP(sbuff, sptr), NN<SSWR::DiscDB::DiscDBEnv::DiscTypeInfo>::ConvertFrom(discType));

		i++;
	}
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnDVDNameSelChg(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	NN<const SSWR::DiscDB::DiscDBEnv::DiscTypeInfo> discType = me->lbDVDName->GetSelectedItem().GetNN<const SSWR::DiscDB::DiscDBEnv::DiscTypeInfo>();
	me->txtDiscType->SetText(discType->discTypeId->ToCString());
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnCboDVDNameSelChg(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	me->UpdateSeries();
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnCboDVDNameTextChg(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	me->UpdateSeries();
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnSeriesSelChg(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	me->UpdateVolume();
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnFinishClicked(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	NN<BurntFile> selectedFile;
	if (me->selectedFile.SetTo(selectedFile))
	{
		if (!me->UpdateFileInfo(selectedFile))
		{
			return;
		}
	}
	if (me->fileList.GetCount() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please browse file first"), CSTR("Burnt Disc"), me);
		return;
	}
	Data::Timestamp theDate;
	Text::StringBuilderUTF8 sbDate;
	Text::StringBuilderUTF8 sbDiscId;
	Text::StringBuilderUTF8 sbDVDId;
	me->txtDate->GetText(sbDate);
	theDate = Data::Timestamp::FromStr(sbDate.ToCString(), 0);
	if (theDate.IsNull())
	{
		me->ui->ShowMsgOK(CSTR("Error in parsing the date"), CSTR("Burnt Disc"), me);
		me->txtDate->Focus();
		return;
	}
	me->txtDiscType->GetText(sbDiscId);
	if (me->env->GetDiscType(sbDiscId.ToCString()).IsNull())
	{
		me->ui->ShowMsgOK(CSTR("Error in the Disc Type"), CSTR("Burnt Disc"), me);
		me->txtDiscType->Focus();
		return;
	}
	me->txtDiscId->GetText(sbDVDId);
	if (sbDVDId.GetLength() < 3)
	{
		me->ui->ShowMsgOK(CSTR("Invalid Disc Id"), CSTR("Burnt Disc"), me);
		me->txtDiscId->Focus();
		return;
	}
	if (me->env->GetBurntDisc(sbDVDId.ToCString()).NotNull())
	{
		me->ui->ShowMsgOK(CSTR("Disc Id already found in database"), CSTR("Burnt Disc"), me);
		me->txtDiscId->Focus();
		return;
	}
	NN<const SSWR::DiscDB::DiscDBEnv::BurntDiscInfo> disc;
	if (me->env->NewBurntDisc(sbDVDId.ToCString(), sbDiscId.ToCString(), theDate).SetTo(disc))
	{
		NN<BurntFile> file;
		UOSInt i;
		UOSInt j;
		i = 0;
		j = me->fileList.GetCount();
		while (i < j)
		{
			file = me->fileList.GetItemNoCheck(i);
			me->env->NewBurntFile(disc->discId->v, i, file->fname->v, file->fSize, file->cate, file->videoId);
			if (file->anime)
			{
				me->env->NewMovies(disc->discId->v, i, file->anime->mainTitle, file->anime->type, file->anime->chapter, file->anime->chapterTitle, file->anime->videoFormat, file->anime->width, file->anime->height, file->anime->fps, file->anime->length, file->anime->audioFormat, file->anime->samplingRate, file->anime->bitRate, file->anime->aspectRatio, file->anime->remark);
			}
			i++;
		}

		NN<IO::Registry> reg;
		if (IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"DVDDB").SetTo(reg))
		{
			UnsafeArray<const WChar> wptr = Text::StrToWCharNew(sbDiscId.ToString());
			reg->SetValue(L"DiscType", wptr);
			Text::StrDelNew(wptr);
			IO::Registry::CloseRegistry(reg);
		}
		me->Close();
	}
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnVolumeSelChg(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	me->UpdateType();
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnRemoveFileClicked(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	UOSInt i = me->lbFileName->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->selectedFile = nullptr;
		me->lbFileName->RemoveItem(i);
		me->BurntFileFree(me->fileList.GetItemNoCheck(i));
		me->fileList.RemoveAt(i);
	}
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnCategorySelChg(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	NN<SSWR::DiscDB::DiscDBEnv::CategoryInfo> cate;
	if (me->cboCategory->GetSelectedItem().GetOpt<SSWR::DiscDB::DiscDBEnv::CategoryInfo>().SetTo(cate))
	{
		if (cate->id->Equals(UTF8STRC("ISO")))
		{
			me->pnlDVDV->SetEnabled(true);
		}
		else if (cate->id->Equals(UTF8STRC("AC")))
		{
			BurntFile *file = (BurntFile*)me->lbFileName->GetSelectedItem().p;
			if (file && file->fname->IndexOfICase(UTF8STRC("DVD")) != INVALID_INDEX)
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

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnDiscIdTextChg(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	Text::StringBuilderUTF8 sb;
	me->txtDiscId->GetText(sb);
    if (sb.GetLength() <= 0)
	{
        me->lbDiscId->SetSelectedIndex(0);
        return;
	}
	sb.ToUpper();
	OSInt i = me->env->GetBurntDiscIndex(sb.ToCString());
	if (i < 0)
	{
		i = ~i;
	}
    me->lbDiscId->SetSelectedIndex((UOSInt)i);
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnSectorSizeClicked(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	UOSInt i;
	UOSInt j;
	UInt64 size;
	UInt64 baseSize;
	Text::StringBuilderUTF8 sb;
	me->txtSectorSize->GetText(sb);
	if (sb.GetLength() > 6)
	{
		me->ui->ShowMsgOK(CSTR("Sector Size too long"), CSTR("Burnt Disc"), me);
		return;
	}
	i = Text::StrHex2UInt32C(sb.ToString());
	size = i * (UInt64)2048;
	baseSize = 0;
	i = 0;
	j = me->fileList.GetCount();
	while (i < j)
	{
		baseSize += me->fileList.GetItemNoCheck(i)->fSize;
		if (size < baseSize)
		{
			while (j > i)
			{
				j--;
				me->lbFileName->RemoveItem(j);
				me->BurntFileFree(me->fileList.GetItemNoCheck(j));
				me->fileList.RemoveAt(j);
			}
			break;
		}
	}
}

void __stdcall SSWR::DiscDB::DiscDBBurntDiscForm::OnAllFileClicked(AnyType userObj)
{
	NN<SSWR::DiscDB::DiscDBBurntDiscForm> me = userObj.GetNN<SSWR::DiscDB::DiscDBBurntDiscForm>();
	NN<BurntFile> selectedFile;
	if (me->selectedFile.SetTo(selectedFile))
	{
		if (me->UpdateFileInfo(selectedFile))
		{
			NN<BurntFile> file;
			UOSInt i;
			UOSInt j;
			i = 0;
			j = me->fileList.GetCount();
			while (i < j)
			{
				file = me->fileList.GetItemNoCheck(i);
				if (file != selectedFile)
				{
					file->cate = selectedFile->cate;
					me->BurntFileUpdateVideo(file);
					file->videoId = selectedFile->videoId;
				}
				i++;
			}
		}
	}
}

SSWR::DiscDB::DiscDBBurntDiscForm::DiscDBBurntDiscForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, SSWR::DiscDB::DiscDBEnv *env) : UI::GUIForm(parent, 632, 483, ui)
{
	this->SetText(CSTR("Burnt Disc"));
	this->SetFont(nullptr, 8.25, false);
	this->env = env;
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
	this->selectedFile = nullptr;

	this->pnlTop = ui->NewPanel(*this);
	this->pnlTop->SetRect(0, 0, 624, 121, false);
	this->pnlTop->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlBurntDisc = ui->NewPanel(this->pnlTop);
	this->pnlBurntDisc->SetRect(0, 0, 472, 121, false);
	this->pnlBurntDisc->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->btnBrowse = ui->NewButton(this->pnlBurntDisc, CSTR("&Browse"));
	this->btnBrowse->SetRect(8, 8, 83, 25, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	this->btnFinish = ui->NewButton(this->pnlBurntDisc, CSTR("&Finsih"));
	this->btnFinish->SetRect(8, 43, 83, 25, false);
	this->btnFinish->HandleButtonClick(OnFinishClicked, this);
	this->btnNewDisc = ui->NewButton(this->pnlBurntDisc, CSTR("&New Disc Type"));
	this->btnNewDisc->SetRect(8, 78, 83, 25, false);
	this->lblDiscId = ui->NewLabel(this->pnlBurntDisc, CSTR("Disc ID"));
	this->lblDiscId->SetRect(96, 9, 56, 25, false);
	this->txtDiscId = ui->NewTextBox(this->pnlBurntDisc, CSTR(""));
	this->txtDiscId->SetRect(160, 9, 96, 20, false);
	this->txtDiscId->HandleTextChanged(OnDiscIdTextChg, this);
	this->lblDiscType = ui->NewLabel(this->pnlBurntDisc, CSTR("Disc Type"));
	this->lblDiscType->SetRect(96, 43, 64, 25, false);
	this->txtDiscType = ui->NewTextBox(this->pnlBurntDisc, CSTR(""));
	this->txtDiscType->SetRect(160, 43, 96, 20, false);
	this->lblDate = ui->NewLabel(this->pnlBurntDisc, CSTR("Date"));
	this->lblDate->SetRect(96, 78, 56, 25, false);
	this->txtDate = ui->NewTextBox(this->pnlBurntDisc, CSTR(""));
	this->txtDate->SetRect(160, 78, 96, 20, false);
	this->lbDiscId = ui->NewListBox(this->pnlBurntDisc, false);
	this->lbDiscId->SetRect(256, 0, 80, 121, false);
	this->lbBrand = ui->NewListBox(this->pnlBurntDisc, false);
	this->lbBrand->SetRect(336, 0, 136, 121, false);
	this->lbBrand->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->lbBrand->HandleSelectionChange(OnBrandSelChg, this);
	this->lbDVDName = ui->NewListBox(this->pnlTop, false);
	this->lbDVDName->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbDVDName->HandleSelectionChange(OnDVDNameSelChg, this);

	this->pnlFile = ui->NewPanel(*this);
	this->pnlFile->SetRect(416, 121, 208, 335, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->lblCategory = ui->NewLabel(this->pnlFile, CSTR("Category"));
	this->lblCategory->SetRect(8, 17, 72, 25, false);
	this->cboCategory = ui->NewComboBox(this->pnlFile, false);
	this->cboCategory->SetRect(80, 17, 121, 21, false);
	this->cboCategory->HandleSelectionChange(OnCategorySelChg, this);
	this->pnlDVDV = ui->NewPanel(this->pnlFile);
	this->pnlDVDV->SetRect(0, 43, 208, 165, false);
	this->lblDVDName = ui->NewLabel(this->pnlDVDV, CSTR("Name"));
	this->lblDVDName->SetRect(8, 17, 48, 25, false);
	this->cboDVDName = ui->NewComboBox(this->pnlDVDV, true);
	this->cboDVDName->SetText(CSTR("Non-Anime"));
	this->cboDVDName->SetRect(64, 17, 121, 21, false);
	this->cboDVDName->HandleSelectionChange(OnCboDVDNameSelChg, this);
//	this->cboDVDName->HandleTextChange(OnCboDVDNameTextChg, this);
	this->lblSeries = ui->NewLabel(this->pnlDVDV, CSTR("Series"));
	this->lblSeries->SetRect(8, 52, 48, 25, false);
	this->cboSeries = ui->NewComboBox(this->pnlDVDV, true);
	this->cboSeries->SetRect(64, 52, 121, 21, false);
	this->cboSeries->HandleSelectionChange(OnSeriesSelChg, this);
	this->lblVolume = ui->NewLabel(this->pnlDVDV, CSTR("Volume"));
	this->lblVolume->SetRect(8, 87, 48, 25, false);
	this->cboVolume = ui->NewComboBox(this->pnlDVDV, true);
	this->cboVolume->SetRect(64, 87, 121, 21, false);
	this->cboVolume->HandleSelectionChange(OnVolumeSelChg, this);
	this->lblDVDType = ui->NewLabel(this->pnlDVDV, CSTR("Type"));
	this->lblDVDType->SetRect(8, 121, 48, 25, false);
	this->cboDVDType = ui->NewComboBox(this->pnlDVDV, false);
	this->cboDVDType->SetRect(64, 121, 121, 21, false);
	this->cboDVDType->AddItem(CSTR("ROM5"), 0);
	this->cboDVDType->AddItem(CSTR("ROM9"), 0);
	this->btnAllFile = ui->NewButton(this->pnlFile, CSTR("All File Same Category"));
	this->btnAllFile->SetRect(24, 217, 136, 25, false);
	this->btnAllFile->HandleButtonClick(OnAllFileClicked, this);
	this->btnRemoveFile = ui->NewButton(this->pnlFile, CSTR("&Remove File"));
	this->btnRemoveFile->SetRect(24, 251, 88, 25, false);
	this->btnRemoveFile->HandleButtonClick(OnRemoveFileClicked, this);
	this->btnBuildMovie = ui->NewButton(this->pnlFile, CSTR("Build Movie"));
	this->btnBuildMovie->SetRect(128, 251, 75, 25, false);
	this->lblSectorSize = ui->NewLabel(this->pnlFile, CSTR("Sector Size"));
	this->lblSectorSize->SetRect(16, 277, 72, 25, false);
	this->txtSectorSize = ui->NewTextBox(this->pnlFile, CSTR("231000"));
	this->txtSectorSize->SetRect(96, 277, 100, 20, false);
	this->btnSectorSize = ui->NewButton(this->pnlFile, CSTR("Apply"));
	this->btnSectorSize->SetRect(120, 303, 75, 25, false);
	this->btnSectorSize->HandleButtonClick(OnSectorSizeClicked, this);
	this->lbFileName = ui->NewListBox(*this, false);
	this->lbFileName->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbFileName->HandleSelectionChange(OnFileNameSelChg, this);

	this->pnlFile->SetEnabled(false);

	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	Data::DateTime dt;
	dt.SetCurrTime();
	sptr = dt.ToString(sbuff, "yyyy-MM-dd");
    this->txtDate->SetText(CSTRP(sbuff, sptr));

	Data::ArrayListNN<SSWR::DiscDB::DiscDBEnv::CategoryInfo> cateList;
	NN<SSWR::DiscDB::DiscDBEnv::CategoryInfo> cate;
	this->env->GetCategories(cateList);
	i = 0;
	j = cateList.GetCount();
	while (i < j)
	{
		cate = cateList.GetItemNoCheck(i);
		this->cboCategory->AddItem(cate->name, cate);
		i++;
	}
	this->UpdateDiscId();
	this->UpdateBrand();
	this->UpdateAnimeName();

	NN<IO::Registry> reg;
	if (IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"DVDDB").SetTo(reg))
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		WChar wbuff[64];
		if (reg->GetValueStr(L"DiscType", wbuff).NotNull())
		{
			
			sptr = Text::StrWChar_UTF8(sbuff, wbuff);
			this->txtDiscType->SetText(CSTRP(sbuff, sptr));
			NN<const SSWR::DiscDB::DiscDBEnv::DiscTypeInfo> discType;
			if (this->env->GetDiscType(CSTRP(sbuff, sptr)).SetTo(discType))
			{
				i = this->lbBrand->GetCount();
				while (i-- > 0)
				{
					if (this->lbBrand->GetItemText(sbuff, i).SetTo(sptr))
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
	this->fileList.FreeAll(BurntFileFree);
}

void SSWR::DiscDB::DiscDBBurntDiscForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}
