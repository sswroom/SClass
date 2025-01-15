#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "SSWR/AVIRead/AVIROCRForm.h"

void __stdcall SSWR::AVIRead::AVIROCRForm::OnFileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIROCRForm> me = userObj.GetNN<SSWR::AVIRead::AVIROCRForm>();
	NN<Parser::ParserList> parsers = me->core->GetParserList();
	UOSInt i = 0;
	UOSInt nFiles = files.GetCount();
	while (i < nFiles)
	{
		IO::StmData::FileData fd(files[i], false);
		NN<Media::ImageList> imgList;
		if (Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::ImageList)).SetTo(imgList))
		{
			imgList->ToStaticImage(0);
			NN<Media::StaticImage> img;
			if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(img))
			{
				me->currImg.Delete();
				me->currImg = img;
				imgList->RemoveImage(0, false);
				imgList.Delete();
				me->pbImg->SetImage(img, false);

				me->lvText->ClearItems();
				me->ClearResults();
				me->ocr.SetParsingImage(img);
				me->ocr.ParseAllInImage();
				break;
			}
			imgList.Delete();
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIROCRForm::OnTextSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROCRForm> me = userObj.GetNN<SSWR::AVIRead::AVIROCRForm>();
	NN<ResultInfo> res;
	if (me->lvText->GetSelectedItem().GetOpt<ResultInfo>().SetTo(res))
	{
		me->pbResult->SetImage(res->resImg);
	}
}

void __stdcall SSWR::AVIRead::AVIROCRForm::OnOCRResult(AnyType userObj, NN<Text::String> txt, Double confidence, Math::RectArea<OSInt> boundary)
{
	NN<SSWR::AVIRead::AVIROCRForm> me = userObj.GetNN<SSWR::AVIRead::AVIROCRForm>();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Media::StaticImage> currImg;
	if (boundary.GetArea() >= 1000 && me->currImg.SetTo(currImg))
	{
		NN<ResultInfo> res = MemAllocNN(ResultInfo);
		res->result = txt->Clone();
		res->confidence = confidence;
		res->area = boundary;
		res->resImg = currImg->CreateSubImage(boundary);
		me->results.Add(res);
		UOSInt i = me->lvText->AddItem(res->result, res.Ptr());
		sptr = Text::StrOSInt(sbuff, boundary.GetArea());
		me->lvText->SetSubItem(i, 1, CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, res->resImg->CalcColorRate());
		me->lvText->SetSubItem(i, 2, CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, confidence);
		me->lvText->SetSubItem(i, 3, CSTRP(sbuff, sptr));
	}
}

void SSWR::AVIRead::AVIROCRForm::ClearResults()
{
	NN<ResultInfo> res;
	Data::ArrayIterator<NN<ResultInfo>> it = this->results.Iterator();
	while (it.HasNext())
	{
		res = it.Next();
		res->result->Release();
		res->resImg.Delete();
		MemFreeNN(res);
	}
	this->results.Clear();
}

SSWR::AVIRead::AVIROCRForm::AVIROCRForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui), ocr(Media::OCREngine::Language::English)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("OCR"));

	this->core = core;
	this->currImg = 0;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->ocr.SetCharWhiteList("0123456789ABCDEFGHIJKLMNPQRSTUVWXYZ");
	this->ocr.HandleOCRResult(OnOCRResult, this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlResult = ui->NewPanel(*this);
	this->pnlResult->SetRect(0, 0, 250, 100, false);
	this->pnlResult->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->pbResult = ui->NewPictureBoxSimple(this->pnlResult, this->core->GetDrawEngine(), false);
	this->pbResult->SetRect(0, 0, 100, 80, false);
	this->pbResult->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvText = ui->NewListView(this->pnlResult, UI::ListViewStyle::Table, 4);
	this->lvText->SetRect(0, 0, 250, 100, false);
	this->lvText->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvText->AddColumn(CSTR("Result"), 100);
	this->lvText->AddColumn(CSTR("Area"), 50);
	this->lvText->AddColumn(CSTR("Color Rate"), 50);
	this->lvText->AddColumn(CSTR("Confidence"), 50);
	this->lvText->HandleSelChg(OnTextSelChg, this);
	this->hspText = ui->NewHSplitter(*this, 3, true);
	this->pbImg = ui->NewPictureBoxDD(*this, this->colorSess, true, false);
	this->pbImg->SetDockType(UI::GUIControl::DOCK_FILL);

	this->HandleDropFiles(OnFileHandler, this);
}

SSWR::AVIRead::AVIROCRForm::~AVIROCRForm()
{
	this->ClearResults();
	this->currImg.Delete();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIROCRForm::OnMonitorChanged()
{
	this->pbImg->ChangeMonitor(this->GetHMonitor());
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
