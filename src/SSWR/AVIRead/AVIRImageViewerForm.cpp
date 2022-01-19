#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "SSWR/AVIRead/AVIRColorSettingForm.h"
#include "SSWR/AVIRead/AVIRImageViewerForm.h"
#include "SSWR/AVIRead/AVIRStringMsgForm.h"
#include "Text/MyString.h"

typedef enum
{
	MNU_IMAGE_SAVE = 101,
	MNU_IMAGE_ENLARGE,
	MNU_IMAGE_NEXT,
	MNU_IMAGE_PREV,
	MNU_MON_COLOR,
	MNU_SWITCH_FS,
	MNU_MOUSE_CURSOR,
	MNU_IMAGE_INFO
} MenuEvent;

void __stdcall SSWR::AVIRead::AVIRImageViewerForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt fileCnt)
{
	SSWR::AVIRead::AVIRImageViewerForm *me = (SSWR::AVIRead::AVIRImageViewerForm *)userObj;
	IO::StmData::FileData *fd;
	UOSInt i;
	Bool succ;
	i = 0;
	while (i < fileCnt)
	{
		NEW_CLASS(fd, IO::StmData::FileData(files[i], false));
		succ = me->ParseFile(fd);
		DEL_CLASS(fd);
		if (succ)
			break;
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRImageViewerForm::OnMoveToNext(void *userObj)
{
	SSWR::AVIRead::AVIRImageViewerForm *me = (SSWR::AVIRead::AVIRImageViewerForm *)userObj;
	if (me->pkgFile && me->fileIndex != (UOSInt)-1)
	{
		IO::IStreamData *fd;
		UTF8Char sbuff[512];
		UOSInt i;
		UOSInt j;
		IO::ParsedObject *pobj;
		Bool found = false;
		i = me->fileIndex + 1;
		j = me->pkgFile->GetCount();
		while (i < j)
		{
			if (me->pkgFile->GetItemType(i) == IO::PackageFile::POT_STREAMDATA)
			{
				me->pkgFile->GetItemName(sbuff, i);
				if (IsImageFileName(sbuff))
				{
					fd = me->pkgFile->GetItemStmData(i);
					if (fd)
					{
						pobj = me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
						DEL_CLASS(fd);
						if (pobj)
						{
							me->SetImage((Media::ImageList*)pobj, true);
							me->fileIndex = i;
							found = true;
							break;
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
			if (me->pkgFile->GetItemType(i) == IO::PackageFile::POT_STREAMDATA)
			{
				me->pkgFile->GetItemName(sbuff, i);
				if (IsImageFileName(sbuff))
				{
					fd = me->pkgFile->GetItemStmData(i);
					if (fd)
					{
						pobj = me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
						DEL_CLASS(fd);
						if (pobj)
						{
							me->SetImage((Media::ImageList*)pobj, true);
							me->fileIndex = i;
							found = true;
							break;
						}
					}
				}
			}
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRImageViewerForm::OnMoveToPrev(void *userObj)
{
	SSWR::AVIRead::AVIRImageViewerForm *me = (SSWR::AVIRead::AVIRImageViewerForm *)userObj;
	if (me->pkgFile && me->fileIndex != (UOSInt)-1)
	{
		IO::IStreamData *fd;
		UTF8Char sbuff[512];
		UOSInt i;
		IO::ParsedObject *pobj;
		Bool found = false;
		i = me->fileIndex;
		while (i-- > 0)
		{
			if (me->pkgFile->GetItemType(i) == IO::PackageFile::POT_STREAMDATA)
			{
				me->pkgFile->GetItemName(sbuff, i);
				if (IsImageFileName(sbuff))
				{
					fd = me->pkgFile->GetItemStmData(i);
					if (fd)
					{
						pobj = me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
						DEL_CLASS(fd);
						if (pobj)
						{
							me->SetImage((Media::ImageList*)pobj, true);
							me->fileIndex = i;
							found = true;
							break;
						}
					}
				}
			}
		}
		if (found)
		{
			return;
		}
		i = me->pkgFile->GetCount();
		while (i-- > me->fileIndex + 1)
		{
			if (me->pkgFile->GetItemType(i) == IO::PackageFile::POT_STREAMDATA)
			{
				me->pkgFile->GetItemName(sbuff, i);
				if (IsImageFileName(sbuff))
				{
					fd = me->pkgFile->GetItemStmData(i);
					if (fd)
					{
						pobj = me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
						DEL_CLASS(fd);
						if (pobj)
						{
							me->SetImage((Media::ImageList*)pobj, true);
							me->fileIndex = i;
							found = true;
							break;
						}
					}
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRImageViewerForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRImageViewerForm *me = (SSWR::AVIRead::AVIRImageViewerForm *)userObj;
	if (me->noMoveCount++ > 4 && !me->hideCursor)
	{
		me->hideCursor = true;
		me->ShowMouseCursor(false);
	}
}

void __stdcall SSWR::AVIRead::AVIRImageViewerForm::OnAniTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRImageViewerForm *me = (SSWR::AVIRead::AVIRImageViewerForm *)userObj;
	if (me->imgTimeoutTick != 0 && me->imgList != 0)
	{
		Data::DateTime dt;
		Int64 currTimeTick;
		UInt32 imgDurMS;
		dt.SetCurrTimeUTC();
		currTimeTick = dt.ToTicks();
		if (currTimeTick >= me->imgTimeoutTick)
		{
			me->imgIndex++;
			if (me->imgIndex >= me->imgList->GetCount())
			{
				me->imgIndex = 0;
			}
			Media::StaticImage *simg = (Media::StaticImage*)me->imgList->GetImage(me->imgIndex, &imgDurMS);
			me->imgTimeoutTick = currTimeTick + imgDurMS;
			me->pbImage->SetImage(simg, true);
		}
	}
}

Bool __stdcall SSWR::AVIRead::AVIRImageViewerForm::OnMouseMove(void *userObj, OSInt scnX, OSInt scnY, MouseButton btn)
{
	SSWR::AVIRead::AVIRImageViewerForm *me = (SSWR::AVIRead::AVIRImageViewerForm *)userObj;
	if (me->hideCursor)
	{
		me->ShowMouseCursor(true);
		me->hideCursor = false;
	}
	me->noMoveCount = 0;
	return false;
}

Bool SSWR::AVIRead::AVIRImageViewerForm::IsImageFileName(const UTF8Char *fileName)
{
	UOSInt fileNameLen = Text::StrCharCnt(fileName);
	if (Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".jpg"))) return true;
	if (Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".png"))) return true;
	if (Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".tif"))) return true;
	if (Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".bmp"))) return true;
	if (Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".gif"))) return true;
	if (Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".ico"))) return true;
	if (Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".pcx"))) return true;
	return false;
}

