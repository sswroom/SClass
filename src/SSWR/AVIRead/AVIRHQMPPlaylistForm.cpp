#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/IMediaPlayer.h"
#include "SSWR/AVIRead/AVIRHQMPPlaylistForm.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"
#include "UI/FolderDialog.h"

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRHQMPPlaylistForm *me = (SSWR::AVIRead::AVIRHQMPPlaylistForm*)userObj;
	Bool changed = false;
	UOSInt i;
	i = 0;
	while (i < nFiles)
	{
		if (me->playlist->AddFile(files[i]))
		{
			changed = true;
		}

		i++;
	}
	if (changed)
	{
		me->UpdatePlaylist();
	}
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnAddClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHQMPPlaylistForm *me = (SSWR::AVIRead::AVIRHQMPPlaylistForm*)userObj;
	UI::FileDialog *dlg;
	Bool changed = false;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"HQMPPlaylist", false));
	dlg->SetAllowMultiSel(true);
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParsedObject::PT_VIDEO_PARSER);
	if (dlg->ShowDialog(me->GetHandle()) == UI::GUIForm::DR_OK)
	{
		i = 0;
		j = dlg->GetFileNameCount();
		while (i < j)
		{
			if (me->playlist->AddFile(dlg->GetFileNames(i)))
			{
				changed = true;
			}

			i++;
		}
		if (changed)
		{
			me->UpdatePlaylist();
		}
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnAddDirClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHQMPPlaylistForm *me = (SSWR::AVIRead::AVIRHQMPPlaylistForm*)userObj;
	UI::FolderDialog *dlg;
	UTF8Char sbuff[512];
	NEW_CLASS(dlg, UI::FolderDialog(L"SSWR", L"AVIRead", L"HQMPPlaylistDir"));
	if (dlg->ShowDialog(me->GetHandle()) == UI::GUIForm::DR_OK)
	{
		const UTF8Char *folder = dlg->GetFolder();
		if (me->AddFolder(sbuff, Text::StrConcat(sbuff, folder)))
		{
			me->UpdatePlaylist();
		}
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnClearClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHQMPPlaylistForm *me = (SSWR::AVIRead::AVIRHQMPPlaylistForm*)userObj;
	me->playlist->ClearFiles();
	me->UpdatePlaylist();
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHQMPPlaylistForm *me = (SSWR::AVIRead::AVIRHQMPPlaylistForm*)userObj;
	if (me->playlist->GetCount() > 0)
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHQMPPlaylistForm *me = (SSWR::AVIRead::AVIRHQMPPlaylistForm*)userObj;
	SDEL_CLASS(me->playlist);
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

Bool SSWR::AVIRead::AVIRHQMPPlaylistForm::AddFolder(UTF8Char *folderBuff, UTF8Char *folderBuffEnd)
{
	if (folderBuffEnd[-1] != IO::Path::PATH_SEPERATOR)
	{
		*folderBuffEnd++ = IO::Path::PATH_SEPERATOR;
	}
	Bool changed = false;
	UTF8Char *sptr;
	IO::Path::PathType pt;
	Text::StrConcat(folderBuffEnd, IO::Path::ALL_FILES);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(folderBuff);
	if (sess)
	{
		while ((sptr = IO::Path::FindNextFile(folderBuffEnd, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PT_FILE)
			{
				Bool toSkip = false;
				if (Text::StrEndsWithICase(folderBuffEnd, (const UTF8Char*)".vob"))
				{
					if (sptr[-6] == '_' && sptr[-5] >= '2' && sptr[-5] <= '9')
					{
						toSkip = true;
					}
				}
				
				if (!toSkip)
				{
					if (this->playlist->AddFile(folderBuff))
					{
						changed = true;
					}
				}
			}
			else if (pt == IO::Path::PT_DIRECTORY)
			{
				if (folderBuffEnd[0] == '.' && folderBuffEnd[1] == 0)
				{
				}
				else if (folderBuffEnd[0] == '.' && folderBuffEnd[1] == '.' && folderBuffEnd[2] == 0)
				{
				}
				else
				{
					AddFolder(folderBuff, sptr);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
	return changed;
}

void SSWR::AVIRead::AVIRHQMPPlaylistForm::UpdatePlaylist()
{
	UOSInt i;
	UOSInt j;
	this->lbPlaylist->ClearItems();
	i = 0;
	j = this->playlist->GetCount();
	while (i < j)
	{
		this->lbPlaylist->AddItem(this->playlist->GetTitle(i), (void*)i);
		i++;
	}
}

SSWR::AVIRead::AVIRHQMPPlaylistForm::AVIRHQMPPlaylistForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::Playlist *playlist) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Create Playlist");

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 56, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbPlaylist, UI::GUIListBox(ui, this, false));
	this->lbPlaylist->SetRect(0, 0, 160, 100, false);
	this->lbPlaylist->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->btnAdd, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"&Add"));
	this->btnAdd->SetRect(16, 16, 75, 23, false);
	this->btnAdd->HandleButtonClick(OnAddClicked, this);
	NEW_CLASS(this->btnAddDir, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"&Add Folder"));
	this->btnAddDir->SetRect(96, 16, 75, 23, false);
	this->btnAddDir->HandleButtonClick(OnAddDirClicked, this);
	NEW_CLASS(this->btnClear, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"Clea&r"));
	this->btnClear->SetRect(176, 16, 75, 23, false);
	this->btnClear->HandleButtonClick(OnClearClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"&OK"));
	this->btnOK->SetRect(256, 16, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"&Cancel"));
	this->btnCancel->SetRect(336, 16, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->HandleDropFiles(OnFileDrop, this);

	NEW_CLASS(this->playlist, Media::Playlist((const UTF8Char*)"HQMP", this->core->GetParserList()));
	if (playlist)
	{
		this->playlist->AppendPlaylist(playlist);
	}
	this->UpdatePlaylist();

}

SSWR::AVIRead::AVIRHQMPPlaylistForm::~AVIRHQMPPlaylistForm()
{
}


void SSWR::AVIRead::AVIRHQMPPlaylistForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Media::Playlist *SSWR::AVIRead::AVIRHQMPPlaylistForm::GetPlaylist()
{
	return this->playlist;
}
