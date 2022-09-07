#include "Stdafx.h"
#include "Exporter/GUIJPGExporter.h"
#include "Exporter/WebPExporter.h"
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
	UI::FolderDialog dlg(L"SSWR", L"AVIRead", L"ImageBatchConv");
	if (sb.GetLength() > 0)
	{
		dlg.SetFolder(sb.ToCString());
	}
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->txtDir->SetText(dlg.GetFolder()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRImageBatchConvForm::OnConvertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImageBatchConvForm *me = (SSWR::AVIRead::AVIRImageBatchConvForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UTF8Char *sptr2;
	UTF8Char *sptr2End;
	Text::StringBuilderUTF8 sb;
	Int32 quality = 0;
	me->txtQuality->GetText(sbuff);
	quality = Text::StrToInt32(sbuff);
	if (quality <= 0 || quality > 100)
	{
		UI::MessageDialog::ShowDialog(CSTR("Invalid Quality"), CSTR("Error"), me);
		return;
	}
	sptr = me->txtDir->GetText(sbuff);
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::Directory)
	{
		UI::MessageDialog::ShowDialog(CSTR("Not a directory"), CSTR("Error"), me);
		return;
	}
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}

	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	void *param;
	IO::Path::FindFileSession *sess;
	Bool succ = true;
	IO::FileExporter *exporter;
	Text::CString ext;
	if (me->radFormatWebP->IsSelected())
	{
		NEW_CLASS(exporter, Exporter::WebPExporter());
		ext = CSTR("webp");
	}
	else
	{
		NEW_CLASS(exporter, Exporter::GUIJPGExporter());
		ext = CSTR("jpg");
	}
	IO::Path::PathType pt;
	Media::ImageList *imgList;
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
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
				sptr2 = Text::StrConcatC(sptr2, sb.ToString(), sb.GetLength());
				IO::Path::CreateDirectory(CSTRP(sbuff2, sptr2));
				*sptr2++ = IO::Path::PATH_SEPERATOR;
			}
		}
		while ((sptrEnd = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::File && !Text::StrEndsWithICaseC(sptr, (UOSInt)(sptrEnd - sptr), ext.v, ext.leng))
			{
				{
					IO::StmData::FileData fd({sbuff, (UOSInt)(sptrEnd - sbuff)}, false);
					imgList = (Media::ImageList*)me->core->GetParserList()->ParseFileType(&fd, IO::ParserType::ImageList);
				}
				if (imgList)
				{
					imgList->ToStaticImage(0);
					((Media::StaticImage*)imgList->GetImage(0, 0))->To32bpp();
					param = exporter->CreateParam(imgList);
					Text::StrConcatC(sptr2, sptr, (UOSInt)(sptrEnd - sptr));
					sptr2End = IO::Path::ReplaceExt(sptr2, ext.v, ext.leng);
					if (param)
					{
						exporter->SetParamInt32(param, 0, quality);
					}
					{
						IO::FileStream fs(CSTRP(sbuff2, sptr2End), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						if (!exporter->ExportFile(&fs, CSTRP(sbuff2, sptr2End), imgList, param))
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("Error in converting to "));
							sb.AppendP(sptr2, sptr2End);
							sb.AppendC(UTF8STRC(", do you want to continue?"));
							if (!UI::MessageDialog::ShowYesNoDialog(sb.ToCString(), CSTR("Image Batch Convert"), me))
							{
								succ = false;
							}
						}
					}

					if (param)
					{
						exporter->DeleteParam(param);
					}
					DEL_CLASS(imgList);
				}
				else
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Error in loading "));
					sb.AppendP(sptr, sptrEnd);
					sb.AppendC(UTF8STRC(", do you want to continue?"));
					if (!UI::MessageDialog::ShowYesNoDialog(sb.ToCString(), CSTR("Image Batch Convert"), me))
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
	DEL_CLASS(exporter);
}

SSWR::AVIRead::AVIRImageBatchConvForm::AVIRImageBatchConvForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 184, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Image Batch Convert"));
	this->SetNoResize(true);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblDir, UI::GUILabel(ui, this, CSTR("Folder")));
	this->lblDir->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtDir, UI::GUITextBox(ui, this, CSTR("")));
	this->txtDir->SetRect(100, 0, 450, 23, false);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this, CSTR("&Browse")));
	this->btnBrowse->SetRect(550, 0, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	NEW_CLASS(this->lblOutFormat, UI::GUILabel(ui, this, CSTR("Output Format")));
	this->lblOutFormat->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->radFormatJPG, UI::GUIRadioButton(ui, this, CSTR("JPEG"), true));
	this->radFormatJPG->SetRect(100, 24, 100, 23, false);
	NEW_CLASS(this->radFormatWebP, UI::GUIRadioButton(ui, this, CSTR("WebP"), false));
	this->radFormatWebP->SetRect(100, 48, 100, 23, false);
	NEW_CLASS(this->lblQuality, UI::GUILabel(ui, this, CSTR("Quality")));
	this->lblQuality->SetRect(0, 72, 100, 23, false);
	NEW_CLASS(this->txtQuality, UI::GUITextBox(ui, this, CSTR("100")));
	this->txtQuality->SetRect(100, 72, 100, 23, false);
	NEW_CLASS(this->chkSubdir, UI::GUICheckBox(ui, this, CSTR("Subdir"), true));
	this->chkSubdir->SetRect(0, 96, 100, 23, false);
	NEW_CLASS(this->txtSubdir, UI::GUITextBox(ui, this, CSTR("JPEG")));
	this->txtSubdir->SetRect(100, 96, 100, 23, false);
	NEW_CLASS(this->btnConvert, UI::GUIButton(ui, this, CSTR("&Convert")));
	this->btnConvert->SetRect(100, 120, 75, 23, false);
	this->btnConvert->HandleButtonClick(OnConvertClicked, this);
}

SSWR::AVIRead::AVIRImageBatchConvForm::~AVIRImageBatchConvForm()
{
}

void SSWR::AVIRead::AVIRImageBatchConvForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
