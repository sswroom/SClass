#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Math/Math_C.h"
#include "SSWR/AVIRead/AVIRChineseForm.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/Unicode.h"
#include "UI/Clipboard.h"
#include "UI/GUIFontDialog.h"
#include <wchar.h>

typedef enum
{
	MNU_CHARCODE = 100,
	MNU_RELATED
} MenuItems;

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnCharChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChineseForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChineseForm>();
	Text::StringBuilderUTF8 sb;
	UTF32Char v;
	me->txtChar->GetText(sb);
	if (sb.GetLength() > 0)
	{
		me->txtChar->SetText(CSTR(""));

		Text::StrReadChar(sb.ToString(), v);
		me->UpdateChar((UInt32)v);
		me->txtRadical->Focus();
	}
}

UI::EventState __stdcall SSWR::AVIRead::AVIRChineseForm::OnCharMouseDown(AnyType userObj, Math::Coord2D<IntOS> scnPos, UI::GUIControl::MouseButton btn)
{
	NN<SSWR::AVIRead::AVIRChineseForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChineseForm>();
	NN<UI::GUIFontDialog> dlg = me->ui->NewFontDialog(me->currFont, 12, false, false);
	NN<Text::String> s;
	if (dlg->ShowDialog(me->GetHandle()) == UI::GUIForm::DR_OK && dlg->GetFontName().SetTo(s))
	{
		me->currFont->Release();
		me->currFont = s->Clone();
		me->UpdateImg();
	}
	dlg.Delete();
	return UI::EventState::ContinueEvent;
}

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnPasteCharCodeClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChineseForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChineseForm>();
	Text::StringBuilderUTF8 sb;
	UInt32 code;
	if (UI::Clipboard::GetString(me->GetHandle(), sb))
	{
		if (sb.StartsWith(UTF8STRC("\\u")))
		{
			if (sb.leng == 6 && sb.Substring(2).Hex2UInt32(code))
			{
				me->UpdateChar(code);
			}
			else if (sb.leng == 12 && sb.StartsWith(6, UTF8STRC("\\u")) && sb.Substring(8).Hex2UInt32(code))
			{
				UInt32 code2;
				sb.TrimToLength(6);
				if (sb.Substring(2).Hex2UInt32(code2))
				{
					UTF16Char u16buff[3];
					u16buff[0] = (UTF16Char)code2;
					u16buff[1] = (UTF16Char)code;
					u16buff[2] = 0;
					UTF32Char outChar;
					Text::StrReadChar(u16buff, outChar);
					me->UpdateChar((UInt32)outChar);
				}
			}
		}
		else if (sb.Hex2UInt32(code))
		{
			me->UpdateChar(code);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnCharPrevClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChineseForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChineseForm>();
	if (me->currChar > 0)
	{
		me->UpdateChar(me->currChar - 1);
	}
}

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnCharNextClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChineseForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChineseForm>();
	me->UpdateChar(me->currChar + 1);
}

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnRadicalChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChineseForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChineseForm>();
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[7];
	UnsafeArray<UTF8Char> sptr;
	UTF32Char v;
	me->txtRadical->GetText(sb);
	if (sb.GetLength() > 0)
	{
		me->txtRadical->SetText(CSTR(""));

		Text::StrReadChar(sb.ToString(), v);
		me->currRadical = (UInt32)v;
		sptr = Text::StrWriteChar(sbuff, v);
		sptr[0] = 0;
		me->lblRadicalV->SetText(CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnRelatedAddChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChineseForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChineseForm>();
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UTF32Char v;
	UIntOS i;
	UIntOS j;
	me->txtRelatedAdd->GetText(sb);
	if (sb.GetLength() > 0)
	{
		me->txtRelatedAdd->SetText(CSTR(""));

		Text::StrReadChar(sb.ToString(), v);
		if (me->currChar != 0)
		{
			if (me->currChar == (UInt32)v)
			{

			}
			else
			{
				Data::ArrayListNative<UInt32> relatedList;
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
				sptr = Text::StrWriteChar(sbuff, (UTF32Char)me->currChar);
				sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
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
				sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
				sb.AppendC(UTF8STRC("\"?"));
				if (me->ui->ShowMsgYesNo(sb.ToCString(), CSTR("Add Relation"), me))
				{
					me->chinese->AddRelation(me->currChar, (UInt32)v);
					me->UpdateRelation();
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnRelatedGoClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChineseForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChineseForm>();
	Data::ArrayListNative<UInt32> relatedChars;
	me->chinese->GetRelatedChars(me->currChar, &relatedChars);
	if (relatedChars.GetCount() > 0)
	{
		me->UpdateChar(relatedChars.GetItem(0));
	}
}

void __stdcall SSWR::AVIRead::AVIRChineseForm::OnFormClosed(AnyType userObj, NN<UI::GUIForm> frm)
{
	NN<SSWR::AVIRead::AVIRChineseForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChineseForm>();
	me->SaveChar();
}

Bool SSWR::AVIRead::AVIRChineseForm::SaveChar()
{
	if (this->currChar == 0)
		return true;
	Int32 strokeCnt;
	Text::StringBuilderUTF8 sb;
	Text::ChineseInfo::CharacterInfo chInfo;
	this->txtStrokeCount->GetText(sb);
	if (!sb.ToInt32(strokeCnt))
	{
		wprintf(L"strokeCount error\r\n");
		return false;
	}
	chInfo.strokeCount = (UInt8)strokeCnt;
	chInfo.radical = this->currRadical;
	sb.ClearStr();
	this->txtPronun1->GetText(sb);
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
	this->txtPronun2->GetText(sb);
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
	this->txtPronun3->GetText(sb);
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
	this->txtPronun4->GetText(sb);
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
	chInfo.charType = (Text::ChineseInfo::CharType)this->cboCharType->GetSelectedItem().GetIntOS();
	chInfo.mainChar = this->chkMainChar->IsChecked();
	return this->chinese->SetCharInfo(this->currChar, &chInfo);
}

void SSWR::AVIRead::AVIRChineseForm::UpdateChar(UInt32 charCode)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UTF8Char u8buff[7];
	UTF16Char u16buff[3];
	UIntOS charCnt;
	UIntOS i;
	if (this->SaveChar())
	{
		sptr = Text::StrHexVal32(sbuff, charCode);
		this->txtCharCode->SetText(CSTRP(sbuff, sptr));
		this->currChar = charCode;
		this->UpdateImg();

		this->txtUTF32Code->SetText(CSTRP(sbuff, sptr));
		charCnt = (UIntOS)(Text::StrWriteChar(u8buff, (UTF32Char)charCode) - u8buff);
		sptr = Text::StrHexBytes(sbuff, u8buff, charCnt, ' ');
		this->txtUTF8Code->SetText(CSTRP(sbuff, sptr));
		u8buff[charCnt] = 0;
		this->txtCurrChar->SetText(Text::CStringNN(u8buff, charCnt));
		charCnt = (UIntOS)(Text::StrWriteChar(u16buff, (UTF32Char)charCode) - u16buff);
		sptr = sbuff;
		i = 0;
		while (i < charCnt)
		{
			if (i > 0) *sptr++ = ' ';
			sptr = Text::StrHexVal16(sptr, u16buff[i]);
			i++;
		}
		this->txtUTF16Code->SetText(CSTRP(sbuff, sptr));
		NN<Text::Unicode::Block> blk;
		if (Text::Unicode::GetBlock(charCode).SetTo(blk))
		{
			this->txtBlockName->SetText(Text::CStringNN(blk->name, blk->nameLen));
			sptr = sbuff;
			sptr = Text::StrHexVal32V(sptr, blk->beginCode);
			*sptr++ = '-';
			sptr = Text::StrHexVal32V(sptr, blk->endCode);
			this->txtBlockRange->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			this->txtBlockName->SetText(CSTR(""));
			this->txtBlockRange->SetText(CSTR(""));
		}
		NN<Text::UnicodeCharacterData::UnicodeData> unicodeData;
		if (this->ucd.GetUnicodeData(charCode).SetTo(unicodeData))
		{
			this->txtCharacterName->SetText(unicodeData->characterName->ToCString());
			this->txtUnicode10Name->SetText(unicodeData->unicode10Name->ToCString());
			if (unicodeData->uppercaseMapping)
			{
				sptr = Text::StrHexVal32V(sbuff, unicodeData->uppercaseMapping);
				this->txtUppercaseCode->SetText(CSTRP(sbuff, sptr));
			}
			else
			{
				this->txtUppercaseCode->SetText(CSTR(""));
			}
			if (unicodeData->lowercaseMapping)
			{
				sptr = Text::StrHexVal32V(sbuff, unicodeData->lowercaseMapping);
				this->txtLowercaseCode->SetText(CSTRP(sbuff, sptr));
			}
			else
			{
				this->txtLowercaseCode->SetText(CSTR(""));
			}
			if (unicodeData->titlecaseMapping)
			{
				sptr = Text::StrHexVal32V(sbuff, unicodeData->titlecaseMapping);
				this->txtTitlecaseCode->SetText(CSTRP(sbuff, sptr));
			}
			else
			{
				this->txtTitlecaseCode->SetText(CSTR(""));
			}
		}
		else
		{
			this->txtCharacterName->SetText(CSTR(""));
			this->txtUnicode10Name->SetText(CSTR(""));
			this->txtUppercaseCode->SetText(CSTR(""));
			this->txtLowercaseCode->SetText(CSTR(""));
			this->txtTitlecaseCode->SetText(CSTR(""));
		}

		Text::StringBuilderUTF8 sb;
		Text::ChineseInfo::CharacterInfo chInfo;
		this->chinese->GetCharInfo(charCode, &chInfo);
		sptr = Text::StrUInt32(sbuff, chInfo.strokeCount);
		this->txtStrokeCount->SetText(CSTRP(sbuff, sptr));
		if (chInfo.radical == 0)
		{
			this->currRadical = 0;
			this->lblRadicalV->SetText(CSTR(""));
		}
		else
		{
			sptr = Text::StrWriteChar(sbuff, (UTF32Char)chInfo.radical);
			sptr[0] = 0;
			this->currRadical = chInfo.radical;
			this->lblRadicalV->SetText(CSTRP(sbuff, sptr));
		}
		if (chInfo.cantonPronun[0])
		{
			sptr = this->chinese->Int2Cantonese(sbuff, chInfo.cantonPronun[0]);
			this->txtPronun1->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			this->txtPronun1->SetText(CSTR(""));
		}
		if (chInfo.cantonPronun[1])
		{
			sptr = this->chinese->Int2Cantonese(sbuff, chInfo.cantonPronun[1]);
			this->txtPronun2->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			this->txtPronun2->SetText(CSTR(""));
		}
		if (chInfo.cantonPronun[2])
		{
			sptr = this->chinese->Int2Cantonese(sbuff, chInfo.cantonPronun[2]);
			this->txtPronun3->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			this->txtPronun3->SetText(CSTR(""));
		}
		if (chInfo.cantonPronun[3])
		{
			sptr = this->chinese->Int2Cantonese(sbuff, chInfo.cantonPronun[3]);
			this->txtPronun4->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			this->txtPronun4->SetText(CSTR(""));
		}
		this->chkMainChar->SetChecked(chInfo.mainChar);
		this->cboCharType->SetSelectedIndex((Text::ChineseInfo::CharType)chInfo.charType);
		this->UpdateRelation();
	}
	else
	{
		this->ui->ShowMsgOK(CSTR("Error in saving char"), CSTR("Error"), this);
	}
}

void SSWR::AVIRead::AVIRChineseForm::UpdateImg()
{
	Math::Size2D<UIntOS> newSize = this->pbChar->GetSizeP();
	NN<Media::DrawImage> dimg;
	if (!this->charImg.SetTo(dimg))
	{
		this->charImg = this->deng->CreateImage32(newSize, Media::AT_ALPHA_ALL_FF);
	}
	else if (dimg->GetWidth() != newSize.x || dimg->GetHeight() != newSize.y)
	{
		this->pbChar->SetImageDImg(nullptr);
		this->deng->DeleteImage(dimg);
		this->charImg = this->deng->CreateImage32(newSize, Media::AT_ALPHA_ALL_FF);
	}
	if (this->charImg.SetTo(dimg))
	{
		UTF8Char sbuff[7];
		NN<Media::DrawBrush> b;
		NN<Media::DrawFont> f;
		b = dimg->NewBrushARGB(0xffffffff);
		dimg->DrawRect(Math::Coord2DDbl(0, 0), newSize.ToDouble(), nullptr, b);
		dimg->DelBrush(b);
		if (this->currChar != 0)
		{
			UIntOS len;
			Math::Size2DDbl sz;
			b = dimg->NewBrushARGB(0xff000000);
			f = dimg->NewFontPx(this->currFont->ToCString(), UIntOS2Double(newSize.y), Media::DrawEngine::DFS_NORMAL, 950);
			len = (UIntOS)(Text::StrWriteChar(sbuff, (UTF32Char)this->currChar) - sbuff);
			sbuff[len] = 0;
			
			sz = dimg->GetTextSize(f, {sbuff, len});
			dimg->DrawString((newSize.ToDouble() - sz) * 0.5, {sbuff, len}, f, b);
			dimg->DelFont(f);
			dimg->DelBrush(b);
		}
		this->pbChar->SetImageDImg(this->charImg);
	}
}

void SSWR::AVIRead::AVIRChineseForm::UpdateRelation()
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = sbuff;
	if (this->currChar == 0)
	{
		this->txtRelatedCurr->SetText(CSTR(""));
	}
	else
	{
		Data::ArrayListNative<UInt32> relatedChars;
		UIntOS i;
		UIntOS j;
		this->chinese->GetRelatedChars(this->currChar, &relatedChars);
		i = 0;
		j = relatedChars.GetCount();
		while (i < j)
		{
			sptr = Text::StrWriteChar(sptr, (UTF32Char)relatedChars.GetItem(i));
			i++;
		}
		*sptr = 0;
		this->txtRelatedCurr->SetText(CSTRP(sbuff, sptr));
	}
}

SSWR::AVIRead::AVIRChineseForm::AVIRChineseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	this->SetText(CSTR("Chinese"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->deng = this->core->GetDrawEngine();
	this->currChar = 0;
	this->charImg = nullptr;
	this->currFont = Text::String::New(UTF8STRC("MingLiU"));
	this->currRadical = 0;
	NEW_CLASSNN(this->chinese, Text::ChineseInfo());

	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("UCD.zip"));
	this->ucd.LoadUCDZip(CSTRP(sbuff, sptr));

	NN<UI::GUIMenu> mnu;
	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&Goto"));
	mnu->AddItem(CSTR("Char Code"), MNU_CHARCODE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu->AddItem(CSTR("Related"), MNU_RELATED, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_R);
	this->SetMenu(this->mnuMain);

	this->lblCharCode = ui->NewLabel(*this, CSTR("Char Code"));
	this->lblCharCode->SetRect(4, 4, 100, 23, false);
	this->txtCharCode = ui->NewTextBox(*this, CSTR("0"));
	this->txtCharCode->SetRect(104, 4, 100, 23, false);
	this->txtCharCode->SetReadOnly(true);
	this->btnPasteCharCode = ui->NewButton(*this, CSTR("Paste"));
	this->btnPasteCharCode->SetRect(204, 4, 75, 23, false);
	this->btnPasteCharCode->HandleButtonClick(OnPasteCharCodeClicked, this);
	this->btnCharPrev = ui->NewButton(*this, CSTR("Prev"));
	this->btnCharPrev->SetRect(104, 28, 55, 23, false);
	this->btnCharPrev->HandleButtonClick(OnCharPrevClicked, this);
	this->btnCharNext = ui->NewButton(*this, CSTR("Next"));
	this->btnCharNext->SetRect(164, 28, 55, 23, false);
	this->btnCharNext->HandleButtonClick(OnCharNextClicked, this);
	this->lblChar = ui->NewLabel(*this, CSTR("Character"));
	this->lblChar->SetRect(4, 52, 100, 23, false);
	this->txtChar = ui->NewTextBox(*this, CSTR(""));
	this->txtChar->SetRect(104, 52, 23, 23, false);
	this->txtChar->HandleTextChanged(OnCharChg, this);
	this->txtChar->Focus();
	this->lblRelatedCurr = ui->NewLabel(*this, CSTR("Related (Curr)"));
	this->lblRelatedCurr->SetRect(4, 100, 100, 23, false);
	this->txtRelatedCurr = ui->NewTextBox(*this, CSTR(""));
	this->txtRelatedCurr->SetRect(104, 100, 100, 23, false);
	this->txtRelatedCurr->SetReadOnly(true);
	this->lblRelatedAdd = ui->NewLabel(*this, CSTR("Related (Add)"));
	this->lblRelatedAdd->SetRect(4, 124, 100, 23, false);
	this->txtRelatedAdd = ui->NewTextBox(*this, CSTR(""));
	this->txtRelatedAdd->SetRect(104, 124, 23, 23, false);
	this->txtRelatedAdd->HandleTextChanged(OnRelatedAddChg, this);
	this->btnRelatedGo = ui->NewButton(*this, CSTR("Show Related"));
	this->btnRelatedGo->SetRect(128, 124, 95, 23, false);
	this->btnRelatedGo->HandleButtonClick(OnRelatedGoClicked, this);
	this->pbChar = ui->NewPictureBoxSimple(*this, this->deng, true);
	this->pbChar->SetRect(284, 4, 256, 256, false);
	this->pbChar->HandleMouseDown(OnCharMouseDown, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetRect(0, 0, 640, 448, false);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_BOTTOM);

	this->tpBaseInfo = this->tcMain->AddTabPage(CSTR("Base Info"));
	this->lblCurrChar = ui->NewLabel(this->tpBaseInfo, CSTR("Character"));
	this->lblCurrChar->SetRect(4, 4, 100, 23, false);
	this->txtCurrChar = ui->NewTextBox(this->tpBaseInfo, CSTR(""));
	this->txtCurrChar->SetFont(nullptr, 48, false);
	this->txtCurrChar->SetRect(104, 4, 90, 71, false);
	this->txtCurrChar->SetReadOnly(true);
	this->lblUTF8Code = ui->NewLabel(this->tpBaseInfo, CSTR("UTF-8"));
	this->lblUTF8Code->SetRect(4, 76, 100, 23, false);
	this->txtUTF8Code = ui->NewTextBox(this->tpBaseInfo, CSTR(""));
	this->txtUTF8Code->SetRect(104, 76, 200, 23, false);
	this->txtUTF8Code->SetReadOnly(true);
	this->lblUTF16Code = ui->NewLabel(this->tpBaseInfo, CSTR("UTF-16"));
	this->lblUTF16Code->SetRect(4, 100, 100, 23, false);
	this->txtUTF16Code = ui->NewTextBox(this->tpBaseInfo, CSTR(""));
	this->txtUTF16Code->SetRect(104, 100, 200, 23, false);
	this->txtUTF16Code->SetReadOnly(true);
	this->lblUTF32Code = ui->NewLabel(this->tpBaseInfo, CSTR("UTF-32"));
	this->lblUTF32Code->SetRect(4, 124, 100, 23, false);
	this->txtUTF32Code = ui->NewTextBox(this->tpBaseInfo, CSTR(""));
	this->txtUTF32Code->SetRect(104, 124, 200, 23, false);
	this->txtUTF32Code->SetReadOnly(true);
	this->lblBlockRange = ui->NewLabel(this->tpBaseInfo, CSTR("Block Range"));
	this->lblBlockRange->SetRect(4, 148, 100, 23, false);
	this->txtBlockRange = ui->NewTextBox(this->tpBaseInfo, CSTR(""));
	this->txtBlockRange->SetRect(104, 148, 200, 23, false);
	this->txtBlockRange->SetReadOnly(true);
	this->lblBlockName = ui->NewLabel(this->tpBaseInfo, CSTR("Block Name"));
	this->lblBlockName->SetRect(4, 172, 100, 23, false);
	this->txtBlockName = ui->NewTextBox(this->tpBaseInfo, CSTR(""));
	this->txtBlockName->SetRect(104, 172, 200, 23, false);
	this->txtBlockName->SetReadOnly(true);
	this->lblCharacterName = ui->NewLabel(this->tpBaseInfo, CSTR("Character Name"));
	this->lblCharacterName->SetRect(4, 196, 100, 23, false);
	this->txtCharacterName = ui->NewTextBox(this->tpBaseInfo, CSTR(""));
	this->txtCharacterName->SetRect(104, 196, 200, 23, false);
	this->txtCharacterName->SetReadOnly(true);
	this->lblUnicode10Name = ui->NewLabel(this->tpBaseInfo, CSTR("Unicode 1.0 Name"));
	this->lblUnicode10Name->SetRect(4, 220, 100, 23, false);
	this->txtUnicode10Name = ui->NewTextBox(this->tpBaseInfo, CSTR(""));
	this->txtUnicode10Name->SetRect(104, 220, 200, 23, false);
	this->txtUnicode10Name->SetReadOnly(true);
	this->lblUppercaseCode = ui->NewLabel(this->tpBaseInfo, CSTR("Uppercase code"));
	this->lblUppercaseCode->SetRect(4, 244, 100, 23, false);
	this->txtUppercaseCode = ui->NewTextBox(this->tpBaseInfo, CSTR(""));
	this->txtUppercaseCode->SetRect(104, 244, 200, 23, false);
	this->txtUppercaseCode->SetReadOnly(true);
	this->lblLowercaseCode = ui->NewLabel(this->tpBaseInfo, CSTR("Lowercase code"));
	this->lblLowercaseCode->SetRect(4, 268, 100, 23, false);
	this->txtLowercaseCode = ui->NewTextBox(this->tpBaseInfo, CSTR(""));
	this->txtLowercaseCode->SetRect(104, 268, 200, 23, false);
	this->txtLowercaseCode->SetReadOnly(true);
	this->lblTitlecaseCode = ui->NewLabel(this->tpBaseInfo, CSTR("Titlecase code"));
	this->lblTitlecaseCode->SetRect(4, 292, 100, 23, false);
	this->txtTitlecaseCode = ui->NewTextBox(this->tpBaseInfo, CSTR(""));
	this->txtTitlecaseCode->SetRect(104, 292, 200, 23, false);
	this->txtTitlecaseCode->SetReadOnly(true);

	this->tpCharInfo = this->tcMain->AddTabPage(CSTR("Char Info"));
	this->lblRadical = ui->NewLabel(this->tpCharInfo, CSTR("Radical"));
	this->lblRadical->SetRect(0, 0, 100, 23, false);
	this->txtRadical = ui->NewTextBox(this->tpCharInfo, CSTR(""));
	this->txtRadical->SetRect(100, 0, 23, 23, false);
	this->txtRadical->HandleTextChanged(OnRadicalChg, this);
	this->lblRadicalV = ui->NewLabel(this->tpCharInfo, CSTR(""));
	this->lblRadicalV->SetRect(124, 0, 100, 23, false);
	this->lblStrokeCount = ui->NewLabel(this->tpCharInfo, CSTR("Stroke Count"));
	this->lblStrokeCount->SetRect(0, 24, 100, 23, false);
	this->txtStrokeCount = ui->NewTextBox(this->tpCharInfo, CSTR("0"));
	this->txtStrokeCount->SetRect(100, 24, 50, 23, false);
	this->lblCharType = ui->NewLabel(this->tpCharInfo, CSTR("Char Type"));
	this->lblCharType->SetRect(0, 48, 100, 23, false);
	this->cboCharType = ui->NewComboBox(this->tpCharInfo, false);
	this->cboCharType->SetRect(100, 48, 150, 23, false);
	this->cboCharType->AddItem(CSTR("Unknown"), (void*)Text::ChineseInfo::CT_UNKNOWN);
	this->cboCharType->AddItem(CSTR("Chinese (Trad.)"), (void*)Text::ChineseInfo::CT_CHINESET);
	this->cboCharType->AddItem(CSTR("Chinese (Simp.)"), (void*)Text::ChineseInfo::CT_CHINESES);
	this->cboCharType->AddItem(CSTR("Chinese (Trad. & Simp.)"), (void*)Text::ChineseInfo::CT_CHINESETS);
	this->cboCharType->AddItem(CSTR("English"), (void*)Text::ChineseInfo::CT_ENGLISH);
	this->cboCharType->AddItem(CSTR("Japanese"), (void*)Text::ChineseInfo::CT_JAPANESE);
	this->cboCharType->AddItem(CSTR("Japanese (Kanji)"), (void*)Text::ChineseInfo::CT_JAPANESE_KANJI);
	this->lblFlags = ui->NewLabel(this->tpCharInfo, CSTR("Flags"));
	this->lblFlags->SetRect(0, 72, 100, 23, false);
	this->chkMainChar = ui->NewCheckBox(this->tpCharInfo, CSTR("Main Char"), false);
	this->chkMainChar->SetRect(100, 72, 120, 23, false);
	this->lblPronun1 = ui->NewLabel(this->tpCharInfo, CSTR("Pronun 1"));
	this->lblPronun1->SetRect(0, 96, 100, 23, false);
	this->txtPronun1 = ui->NewTextBox(this->tpCharInfo, CSTR(""));
	this->txtPronun1->SetRect(100, 96, 100, 23, false);
	this->lblPronun2 = ui->NewLabel(this->tpCharInfo, CSTR("Pronun 2"));
	this->lblPronun2->SetRect(0, 120, 100, 23, false);
	this->txtPronun2 = ui->NewTextBox(this->tpCharInfo, CSTR(""));
	this->txtPronun2->SetRect(100, 120, 100, 23, false);
	this->lblPronun3 = ui->NewLabel(this->tpCharInfo, CSTR("Pronun 3"));
	this->lblPronun3->SetRect(0, 144, 100, 23, false);
	this->txtPronun3 = ui->NewTextBox(this->tpCharInfo, CSTR(""));
	this->txtPronun3->SetRect(100, 144, 100, 23, false);
	this->lblPronun4 = ui->NewLabel(this->tpCharInfo, CSTR("Pronun 4"));
	this->lblPronun4->SetRect(0, 168, 100, 23, false);
	this->txtPronun4 = ui->NewTextBox(this->tpCharInfo, CSTR(""));
	this->txtPronun4->SetRect(100, 168, 100, 23, false);

	this->HandleFormClosed(OnFormClosed, this);
}

SSWR::AVIRead::AVIRChineseForm::~AVIRChineseForm()
{
	NN<Media::DrawImage> img;
	if (this->charImg.SetTo(img))
	{
		this->deng->DeleteImage(img);
		this->charImg = nullptr;
	}
	this->currFont->Release();
	this->chinese.Delete();
}

void SSWR::AVIRead::AVIRChineseForm::OnMonitorChanged()
{
	Optional<MonitorHandle> hMon = this->GetHMonitor();
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

