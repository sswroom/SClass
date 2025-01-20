#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/ModemController.h"
#include "IO/StmData/MemoryDataCopy.h"
#include "Media/GDIEngine.h"
#include "SSWR/AVIReadCE/AVIRCoreCE.h"
#include "UI/GUIFileDialog.h"
#include "UI/GUIForm.h"

SSWR::AVIReadCE::AVIRCoreCE::AVIRCoreCE(NN<UI::GUICore> ui) : SSWR::AVIRead::AVIRCore(ui)
{
	this->eng = ui->CreateDrawEngine();
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
	if (this->batchLyrs.NotNull())
	{
/*		AVIRead::AVIRGISForm *gisForm;
		Map::MapEnv *env;
		NEW_CLASS(env, Map::MapEnv(L"Untitled", 0xffc0c0ff, this->batchLyrs->GetItem(0)->GetCoordinateSystem()->Clone()));
		NEW_CLASS(gisForm, AVIRead::AVIRGISForm(this->ui->GetHInst(), 0, this->ui, this, env));
		gisForm->AddLayers(this->batchLyrs);*/
		this->batchLyrs.Delete();
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
				NEW_CLASS(this->batchLyrs, Data::ArrayList<Map::MapDrawLayer*>());
			}
			this->batchLyrs->Add((Map::MapDrawLayer *)pobj);
		}
		else
		{
			Map::MapDrawLayer *lyr = (Map::MapDrawLayer*)pobj;
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
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSectorForm(this->ui->GetHInst(), 0, this->ui, this, (IO::SectorData *)pobj));
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

void SSWR::AVIReadCE::AVIRCoreCE::SaveData(NN<UI::GUIForm> ownerForm, NN<IO::ParsedObject> pobj, const WChar *dialogName)
{
	OSInt i;
	OSInt j;
	OSInt k;
	Data::ArrayListNN<IO::FileExporter> exp;
	this->exporters.GetSupportedExporters(exp, pobj);
	if (exp.GetCount() == 0)
	{
		ui->ShowMsgOK(CSTR("No supported exporter found"), CSTR("Save"), ownerForm);
	}
	else
	{
		Data::ArrayListNN<IO::FileExporter> exp2;
		NN<IO::FileExporter> fileExp;
		UTF8Char sbuff1[256];
		UnsafeArray<UTF8Char> sptr;
		UTF8Char sbuff2[256];
		Text::StringBuilderUTF8 sb;

		NN<UI::GUIFileDialog> sfd = ui->NewFileDialog(L"SSWR", L"AVIRead", dialogName, true);
		i = 0;
		j = exp.GetCount();
		while (i < j)
		{
			fileExp = exp.GetItemNoCheck(i);
			k = 0;
			while (fileExp->GetOutputName(k, sbuff1, sbuff2))
			{
				sfd->AddFilter(Text::CStringNN::FromPtr(sbuff2), Text::CStringNN::FromPtr(sbuff1));
				exp2.Add(fileExp);
				k++;
			}
			i++;
		}
		sptr = pobj->GetSourceName(sbuff1);
		if ((i = Text::StrLastIndexOfCharC(sbuff1, (UOSInt)(sptr - sbuff1), '.')) != INVALID_INDEX)
		{
			sbuff1[i] = 0;
			sptr = &sbuff1[i];
		}
		sfd->SetFileName(CSTRP(sbuff1, sptr));
		if (sfd->ShowDialog(ownerForm->GetHandle()))
		{
			NN<IO::FileStream> fs;
			IO::FileExporter::SupportType suppType;
			fileExp = exp2.GetItemNoCheck(sfd->GetFilterIndex());
			suppType = fileExp->IsObjectSupported(pobj);
			if (fileExp->GetParamCnt() > 0)
			{
/*				void *param;
				param = fileExp->CreateParam(pobj);
				if (param == 0)
				{
					UI::MessageDialog::ShowDialog(L"Error in initializing parameters", L"Save Data");
				}
				else
				{
					AVIRExportParamForm frm(ui->GetHInst(), 0, this->ui, fileExp, param);
					if (frm.ShowDialog(ownerForm) == UI::GUIForm::DR_OK)
					{
						if (suppType == IO::FileExporter::SupportType::PathOnly)
						{
							if (!fileExp->ExportFile(0, sfd.GetFileName(), pobj, param))
							{
								UI::MessageDialog::ShowDialog(L"Error in saving file", L"Save Data");
							}
						}
						else
						{
							NEW_CLASS(fs, IO::FileStream(sfd.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
							if (!fileExp->ExportFile(fs, sfd.GetFileName(), pobj, param))
							{
								UI::MessageDialog::ShowDialog(L"Error in saving file", L"Save Data");
							}
							DEL_CLASS(fs);
						}
					}
					fileExp->DeleteParam(param);
				}*/
			}
			else
			{
				if (suppType == IO::FileExporter::SupportType::PathOnly)
				{
					if (!fileExp->ExportFile(0, sfd->GetFileName()->ToCString(), pobj, 0))
					{
						ui->ShowMsgOK(CSTR("Error in saving file"), CSTR("Save Data"), ownerForm);
					}
				}
				else
				{
					NEW_CLASSNN(fs, IO::FileStream(sfd->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					if (!fileExp->ExportFile(fs, sfd->GetFileName()->ToCString(), pobj, 0))
					{
						ui->ShowMsgOK(CSTR("Error in saving file"), CSTR("Save Data"), ownerForm);
					}
					fs.Delete();
				}
			}
		}
		sfd.Delete();
	}
}

Optional<Media::Printer> SSWR::AVIReadCE::AVIRCoreCE::SelectPrinter(Optional<UI::GUIForm> frm)
{
	Media::Printer *printer = 0;
/*	SSWR::AVIRead::AVIRSelPrinterForm selFrm(this->ui->GetHInst(), 0, this->ui, this);
	if (selFrm.ShowDialog(frm) == UI::GUIForm::DR_OK)
	{
		printer = selFrm.printer;
	}*/
	return printer;
}