SSWR::AVIRead::AVIRImageViewerForm::AVIRImageViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"Image Viewer");

	this->imgTimeoutTick = 0;
	this->core = core;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->imgList = 0;
	this->pkgFile = 0;
	this->allowEnlarge = false;
	this->hideCursor = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	UI::GUIMenu *mnu;
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Setting");
	mnu->AddItem((const UTF8Char*)"Set &Monitor Color", MNU_MON_COLOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Image");
	mnu->AddItem((const UTF8Char*)"&Save", MNU_IMAGE_SAVE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu->AddItem((const UTF8Char*)"&Allow Enlarge", MNU_IMAGE_ENLARGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Info", MNU_IMAGE_INFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Present");
	mnu->AddItem((const UTF8Char*)"&Next Image", MNU_IMAGE_NEXT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_PAGEDOWN);
	mnu->AddItem((const UTF8Char*)"&Prev Image", MNU_IMAGE_PREV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_PAGEUP);
	mnu->AddItem((const UTF8Char*)"Switch &Fullscreen", MNU_SWITCH_FS, UI::GUIMenu::KM_SHIFT, UI::GUIControl::GK_F5);
	mnu->AddItem((const UTF8Char*)"Hide Mouse Cursor", MNU_MOUSE_CURSOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_OEM_PERIOD);
	this->SetMenu(this->mnuMain);

	NEW_CLASS(this->pbImage, UI::GUIPictureBoxDD(ui, this, this->colorSess, this->allowEnlarge, false));
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
	SDEL_CLASS(this->imgList);
	SDEL_CLASS(this->pkgFile);
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRImageViewerForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_IMAGE_SAVE:
		this->core->SaveData(this, this->imgList, L"SaveImage");
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
			SSWR::AVIRead::AVIRColorSettingForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRColorSettingForm(0, this->ui, this->core, this->GetHMonitor()));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
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
		if (this->imgList) 
		{
			SSWR::AVIRead::AVIRStringMsgForm *frm;
			Text::StringBuilderUTF8 sbImg;
			Text::StringBuilderUTF8 sbTitle;
			this->imgList->ToString(&sbImg);
			sbTitle.AppendC(UTF8STRC("Image info for "));
			sbTitle.Append(this->imgList->GetSourceNameObj());
			NEW_CLASS(frm, SSWR::AVIRead::AVIRStringMsgForm(0, this->ui, this->core, sbTitle.ToString(), sbImg.ToString()));
			frm->ShowDialog(this);
			DEL_CLASS(frm);
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

void SSWR::AVIRead::AVIRImageViewerForm::SetImage(Media::ImageList *imgList, Bool sameDir)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UOSInt i;
	UOSInt j;
	this->pbImage->SetImage(0, false);
	SDEL_CLASS(this->imgList);
	if (!sameDir)
	{
		SDEL_CLASS(this->pkgFile);
	}
	this->imgList = imgList;
	if (this->imgList)
	{
		UInt32 imgDurMS;
		this->imgIndex = 0;
		this->imgList->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Image Viewer - ")));
		this->SetText(sbuff);
		this->pbImage->SetImage(this->imgList->GetImage(0, &imgDurMS), false);
		if (imgDurMS != 0 && this->imgList->GetCount() > 1)
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
			this->imgList->GetSourceName(sbuff);
			i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
			if (i != INVALID_INDEX)
			{
				sbuff[i] = 0;
				NEW_CLASS(this->pkgFile, IO::DirectoryPackage(sbuff));
				((IO::DirectoryPackage*)this->pkgFile)->Sort();
				j = this->pkgFile->GetCount();
				while (j-- > 0)
				{
					this->pkgFile->GetItemName(sbuff2, j);
					if (Text::StrEquals(&sbuff[i + 1], sbuff2))
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
		this->SetText((const UTF8Char*)"Image Viewer");
	}
}

Bool SSWR::AVIRead::AVIRImageViewerForm::ParseFile(IO::IStreamData *fd)
{
	IO::ParsedObject *pobj;
	IO::ParserType pt;
	IO::IStreamData *fd2;
	UTF8Char sbuff[512];

	pobj = this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
	if (pobj)
	{
		this->SetImage((Media::ImageList*)pobj, false);
		return true;
	}

	pobj = this->core->GetParserList()->ParseFileType(fd, IO::ParserType::PackageFile);
	if (pobj)
	{
		IO::PackageFile *pf;
		UOSInt i;
		UOSInt j;
		Bool found = false;
		IO::ParsedObject *pobj2;

		pf = (IO::PackageFile *)pobj;
		i = 0;
		j = pf->GetCount();
		while (i < j)
		{
			if (pf->GetItemType(i) == IO::PackageFile::POT_STREAMDATA)
			{
				pf->GetItemName(sbuff, i);
				if (IsImageFileName(sbuff))
				{
					fd2 = pf->GetItemStmData(i);
					if (fd2)
					{
						pobj2 = this->core->GetParserList()->ParseFile(fd2, &pt);
						DEL_CLASS(fd2);
						if (pobj2)
						{
							if (pt == IO::ParserType::ImageList)
							{
								found = true;
								SDEL_CLASS(this->pkgFile);
								this->pkgFile = pf;
								this->fileIndex = i;
								this->SetImage((Media::ImageList*)pobj2, true);
								break;
							}
							else
							{
								DEL_CLASS(pobj2);
							}
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
			DEL_CLASS(pobj);
			return false;
		}
	}
	return false;
}
