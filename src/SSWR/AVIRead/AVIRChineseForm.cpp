#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRChineseForm.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FontDialog.h"
#include "UI/MessageDialog.h"
#include <wchar.h>

typedef enum
{
	MNU_CHARCODE = 100,
	MNU_RELATED
} MenuItems;

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnCharChg(void *userObj)
{
	SSWR::AVIRead::AVIRChineseForm *me = (SSWR::AVIRead::AVIRChineseForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UTF32Char v;
	me->txtChar->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		me->txtChar->SetText((const UTF8Char*)"");

		Text::StrReadChar(sb.ToString(), &v);
		me->UpdateChar((UInt32)v);
		me->txtRadical->Focus();
	}
}

Bool __stdcall SSWR::AVIRead::AVIRChineseForm::OnCharMouseDown(void *userObj, OSInt x, OSInt y, UI::GUIControl::MouseButton btn)
{
	SSWR::AVIRead::AVIRChineseForm *me = (SSWR::AVIRead::AVIRChineseForm *)userObj;
	UI::FontDialog *dlg;
	NEW_CLASS(dlg, UI::FontDialog(me->currFont, 12, false, false));
	if (dlg->ShowDialog(me->GetHandle()) == UI::GUIForm::DR_OK)
	{
		SDEL_TEXT(me->currFont);
		me->currFont = Text::StrCopyNew(dlg->GetFontName());
		me->UpdateImg();
	}
	DEL_CLASS(dlg);
	return false;
}

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnCharPrevClicked(void *userObj)
{
	SSWR::AVIRead::AVIRChineseForm *me = (SSWR::AVIRead::AVIRChineseForm *)userObj;
	if (me->currChar > 0)
	{
		me->UpdateChar(me->currChar - 1);
	}
}

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnCharNextClicked(void *userObj)
{
	SSWR::AVIRead::AVIRChineseForm *me = (SSWR::AVIRead::AVIRChineseForm *)userObj;
	me->UpdateChar(me->currChar + 1);
}

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnRadicalChg(void *userObj)
{
	SSWR::AVIRead::AVIRChineseForm *me = (SSWR::AVIRead::AVIRChineseForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[7];
	UTF32Char v;
	me->txtRadical->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		me->txtRadical->SetText((const UTF8Char*)"");

		Text::StrReadChar(sb.ToString(), &v);
		me->currRadical = (UInt32)v;
		Text::StrWriteChar(sbuff, v)[0] = 0;
		me->lblRadicalV->SetText(sbuff);
	}
}

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnRelatedAddChg(void *userObj)
{
	SSWR::AVIRead::AVIRChineseForm *me = (SSWR::AVIRead::AVIRChineseForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UTF32Char v;
	UOSInt i;
	UOSInt j;
	me->txtRelatedAdd->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		me->txtRelatedAdd->SetText((const UTF8Char*)"");

		Text::StrReadChar(sb.ToString(), &v);
		if (me->currChar != 0)
		{
			if (me->currChar == (UInt32)v)
			{

			}
			else
			{
				Data::ArrayList<UInt32> relatedList;
				me->chinese->GetRelatedChars(me->currChar, &relatedList);
				i = relatedList.GetCount();
				while (i-- > 0)
				{
					if (relatedList.GetItem(i) == (UInt32)v)
					{
						return;
					}
				}

				relatedList.Clear();
				relatedList.Add((UInt32)v);
				me->chinese->GetRelatedChars(me->currChar, &relatedList);

				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Are you sure that \""));
				Text::StrWriteChar(sbuff, (UTF32Char)me->currChar)[0] = 0;
				sb.Append(sbuff);
				sb.AppendC(UTF8STRC("\" is related to \""));
				i = 0;
				j = relatedList.GetCount();
				sptr = sbuff;
				while (i < j)
				{
					sptr = Text::StrWriteChar(sptr, (UTF32Char)relatedList.GetItem(i));
					i++;
				}
				*sptr = 0;
				sb.Append(sbuff);
				sb.AppendC(UTF8STRC("\"?"));
				if (UI::MessageDialog::ShowYesNoDialog(sb.ToString(), (const UTF8Char*)"Add Relation", me))
				{
					me->chinese->AddRelation(me->currChar, (UInt32)v);
					me->UpdateRelation();
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnRelatedGoClicked(void *userObj)
{
	SSWR::AVIRead::AVIRChineseForm *me = (SSWR::AVIRead::AVIRChineseForm *)userObj;
	Data::ArrayList<UInt32> relatedChars;
	me->chinese->GetRelatedChars(me->currChar, &relatedChars);
	if (relatedChars.GetCount() > 0)
	{
		me->UpdateChar(relatedChars.GetItem(0));
	}
}

Bool SSWR::AVIRead::AVIRChineseForm::SaveChar()
{
	if (this->currChar == 0)
		return true;
	Int32 strokeCnt;
	Text::StringBuilderUTF8 sb;
	Text::ChineseInfo::CharacterInfo chInfo;
	this->txtStrokeCount->GetText(&sb);
	if (!sb.ToInt32(&strokeCnt))
	{
		wprintf(L"strokeCount error\r\n");
		return false;
	}
	chInfo.strokeCount = (UInt8)strokeCnt;
	chInfo.radical = this->currRadical;
	sb.ClearStr();
	this->txtPronun1->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		chInfo.cantonPronun[0] = 0;
	}
	else
	{
		chInfo.cantonPronun[0] = this->chinese->Cantonese2Int(sb.ToString());
		if (chInfo.cantonPronun[0] == 0)
		{
			wprintf(L"Pronun1 error\r\n");
			return false;
		}
	}
	sb.ClearStr();
	this->txtPronun2->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		chInfo.cantonPronun[1] = 0;
	}
	else
	{
		chInfo.cantonPronun[1] = this->chinese->Cantonese2Int(sb.ToString());
		if (chInfo.cantonPronun[1] == 0)
		{
			wprintf(L"Pronun2 error\r\n");
			return false;
		}
	}
	sb.ClearStr();
	this->txtPronun3->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		chInfo.cantonPronun[2] = 0;
	}
	else
	{
		chInfo.cantonPronun[2] = this->chinese->Cantonese2Int(sb.ToString());
		if (chInfo.cantonPronun[2] == 0)
		{
			wprintf(L"Pronun3 error\r\n");
			return false;
		}
	}
	sb.ClearStr();
	this->txtPronun4->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		chInfo.cantonPronun[3] = 0;
	}
	else
	{
		chInfo.cantonPronun[3] = this->chinese->Cantonese2Int(sb.ToString());
		if (chInfo.cantonPronun[3] == 0)
		{
			wprintf(L"Pronun4 error\r\n");
			return false;
		}
	}
	chInfo.charType = (Text::ChineseInfo::CharType)(OSInt)this->cboCharType->GetSelectedItem();
	chInfo.mainChar = this->chkMainChar->IsChecked();
	return this->chinese->SetCharInfo(this->currChar, &chInfo);
}

