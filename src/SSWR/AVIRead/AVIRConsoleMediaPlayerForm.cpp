#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Media/ICCProfile.h"
#include "SSWR/AVIRead/AVIRCaptureDevForm.h"
#include "SSWR/AVIRead/AVIRConsoleMediaPlayerForm.h"

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnStopClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm>();
	me->player->PBStop();
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnCaptureDevClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm>();
	SSWR::AVIRead::AVIRCaptureDevForm dlg(0, me->ui, me->core);
	NN<Media::IVideoCapture> capture;
	me->player->CloseFile();
	if (dlg.ShowDialog(me) == UI::GUIForm::DR_OK && dlg.capture.SetTo(capture))
	{
		UTF8Char sbuff[256];
		UnsafeArray<UTF8Char> sptr;
		NN<Media::MediaFile> mf;
		sbuff[0] = 0;
		sptr = capture->GetSourceName(sbuff).Or(sbuff);
		NEW_CLASSNN(mf, Media::MediaFile(CSTRP(sbuff, sptr)));
		mf->AddSource(capture, 0);
		if (me->player->OpenVideo(mf))
		{
			me->UpdateColorDisp();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm>();
	UOSInt i = 0;
	UOSInt nFiles = files.GetCount();
	while (i < nFiles)
	{
		if (files[i]->EndsWith(UTF8STRC(".icm")) || files[i]->EndsWith(UTF8STRC(".icc")))
		{
			me->OpenICC(files[i]->ToCString());
			return;
		}
		else if (me->player->OpenFile(files[i]->ToCString(), IO::ParserType::MediaFile))
		{
			me->UpdateColorDisp();
			return;
		}
		i++;
	}
	me->ui->ShowMsgOK(CSTR("Error in loading files"), CSTR("Console Media Player"), me);
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnRotateChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm>();
	me->player->SetRotateType((Media::RotateType)me->cboRotate->GetSelectedItem().GetOSInt());
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnSurfaceBugChg(AnyType userObj, Bool newVal)
{
	NN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm>();
	me->player->SetSurfaceBugMode(me->chkSurfaceBug->IsChecked());
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnYUVTypeChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm>();
	if (!me->videoOpening)
		me->player->GetVideoRenderer()->SetSrcYUVType((Media::ColorProfile::YUVType)me->cboYUVType->GetSelectedItem().GetOSInt());
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnRGBTransChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm>();
	if (!me->videoOpening)
		me->player->GetVideoRenderer()->SetSrcRGBType((Media::CS::TransferType)me->cboRGBTrans->GetSelectedItem().GetOSInt());
}

void __stdcall SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnColorPrimariesChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRConsoleMediaPlayerForm>();
	if (!me->videoOpening)
		me->player->GetVideoRenderer()->SetSrcPrimaries((Media::ColorProfile::ColorType)me->cboColorPrimaries->GetSelectedItem().GetOSInt());
}

void SSWR::AVIRead::AVIRConsoleMediaPlayerForm::AddYUVType(Media::ColorProfile::YUVType yuvType)
{
	this->cboYUVType->AddItem(Media::ColorProfile::YUVTypeGetName(yuvType), (void*)(OSInt)yuvType);
}

void SSWR::AVIRead::AVIRConsoleMediaPlayerForm::AddRGBTrans(Media::CS::TransferType rgbType)
{
	this->cboRGBTrans->AddItem(Media::CS::TransferTypeGetName(rgbType), (void*)(OSInt)rgbType);
}

void SSWR::AVIRead::AVIRConsoleMediaPlayerForm::AddColorPrimaries(Media::ColorProfile::ColorType colorType)
{
	this->cboColorPrimaries->AddItem(Media::ColorProfile::ColorTypeGetName(colorType), (void*)(OSInt)colorType);
}

void SSWR::AVIRead::AVIRConsoleMediaPlayerForm::UpdateColorDisp()
{
	Media::VideoRenderer::RendererStatus2 status;
	this->videoOpening = true;
	this->player->GetVideoRenderer()->GetStatus(status);
	UOSInt i;
	i = this->cboYUVType->GetCount();
	while (i-- > 0)
	{
		if (this->cboYUVType->GetItem(i) == (void*)status.srcYUVType)
		{
			this->cboYUVType->SetSelectedIndex(i);
			break;
		}
	}

	Media::CS::TransferType tranType = status.color.rtransfer.GetTranType();
	i = this->cboRGBTrans->GetCount();
	while (i-- > 0)
	{
		if (this->cboRGBTrans->GetItem(i) == (void*)tranType)
		{
			this->cboRGBTrans->SetSelectedIndex(i);
			break;
		}
	}

	i = this->cboColorPrimaries->GetCount();
	while (i-- > 0)
	{
		if (this->cboColorPrimaries->GetItem(i) == (void*)status.color.primaries.colorType)
		{
			this->cboColorPrimaries->SetSelectedIndex(i);
			break;
		}
	}
	this->videoOpening = false;
}

Bool SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OpenICC(Text::CStringNN iccFile)
{
	IO::FileStream fs(iccFile, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	Bool succ = false;
	UInt64 len = fs.GetLength();
	Bool changed = false;
	if (len > 4 && len <= 16384)
	{
		Data::ByteBuffer buff((UOSInt)len);
		if (fs.Read(buff) == len)
		{
			NN<Media::ICCProfile> icc;
			if (Media::ICCProfile::Parse(buff).SetTo(icc))
			{
				Media::CS::TransferParam param;
				if (icc->GetRedTransferParam(param))
				{
					this->player->GetVideoRenderer()->SetSrcRGBTransfer(param);
					changed = true;
				}
				Media::ColorProfile::ColorPrimaries primaries;
				if (icc->GetColorPrimaries(primaries))
				{
					this->player->GetVideoRenderer()->SetSrcPrimaries(primaries);
					changed = true;
				}
				icc.Delete();
				succ = true;
			}
		}
	}
	if (changed)
	{
		this->UpdateColorDisp();
	}
	return succ;
}

SSWR::AVIRead::AVIRConsoleMediaPlayerForm::AVIRConsoleMediaPlayerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 320, 240, ui)
{
	this->SetText(CSTR("Console Media Player"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->listener = 0;
	this->videoOpening = false;
	NEW_CLASS(this->player, Media::ConsoleMediaPlayer(this->core->GetMonitorMgr(), this->core->GetColorMgr(), this->core->GetParserList(), this->core->GetAudioDevice()));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblPort = ui->NewLabel(*this, CSTR("Control Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR(""));
	this->txtPort->SetRect(104, 4, 60, 23, false);
	this->txtPort->SetReadOnly(true);
	this->btnStop = ui->NewButton(*this, CSTR("Stop"));
	this->btnStop->SetRect(4, 28, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClicked, this);
	this->btnCaptureDev = ui->NewButton(*this, CSTR("Capture Device"));
	this->btnCaptureDev->SetRect(4, 52, 150, 23, false);
	this->btnCaptureDev->HandleButtonClick(OnCaptureDevClicked, this);
	this->lblRotate = ui->NewLabel(*this, CSTR("Rotate"));
	this->lblRotate->SetRect(4, 76, 100, 23, false);
	this->cboRotate = ui->NewComboBox(*this, false);
	this->cboRotate->SetRect(104, 76, 100, 23, false);
	this->cboRotate->AddItem(CSTR("No Rotate"), (void*)Media::RotateType::None);
	this->cboRotate->AddItem(CSTR("CW 90"), (void*)Media::RotateType::CW_90);
	this->cboRotate->AddItem(CSTR("CW 180"), (void*)Media::RotateType::CW_180);
	this->cboRotate->AddItem(CSTR("CW 270"), (void*)Media::RotateType::CW_270);
	this->cboRotate->AddItem(CSTR("HFlip"), (void*)Media::RotateType::HFLIP);
	this->cboRotate->AddItem(CSTR("HF CW 90"), (void*)Media::RotateType::HFLIP_CW_90);
	this->cboRotate->AddItem(CSTR("HF CW 180"), (void*)Media::RotateType::HFLIP_CW_180);
	this->cboRotate->AddItem(CSTR("HF CW 270"), (void*)Media::RotateType::HFLIP_CW_270);
	this->cboRotate->SetSelectedIndex(0);
	this->cboRotate->HandleSelectionChange(OnRotateChg, this);
	this->chkSurfaceBug = ui->NewCheckBox(*this, CSTR("Surface Bug"), true);
	this->chkSurfaceBug->SetRect(4, 100, 200, 23, false);
	this->chkSurfaceBug->HandleCheckedChange(OnSurfaceBugChg, this);
	this->lblYUVType = ui->NewLabel(*this, CSTR("YUV Type"));
	this->lblYUVType->SetRect(4, 124, 100, 23, false);
	this->cboYUVType = ui->NewComboBox(*this, false);
	this->cboYUVType->SetRect(104, 124, 150, 23, false);
	this->AddYUVType(Media::ColorProfile::YUVT_BT601);
	this->AddYUVType(Media::ColorProfile::YUVT_BT709);
	this->AddYUVType(Media::ColorProfile::YUVT_FCC);
	this->AddYUVType(Media::ColorProfile::YUVT_BT470BG);
	this->AddYUVType(Media::ColorProfile::YUVT_SMPTE170M);
	this->AddYUVType(Media::ColorProfile::YUVT_SMPTE240M);
	this->AddYUVType(Media::ColorProfile::YUVT_BT2020);
	this->AddYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_BT601));
	this->AddYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_BT709));
	this->AddYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_FCC));
	this->AddYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_BT470BG));
	this->AddYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_SMPTE170M));
	this->AddYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_SMPTE240M));
	this->AddYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_BT2020));
	this->cboYUVType->SetSelectedIndex(0);
	this->cboYUVType->HandleSelectionChange(OnYUVTypeChg, this);
	this->lblRGBTrans = ui->NewLabel(*this, CSTR("RGB Transfer"));
	this->lblRGBTrans->SetRect(4, 148, 100, 23, false);
	this->cboRGBTrans = ui->NewComboBox(*this, false);
	this->cboRGBTrans->SetRect(104, 148, 150, 23, false);
	this->AddRGBTrans(Media::CS::TRANT_sRGB);
	this->AddRGBTrans(Media::CS::TRANT_BT709);
	this->AddRGBTrans(Media::CS::TRANT_GAMMA);
	this->AddRGBTrans(Media::CS::TRANT_SMPTE240);
	this->AddRGBTrans(Media::CS::TRANT_LINEAR);
	this->AddRGBTrans(Media::CS::TRANT_BT1361);
	this->AddRGBTrans(Media::CS::TRANT_BT2100);
	this->AddRGBTrans(Media::CS::TRANT_LOG100);
	this->AddRGBTrans(Media::CS::TRANT_LOGSQRT10);
	this->AddRGBTrans(Media::CS::TRANT_PROTUNE);
	this->AddRGBTrans(Media::CS::TRANT_NTSC);
	this->AddRGBTrans(Media::CS::TRANT_HLG);
	this->AddRGBTrans(Media::CS::TRANT_SLOG);
	this->AddRGBTrans(Media::CS::TRANT_SLOG1);
	this->AddRGBTrans(Media::CS::TRANT_SLOG2);
	this->AddRGBTrans(Media::CS::TRANT_SLOG3);
	this->AddRGBTrans(Media::CS::TRANT_VLOG);
	this->AddRGBTrans(Media::CS::TRANT_NLOG);
	this->cboRGBTrans->SetSelectedIndex(0);
	this->cboRGBTrans->HandleSelectionChange(OnRGBTransChg, this);
	this->lblColorPrimaries = ui->NewLabel(*this, CSTR("Color Primaries"));
	this->lblColorPrimaries->SetRect(4, 172, 100, 23, false);
	this->cboColorPrimaries = ui->NewComboBox(*this, false);
	this->cboColorPrimaries->SetRect(104, 172, 150, 23, false);
	this->AddColorPrimaries(Media::ColorProfile::CT_VUNKNOWN);
	this->AddColorPrimaries(Media::ColorProfile::CT_SRGB);
	this->AddColorPrimaries(Media::ColorProfile::CT_BT470M);
	this->AddColorPrimaries(Media::ColorProfile::CT_BT470BG);
	this->AddColorPrimaries(Media::ColorProfile::CT_SMPTE170M);
	this->AddColorPrimaries(Media::ColorProfile::CT_SMPTE240M);
	this->AddColorPrimaries(Media::ColorProfile::CT_GENERIC_FILM);
	this->AddColorPrimaries(Media::ColorProfile::CT_BT2020);
	this->AddColorPrimaries(Media::ColorProfile::CT_ADOBE);
	this->AddColorPrimaries(Media::ColorProfile::CT_APPLE);
	this->AddColorPrimaries(Media::ColorProfile::CT_CIERGB);
	this->AddColorPrimaries(Media::ColorProfile::CT_COLORMATCH);
	this->AddColorPrimaries(Media::ColorProfile::CT_WIDE);
	this->AddColorPrimaries(Media::ColorProfile::CT_SGAMUT);
	this->AddColorPrimaries(Media::ColorProfile::CT_SGAMUTCINE);
	this->AddColorPrimaries(Media::ColorProfile::CT_DCI_P3);
	this->AddColorPrimaries(Media::ColorProfile::CT_ACESGAMUT);
	this->AddColorPrimaries(Media::ColorProfile::CT_ALEXAWIDE);
	this->AddColorPrimaries(Media::ColorProfile::CT_VGAMUT);
	this->AddColorPrimaries(Media::ColorProfile::CT_GOPRO_PROTUNE);
	this->cboColorPrimaries->SetSelectedIndex(0);
	this->cboColorPrimaries->HandleSelectionChange(OnColorPrimariesChg, this);

	if (this->player->IsError())
	{
		this->ui->ShowMsgOK(CSTR("Error in initialize player"), CSTR("Console Media Player"), this);
	}
	this->player->SetSurfaceBugMode(true);

	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UInt16 port = 8080;
	NEW_CLASSNN(this->webIface, Media::MediaPlayerWebInterface(this->player, false));
	while (port < 8090)
	{
		NEW_CLASS(this->listener, Net::WebServer::WebListener(this->core->GetSocketFactory(), 0, this->webIface, port, 10, 1, 2, CSTR("ConsoleMediaPlayer/1.0"), false, Net::WebServer::KeepAlive::Default, true));
		if (this->listener->IsError())
		{
			DEL_CLASS(this->listener);
			this->listener = 0;
			port++;
		}
		else
		{
			sptr = Text::StrUInt16(sbuff, port);
			this->txtPort->SetText(CSTRP(sbuff, sptr));
			break;
		}
	}
	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRConsoleMediaPlayerForm::~AVIRConsoleMediaPlayerForm()
{
	SDEL_CLASS(this->listener);
	this->webIface.Delete();
	DEL_CLASS(this->player);
}

void SSWR::AVIRead::AVIRConsoleMediaPlayerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
