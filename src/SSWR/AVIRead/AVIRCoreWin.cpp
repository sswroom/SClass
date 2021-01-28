#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "IO/StmData/MemoryData2.h"
#include "SSWR/AVIRead/AVIRCodeProjectForm.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRDBForm.h"
#include "SSWR/AVIRead/AVIRRAWMonitorForm.h"
#include "SSWR/AVIRead/AVIRExeForm.h"
#include "SSWR/AVIRead/AVIRExportParamForm.h"
#include "SSWR/AVIRead/AVIRFileChkForm.h"
#include "SSWR/AVIRead/AVIRFontRendererForm.h"
#include "SSWR/AVIRead/AVIRGISForm.h"
#include "SSWR/AVIRead/AVIRGPSTrackerForm.h"
#include "SSWR/AVIRead/AVIRImageForm.h"
#include "SSWR/AVIRead/AVIRJavaClassForm.h"
#include "SSWR/AVIRead/AVIRLogFileForm.h"
#include "SSWR/AVIRead/AVIRLUTForm.h"
#include "SSWR/AVIRead/AVIRMediaForm.h"
#include "SSWR/AVIRead/AVIRMIMEViewerForm.h"
#include "SSWR/AVIRead/AVIRPackageForm.h"
#include "SSWR/AVIRead/AVIRPlaylistForm.h"
#include "SSWR/AVIRead/AVIRSectorForm.h"
#include "SSWR/AVIRead/AVIRSelPrinterForm.h"
#include "SSWR/AVIRead/AVIRSMakeForm.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

SSWR::AVIRead::AVIRCoreWin::AVIRCoreWin(UI::GUICore *ui) : SSWR::AVIRead::AVIRCore(ui)
{
}

SSWR::AVIRead::AVIRCoreWin::~AVIRCoreWin()
{
}