void SSWR::AVIRead::AVIRChineseForm::UpdateChar(UInt32 charCode)
{
	UTF8Char sbuff[9];
	UTF8Char cbuff[9];
	if (this->SaveChar())
	{
		Text::StrHexVal32(sbuff, charCode);
		this->txtCharCode->SetText(sbuff);
		this->currChar = charCode;
		this->UpdateImg();

		Text::StringBuilderUTF8 sb;
		Text::ChineseInfo::CharacterInfo chInfo;
		this->chinese->GetCharInfo(charCode, &chInfo);
		Text::StrUInt32(sbuff, chInfo.strokeCount);
		this->txtStrokeCount->SetText(sbuff);
		if (chInfo.radical == 0)
		{
			this->currRadical = 0;
			this->lblRadicalV->SetText((const UTF8Char*)"");
		}
		else
		{
			Text::StrWriteChar(sbuff, (UTF32Char)chInfo.radical)[0] = 0;
			this->currRadical = chInfo.radical;
			this->lblRadicalV->SetText(sbuff);
		}
		if (chInfo.cantonPronun[0])
		{
			this->chinese->Int2Cantonese(cbuff, chInfo.cantonPronun[0]);
			sb.ClearStr();
			sb.Append(cbuff);
			this->txtPronun1->SetText(sb.ToString());
		}
		else
		{
			this->txtPronun1->SetText((const UTF8Char*)"");
		}
		if (chInfo.cantonPronun[1])
		{
			this->chinese->Int2Cantonese(cbuff, chInfo.cantonPronun[1]);
			sb.ClearStr();
			sb.Append(cbuff);
			this->txtPronun2->SetText(sb.ToString());
		}
		else
		{
			this->txtPronun2->SetText((const UTF8Char*)"");
		}
		if (chInfo.cantonPronun[2])
		{
			this->chinese->Int2Cantonese(cbuff, chInfo.cantonPronun[2]);
			sb.ClearStr();
			sb.Append(cbuff);
			this->txtPronun3->SetText(sb.ToString());
		}
		else
		{
			this->txtPronun3->SetText((const UTF8Char*)"");
		}
		if (chInfo.cantonPronun[3])
		{
			this->chinese->Int2Cantonese(cbuff, chInfo.cantonPronun[3]);
			sb.ClearStr();
			sb.Append(cbuff);
			this->txtPronun4->SetText(sb.ToString());
		}
		else
		{
			this->txtPronun4->SetText((const UTF8Char*)"");
		}
		this->chkMainChar->SetChecked(chInfo.mainChar);
		this->cboCharType->SetSelectedIndex((Text::ChineseInfo::CharType)chInfo.charType);
		this->UpdateRelation();
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in saving char", (const UTF8Char*)"Error", this);
	}
}

