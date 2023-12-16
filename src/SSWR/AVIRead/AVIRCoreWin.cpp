#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "IO/MemoryStream.h"
#include "IO/NullStream.h"
#include "IO/StmData/MemoryDataCopy.h"
#include "SSWR/AVIRead/AVIRASN1DataForm.h"
#include "SSWR/AVIRead/AVIRBTScanLogForm.h"
#include "SSWR/AVIRead/AVIRCodeProjectForm.h"
#include "SSWR/AVIRead/AVIRCoordSysForm.h"
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
#include "SSWR/AVIRead/AVIRPDFObjectForm.h"
#include "SSWR/AVIRead/AVIRPlaylistForm.h"
#include "SSWR/AVIRead/AVIRSectorForm.h"
#include "SSWR/AVIRead/AVIRSelPrinterForm.h"
#include "SSWR/AVIRead/AVIRSMakeForm.h"
#include "SSWR/AVIRead/AVIRSystemInfoLogForm.h"
#include "UI/FileDialog.h"

SSWR::AVIRead::AVIRCoreWin::AVIRCoreWin(NotNullPtr<UI::GUICore> ui) : SSWR::AVIRead::AVIRCore(ui)
{
}

SSWR::AVIRead::AVIRCoreWin::~AVIRCoreWin()
{
}

