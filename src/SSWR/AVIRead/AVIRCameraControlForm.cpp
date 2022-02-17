#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/GPSNMEA.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/MemoryData.h"
#include "SSWR/AVIRead/AVIRGISForm.h"
#include "SSWR/AVIRead/AVIRCameraControlForm.h"
#include "Text/MyString.h"
#include "UI/FileDialog.h"
#include "UI/FolderDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRCameraControlForm::OnDownloadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCameraControlForm *me = (SSWR::AVIRead::AVIRCameraControlForm*)userObj;
	Data::ArrayList<UOSInt> selIndices;
	me->lvFiles->GetSelectedIndices(&selIndices);
	if (selIndices.GetCount() <= 0)
	{
		return;
	}
	else if (selIndices.GetCount() == 1)
	{
		UI::FileDialog *dlg;
		IO::CameraControl::FileInfo *file = (IO::CameraControl::FileInfo*)me->lvFiles->GetItem(selIndices.GetItem(0));
		NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"CameraControlFile", true));
		dlg->SetFileName(file->fileName);
		if (dlg->ShowDialog(me->GetHandle()))
		{
			Data::DateTime dt;
			IO::FileStream *fs;
			Bool succ;
			NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			succ = me->camera->GetFile(file, fs);
			if (file->fileTimeTicks)
			{
				dt.SetTicks(file->fileTimeTicks);
				fs->SetFileTimes(0, 0, &dt);
			}
			DEL_CLASS(fs);
			if (!succ)
			{
				IO::Path::DeleteFile(dlg->GetFileName()->v);
				UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in downloading the file", (const UTF8Char*)"Camera Control", me);
			}
		}
		DEL_CLASS(dlg);
	}
	else
	{
		Data::DateTime dt;
		Text::StringBuilderUTF8 sb;
		UI::FolderDialog *dlg;
		NEW_CLASS(dlg, UI::FolderDialog(L"SSWR", L"AVIRead", L"CameraControlFolder"));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			IO::FileStream *fs;
			Bool succ = true;
			UOSInt i = 0;
			UOSInt j = selIndices.GetCount();
			while (i < j)
			{
				IO::CameraControl::FileInfo *file = (IO::CameraControl::FileInfo*)me->lvFiles->GetItem(selIndices.GetItem(i));
				sb.ClearStr();
				sb.Append(dlg->GetFolder());
				if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
				{
					sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				}
				sb.AppendSlow(file->fileName);
				
				NEW_CLASS(fs, IO::FileStream(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				succ = me->camera->GetFile(file, fs);
				if (file->fileTimeTicks)
				{
					dt.SetTicks(file->fileTimeTicks);
					fs->SetFileTimes(0, 0, &dt);
				}
				DEL_CLASS(fs);
				if (!succ)
				{
					IO::Path::DeleteFile(sb.ToString());
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Error in downloading "));
					sb.AppendSlow(file->fileName);
					sb.AppendC(UTF8STRC(", continue?"));
					if (!UI::MessageDialog::ShowYesNoDialog(sb.ToString(), (const UTF8Char*)"Camera Control", me))
					{
						break;
					}
				}
				i++;
			}
			if (succ)
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"Finish downloading selected files", (const UTF8Char*)"Camera Control", me);
			}
		}
		DEL_CLASS(dlg);
	}
}

void __stdcall SSWR::AVIRead::AVIRCameraControlForm::OnFilesDblClick(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRCameraControlForm *me = (SSWR::AVIRead::AVIRCameraControlForm*)userObj;
	IO::CameraControl::FileInfo *file = (IO::CameraControl::FileInfo*)me->lvFiles->GetItem(index);
	if (file == 0)
		return;
	if (file->fileType == IO::CameraControl::FT_IMAGE)
	{
		if (Text::StrEndsWithICase(file->fileName, (const UTF8Char*)".JPG"))
		{
			IO::MemoryStream *mstm;
			NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("SSWR.AVIRead.AVIRCameraControlForm.OnFilesDblClick.mstm")));
			if (me->camera->GetFile(file, mstm))
			{
				UOSInt size;
				UInt8 *buff = mstm->GetBuff(&size);
				IO::StmData::MemoryData *fd;
				NEW_CLASS(fd, IO::StmData::MemoryData(buff, size));
				IO::ParsedObject *pobj = me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
				DEL_CLASS(fd);
				if (pobj)
				{
					me->core->OpenObject(pobj);
				}
			}
			DEL_CLASS(mstm);
		}
	}
	else if (file->fileType == IO::CameraControl::FT_GPSLOG)
	{
		if (Text::StrEndsWithICase(file->fileName, (const UTF8Char*)".LOG"))
		{
			IO::MemoryStream *mstm;
			NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("SSWR.AVIRead.AVIRCameraControlForm.OnFilesDblClick.mstm2")));
			if (me->camera->GetFile(file, mstm))
			{
				mstm->SeekFromBeginning(0);
				Map::GPSTrack *trk = IO::GPSNMEA::NMEA2Track(mstm, {file->fileName, Text::StrCharCnt(file->fileName)});
				SSWR::AVIRead::AVIRGISForm *frm = me->core->GetGISForm();
				if (frm)
				{
					frm->AddLayer(trk);
				}
				else
				{
					me->core->OpenObject(trk);
				}
			}
			DEL_CLASS(mstm);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRCameraControlForm::OnFilesSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRCameraControlForm *me = (SSWR::AVIRead::AVIRCameraControlForm*)userObj;
	IO::CameraControl::FileInfo *file = (IO::CameraControl::FileInfo*)me->lvFiles->GetSelectedItem();
	if (file == 0)
		return;
	Media::ImageList *previewImg = me->previewMap->Get(file->fileName);
	if (previewImg)
	{
		me->pbPreview->SetImage((Media::StaticImage*)previewImg->GetImage(0, 0));
		return;
	}
	IO::MemoryStream *mstm;
	NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("SSWR.AVIRead.AVIROlympusCameraForm.OnFilesSelChg.mstm")));
	if (me->camera->GetThumbnailFile(file, mstm))
	{
		UOSInt size;
		UInt8 *buff = mstm->GetBuff(&size);
		IO::StmData::MemoryData *fd;
		NEW_CLASS(fd, IO::StmData::MemoryData(buff, size));
		previewImg = (Media::ImageList*)me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
		DEL_CLASS(fd);
		if (previewImg)
		{
			previewImg->ToStaticImage(0);
			me->previewMap->Put(file->fileName, previewImg);
			me->pbPreview->SetImage((Media::StaticImage*)previewImg->GetImage(0, 0));
		}
	}
	DEL_CLASS(mstm);
}