void SSWR::AVIRead::AVIRChineseForm::UpdateImg()
{
	UOSInt newW;
	UOSInt newH;
	this->pbChar->GetSizeP(&newW, &newH);

	if (this->charImg == 0)
	{
		this->charImg = this->deng->CreateImage32(newW, newH, Media::AT_NO_ALPHA);
	}
	else if (this->charImg->GetWidth() != newW || this->charImg->GetHeight() != newH)
	{
		this->pbChar->SetImageDImg(0);
		this->deng->DeleteImage(this->charImg);
		this->charImg = this->deng->CreateImage32(newW, newH, Media::AT_NO_ALPHA);
	}
	UTF8Char sbuff[7];
	Media::DrawBrush *b;
	Media::DrawFont *f;
	b = this->charImg->NewBrushARGB(0xffffffff);
	this->charImg->DrawRect(0, 0, Math::UOSInt2Double(newW), Math::UOSInt2Double(newH), 0, b);
	this->charImg->DelBrush(b);
	if (this->currChar != 0)
	{
		UOSInt len;
		Double sz[2];
		b = this->charImg->NewBrushARGB(0xff000000);
		f = this->charImg->NewFontPx(this->currFont, Math::UOSInt2Double(newH), Media::DrawEngine::DFS_NORMAL, 950);
		len = (UOSInt)(Text::StrWriteChar(sbuff, (UTF32Char)this->currChar) - sbuff);
		sbuff[len] = 0;
		
		this->charImg->GetTextSizeC(f, sbuff, len, sz);
		this->charImg->DrawString((Math::UOSInt2Double(newW) - sz[0]) * 0.5, (Math::UOSInt2Double(newH) - sz[1]) * 0.5, sbuff, f, b);
		this->charImg->DelFont(f);
		this->charImg->DelBrush(b);
	}
	this->pbChar->SetImageDImg(this->charImg);
}

void SSWR::AVIRead::AVIRChineseForm::UpdateRelation()
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = sbuff;
	if (this->currChar == 0)
	{
		this->txtRelatedCurr->SetText((const UTF8Char*)"");
	}
	else
	{
		Data::ArrayList<UInt32> relatedChars;
		UOSInt i;
		UOSInt j;
		this->chinese->GetRelatedChars(this->currChar, &relatedChars);
		i = 0;
		j = relatedChars.GetCount();
		while (i < j)
		{
			sptr = Text::StrWriteChar(sptr, (UTF32Char)relatedChars.GetItem(i));
			i++;
		}
		*sptr = 0;
		this->txtRelatedCurr->SetText(sbuff);
	}
}

