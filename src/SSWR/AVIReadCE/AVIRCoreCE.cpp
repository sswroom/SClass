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
/*		NN<AVIRead::AVIRGISForm> gisForm;
		NN<Map::MapEnv> env;
		NEW_CLASSNN(env, Map::MapEnv(L"Untitled", 0xffc0c0ff, this->batchLyrs->GetItem(0)->GetCoordinateSystem()->Clone()));
		NEW_CLASSNN(gisForm, AVIRead::AVIRGISForm(this->ui->GetHInst(), 0, this->ui, this, env));
		gisForm->AddLayers(this->batchLyrs);*/
		this->batchLyrs.Delete();
//		InitForm(gisForm);
	}
}

void SSWR::AVIReadCE::AVIRCoreCE::OpenObject(NN<IO::ParsedObject> pobj)
{
	NN<UI::GUIForm> frm;
	IO::ParserType pt = pobj->GetParserType();
	switch (pt)
	{
/*	case IO::ParserType::MapEnv:
		NEW_CLASSNN(frm, AVIRead::AVIRGISForm(this->ui->GetHInst(), 0, this->ui, this, (Map::MapEnv*)pobj));
		InitForm(frm);
		break;
	case IO::ParserType::MapLayer:
		if (this->batchLoad)
		{
			if (this->batchLyrs == 0)
			{
				NEW_CLASSNN(this->batchLyrs, Data::ArrayList<Map::MapDrawLayer*>());
			}
			this->batchLyrs->Add((Map::MapDrawLayer *)pobj);
		}
		else
		{
			NN<Map::MapDrawLayer> lyr = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
			NN<Map::MapEnv> env;
			NEW_CLASSNN(env, Map::MapEnv(L"Untitled", 0xffc0c0ff, lyr->GetCoordinateSystem()->Clone()));
			NEW_CLASSNN(frm, AVIRead::AVIRGISForm(this->ui->GetHInst(), 0, this->ui, this, env));
			((AVIRead::AVIRGISForm*)frm)->AddLayer(lyr);
			InitForm(frm);
		}
		break;
	case IO::ParserType::MAP_LAYER_COLL:
		{
			NN<Map::MapLayerCollection> lyrColl = NN<Map::MapLayerCollection>::ConvertFrom(pobj);
			NN<Map::MapEnv> env;
			NEW_CLASSNN(env, Map::MapEnv(L"Untitled", 0xffc0c0ff, lyrColl->GetItem(0)->GetCoordinateSystem()->Clone()));
			env->AddLayerColl(0, lyrColl, true);
			NEW_CLASSNN(frm, AVIRead::AVIRGISForm(this->ui->GetHInst(), 0, this->ui, this, env));
			InitForm(frm);
		}
		break;
	case IO::ParserType::ImageList:
		NEW_CLASSNN(frm, AVIRead::AVIRImageForm(this->ui->GetHInst(), 0, this->ui, this, (Media::ImageList*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::PackageFile:
		NEW_CLASSNN(frm, AVIRead::AVIRPackageForm(this->ui->GetHInst(), 0, this->ui, this, (IO::PackageFile*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::MediaFile:
		NEW_CLASSNN(frm, AVIRead::AVIRMediaForm(this->ui->GetHInst(), 0, this->ui, this, (Media::MediaFile*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::EXEFile:
		NEW_CLASSNN(frm, AVIRead::AVIRExeForm(this->ui->GetHInst(), 0, this->ui, this, (IO::EXEFile*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::ReadingDB:
		NEW_CLASSNN(frm, AVIRead::AVIRDBForm(this->ui->GetHInst(), 0, this->ui, this, (DB::ReadingDB*)pobj, true));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::FileCheck:
		NEW_CLASSNN(frm, AVIRead::AVIRFileChkForm(this->ui->GetHInst(), 0, this->ui, this, (IO::FileCheck*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::LogFile:
		NEW_CLASSNN(frm, AVIRead::AVIRLogFileForm(this->ui->GetHInst(), 0, this->ui, this, (IO::LogFile*)pobj));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::Stream:
		{
			Int64 totalSize = 0;
			IntOS thisSize;
			UInt8 *buff;
			IO::Stream *stm = (IO::Stream *)pobj;
			NN<IO::StmData::MemoryData2> data;
			NN<IO::MemoryStream> mstm;
			NEW_CLASSNN(mstm, IO::MemoryStream(L"SSWR.AVIRead.AVIRCore.OpenObject"));
			buff = MemAlloc(UInt8, 1048576);
			while (totalSize < 104857600)
			{
				thisSize = stm->Read(buff, 1048576);
				if (thisSize == 0)
					break;
				mstm->Write(buff, thisSize);
				totalSize += thisSize;
			}
			pobj.Delete();
			MemFree(buff);
			buff = mstm->GetBuff(&thisSize);
			if (thisSize > 0)
			{
				NEW_CLASSNN(data, IO::StmData::MemoryData2(buff, thisSize));
				mstm.Delete();
				this->LoadData(data);
				data.Delete();
			}
			else
			{
				mstm.Delete();
			}
			break;
		}
	case IO::ParserType::Playlist:
		{
			NN<SSWR::AVIRead::AVIRPlaylistForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRPlaylistForm(this->ui->GetHInst(), 0, this->ui, this, (Media::Playlist*)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::SectorData:
		{
			NN<SSWR::AVIRead::AVIRSectorForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRSectorForm(this->ui->GetHInst(), 0, this->ui, this, (IO::SectorData *)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::CodeProject:
		{
			NN<SSWR::AVIRead::AVIRCodeProjectForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRCodeProjectForm(this->ui->GetHInst(), 0, this->ui, this, (Text::CodeProject*)pobj));
			InitForm(frm);
			frm->Show();
		}
		break;*/
	case IO::ParserType::TextDocument:
	case IO::ParserType::Workbook:
	case IO::ParserType::CoordinateSystem:
	default:
		pobj.Delete();
		break;
	}
}

void SSWR::AVIReadCE::AVIRCoreCE::SaveData(NN<UI::GUIForm> ownerForm, NN<IO::ParsedObject> pobj, const WChar *dialogName)
{
	IntOS i;
	IntOS j;
	IntOS k;
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
		if ((i = Text::StrLastIndexOfCharC(sbuff1, (UIntOS)(sptr - sbuff1), '.')) != INVALID_INDEX)
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
							NEW_CLASSNN(fs, IO::FileStream(sfd.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
							if (!fileExp->ExportFile(fs, sfd.GetFileName(), pobj, param))
							{
								UI::MessageDialog::ShowDialog(L"Error in saving file", L"Save Data");
							}
							fs.Delete();
						}
					}
					fileExp->DeleteParam(param);
				}*/
			}
			else
			{
				if (suppType == IO::FileExporter::SupportType::PathOnly)
				{
					if (!fileExp->ExportFile(nullptr, sfd->GetFileName()->ToCString(), pobj, nullptr))
					{
						ui->ShowMsgOK(CSTR("Error in saving file"), CSTR("Save Data"), ownerForm);
					}
				}
				else
				{
					NEW_CLASSNN(fs, IO::FileStream(sfd->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					if (!fileExp->ExportFile(fs, sfd->GetFileName()->ToCString(), pobj, nullptr))
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
