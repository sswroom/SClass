#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "IO/StmData/FileData.h"
#include "SSWR/AVIRead/AVIRGLBViewerForm.h"
#include "Text/JSText.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRGLBViewerForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRGLBViewerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGLBViewerForm>();
	UOSInt i = 0;
	UOSInt nFiles = files.GetCount();
	while (i < nFiles)
	{
		if (me->LoadFile(files[i]))
		{
			break;
		}
		i++;
	}
}

Bool SSWR::AVIRead::AVIRGLBViewerForm::LoadFile(NN<Text::String> fileName)
{
	UInt8 hdr[40];
	IO::StmData::FileData fd(fileName, false);
	if (fd.GetRealData(0, 40, BYTEARR(hdr)) != 40)
	{
		return false;
	}
	UInt64 fileOfst = 0;
	UInt64 fileLen = fd.GetDataSize();
	if (ReadNInt32(hdr) == *(Int32*)"b3dm" && ReadUInt32(&hdr[4]) == 1 && ReadUInt32(&hdr[8]) == fileLen && ReadNInt32(&hdr[28]) == *(Int32*)"glTF" && ReadUInt32(&hdr[36]) == fileLen - 28)
	{
		fileOfst = 28;
		if (fd.GetRealData(28, 40, BYTEARR(hdr)) != 40)
		{
			return false;
		}
		fileLen -= 28;
	}
	else if (ReadNInt32(hdr) == *(Int32*)"glTF" && ReadUInt32(&hdr[8]) == fileLen)
	{
	}
	else
	{
		return false;
	}
	UInt32 ver = ReadUInt32(&hdr[4]);
	UInt32 jsonLen = ReadUInt32(&hdr[12]);
	if (jsonLen + 20 > fileLen)
	{
		return false;
	}
	if (ver == 1)
	{
		if (ReadUInt32(&hdr[16]) != 0)
		{
			return false;
		}
		UInt64 ofst = 20 + jsonLen;
		return this->LoadData(fd.GetPartialData(fileOfst + 20, jsonLen), fd.GetPartialData(fileOfst + ofst, fileLen - ofst));
	}
	else if (ver == 2)
	{
		if (ReadNInt32(&hdr[16]) != *(Int32*)"JSON")
		{
			return false;
		}
		UInt64 ofst = 20 + jsonLen;
		if (fd.GetRealData(fileOfst + ofst, 8, BYTEARR(hdr).SubArray(12)) != 8)
		{
			return false;
		}
		if (ReadNInt32(&hdr[16]) != *(Int32*)"BIN")
		{
			return false;
		}
		return this->LoadData(fd.GetPartialData(fileOfst + 20, jsonLen), fd.GetPartialData(fileOfst + ofst + 8, fileLen - 8 - ofst));
	}
	else
	{
		return false;
	}
}

Bool SSWR::AVIRead::AVIRGLBViewerForm::LoadData(NN<IO::StreamData> jsonFD, NN<IO::StreamData> binBuffFD)
{
	UOSInt jsonLen = (UOSInt)jsonFD->GetDataSize();
	Data::ByteBuffer jsonBuff(jsonLen);
	if (jsonFD->GetRealData(0, jsonLen, jsonBuff) != jsonLen)
	{
		jsonFD.Delete();
		binBuffFD.Delete();
		return false;
	}
	Text::StringBuilderUTF8 sb;
	Text::JSText::JSONWellFormat(jsonBuff.Arr(), jsonLen, 0, sb);
	this->txtJSON->SetText(sb.ToCString());
	this->hfvBinBuff->LoadData(binBuffFD, 0);
	jsonFD.Delete();
	return true;
}

SSWR::AVIRead::AVIRGLBViewerForm::AVIRGLBViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("GLB Viewer"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlBinBuff = ui->NewPanel(*this);
	this->pnlBinBuff->SetRect(0, 0, 100, 200, false);
	this->pnlBinBuff->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->vspMain = ui->NewVSplitter(*this, 3, true);
	this->pnlJSON = ui->NewPanel(*this);
	this->pnlJSON->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblJSON = ui->NewLabel(this->pnlJSON, CSTR("JSON"));
	this->lblJSON->SetRect(0, 0, 100, 23, false);
	this->lblJSON->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->txtJSON = ui->NewTextBox(this->pnlJSON, CSTR(""), true);
	this->txtJSON->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtJSON->SetReadOnly(true);
	this->lblBinBuff = ui->NewLabel(this->pnlBinBuff, CSTR("Binary Buffer"));
	this->lblBinBuff->SetRect(0, 0, 100, 23, false);
	this->lblBinBuff->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->hfvBinBuff, UI::GUIHexFileView(ui, this->pnlBinBuff, this->core->GetDrawEngine()));
	this->hfvBinBuff->SetDockType(UI::GUIControl::DOCK_FILL);

	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRGLBViewerForm::~AVIRGLBViewerForm()
{
}

void SSWR::AVIRead::AVIRGLBViewerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
