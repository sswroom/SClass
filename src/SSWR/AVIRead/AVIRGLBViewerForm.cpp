#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "SSWR/AVIRead/AVIRGLBViewerForm.h"
#include "Text/JSText.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRGLBViewerForm::OnFileDrop(void *userObj, Text::String **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRGLBViewerForm *me = (SSWR::AVIRead::AVIRGLBViewerForm*)userObj;
	UOSInt i = 0;
	while (i < nFiles)
	{
		if (me->LoadFile(files[i]))
		{
			break;
		}
	}
}

Bool SSWR::AVIRead::AVIRGLBViewerForm::LoadFile(Text::String *fileName)
{
	UInt8 hdr[20];
	IO::StmData::FileData fd(fileName, false);
	if (fd.GetRealData(0, 20, hdr) != 20)
	{
		return false;
	}
	UInt64 fileLen = fd.GetDataSize();
	if (ReadNInt32(hdr) != *(Int32*)"glTF" || ReadUInt32(&hdr[8]) != fileLen)
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
		UInt8 *jsonBuff = MemAlloc(UInt8, jsonLen);
		if (fd.GetRealData(20, jsonLen, jsonBuff) != jsonLen)
		{
			MemFree(jsonBuff);
			return false;
		}
		Text::StringBuilderUTF8 sb;
		Text::JSText::JSONWellFormat(jsonBuff, jsonLen, 0, &sb);
		this->txtJSON->SetText(sb.ToCString());
		MemFree(jsonBuff);
		this->hfvBinBuff->LoadData(fd.GetPartialData(20 + jsonLen, fileLen - 20 - jsonLen));
		return true;
	}
	else if (ver == 2)
	{
		if (ReadNInt32(&hdr[16]) != *(Int32*)"JSON")
		{
			return false;
		}
		UInt8 *jsonBuff = MemAlloc(UInt8, jsonLen);
		if (fd.GetRealData(20, jsonLen, jsonBuff) != jsonLen)
		{
			MemFree(jsonBuff);
			return false;
		}
		Text::StringBuilderUTF8 sb;
		Text::JSText::JSONWellFormat(jsonBuff, jsonLen, 0, &sb);
		this->txtJSON->SetText(sb.ToCString());
		MemFree(jsonBuff);
		UInt64 ofst = 20 + jsonLen;
		if (fd.GetRealData(ofst, 8, &hdr[12]) != 8)
		{
			return false;
		}
		if (ReadNInt32(&hdr[16]) != *(Int32*)"BIN")
		{
			return false;
		}
		this->hfvBinBuff->LoadData(fd.GetPartialData(ofst + 8, fileLen - 8 - ofst));
		return true;
	}
	else
	{
		return false;
	}
}

SSWR::AVIRead::AVIRGLBViewerForm::AVIRGLBViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("GLB Viewer"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlBinBuff, UI::GUIPanel(ui, this));
	this->pnlBinBuff->SetRect(0, 0, 100, 200, false);
	this->pnlBinBuff->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->vspMain, UI::GUIVSplitter(ui, this, 3, true));
	NEW_CLASS(this->pnlJSON, UI::GUIPanel(ui, this));
	this->pnlJSON->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblJSON, UI::GUILabel(ui, this->pnlJSON, CSTR("JSON")));
	this->lblJSON->SetRect(0, 0, 100, 23, false);
	this->lblJSON->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->txtJSON, UI::GUITextBox(ui, this->pnlJSON, CSTR(""), true));
	this->txtJSON->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtJSON->SetReadOnly(true);
	NEW_CLASS(this->lblBinBuff, UI::GUILabel(ui, this->pnlBinBuff, CSTR("Binary Buffer")));
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
