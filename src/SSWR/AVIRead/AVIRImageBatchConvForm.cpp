#include "Stdafx.h"
#include "Exporter/GUIJPGExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "SSWR/AVIRead/AVIRImageBatchConvForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FolderDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRImageBatchConvForm::OnBrowseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImageBatchConvForm *me = (SSWR::AVIRead::AVIRImageBatchConvForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtDir->GetText(&sb);
	UI::FolderDialog *dlg;
	NEW_CLASS(dlg, UI::FolderDialog(L"SSWR", L"AVIRead", L"ImageBatchConv"));
	if (sb.GetLength() > 0)
	{
		dlg->SetFolder(sb.ToString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtDir->SetText(dlg->GetFolder());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRImageBatchConvForm::OnConvertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImageBatchConvForm *me = (SSWR::AVIRead::AVIRImageBatchConvForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	Text::StringBuilderUTF8 sb;
	Int32 quality = 0;
	me->txtQuality->GetText(sbuff);
	quality = Text::StrToInt32(sbuff);
	if (quality <= 0 || quality > 100)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Invalid Quality", (const UTF8Char*)"Error", me);
		return;
	}
	sptr = me->txtDir->GetText(sbuff);
	if (IO::Path::GetPathType(sbuff) != IO::Path::PT_DIRECTORY)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Not a directory", (const UTF8Char*)"Error", me);
		return;
	}
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}

	Text::StrConcat(sptr, (const UTF8Char*)"*.tif");
	void *param;
	IO::Path::FindFileSession *sess;
	Bool succ = true;
	Exporter::GUIJPGExporter exporter;
	IO::StmData::FileData *fd;
	IO::FileStream *fs;
	IO::Path::PathType pt;
	Media::ImageList *imgList;
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		*sptr = 0;
		sptr2 = Text::StrConcat(sbuff2, sbuff);
		if (me->chkSubdir->IsChecked())
		{
			sb.ClearStr();
			me->txtSubdir->GetText(&sb);
			if (sb.GetLength() > 0)
			{
				sptr2 = Text::StrConcat(sptr2, sb.ToString());
				IO::Path::CreateDirectory(sbuff2);
				*sptr2++ = IO::Path::PATH_SEPERATOR;
			}
		}
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
		{
			if (pt == IO::Path::PT_FILE)
			{
				NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
				imgList = (Media::ImageList*)me->core->GetParserList()->ParseFileType(fd, IO::ParsedObject::PT_IMAGE_LIST_PARSER);
				DEL_CLASS(fd);
				if (imgList)
				{
					imgList->ToStaticImage(0);
					((Media::StaticImage*)imgList->GetImage(0, 0))->To32bpp();
					param = exporter.CreateParam(imgList);
					Text::StrConcat(sptr2, sptr);
					IO::Path::ReplaceExt(sptr2, (const UTF8Char*)"jpg");
					if (param)
					{
						exporter.SetParamInt32(param, 0, quality);
					}
					NEW_CLASS(fs, IO::FileStream(sbuff2, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
					if (!exporter.ExportFile(fs, sbuff2, imgList, param))
					{
						sb.ClearStr();
						sb.Append((const UTF8Char*)"Error in converting to ");
						sb.Append(sptr2);
						sb.Append((const UTF8Char*)", do you want to continue?");
						if (!UI::MessageDialog::ShowYesNoDialog(sb.ToString(), (const UTF8Char*)"Image Batch Convert", me))
						{
							succ = false;
						}
					}
					DEL_CLASS(fs);

					if (param)
					{
						exporter.DeleteParam(param);
					}
					DEL_CLASS(imgList);
				}
				else
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"Error in loading ");
					sb.Append(sptr);
					sb.Append((const UTF8Char*)", do you want to continue?");
					if (!UI::MessageDialog::ShowYesNoDialog(sb.ToString(), (const UTF8Char*)"Image Batch Convert", me))
					{
						succ = false;
					}
				}
			}
			if (!succ)
			{
				break;
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

SSWR::AVIRead::AVIRImageBatchConvForm::AVIRImageBatchConvForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 184, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Image Batch Convert");
	this->SetNoResize(true);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblDir, UI::GUILabel(ui, this, (const UTF8Char*)"Folder"));
	this->lblDir->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtDir, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtDir->SetRect(100, 0, 450, 23, false);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this, (const UTF8Char*)"&Browse"));
	this->btnBrowse->SetRect(550, 0, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	NEW_CLASS(this->lblQuality, UI::GUILabel(ui, this, (const UTF8Char*)"Quality"));
	this->lblQuality->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtQuality, UI::GUITextBox(ui, this, (const UTF8Char*)"100"));
	this->txtQuality->SetRect(100, 24, 100, 23, false);
	NEW_CLASS(this->chkSubdir, UI::GUICheckBox(ui, this, (const UTF8Char*)"Subdir", true));
	this->chkSubdir->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->txtSubdir, UI::GUITextBox(ui, this, (const UTF8Char*)"JPEG"));
	this->txtSubdir->SetRect(100, 48, 100, 23, false);
	NEW_CLASS(this->btnConvert, UI::GUIButton(ui, this, (const UTF8Char*)"&Convert"));
	this->btnConvert->SetRect(100, 72, 75, 23, false);
	this->btnConvert->HandleButtonClick(OnConvertClicked, this);
}

SSWR::AVIRead::AVIRImageBatchConvForm::~AVIRImageBatchConvForm()
{
}

void SSWR::AVIRead::AVIRImageBatchConvForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
