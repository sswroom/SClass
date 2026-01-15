#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "SSWR/AVIRead/AVIRColorSettingForm.h"
#include "SSWR/AVIRead/AVIRFileRenameForm.h"
#include "SSWR/AVIRead/AVIRImageViewerForm.h"
#include "SSWR/AVIRead/AVIRStringMsgForm.h"
#include "Text/MyString.h"

typedef enum
{
	MNU_IMAGE_SAVE = 101,
	MNU_IMAGE_RENAME,
	MNU_IMAGE_ENLARGE,
	MNU_IMAGE_NEXT,
	MNU_IMAGE_PREV,
	MNU_MON_COLOR,
	MNU_SWITCH_FS,
	MNU_MOUSE_CURSOR,
	MNU_IMAGE_INFO
} MenuEvent;

void __stdcall SSWR::AVIRead::AVIRImageViewerForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRImageViewerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageViewerForm>();
	UIntOS i;
	UIntOS fileCnt = files.GetCount();
	Bool succ;
	i = 0;
	while (i < fileCnt)
	{
		IO::StmData::FileData fd(files[i], false);
		succ = me->ParseFile(fd);
		if (succ)
			break;
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRImageViewerForm::OnMoveToNext(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageViewerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageViewerForm>();
	NN<IO::PackageFile> pkgFile;
	if (me->pkgFile.SetTo(pkgFile) && me->fileIndex != (UIntOS)-1)
	{
		NN<IO::StreamData> fd;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UIntOS i;
		UIntOS j;
		NN<IO::ParsedObject> pobj;
		Bool found = false;
		i = me->fileIndex + 1;
		j = pkgFile->GetCount();
		while (i < j)
		{
			if (pkgFile->GetItemType(i) == IO::PackageFile::PackObjectType::StreamData)
			{
				if (pkgFile->GetItemName(sbuff, i).SetTo(sptr) && IsImageFileName(CSTRP(sbuff, sptr)))
				{
					if (pkgFile->GetItemStmDataNew(i).SetTo(fd))
					{
						if (me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList).SetTo(pobj))
						{
							fd.Delete();
							me->SetImage(NN<Media::ImageList>::ConvertFrom(pobj), true);
							me->fileIndex = i;
							found = true;
							break;
						}
						else
						{
							fd.Delete();
						}
					}
				}
			}
			i++;
		}
		if (found)
		{
			return;
		}
		i = 0;
		while (i < me->fileIndex)
		{
			if (pkgFile->GetItemType(i) == IO::PackageFile::PackObjectType::StreamData)
			{
				if (pkgFile->GetItemName(sbuff, i).SetTo(sptr) && IsImageFileName(CSTRP(sbuff, sptr)))
				{
					if (pkgFile->GetItemStmDataNew(i).SetTo(fd))
					{
						if (me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList).SetTo(pobj))
						{
							fd.Delete();
							me->SetImage(NN<Media::ImageList>::ConvertFrom(pobj), true);
							me->fileIndex = i;
							found = true;
							break;
						}
						else
						{
							fd.Delete();
						}
					}
				}
			}
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRImageViewerForm::OnMoveToPrev(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageViewerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageViewerForm>();
	NN<IO::PackageFile> pkgFile;
	if (me->pkgFile.SetTo(pkgFile) && me->fileIndex != (UIntOS)-1)
	{
		NN<IO::StreamData> fd;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UIntOS i;
		NN<IO::ParsedObject> pobj;
		Bool found = false;
		i = me->fileIndex;
		while (i-- > 0)
		{
			if (pkgFile->GetItemType(i) == IO::PackageFile::PackObjectType::StreamData)
			{
				if (pkgFile->GetItemName(sbuff, i).SetTo(sptr) && IsImageFileName(CSTRP(sbuff, sptr)))
				{
					if (pkgFile->GetItemStmDataNew(i).SetTo(fd))
					{
						if (me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList).SetTo(pobj))
						{
							fd.Delete();
							me->SetImage(NN<Media::ImageList>::ConvertFrom(pobj), true);
							me->fileIndex = i;
							found = true;
							break;
						}
						else
						{
							fd.Delete();
						}
					}
				}
			}
		}
		if (found)
		{
			return;
		}
		i = pkgFile->GetCount();
		while (i-- > me->fileIndex + 1)
		{
			if (pkgFile->GetItemType(i) == IO::PackageFile::PackObjectType::StreamData)
			{
				if (pkgFile->GetItemName(sbuff, i).SetTo(sptr) && IsImageFileName(CSTRP(sbuff, sptr)))
				{
					if (pkgFile->GetItemStmDataNew(i).SetTo(fd))
					{
						if (me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList).SetTo(pobj))
						{
							fd.Delete();
							me->SetImage(NN<Media::ImageList>::ConvertFrom(pobj), true);
							me->fileIndex = i;
							found = true;
							break;
						}
						else
						{
							fd.Delete();
						}
					}
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRImageViewerForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageViewerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageViewerForm>();
	if (me->noMoveCount++ > 4 && !me->hideCursor)
	{
		me->hideCursor = true;
		me->ShowMouseCursor(false);
	}
}

void __stdcall SSWR::AVIRead::AVIRImageViewerForm::OnAniTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageViewerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageViewerForm>();
	NN<Media::ImageList> imgList;
	if (me->imgTimeoutTick != 0 && me->imgList.SetTo(imgList))
	{
		Data::DateTime dt;
		Int64 currTimeTick;
		UInt32 imgDurMS;
		dt.SetCurrTimeUTC();
		currTimeTick = dt.ToTicks();
		if (currTimeTick >= me->imgTimeoutTick)
		{
			me->imgIndex++;
			if (me->imgIndex >= imgList->GetCount())
			{
				me->imgIndex = 0;
			}
			Optional<Media::StaticImage> simg = Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(me->imgIndex, imgDurMS));
			me->imgTimeoutTick = currTimeTick + imgDurMS;
			me->pbImage->SetImage(simg, true);
		}
	}
}

UI::EventState __stdcall SSWR::AVIRead::AVIRImageViewerForm::OnMouseMove(AnyType userObj, Math::Coord2D<IntOS> scnPos, MouseButton btn)
{
	NN<SSWR::AVIRead::AVIRImageViewerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageViewerForm>();
	if (me->hideCursor)
	{
		me->ShowMouseCursor(true);
		me->hideCursor = false;
	}
	me->noMoveCount = 0;
	return UI::EventState::ContinueEvent;
}

Bool SSWR::AVIRead::AVIRImageViewerForm::IsImageFileName(Text::CStringNN fileName)
{
	if (fileName.EndsWithICase(UTF8STRC(".jpg"))) return true;
	if (fileName.EndsWithICase(UTF8STRC(".png"))) return true;
	if (fileName.EndsWithICase(UTF8STRC(".tif"))) return true;
	if (fileName.EndsWithICase(UTF8STRC(".bmp"))) return true;
	if (fileName.EndsWithICase(UTF8STRC(".gif"))) return true;
	if (fileName.EndsWithICase(UTF8STRC(".ico"))) return true;
	if (fileName.EndsWithICase(UTF8STRC(".pcx"))) return true;
	if (fileName.EndsWithICase(UTF8STRC(".heic"))) return true;
	if (fileName.EndsWithICase(UTF8STRC(".heif"))) return true;
	if (fileName.EndsWithICase(UTF8STRC(".avif"))) return true;
	if (fileName.EndsWithICase(UTF8STRC(".webp"))) return true;
	return false;
}

SSWR::AVIRead::AVIRImageViewerForm::AVIRImageViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Image Viewer"));

	this->imgTimeoutTick = 0;
	this->core = core;
	this->colorSess = this->core->GetColorManager()->CreateSess(this->GetHMonitor());
	this->imgList = nullptr;
	this->pkgFile = nullptr;
	this->allowEnlarge = false;
	this->hideCursor = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NN<UI::GUIMenu> mnu;
	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&Setting"));
	mnu->AddItem(CSTR("Set &Monitor Color"), MNU_MON_COLOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Image"));
	mnu->AddItem(CSTR("&Save"), MNU_IMAGE_SAVE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu->AddItem(CSTR("Rena&me"), MNU_IMAGE_RENAME, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_M);
	mnu->AddItem(CSTR("&Allow Enlarge"), MNU_IMAGE_ENLARGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Info"), MNU_IMAGE_INFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Present"));
	mnu->AddItem(CSTR("&Next Image"), MNU_IMAGE_NEXT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_PAGEDOWN);
	mnu->AddItem(CSTR("&Prev Image"), MNU_IMAGE_PREV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_PAGEUP);
	mnu->AddItem(CSTR("Switch &Fullscreen"), MNU_SWITCH_FS, UI::GUIMenu::KM_SHIFT, UI::GUIControl::GK_F5);
	mnu->AddItem(CSTR("Hide Mouse Cursor"), MNU_MOUSE_CURSOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_OEM_PERIOD);
	this->SetMenu(this->mnuMain);

	this->pbImage = ui->NewPictureBoxDD(*this, this->colorSess, this->allowEnlarge, false);
	this->pbImage->SetUserFSMode(UI::GUIDDrawControl::SM_VFS);
	this->pbImage->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbImage->HandleMoveToNext(OnMoveToNext, this);
	this->pbImage->HandleMoveToPrev(OnMoveToPrev, this);
	this->pbImage->HandleMouseMove(OnMouseMove, this);
	this->pbImage->EnableLRGBLimit(true);

	this->AddTimer(1000, OnTimerTick, this);
	this->pbImage->Focus();
	this->AddTimer(18, OnAniTimerTick, this);

	this->SetFormState(UI::GUIForm::FS_MAXIMIZED);
	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRImageViewerForm::~AVIRImageViewerForm()
{
	if (this->hideCursor)
	{
		this->ShowMouseCursor(true);
		this->hideCursor = false;
	}
	this->imgList.Delete();
	this->pkgFile.Delete();
	this->ClearChildren();
	this->core->GetDrawEngine()->EndColorSess(this->colorSess);
	this->core->GetColorManager()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRImageViewerForm::EventMenuClicked(UInt16 cmdId)
{
	NN<Media::ImageList> imgList;
	switch (cmdId)
	{
	case MNU_IMAGE_SAVE:
		if (this->imgList.SetTo(imgList))
		{
			this->core->SaveData(*this, imgList, L"SaveImage");
		}
		break;
	case MNU_IMAGE_RENAME:
		if (this->imgList.SetTo(imgList))
		{
			SSWR::AVIRead::AVIRFileRenameForm frm(nullptr, this->ui, this->core, imgList->GetSourceNameObj());
			if (frm.ShowDialog(this))
			{
				imgList->SetSourceName(frm.GetFileName());
				UTF8Char sbuff[512];
				UnsafeArray<UTF8Char> sptr;
				sptr = imgList->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Image Viewer - ")));
				this->SetText(CSTRP(sbuff, sptr));
			}
		}
		break;
	case MNU_IMAGE_ENLARGE:
		this->allowEnlarge = !this->allowEnlarge;
		this->pbImage->SetAllowEnlarge(this->allowEnlarge);
		break;
	case MNU_IMAGE_NEXT:
		OnMoveToNext(this);
		break;
	case MNU_IMAGE_PREV:
		OnMoveToPrev(this);
		break;
	case MNU_MON_COLOR:
		{
			SSWR::AVIRead::AVIRColorSettingForm frm(nullptr, this->ui, this->core, this->GetHMonitor());
			frm.ShowDialog(this);
		}
		break;
	case MNU_SWITCH_FS:
		{
			this->pbImage->SwitchFullScreen(!this->pbImage->IsFullScreen(), true);
		}
		break;
	case MNU_MOUSE_CURSOR:
		if (!this->hideCursor)
		{
			this->ShowMouseCursor(false);
			this->hideCursor = true;
		}
		break;
	case MNU_IMAGE_INFO:
		if (this->imgList.SetTo(imgList)) 
		{
			Text::StringBuilderUTF8 sbImg;
			Text::StringBuilderUTF8 sbTitle;
			imgList->ToString(sbImg);
			sbTitle.AppendC(UTF8STRC("Image info for "));
			sbTitle.Append(imgList->GetSourceNameObj());
			SSWR::AVIRead::AVIRStringMsgForm frm(nullptr, this->ui, this->core, sbTitle.ToCString(), sbImg.ToCString());
			frm.ShowDialog(this);
		}
		break;
	}
}

void SSWR::AVIRead::AVIRImageViewerForm::OnFocus()
{
	this->pbImage->Focus();
}

void SSWR::AVIRead::AVIRImageViewerForm::OnMonitorChanged()
{
	this->pbImage->ChangeMonitor(this->GetHMonitor());
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRImageViewerForm::SetImage(Optional<Media::ImageList> imgList, Bool sameDir)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr2;
	UIntOS i;
	UIntOS j;
	this->pbImage->SetImage(nullptr, false);
	this->imgList.Delete();
	if (!sameDir)
	{
		this->pkgFile.Delete();
	}
	this->imgList = imgList;
	NN<Media::ImageList> nnimgList;
	if (this->imgList.SetTo(nnimgList))
	{
		UInt32 imgDurMS;
		this->imgIndex = 0;
		sptr = nnimgList->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Image Viewer - ")));
		this->SetText(CSTRP(sbuff, sptr));
		this->pbImage->SetImage(nnimgList->GetImage(0, imgDurMS), false);
		if (imgDurMS != 0 && nnimgList->GetCount() > 1)
		{
			Data::DateTime dt;
			this->imgTimeoutTick = dt.ToTicks() + imgDurMS;
		}
		else
		{
			this->imgTimeoutTick = 0;
		}

		if (!sameDir)
		{
			this->fileIndex = INVALID_INDEX;
			sptr = nnimgList->GetSourceName(sbuff);
			i = Text::StrLastIndexOfCharC(sbuff, (UIntOS)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
			if (i != INVALID_INDEX)
			{
				sbuff[i] = 0;
				NN<IO::DirectoryPackage> pkgFile;
				NEW_CLASSNN(pkgFile, IO::DirectoryPackage({sbuff, i}));
				this->pkgFile = pkgFile;
				pkgFile->Sort();
				j = pkgFile->GetCount();
				while (j-- > 0)
				{
					if (pkgFile->GetItemName(sbuff2, j).SetTo(sptr2) && Text::StrEqualsC(&sbuff[i + 1], (UIntOS)(sptr - &sbuff[i + 1]), sbuff2, (UIntOS)(sptr2 - sbuff2)))
					{
						this->fileIndex = j;
						break;
					}
				}
			}
		}
	}
	else
	{
		this->SetText(CSTR("Image Viewer"));
	}
}

Bool SSWR::AVIRead::AVIRImageViewerForm::ParseFile(NN<IO::StreamData> fd)
{
	NN<IO::ParsedObject> pobj;
	NN<IO::StreamData> fd2;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;

	if (this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList).SetTo(pobj))
	{
		this->SetImage(NN<Media::ImageList>::ConvertFrom(pobj), false);
		return true;
	}

	if (this->core->GetParserList()->ParseFileType(fd, IO::ParserType::PackageFile).SetTo(pobj))
	{
		NN<IO::PackageFile> pf;
		UIntOS i;
		UIntOS j;
		Bool found = false;
		NN<IO::ParsedObject> pobj2;

		pf = NN<IO::PackageFile>::ConvertFrom(pobj);
		i = 0;
		j = pf->GetCount();
		while (i < j)
		{
			if (pf->GetItemType(i) == IO::PackageFile::PackObjectType::StreamData)
			{
				if (pf->GetItemName(sbuff, i).SetTo(sptr) && IsImageFileName(CSTRP(sbuff, sptr)))
				{
					if (pf->GetItemStmDataNew(i).SetTo(fd2))
					{
						if (this->core->GetParserList()->ParseFile(fd2).SetTo(pobj2))
						{
							fd2.Delete();
							if (pobj2->GetParserType() == IO::ParserType::ImageList)
							{
								found = true;
								this->pkgFile.Delete();
								this->pkgFile = pf;
								this->fileIndex = i;
								this->SetImage(NN<Media::ImageList>::ConvertFrom(pobj2), true);
								break;
							}
							else
							{
								pobj2.Delete();
							}
						}
						else
						{
							fd2.Delete();
						}
					}
				}
			}
			i++;
		}
		if (found)
		{
			return true;
		}
		else
		{
			pobj.Delete();
			return false;
		}
	}
	return false;
}
