#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Math/Math_C.h"
#include "Media/ImageList.h"
#include "Parser/FullParserList.h"
#include "SSWR/ColorDemo/ColorDemoForm.h"

void __stdcall SSWR::ColorDemo::ColorDemoForm::FileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::ColorDemo::ColorDemoForm> me = userObj.GetNN<SSWR::ColorDemo::ColorDemoForm>();
	if (files.GetCount() > 0)
	{
		UIntOS i = 0;
		while (i < files.GetCount())
		{
			NN<IO::StmData::FileData> fd;
			NEW_CLASSNN(fd, IO::StmData::FileData(files[i], false));
			Optional<Media::ImageList> imgList = Optional<Media::ImageList>::ConvertFrom(me->parsers->ParseFileType(fd, IO::ParserType::ImageList));
			NN<Media::ImageList> nnimgList;
			fd.Delete();
			if (imgList.SetTo(nnimgList))
			{
				if (nnimgList->GetCount() > 0)
				{
					UInt32 t;
					me->currImage.Delete();
					NN<Media::Image> img;
					if (nnimgList->GetImage2(0, t).SetTo(img))
						me->currImage = img->CreateStaticImage();
					nnimgList.Delete();
					me->CreatePrevImage();
					break;
				}
				else
				{
					nnimgList.Delete();
				}
			}
		}
	}
}

void __stdcall SSWR::ColorDemo::ColorDemoForm::OnValueChanged(AnyType userObj, UIntOS scrollPos)
{
	NN<SSWR::ColorDemo::ColorDemoForm> me = userObj.GetNN<SSWR::ColorDemo::ColorDemoForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Double v = (scrollPos - 400) * 0.01;
	sptr = Text::StrDouble(sbuff, v);
	me->currValue = Math_Pow(2, v);
	me->lblValue->SetText(CSTRP(sbuff, sptr));

	if (me->currImage.NotNull())
	{
		me->UpdatePrevImage();
	}
}

void __stdcall SSWR::ColorDemo::ColorDemoForm::OnPBResized(AnyType userObj)
{
	NN<SSWR::ColorDemo::ColorDemoForm> me = userObj.GetNN<SSWR::ColorDemo::ColorDemoForm>();
	me->CreatePrevImage();
}

void SSWR::ColorDemo::ColorDemoForm::CreatePrevImage()
{
	this->pbMain->SetImage(nullptr, false);
	this->currPrevImage.Delete();
	this->currDispImage.Delete();
	NN<Media::StaticImage> currImage;
	if (this->currImage.SetTo(currImage))
	{
		this->currPrevImage = this->pbMain->CreatePreviewImage(currImage);
		if (this->currPrevImage.SetTo(currImage))
		{
			this->currDispImage = currImage->CreateStaticImage();
			this->UpdatePrevImage();
		}
	}
}

void SSWR::ColorDemo::ColorDemoForm::UpdatePrevImage()
{
	NN<Media::StaticImage> currPrevImage;
	NN<Media::StaticImage> currDispImage;
	if (!this->currPrevImage.SetTo(currPrevImage) || !this->currDispImage.SetTo(currDispImage))
	{
		this->pbMain->SetImage(nullptr, false);
		return;
	}
	Media::ColorProfile color(currPrevImage->info.color);
	if (color.GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		color.GetRTranParam()->Set(NN<const Media::CS::TransferParam>(this->colorMgr->GetDefPProfile()->GetRTranParam()));
		color.GetGTranParam()->Set(NN<const Media::CS::TransferParam>(this->colorMgr->GetDefPProfile()->GetGTranParam()));
		color.GetBTranParam()->Set(NN<const Media::CS::TransferParam>(this->colorMgr->GetDefPProfile()->GetBTranParam()));
	}
	else if (color.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		color.GetRTranParam()->Set(NN<const Media::CS::TransferParam>(this->colorMgr->GetDefVProfile()->GetRTranParam()));
		color.GetGTranParam()->Set(NN<const Media::CS::TransferParam>(this->colorMgr->GetDefVProfile()->GetGTranParam()));
		color.GetBTranParam()->Set(NN<const Media::CS::TransferParam>(this->colorMgr->GetDefVProfile()->GetBTranParam()));
	}
	this->rgbFilter->SetParameter(0, this->currValue, 1.0, color, currPrevImage->info.storeBPP, currPrevImage->info.pf, 200);
	this->rgbFilter->ProcessImage(currPrevImage->data, currDispImage->data, currPrevImage->info.dispSize.x, currPrevImage->info.dispSize.y, currPrevImage->info.storeSize.x * (currPrevImage->info.storeBPP >> 3), currPrevImage->info.storeSize.x * (currPrevImage->info.storeBPP >> 3), false);
	this->pbMain->SetImage(currDispImage, true);
}

SSWR::ColorDemo::ColorDemoForm::ColorDemoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Media::ColorManager> colorMgr) : UI::GUIForm(parent, 1024, 480, ui)
{
	this->SetText(CSTR("ColorDemo"));
	this->HandleDropFiles(FileHandler, this);

	NEW_CLASSNN(this->parsers, Parser::FullParserList());
	NEW_CLASSNN(this->rgbFilter, Media::RGBColorFilter(colorMgr));
	this->colorMgr = colorMgr;
	this->colorSess = this->colorMgr->CreateSess(this->GetHMonitor());
	this->currValue = 1;
	this->currImage = nullptr;
	this->currPrevImage = nullptr;
	this->currDispImage = nullptr;

	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetRect(0, 0, 10, 3 + 1, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblValue = ui->NewLabel(this->pnlMain, CSTR("0"));
	this->lblValue->SetRect(0, 0, 50, 24, false);
	this->lblValue->SetDockType(UI::GUIControl::DOCK_RIGHT);
/*	this->tbValue = ui->NewTrackBar(this->pnlMain, 0, 800, 400);
	this->tbValue->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tbValue->HandleScrolled(OnValueChanged, this);*/
	this->hsbValue = ui->NewHScrollBar(this->pnlMain, 3);
	this->hsbValue->SetDockType(UI::GUIControl::DOCK_FILL);
	this->hsbValue->InitScrollBar(0, 809, 400, 10);
	this->hsbValue->HandlePosChanged(OnValueChanged, this);
	this->pbMain = ui->NewPictureBoxDD(*this, this->colorSess, false, false);
	this->pbMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbMain->HandleSizeChanged(OnPBResized, this);
}

SSWR::ColorDemo::ColorDemoForm::~ColorDemoForm()
{
	this->parsers.Delete();
	this->rgbFilter.Delete();
	this->currImage.Delete();
	this->currPrevImage.Delete();
	this->currDispImage.Delete();
	this->colorMgr->DeleteSess(this->colorSess);
}

void SSWR::ColorDemo::ColorDemoForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
}