void SSWR::AVIRead::AVIRCoreWin::OpenObject(NotNullPtr<IO::ParsedObject> pobj)
{
	UI::GUIForm *frm;
	IO::ParserType pt = pobj->GetParserType();
	switch (pt)
	{
	case IO::ParserType::MapEnv:
		NEW_CLASS(frm, AVIRead::AVIRGISForm(0, this->ui, *this, NotNullPtr<Map::MapEnv>::ConvertFrom(pobj), ((Map::MapEnv*)pobj.Ptr())->CreateMapView(Math::Size2DDbl(320, 240))));
		InitForm(frm);
		break;
	case IO::ParserType::MapLayer:
		if (this->batchLoad)
		{
			if (this->batchLyrs == 0)
			{
				NEW_CLASS(this->batchLyrs, Data::ArrayList<Map::MapDrawLayer*>());
			}
			this->batchLyrs->Add((Map::MapDrawLayer *)pobj.Ptr());
		}
		else
		{
			NotNullPtr<Map::MapDrawLayer> lyr = NotNullPtr<Map::MapDrawLayer>::ConvertFrom(pobj);
			NotNullPtr<Map::MapEnv> env;
			NEW_CLASSNN(env, Map::MapEnv(CSTR("Untitled"), 0xffc0c0ff, lyr->GetCoordinateSystem()->Clone()));
			NEW_CLASS(frm, AVIRead::AVIRGISForm(0, this->ui, *this, env, lyr->CreateMapView(Math::Size2DDbl(320, 240))));
			((AVIRead::AVIRGISForm*)frm)->AddLayer(lyr);
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::ImageList:
		NEW_CLASS(frm, AVIRead::AVIRImageForm(0, this->ui, *this, NotNullPtr<Media::ImageList>::ConvertFrom(pobj)));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::PackageFile:
		NEW_CLASS(frm, AVIRead::AVIRPackageForm(0, this->ui, *this, NotNullPtr<IO::PackageFile>::ConvertFrom(pobj)));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::MediaFile:
		NEW_CLASS(frm, AVIRead::AVIRMediaForm(0, this->ui, *this, (Media::MediaFile*)pobj.Ptr()));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::EXEFile:
		NEW_CLASS(frm, AVIRead::AVIRExeForm(0, this->ui, *this, (IO::EXEFile*)pobj.Ptr()));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::ReadingDB:
		NEW_CLASS(frm, AVIRead::AVIRDBForm(0, this->ui, *this, NotNullPtr<DB::ReadingDB>::ConvertFrom(pobj), true));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::FileCheck:
		NEW_CLASS(frm, AVIRead::AVIRFileChkForm(0, this->ui, *this, NotNullPtr<IO::FileCheck>::ConvertFrom(pobj)));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::LogFile:
		NEW_CLASS(frm, AVIRead::AVIRLogFileForm(0, this->ui, *this, (IO::LogFile*)pobj.Ptr()));
		InitForm(frm);
		frm->Show();
		break;
	case IO::ParserType::Stream:
		{
			UInt64 totalSize = 0;
			UOSInt thisSize;
			IO::Stream *stm = (IO::Stream *)pobj.Ptr();
			IO::MemoryStream *mstm;
			NEW_CLASS(mstm, IO::MemoryStream());
			{
				Data::ByteBuffer buff(1048576);
				while (totalSize < 104857600)
				{
					thisSize = stm->Read(buff);
					if (thisSize == 0)
						break;
					mstm->Write(buff.Ptr(), thisSize);
					totalSize += thisSize;
				}
			}
			if (mstm->GetLength() > 0)
			{
				IO::StmData::MemoryDataCopy data(mstm->GetArray());
				data.SetFullName(stm->GetSourceNameObj()->ToCString());
				DEL_CLASS(mstm);
				this->LoadData(data, 0);
			}
			else
			{
				DEL_CLASS(mstm);
			}
			pobj.Delete();
			break;
		}
	case IO::ParserType::Playlist:
		{
			SSWR::AVIRead::AVIRPlaylistForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRPlaylistForm(0, this->ui, *this, (Media::Playlist*)pobj.Ptr()));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::SectorData:
		{
			SSWR::AVIRead::AVIRSectorForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSectorForm(0, this->ui, *this, NotNullPtr<IO::ISectorData>::ConvertFrom(pobj)));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::CodeProject:
		{
			SSWR::AVIRead::AVIRCodeProjectForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRCodeProjectForm(0, this->ui, *this, (Text::CodeProject*)pobj.Ptr()));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::LUT:
		{
			SSWR::AVIRead::AVIRLUTForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRLUTForm(0, this->ui, *this, (Media::LUT*)pobj.Ptr()));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::FontRenderer:
		{
			SSWR::AVIRead::AVIRFontRendererForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRFontRendererForm(0, this->ui, *this, (Media::FontRenderer*)pobj.Ptr()));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::MIMEObject:
		{
			SSWR::AVIRead::AVIRMIMEViewerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRMIMEViewerForm(0, this->ui, *this, (Text::IMIMEObj *)pobj.Ptr()));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::EthernetAnalyzer:
		{
			SSWR::AVIRead::AVIRRAWMonitorForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRRAWMonitorForm(0, this->ui, *this, (Net::EthernetAnalyzer*)pobj.Ptr()));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::JavaClass:
		{
			SSWR::AVIRead::AVIRJavaClassForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRJavaClassForm(0, this->ui, *this, (IO::JavaClass*)pobj.Ptr()));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::Smake:
		{
			SSWR::AVIRead::AVIRSMakeForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSMakeForm(0, this->ui, *this, NotNullPtr<IO::SMake>::ConvertFrom(pobj)));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::ASN1Data:
		{
			SSWR::AVIRead::AVIRASN1DataForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRASN1DataForm(0, this->ui, *this, NotNullPtr<Net::ASN1Data>::ConvertFrom(pobj)));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::BTScanLog:
		{
			SSWR::AVIRead::AVIRBTScanLogForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRBTScanLogForm(0, this->ui, *this, (IO::BTScanLog*)pobj.Ptr()));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::SystemInfoLog:
		{
			SSWR::AVIRead::AVIRSystemInfoLogForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSystemInfoLogForm(0, this->ui, *this, (IO::SystemInfoLog*)pobj.Ptr()));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::CoordinateSystem:
		{
			SSWR::AVIRead::AVIRCoordSysForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRCoordSysForm(0, this->ui, *this, (Math::CoordinateSystem*)pobj.Ptr()));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::PDFDocument:
		{
			SSWR::AVIRead::AVIRPDFObjectForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRPDFObjectForm(0, this->ui, *this, (Media::PDFDocument*)pobj.Ptr()));
			InitForm(frm);
			frm->Show();
		}
		break;
	case IO::ParserType::JasperReport:
	case IO::ParserType::TextDocument:
	case IO::ParserType::Workbook:
	case IO::ParserType::VectorDocument:
	case IO::ParserType::Unknown:
	default:
		pobj.Delete();
		break;
	}
}

