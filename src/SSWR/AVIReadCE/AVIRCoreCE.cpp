#include "stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/ModemController.h"
#include "IO/StmData/MemoryData2.h"
#include "Media/GDIEngine.h"
#include "SSWR/AVIReadCE/AVIRCoreCE.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"
#include "UI/GUIForm.h"

SSWR::AVIReadCE::AVIRCoreCE::AVIRCoreCE(UI::GUICore *ui) : SSWR::AVIRead::AVIRCore(ui)
{
	NEW_CLASS(this->eng, Media::GDIEngine());
}

SSWR::AVIReadCE::AVIRCoreCE::~AVIRCoreCE()
{
}

void SSWR::AVIReadCE::AVIRCoreCE::BeginLoad()
{
	this->batchLoad = true;
}

void SSWR::AVIReadCE::AVIRCoreCE::EndLoad()
{
	this->batchLoad = false;
	if (this->batchLyrs)
	{
/*		AVIRead::AVIRGISForm *gisForm;
		Map::MapEnv *env;
		NEW_CLASS(env, Map::MapEnv(L"Untitled", 0xffc0c0ff, this->batchLyrs->GetItem(0)->GetCoordinateSystem()->Clone()));
		NEW_CLASS(gisForm, AVIRead::AVIRGISForm(this->ui->GetHInst(), 0, this->ui, this, env));
		gisForm->AddLayers(this->batchLyrs);*/
		DEL_CLASS(this->batchLyrs);
		this->batchLyrs = 0;
//		InitForm(gisForm);
	}
}

void SSWR::AVIReadCE::AVIRCoreCE::OpenObject(IO::ParsedObject *pobj)
{
	UI::GUIForm *frm;
	IO::ParserType pt = pobj->GetParserType();
	switch (pt)
	{
/*	case IO::ParserType::MapEnv:
		NEW_CLASS(frm, AVIRead::AVIRGISForm(this->ui->GetHInst(), 0, this->ui, this, (Map::MapEnv*)pobj));
		InitForm(frm);
		break;
	case IO::ParserType::MapLayer:
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
			NEW_CLASS(env, Map::MapEnv(L"Untitled", 0xffc0c0ff, lyr->GetCoordinateSystem()->Clone()));
			NEW_CLASS(frm, AVIRead::AVIRGISForm(this->ui->GetHInst(), 0, this->ui, this, env));
			((AVIRead::AVIRGISForm*)frm)->AddLayer(lyr);
			InitForm(frm);
		}
		break;
	case IO::ParserType::MAP_LAYER_COLL:
		{
			Map::MapLayerCollection *lyrColl = (Map::MapLayerCollection*)pobj;
			Map::MapEnv *env;
			NEW_CLASS(env, Map::MapEnv(L"Untitled", 0xffc0c0ff, lyrColl->GetItem(0)->GetCoordinateSystem()->Clone()));
			env->AddLayerColl(0, lyrColl, true);
			NEW_CLASS(frm, AVIRead::AVIRGISForm(this->ui->GetHInst(), 0, this->ui, this, env));
			InitForm(frm);
		}
		break;
	case IO::ParserType::ImageList:
		NEW_CLASS(frm, AVIRead::AVIRImageForm(this->ui->GetHInst(), 0, this->ui, this, (Media::ImageList*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::PackageFile:
		NEW_CLASS(frm, AVIRead::AVIRPackageForm(this->ui->GetHInst(), 0, this->ui, this, (IO::PackageFile*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::MediaFile:
		NEW_CLASS(frm, AVIRead::AVIRMediaForm(this->ui->GetHInst(), 0, this->ui, this, (Media::MediaFile*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::EXEFile:
		NEW_CLASS(frm, AVIRead::AVIRExeForm(this->ui->GetHInst(), 0, this->ui, this, (IO::EXEFile*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::ReadingDB:
		NEW_CLASS(frm, AVIRead::AVIRDBForm(this->ui->GetHInst(), 0, this->ui, this, (DB::ReadingDB*)pobj, true));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::FileCheck:
		NEW_CLASS(frm, AVIRead::AVIRFileChkForm(this->ui->GetHInst(), 0, this->ui, this, (IO::FileCheck*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::LogFile:
		NEW_CLASS(frm, AVIRead::AVIRLogFileForm(this->ui->GetHInst(), 0, this->ui, this, (IO::LogFile*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::Stream:
		{
			Int64 totalSize = 0;
			OSInt thisSize;
			UInt8 *buff;
			IO::Stream *stm = (IO::Stream *)pobj;
			IO::StmData::MemoryData2 *data;
			IO::MemoryStream *mstm;
			NEW_CLASS(mstm, IO::MemoryStream(L"SSWR.AVIRead.AVIRCore.OpenObject"));
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
				this->LoadData(data);
				DEL_CLASS(data);
			}
			else
			{
				DEL_CLASS(mstm);
			}
			break;
		}
	case IO::ParserType::Playlist:
		{
			SSWR::AVIRead::AVIRPlaylistForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRPlaylistForm(this->ui->GetHInst(), 0, this->ui, this, (Media::Playlist*)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::SectorData:
		{
			SSWR::AVIRead::AVIRSectorForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSectorForm(this->ui->GetHInst(), 0, this->ui, this, (IO::ISectorData *)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::CodeProject:
		{
			SSWR::AVIRead::AVIRCodeProjectForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRCodeProjectForm(this->ui->GetHInst(), 0, this->ui, this, (Text::CodeProject*)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;*/
	case IO::ParserType::TextDocument:
	case IO::ParserType::Workbook:
	case IO::ParserType::CoordinateSystem:
	default:
		DEL_CLASS(pobj);
		break;
	}
}