void SSWR::AVIRead::AVIRCoreWin::OpenObject(IO::ParsedObject *pobj)
{
	UI::GUIForm *frm;
	IO::ParsedObject::ParserType pt = pobj->GetParserType();
	switch (pt)
	{
	case IO::ParsedObject::PT_MAP_ENV_PARSER:
		NEW_CLASS(frm, AVIRead::AVIRGISForm(0, this->ui, this, (Map::MapEnv*)pobj, ((Map::MapEnv*)pobj)->CreateMapView(320, 240)));
		InitForm(frm);
		break;
	case IO::ParsedObject::PT_MAP_LAYER_PARSER:
		if (this->batchLoad)
		{
			if (this->batchLyrs == 0)
			{
				NEW_CLASS(this->batchLyrs, Data::ArrayList<Map::IMapDrawLayer*>());
			}
			this->batchLyrs->Add((Map::IMapDrawLayer *)pobj);
		}
		else
		{
			Map::IMapDrawLayer *lyr = (Map::IMapDrawLayer*)pobj;
			Map::MapEnv *env;
			NEW_CLASS(env, Map::MapEnv((const UTF8Char*)"Untitled", 0xffc0c0ff, lyr->GetCoordinateSystem()->Clone()));
			NEW_CLASS(frm, AVIRead::AVIRGISForm(0, this->ui, this, env, lyr->CreateMapView(320, 240)));
			((AVIRead::AVIRGISForm*)frm)->AddLayer(lyr);
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParsedObject::PT_IMAGE_LIST_PARSER:
		NEW_CLASS(frm, AVIRead::AVIRImageForm(0, this->ui, this, (Media::ImageList*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParsedObject::PT_PACKAGE_PARSER:
		NEW_CLASS(frm, AVIRead::AVIRPackageForm(0, this->ui, this, (IO::PackageFile*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParsedObject::PT_VIDEO_PARSER:
		NEW_CLASS(frm, AVIRead::AVIRMediaForm(0, this->ui, this, (Media::MediaFile*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParsedObject::PT_EXE_PARSER:
		NEW_CLASS(frm, AVIRead::AVIRExeForm(0, this->ui, this, (IO::EXEFile*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParsedObject::PT_READINGDB_PARSER:
		NEW_CLASS(frm, AVIRead::AVIRDBForm(0, this->ui, this, (DB::ReadingDB*)pobj, true));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParsedObject::PT_FILE_CHECK:
		NEW_CLASS(frm, AVIRead::AVIRFileChkForm(0, this->ui, this, (IO::FileCheck*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParsedObject::PT_LOG_FILE:
		NEW_CLASS(frm, AVIRead::AVIRLogFileForm(0, this->ui, this, (IO::LogFile*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParsedObject::PT_STREAM:
		{
			UInt64 totalSize = 0;
			UOSInt thisSize;
			UInt8 *buff;
			IO::Stream *stm = (IO::Stream *)pobj;
			IO::StmData::MemoryData2 *data;
			IO::MemoryStream *mstm;
			NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SSWR.AVIRead.AVIRCore.OpenObject"));
			buff = MemAlloc(UInt8, 1048576);
			while (totalSize < 104857600)
			{
				thisSize = stm->Read(buff, 1048576);
				if (thisSize == 0)
					break;
				mstm->Write(buff, thisSize);
				totalSize += thisSize;
			}
			DEL_CLASS(pobj);
			MemFree(buff);
			buff = mstm->GetBuff(&thisSize);
			if (thisSize > 0)
			{
				NEW_CLASS(data, IO::StmData::MemoryData2(buff, thisSize));
				DEL_CLASS(mstm);
				this->LoadData(data, 0);
				DEL_CLASS(data);
			}
			else
			{
				DEL_CLASS(mstm);
			}
			break;
		}
	case IO::ParsedObject::PT_PLAYLIST:
		{
			SSWR::AVIRead::AVIRPlaylistForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRPlaylistForm(0, this->ui, this, (Media::Playlist*)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParsedObject::PT_SECTOR_DATA:
		{
			SSWR::AVIRead::AVIRSectorForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSectorForm(0, this->ui, this, (IO::ISectorData *)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParsedObject::PT_CODEPROJECT:
		{
			SSWR::AVIRead::AVIRCodeProjectForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRCodeProjectForm(0, this->ui, this, (Text::CodeProject*)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParsedObject::PT_LUT:
		{
			SSWR::AVIRead::AVIRLUTForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRLUTForm(0, this->ui, this, (Media::LUT*)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParsedObject::PT_FONT_RENDERER:
		{
			SSWR::AVIRead::AVIRFontRendererForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRFontRendererForm(0, this->ui, this, (Media::FontRenderer*)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParsedObject::PT_MIME_OBJECT:
		{
			SSWR::AVIRead::AVIRMIMEViewerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRMIMEViewerForm(0, this->ui, this, (Text::IMIMEObj *)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParsedObject::PT_ETHERNET_ANALYZER:
		{
			SSWR::AVIRead::AVIRRAWMonitorForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRRAWMonitorForm(0, this->ui, this, (Net::EthernetAnalyzer*)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParsedObject::PT_JAVA_CLASS:
		{
			SSWR::AVIRead::AVIRJavaClassForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRJavaClassForm(0, this->ui, this, (IO::JavaClass*)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParsedObject::PT_SMAKE:
		{
			SSWR::AVIRead::AVIRSMakeForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSMakeForm(0, this->ui, this, (IO::SMake*)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParsedObject::PT_TEXT_DOCUMENT:
	case IO::ParsedObject::PT_WORKBOOK:
	case IO::ParsedObject::PT_COORDINATE_SYSTEM:
	default:
		DEL_CLASS(pobj);
		break;
	}
}

void SSWR::AVIRead::AVIRCoreWin::SaveData(UI::GUIForm *ownerForm, IO::ParsedObject *pobj, const WChar *dialogName)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Data::ArrayList<IO::FileExporter*> *exp;
	NEW_CLASS(exp, Data::ArrayList<IO::FileExporter*>());
	this->exporters->GetSupportedExporters(exp, pobj);
	if (exp->GetCount() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"No supported exporter found", (const UTF8Char*)"Save", ownerForm);
	}
	else
	{
		Data::ArrayList<IO::FileExporter*> *exp2;
		IO::FileExporter *fileExp;
		UTF8Char sbuff1[256];
		UTF8Char sbuff2[256];
		UTF8Char u8buff[256];
		Text::StringBuilderUTF8 *sb;
		NEW_CLASS(exp2, Data::ArrayList<IO::FileExporter*>());
		NEW_CLASS(sb, Text::StringBuilderUTF8());

		UI::FileDialog *sfd;
		NEW_CLASS(sfd, UI::FileDialog(L"SSWR", L"AVIRead", dialogName, true));
		i = 0;
		j = exp->GetCount();
		while (i < j)
		{
			fileExp = exp->GetItem(i);
			k = 0;
			while (fileExp->GetOutputName(k, sbuff1, sbuff2))
			{
				sfd->AddFilter(sbuff2, sbuff1);
				exp2->Add(fileExp);
				k++;
			}
			i++;
		}
		if (pobj->GetSourceName(u8buff))
		{
			if ((i = Text::StrLastIndexOf(u8buff, '.')) >= 0)
			{
				u8buff[i] = 0;
			}
			sfd->SetFileName(u8buff);
		}
		if (sfd->ShowDialog(ownerForm->GetHandle()))
		{
			IO::FileStream *fs;
			IO::FileExporter::SupportType suppType;
			fileExp = exp2->GetItem(sfd->GetFilterIndex());
			suppType = fileExp->IsObjectSupported(pobj);
			if (fileExp->GetParamCnt() > 0)
			{
				AVIRExportParamForm *frm;
				void *param;
				param = fileExp->CreateParam(pobj);
				if (param == 0)
				{
					UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in initializing parameters", (const UTF8Char*)"Save Data", ownerForm);
				}
				else
				{
					NEW_CLASS(frm, AVIRExportParamForm(0, this->ui, this, fileExp, param));
					if (frm->ShowDialog(ownerForm) == UI::GUIForm::DR_OK)
					{
						if (suppType == IO::FileExporter::ST_PATH_ONLY)
						{
							if (!fileExp->ExportFile(0, sfd->GetFileName(), pobj, param))
							{
								UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in saving file", (const UTF8Char*)"Save Data", ownerForm);
							}
						}
						else
						{
							NEW_CLASS(fs, IO::FileStream(sfd->GetFileName(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
							if (!fileExp->ExportFile(fs, sfd->GetFileName(), pobj, param))
							{
								UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in saving file", (const UTF8Char*)"Save Data", ownerForm);
							}
							DEL_CLASS(fs);
						}
					}
					fileExp->DeleteParam(param);
					DEL_CLASS(frm);
				}
			}
			else
			{
				if (suppType == IO::FileExporter::ST_PATH_ONLY)
				{
					if (!fileExp->ExportFile(0, sfd->GetFileName(), pobj, 0))
					{
						UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in saving file", (const UTF8Char*)"Save Data", ownerForm);
					}
				}
				else
				{
					NEW_CLASS(fs, IO::FileStream(sfd->GetFileName(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
					if (!fileExp->ExportFile(fs, sfd->GetFileName(), pobj, 0))
					{
						UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in saving file", (const UTF8Char*)"Save Data", ownerForm);
					}
					DEL_CLASS(fs);
				}
			}
		}

		DEL_CLASS(sfd);
		DEL_CLASS(sb);
		DEL_CLASS(exp2);
	}
	DEL_CLASS(exp);
}

Media::Printer *SSWR::AVIRead::AVIRCoreWin::SelectPrinter(UI::GUIForm *frm)
{
	Media::Printer *printer = 0;
	SSWR::AVIRead::AVIRSelPrinterForm *selFrm;
	NEW_CLASS(selFrm, SSWR::AVIRead::AVIRSelPrinterForm(0, this->ui, this));
	if (selFrm->ShowDialog(frm) == UI::GUIForm::DR_OK)
	{
		printer = selFrm->printer;
	}
	DEL_CLASS(selFrm);
	return printer;
}