SSWR::AVIRead::AVIRCameraControlForm::AVIRCameraControlForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::CameraControl *camera) : UI::GUIForm(parent, 640, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Camera Control"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->camera = camera;
	NEW_CLASS(this->previewMap, Data::StringUTF8Map<Media::ImageList*>());

	NEW_CLASS(this->lvInfo, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvInfo->SetRect(0, 0, 100, 96, false);
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvInfo->SetFullRowSelect(true);
	this->lvInfo->SetShowGrid(true);
	this->lvInfo->AddColumn((const UTF8Char*)"Name", 200);
	this->lvInfo->AddColumn((const UTF8Char*)"Value", 400);
	NEW_CLASS(this->vspInfo, UI::GUIVSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 160, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->pbPreview, UI::GUIPictureBoxSimple(ui, this->pnlControl, this->core->GetDrawEngine(), false));
	this->pbPreview->SetRect(0, 0, 160, 160, false);
	NEW_CLASS(this->btnDownload, UI::GUIButton(ui, this->pnlControl, CSTR("Download")));
	this->btnDownload->SetRect(164, 4, 75, 23, false);
	this->btnDownload->HandleButtonClick(OnDownloadClicked, this);
	NEW_CLASS(this->lvFiles, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvFiles->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFiles->SetFullRowSelect(true);
	this->lvFiles->SetShowGrid(true);
	this->lvFiles->AddColumn((const UTF8Char*)"File Name", 200);
	this->lvFiles->AddColumn((const UTF8Char*)"File Path", 200);
	this->lvFiles->AddColumn((const UTF8Char*)"File Size", 120);
	this->lvFiles->AddColumn((const UTF8Char*)"Modify Date", 150);
	this->lvFiles->HandleDblClk(OnFilesDblClick, this);
	this->lvFiles->HandleSelChg(OnFilesSelChg, this);

	Data::ArrayList<Text::String *> nameList;
	Data::ArrayList<Text::String *> valueList;
	UOSInt i;
	UOSInt j;
	this->camera->GetInfoList(&nameList, &valueList);
	i = 0;
	j = nameList.GetCount();
	while (i < j)
	{
		this->lvInfo->AddItem(nameList.GetItem(i), 0);
		this->lvInfo->SetSubItem(i, 1, valueList.GetItem(i));
		i++;
	}
	this->camera->FreeInfoList(&nameList, &valueList);

	Data::ArrayList<IO::CameraControl::FileInfo *> fileList;
	IO::CameraControl::FileInfo *file;
	UTF8Char sbuff[32];
	Data::DateTime dt;
	i = 0;
	j = this->camera->GetFileList(&fileList);
	while (i < j)
	{
		file = fileList.GetItem(i);
		this->lvFiles->AddItem({file->fileName, Text::StrCharCnt(file->fileName)}, file);
		this->lvFiles->SetSubItem(i, 1, file->filePath);
		Text::StrUInt64(sbuff, file->fileSize);
		this->lvFiles->SetSubItem(i, 2, sbuff);
		if (file->fileTimeTicks)
		{
			dt.SetTicks(file->fileTimeTicks);
			dt.ToLocalTime();
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			this->lvFiles->SetSubItem(i, 3, sbuff);
		}
		else
		{
			this->lvFiles->SetSubItem(i, 3, (const UTF8Char*)"-");
		}
		
		i++;
	}
}

SSWR::AVIRead::AVIRCameraControlForm::~AVIRCameraControlForm()
{
	this->ClearChildren();
	DEL_CLASS(this->camera);
	Data::ArrayList<Media::ImageList*> *previewList = this->previewMap->GetValues();
	UOSInt i = previewList->GetCount();
	Media::ImageList *previewImg;
	while (i-- > 0)
	{
		previewImg = previewList->GetItem(i);
		DEL_CLASS(previewImg);
	}
	DEL_CLASS(this->previewMap);
}

void SSWR::AVIRead::AVIRCameraControlForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