void SSWR::AVIReadCE::AVIRCoreCE::SaveData(UI::GUIForm *ownerForm, IO::ParsedObject *pobj, const WChar *dialogName)
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
		if (pobj->GetSourceName(sbuff1))
		{
			if ((i = Text::StrLastIndexOf(sbuff1, '.')) != INVALID_INDEX)
			{
				sbuff1[i] = 0;
			}
			sfd->SetFileName(sbuff1);
		}
		if (sfd->ShowDialog(ownerForm->GetHandle()))
		{
			IO::FileStream *fs;
			IO::FileExporter::SupportType suppType;
			fileExp = exp2->GetItem(sfd->GetFilterIndex());
			suppType = fileExp->IsObjectSupported(pobj);
			if (fileExp->GetParamCnt() > 0)
			{
/*				AVIRExportParamForm *frm;
				void *param;
				param = fileExp->CreateParam(pobj);
				if (param == 0)
				{
					UI::MessageDialog::ShowDialog(L"Error in initializing parameters", L"Save Data");
				}
				else
				{
					NEW_CLASS(frm, AVIRExportParamForm(ui->GetHInst(), 0, this->ui, fileExp, param));
					if (frm->ShowDialog(ownerForm) == UI::GUIForm::DR_OK)
					{
						if (suppType == IO::FileExporter::SupportType::PathOnly)
						{
							if (!fileExp->ExportFile(0, sfd->GetFileName(), pobj, param))
							{
								UI::MessageDialog::ShowDialog(L"Error in saving file", L"Save Data");
							}
						}
						else
						{
							NEW_CLASS(fs, IO::FileStream(sfd->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
							if (!fileExp->ExportFile(fs, sfd->GetFileName(), pobj, param))
							{
								UI::MessageDialog::ShowDialog(L"Error in saving file", L"Save Data");
							}
							DEL_CLASS(fs);
						}
					}
					fileExp->DeleteParam(param);
					DEL_CLASS(frm);
				}*/
			}
			else
			{
				if (suppType == IO::FileExporter::SupportType::PathOnly)
				{
					if (!fileExp->ExportFile(0, sfd->GetFileName(), pobj, 0))
					{
						UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in saving file", (const UTF8Char*)"Save Data", ownerForm);
					}
				}
				else
				{
					NEW_CLASS(fs, IO::FileStream(sfd->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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

void SSWR::AVIReadCE::AVIRCoreCE::OpenGSMModem(IO::Stream *modemPort)
{
//	SSWR::AVIRead::AVIRGSMModemForm *frm;
	IO::ModemController *modem;
	IO::ATCommandChannel *channel;
/*	NEW_CLASS(channel, IO::ATCommandChannel(modemPort));
	NEW_CLASS(modem, IO::ModemController(channel));

	NEW_CLASS(frm, SSWR::AVIRead::AVIRGSMModemForm(ui->GetHInst(), 0, ui, this, modem, channel, modemPort));
	InitForm(frm);
	frm->Show();*/
}

Media::Printer *SSWR::AVIReadCE::AVIRCoreCE::SelectPrinter(UI::GUIForm *frm)
{
	Media::Printer *printer = 0;
/*	SSWR::AVIRead::AVIRSelPrinterForm *selFrm;
	NEW_CLASS(selFrm, SSWR::AVIRead::AVIRSelPrinterForm(this->ui->GetHInst(), 0, this->ui, this));
	if (selFrm->ShowDialog(frm) == UI::GUIForm::DR_OK)
	{
		printer = selFrm->printer;
	}
	DEL_CLASS(selFrm);*/
	return printer;
}

IO::Stream *SSWR::AVIReadCE::AVIRCoreCE::OpenStream(StreamType *st, UI::GUIForm *ownerFrm, Int32 defBaudRate, Bool allowReadOnly)
{
	IO::Stream *retStm = 0;

	return retStm;
}