void SSWR::AVIRead::AVIRCoreWin::SaveData(UI::GUIForm *ownerForm, NotNullPtr<IO::ParsedObject> pobj, const WChar *dialogName)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Data::ArrayList<IO::FileExporter*> exp;
	this->exporters.GetSupportedExporters(&exp, pobj);
	if (exp.GetCount() == 0)
	{
		this->ui->ShowMsgOK(CSTR("No supported exporter found"), CSTR("Save"), ownerForm);
	}
	else
	{
		IO::FileExporter *fileExp;
		UTF8Char sbuff1[256];
		UTF8Char sbuff2[256];
		UTF8Char sbuff3[256];
		UTF8Char *sptr;
		Data::ArrayList<IO::FileExporter*> exp2;
		Text::StringBuilderUTF8 sb;

		UI::FileDialog sfd(L"SSWR", L"AVIRead", dialogName, true);
		i = 0;
		j = exp.GetCount();
		while (i < j)
		{
			fileExp = exp.GetItem(i);
			k = 0;
			while (fileExp->GetOutputName(k, sbuff1, sbuff2))
			{
				sfd.AddFilter(Text::CString::FromPtr(sbuff2), Text::CString::FromPtr(sbuff1));
				exp2.Add(fileExp);
				k++;
			}
			i++;
		}
		if ((sptr = pobj->GetSourceName(sbuff3)) != 0)
		{
			if ((i = Text::StrLastIndexOfCharC(sbuff3, (UOSInt)(sptr - sbuff3), '.')) != INVALID_INDEX)
			{
				sbuff3[i] = 0;
				sptr = &sbuff3[i];
			}
			sfd.SetFileName(CSTRP(sbuff3, sptr));
		}
		if (sfd.ShowDialog(ownerForm->GetHandle()))
		{
			IO::FileExporter::SupportType suppType;
			fileExp = exp2.GetItem((UOSInt)sfd.GetFilterIndex());
			suppType = fileExp->IsObjectSupported(pobj);
			if (fileExp->GetParamCnt() > 0)
			{
				void *param;
				param = fileExp->CreateParam(pobj);
				if (param == 0)
				{
					this->ui->ShowMsgOK(CSTR("Error in initializing parameters"), CSTR("Save Data"), ownerForm);
				}
				else
				{
					AVIRExportParamForm dlg(0, this->ui, *this, fileExp, param);
					if (dlg.ShowDialog(ownerForm) == UI::GUIForm::DR_OK)
					{
						if (suppType == IO::FileExporter::SupportType::PathOnly)
						{
							IO::NullStream stm;
							if (!fileExp->ExportFile(stm, sfd.GetFileName()->ToCString(), pobj, param))
							{
								this->ui->ShowMsgOK(CSTR("Error in saving file"), CSTR("Save Data"), ownerForm);
							}
						}
						else
						{
							IO::FileStream fs(sfd.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							if (!fileExp->ExportFile(fs, sfd.GetFileName()->ToCString(), pobj, param))
							{
								this->ui->ShowMsgOK(CSTR("Error in saving file"), CSTR("Save Data"), ownerForm);
							}
						}
					}
					fileExp->DeleteParam(param);
				}
			}
			else
			{
				if (suppType == IO::FileExporter::SupportType::PathOnly)
				{
					IO::NullStream stm;
					if (!fileExp->ExportFile(stm, sfd.GetFileName()->ToCString(), pobj, 0))
					{
						this->ui->ShowMsgOK(CSTR("Error in saving file"), CSTR("Save Data"), ownerForm);
					}
				}
				else
				{
					IO::FileStream fs(sfd.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					if (!fileExp->ExportFile(fs, sfd.GetFileName()->ToCString(), pobj, 0))
					{
						this->ui->ShowMsgOK(CSTR("Error in saving file"), CSTR("Save Data"), ownerForm);
					}
				}
			}
		}
	}
}

Media::Printer *SSWR::AVIRead::AVIRCoreWin::SelectPrinter(UI::GUIForm *frm)
{
	Media::Printer *printer = 0;
	SSWR::AVIRead::AVIRSelPrinterForm selFrm(0, this->ui, *this);
	if (selFrm.ShowDialog(frm) == UI::GUIForm::DR_OK)
	{
		printer = selFrm.printer;
	}
	return printer;
}