SSWR::AVIRead::AVIRChineseForm::AVIRChineseForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Chinese");
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->deng = this->core->GetDrawEngine();
	this->currChar = 0;
	this->charImg = 0;
	this->currFont = Text::StrCopyNew((const UTF8Char*)"MingLiU");
	this->currRadical = 0;
	NEW_CLASS(this->chinese, Text::ChineseInfo());

	UI::GUIMenu *mnu;
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Goto");
	mnu->AddItem((const UTF8Char*)"Char Code", MNU_CHARCODE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu->AddItem((const UTF8Char*)"Related", MNU_RELATED, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_R);
	this->SetMenu(this->mnuMain);

	NEW_CLASS(this->lblCharCode, UI::GUILabel(ui, this, (const UTF8Char*)"Char Code"));
	this->lblCharCode->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtCharCode, UI::GUITextBox(ui, this, (const UTF8Char*)"0"));
	this->txtCharCode->SetRect(104, 4, 100, 23, false);
	this->txtCharCode->SetReadOnly(true);
	NEW_CLASS(this->btnCharPrev, UI::GUIButton(ui, this, (const UTF8Char*)"Prev"));
	this->btnCharPrev->SetRect(104, 28, 55, 23, false);
	this->btnCharPrev->HandleButtonClick(OnCharPrevClicked, this);
	NEW_CLASS(this->btnCharNext, UI::GUIButton(ui, this, (const UTF8Char*)"Next"));
	this->btnCharNext->SetRect(164, 28, 55, 23, false);
	this->btnCharNext->HandleButtonClick(OnCharNextClicked, this);
	NEW_CLASS(this->lblChar, UI::GUILabel(ui, this, (const UTF8Char*)"Character"));
	this->lblChar->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtChar, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtChar->SetRect(104, 52, 23, 23, false);
	this->txtChar->HandleTextChanged(OnCharChg, this);
	this->txtChar->Focus();
	NEW_CLASS(this->lblRelatedCurr, UI::GUILabel(ui, this, (const UTF8Char*)"Related (Curr)"));
	this->lblRelatedCurr->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtRelatedCurr, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtRelatedCurr->SetRect(104, 100, 100, 23, false);
	this->txtRelatedCurr->SetReadOnly(true);
	NEW_CLASS(this->lblRelatedAdd, UI::GUILabel(ui, this, (const UTF8Char*)"Related (Add)"));
	this->lblRelatedAdd->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtRelatedAdd, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtRelatedAdd->SetRect(104, 124, 23, 23, false);
	this->txtRelatedAdd->HandleTextChanged(OnRelatedAddChg, this);
	NEW_CLASS(this->btnRelatedGo, UI::GUIButton(ui, this, (const UTF8Char*)"Show Related"));
	this->btnRelatedGo->SetRect(128, 124, 95, 23, false);
	this->btnRelatedGo->HandleButtonClick(OnRelatedGoClicked, this);
	NEW_CLASS(this->pbChar, UI::GUIPictureBoxSimple(ui, this, this->deng, true));
	this->pbChar->SetRect(240, 4, 256, 256, false);
	this->pbChar->HandleMouseDown(OnCharMouseDown, this);

	NEW_CLASS(this->grpCharInfo, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Char Info"));
	this->grpCharInfo->SetRect(0, 0, 640, 448, false);
	this->grpCharInfo->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lblRadical, UI::GUILabel(ui, this->grpCharInfo, (const UTF8Char*)"Radical"));
	this->lblRadical->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtRadical, UI::GUITextBox(ui, this->grpCharInfo, (const UTF8Char*)""));
	this->txtRadical->SetRect(100, 0, 23, 23, false);
	this->txtRadical->HandleTextChanged(OnRadicalChg, this);
	NEW_CLASS(this->lblRadicalV, UI::GUILabel(ui, this->grpCharInfo, (const UTF8Char*)""));
	this->lblRadicalV->SetRect(124, 0, 100, 23, false);
	NEW_CLASS(this->lblStrokeCount, UI::GUILabel(ui, this->grpCharInfo, (const UTF8Char*)"Stroke Count"));
	this->lblStrokeCount->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtStrokeCount, UI::GUITextBox(ui, this->grpCharInfo, (const UTF8Char*)"0"));
	this->txtStrokeCount->SetRect(100, 24, 50, 23, false);
	NEW_CLASS(this->lblCharType, UI::GUILabel(ui, this->grpCharInfo, (const UTF8Char*)"Char Type"));
	this->lblCharType->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->cboCharType, UI::GUIComboBox(ui, this->grpCharInfo, false));
	this->cboCharType->SetRect(100, 48, 150, 23, false);
	this->cboCharType->AddItem((const UTF8Char*)"Unknown", (void*)Text::ChineseInfo::CT_UNKNOWN);
	this->cboCharType->AddItem((const UTF8Char*)"Chinese (Trad.)", (void*)Text::ChineseInfo::CT_CHINESET);
	this->cboCharType->AddItem((const UTF8Char*)"Chinese (Simp.)", (void*)Text::ChineseInfo::CT_CHINESES);
	this->cboCharType->AddItem((const UTF8Char*)"Chinese (Trad. & Simp.)", (void*)Text::ChineseInfo::CT_CHINESETS);
	this->cboCharType->AddItem((const UTF8Char*)"English", (void*)Text::ChineseInfo::CT_ENGLISH);
	this->cboCharType->AddItem((const UTF8Char*)"Japanese", (void*)Text::ChineseInfo::CT_JAPANESE);
	this->cboCharType->AddItem((const UTF8Char*)"Japanese (Kanji)", (void*)Text::ChineseInfo::CT_JAPANESE_KANJI);
	NEW_CLASS(this->lblFlags, UI::GUILabel(ui, this->grpCharInfo, (const UTF8Char*)"Flags"));
	this->lblFlags->SetRect(0, 72, 100, 23, false);
	NEW_CLASS(this->chkMainChar, UI::GUICheckBox(ui, this->grpCharInfo, (const UTF8Char*)"Main Char", false));
	this->chkMainChar->SetRect(100, 72, 120, 23, false);
	NEW_CLASS(this->lblPronun1, UI::GUILabel(ui, this->grpCharInfo, (const UTF8Char*)"Pronun 1"));
	this->lblPronun1->SetRect(0, 96, 100, 23, false);
	NEW_CLASS(this->txtPronun1, UI::GUITextBox(ui, this->grpCharInfo, (const UTF8Char*)""));
	this->txtPronun1->SetRect(100, 96, 100, 23, false);
	NEW_CLASS(this->lblPronun2, UI::GUILabel(ui, this->grpCharInfo, (const UTF8Char*)"Pronun 2"));
	this->lblPronun2->SetRect(0, 120, 100, 23, false);
	NEW_CLASS(this->txtPronun2, UI::GUITextBox(ui, this->grpCharInfo, (const UTF8Char*)""));
	this->txtPronun2->SetRect(100, 120, 100, 23, false);
	NEW_CLASS(this->lblPronun3, UI::GUILabel(ui, this->grpCharInfo, (const UTF8Char*)"Pronun 3"));
	this->lblPronun3->SetRect(0, 144, 100, 23, false);
	NEW_CLASS(this->txtPronun3, UI::GUITextBox(ui, this->grpCharInfo, (const UTF8Char*)""));
	this->txtPronun3->SetRect(100, 144, 100, 23, false);
	NEW_CLASS(this->lblPronun4, UI::GUILabel(ui, this->grpCharInfo, (const UTF8Char*)"Pronun 4"));
	this->lblPronun4->SetRect(0, 168, 100, 23, false);
	NEW_CLASS(this->txtPronun4, UI::GUITextBox(ui, this->grpCharInfo, (const UTF8Char*)""));
	this->txtPronun4->SetRect(100, 168, 100, 23, false);
}

SSWR::AVIRead::AVIRChineseForm::~AVIRChineseForm()
{
	this->SaveChar();
	if (this->charImg)
	{
		this->deng->DeleteImage(this->charImg);
		this->charImg = 0;
	}
	SDEL_TEXT(this->currFont);
	DEL_CLASS(this->chinese);
}

void SSWR::AVIRead::AVIRChineseForm::OnMonitorChanged()
{
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(hMon), this->core->GetMonitorDDPI(hMon));
	this->UpdateImg();
}

void SSWR::AVIRead::AVIRChineseForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_CHARCODE:
		this->txtChar->Focus();
		break;
	case MNU_RELATED:
		this->txtRelatedAdd->Focus();
		break;
	}
}

