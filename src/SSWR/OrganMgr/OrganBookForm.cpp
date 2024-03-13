#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSortFunc.h"
#include "SSWR/OrganMgr/OrganBookForm.h"
#include "Text/StringTool.h"
#include "UI/Clipboard.h"

SSWR::OrganMgr::BookArrayList::BookArrayList() : Data::SortableArrayList<OrganBook*>()
{
}

SSWR::OrganMgr::BookArrayList::~BookArrayList()
{
}

void SSWR::OrganMgr::BookArrayList::Sort()
{
	Data::Sort::ArtificialQuickSortFunc<OrganBook*>::Sort(*this, CompareBook);
}

OSInt __stdcall SSWR::OrganMgr::BookArrayList::CompareBook(OrganBook *book1, OrganBook *book2)
{
	OSInt ret = book1->GetPublishDate().CompareTo(book2->GetPublishDate());
	if (ret == 0)
	{
		ret = Text::StringTool::CompareICase(book1->GetDispAuthor(), book2->GetDispAuthor());
		if (ret == 0)
		{
			ret = Text::StringTool::CompareICase(book1->GetTitle(), book2->GetTitle());
		}
	}
	return ret;
}

OSInt SSWR::OrganMgr::BookArrayList::Compare(OrganBook *book1, OrganBook *book2) const
{
	return CompareBook(book1, book2);
}

void __stdcall SSWR::OrganMgr::OrganBookForm::OnBookPublishChg(void *userObj, NotNullPtr<Data::DateTime> newDate)
{
	OrganBookForm *me = (OrganBookForm*)userObj;
	Data::Timestamp currTime = Data::DateTime(newDate->GetYear(), 1, 1, 0, 0, 0).ToTimestamp();
	OSInt i = 0;
	OSInt j = (OSInt)me->bookList.GetCount() - 1;
	OSInt k;
	OrganBook *book;
	while (i <= j)
	{
		k = (i + j) >> 1;
		book = me->bookList.GetItem((UOSInt)k);
		if (book->GetPublishDate().CompareTo(currTime) >= 0)
		{
			j = k - 1;
		}
		else
		{
			i = k + 1;
		}
	}
	if (i >= (OSInt)me->bookList.GetCount())
	{
		if (me->bookList.GetCount() > 0)
		{
			me->lvBook->EnsureVisible(me->bookList.GetCount() - 1);
			me->lvBook->SetSelectedIndex(me->bookList.GetCount() - 1);
		}
	}
	else
	{
		Math::Size2D<UOSInt> sz = me->lvBook->GetSizeP();
		j = (OSInt)sz.y >> 1;
		Int32 rect[4];
		me->lvBook->GetItemRectP((UOSInt)i, rect);
		if (rect[1] > j)
		{
			k = i + j / rect[3];
			if (k >= (OSInt)me->bookList.GetCount())
				k = (OSInt)me->bookList.GetCount() - 1;
		}
		else
		{
			k = i - j / rect[3];
			if (k < 0)
				k = 0;
		}
		me->lvBook->EnsureVisible((UOSInt)k);
		me->lvBook->SetSelectedIndex((UOSInt)i);
	}
}

void __stdcall SSWR::OrganMgr::OrganBookForm::OnBookAddClicked(void *userObj)
{
	OrganBookForm *me = (OrganBookForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sb3;
	Text::StringBuilderUTF8 sb4;
	me->txtBookAuthor->GetText(sb);
	me->txtBookTitle->GetText(sb2);
	me->txtBookSource->GetText(sb3);
	me->txtBookURL->GetText(sb4);
	if (sb.GetLength() <= 0)
	{
		me->ui->ShowMsgOK(me->env->GetLang(CSTR("BookFormInputAuthor")), CSTR("Error"), me);
		return;
	}
	if (sb2.GetLength() <= 0)
	{
		me->ui->ShowMsgOK(me->env->GetLang(CSTR("BookFormInputName")), CSTR("Error"), me);
		return;
	}
	if (sb3.GetLength() <= 0)
	{
		me->ui->ShowMsgOK(me->env->GetLang(CSTR("BookFormInputSource")), CSTR("Error"), me);
		return;
	}
	Data::DateTime publishDate;
	me->dtpBookPublish->GetSelectedTime(publishDate);
	if (!me->env->NewBook(sb2.ToCString(), sb.ToCString(), sb3.ToCString(), publishDate.ToTimestamp(), sb4.ToCString()))
	{
		me->ui->ShowMsgOK(me->env->GetLang(CSTR("BookFormDBError")), CSTR("Error"), me);
		return;
	}

	me->changed = true;
	me->bookList.Clear();
	me->env->GetBooksAll(&me->bookList);
	me->bookList.Sort();
	me->UpdateBookList();
	me->txtBookAuthor->SetText(CSTR(""));
	me->txtBookTitle->SetText(CSTR(""));
	me->txtBookSource->SetText(CSTR(""));
	me->txtBookURL->SetText(CSTR(""));
}

void __stdcall SSWR::OrganMgr::OrganBookForm::OnBookPasteClicked(void *userObj)
{
	OrganBookForm *me = (OrganBookForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Data::DateTime dt;
	UI::Clipboard::GetString(me->GetHandle(), sb);
	if (sb.GetLength() > 0)
	{
		sb.Replace('\n', ' ');
		sb.Replace('\r', ' ');
		sb.ReplaceStr(UTF8STRC("  "), UTF8STRC(" "));

		UOSInt digitCnt = 0;
		UOSInt i = 1;
		UOSInt j;
		UOSInt k;
		const UTF8Char *chars = sb.ToString();
		Bool found = false;
		j = sb.GetLength();
		while (i < j)
		{
			if (chars[i] >= '0' && chars[i] <= '9')
			{
				digitCnt++;
				if (digitCnt == 4)
				{
					if (chars[i - 4] == '(')
					{
						sb2.ClearStr();
						sb2.AppendC(chars, i - 4);
						sb2.Trim();
						me->txtBookAuthor->SetText(sb2.ToCString());
						sb2.ClearStr();
						sb2.AppendChar(chars[i - 3], 4);
						dt.SetValue((UInt16)sb2.ToInt32(), 1, 1, 0, 0, 0, 0);
						me->dtpBookPublish->SetValue(dt);
						sb.SetSubstr(i + 2);
						sb.Trim();
					}
					else
					{
						sb2.ClearStr();
						sb2.AppendC(chars, i - 3);
						sb2.Trim();
						me->txtBookAuthor->SetText(sb2.ToCString());
						sb2.ClearStr();
						sb2.AppendChar(chars[i - 3], 4);
						dt.SetValue((UInt16)sb2.ToInt32(), 1, 1, 0, 0, 0, 0);
						me->dtpBookPublish->SetValue(dt);
						sb.SetSubstr(i + 1);
						sb.Trim();
					}

					chars = sb.ToString();
					j = sb.GetLength();
					i = 0;
					while (true)
					{
						i = sb.IndexOf(UTF8STRC("."), i);
						if (i == INVALID_INDEX)
						{
							me->txtBookTitle->SetText(sb.ToCString());
							me->txtBookSource->SetText(CSTR(""));
							me->txtBookURL->SetText(CSTR(""));
							break;
						}

						i++;
						k = i;
						while (k < j)
						{
							if (chars[k] == ' ')
							{

							}
							else
							{
								if (chars[k] >= 'A' && chars[k] <= 'Z')
								{
									found = true;
								}
								break;
							}
							k++;
						}
						if (found)
						{
							sb2.ClearStr();
							sb2.AppendC(chars, i - 1);
							me->txtBookTitle->SetText(sb2.ToCString());
							sb.SetSubstr(k);
							sb.Trim();
							k = sb.IndexOf(UTF8STRC("http://"));
							if (k != INVALID_INDEX)
							{
								sb2.ClearStr();
								sb2.AppendC(sb.ToString(), k);
								sb2.Trim();
								me->txtBookSource->SetText(sb2.ToCString());
								sb.SetSubstr(k)->Trim();
								me->txtBookURL->SetText(sb.ToCString());
							}
							else
							{
								me->txtBookSource->SetText(sb.ToCString());
								me->txtBookURL->SetText(CSTR(""));
							}
							break;
						}
					}
					found = true;
					break;
				}
			}
			else
			{
				digitCnt = 0;
			}
			i++;
		}
	}
}

void SSWR::OrganMgr::OrganBookForm::UpdateBookList()
{
	OrganBook *book;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt i = 0;
	UOSInt j = this->bookList.GetCount();
	UOSInt k;
	this->lvBook->ClearItems();
	while (i < j)
	{
		book = this->bookList.GetItem(i);;
		sptr = book->GetPublishDate().ToString(sbuff, "yyyy-MM");
		k = this->lvBook->AddItem(CSTRP(sbuff, sptr), book);
		this->lvBook->SetSubItem(k, 1, Text::String::OrEmpty(book->GetDispAuthor()));
		this->lvBook->SetSubItem(k, 2, Text::String::OrEmpty(book->GetTitle()));
		this->lvBook->SetSubItem(k, 3, Text::String::OrEmpty(book->GetPress()));
		i++;
	}
}

SSWR::OrganMgr::OrganBookForm::OrganBookForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->env = env;
	this->changed = false;

	this->SetText(this->env->GetLang(CSTR("BookFormTitle")));
	this->SetFont(0, 0, 10.5, false);

	this->pnlBook = ui->NewPanel(*this);
	this->pnlBook->SetRect(0, 0, 168, 500, false);
	this->pnlBook->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->pnlBookDet = ui->NewPanel(this->pnlBook);
	this->pnlBookDet->SetRect(0, 0, 168, 320, false);
	this->pnlBookDet->SetDockType(UI::GUIControl::DOCK_TOP);
	this->hspBook = ui->NewHSplitter(*this, 3, true);
	this->lvBook = ui->NewListView(*this, UI::ListViewStyle::Table, 4);
	this->lvBook->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvBook->SetShowGrid(true);
	this->lvBook->SetFullRowSelect(true);
	this->lvBook->AddColumn(this->env->GetLang(CSTR("BookFormColDate")), 60);
	this->lvBook->AddColumn(this->env->GetLang(CSTR("BookFormColAuthor")), 120);
	this->lvBook->AddColumn(this->env->GetLang(CSTR("BookFormColName")), 400);
	this->lvBook->AddColumn(this->env->GetLang(CSTR("BookFormColSource")), 60);
	this->btnBookPaste = ui->NewButton(this->pnlBookDet, this->env->GetLang(CSTR("BookFormBookPaste")));
	this->btnBookPaste->SetRect(88, 0, 75, 23, false);
	this->btnBookPaste->HandleButtonClick(OnBookPasteClicked, this);
	this->lblBookPublish = ui->NewLabel(this->pnlBookDet, this->env->GetLang(CSTR("BookFormBookPublish")));
	this->lblBookPublish->SetRect(0, 0, 70, 23, false);
	this->dtpBookPublish = ui->NewDateTimePicker(this->pnlBookDet, true);
	this->dtpBookPublish->SetRect(0, 24, 88, 19, false);
	this->dtpBookPublish->SetFormat("yyyy/MM/dd");
	this->dtpBookPublish->HandleDateChange(OnBookPublishChg, this);
	this->lblBookAuthor = ui->NewLabel(this->pnlBookDet, this->env->GetLang(CSTR("BookFormBookAuthor")));
	this->lblBookAuthor->SetRect(0, 48, 100, 23, false);
	this->txtBookAuthor = ui->NewTextBox(this->pnlBookDet, CSTR(""));
	this->txtBookAuthor->SetRect(0, 72, 168, 19, false);
	this->lblBookTitle = ui->NewLabel(this->pnlBookDet, this->env->GetLang(CSTR("BookFormBookTitle")));
	this->lblBookTitle->SetRect(0, 96, 100, 23, false);
	this->txtBookTitle = ui->NewTextBox(this->pnlBookDet, CSTR(""));
	this->txtBookTitle->SetRect(0, 120, 168, 19, false);
	this->lblBookSource = ui->NewLabel(this->pnlBookDet, this->env->GetLang(CSTR("BookFormBookSource")));
	this->lblBookSource->SetRect(0, 144, 100, 23, false);
	this->txtBookSource = ui->NewTextBox(this->pnlBookDet, CSTR(""));
	this->txtBookSource->SetRect(0, 168, 168, 19, false);
	this->lblBookURL = ui->NewLabel(this->pnlBookDet, this->env->GetLang(CSTR("BookFormBookURL")));
	this->lblBookURL->SetRect(0, 192, 100, 23, false);
	this->txtBookURL = ui->NewTextBox(this->pnlBookDet, CSTR(""));
	this->txtBookURL->SetRect(0, 216, 168, 19, false);
	this->lblBookGroup = ui->NewLabel(this->pnlBookDet, this->env->GetLang(CSTR("BookFormBookGroup")));
	this->lblBookGroup->SetRect(0, 240, 100, 23, false);
	this->txtBookGroup = ui->NewTextBox(this->pnlBookDet, CSTR(""));
	this->txtBookGroup->SetRect(0, 264, 168, 19, false);
	this->btnBookAdd = ui->NewButton(this->pnlBookDet, this->env->GetLang(CSTR("BookFormBookAdd")));
	this->btnBookAdd->SetRect(16, 288, 75, 23, false);
	this->btnBookAdd->HandleButtonClick(OnBookAddClicked, this);
	this->tcBook = ui->NewTabControl(this->pnlBook);
	this->tcBook->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpBookRef = this->tcBook->AddTabPage(this->env->GetLang(CSTR("BookFormTabRef")));
	this->tpBookAuthor = this->tcBook->AddTabPage(this->env->GetLang(CSTR("BookFormTabAuthor")));

	this->env->GetBooksAll(&this->bookList);
	this->bookList.Sort();
	this->UpdateBookList();
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganBookForm::~OrganBookForm()
{
}

void SSWR::OrganMgr::OrganBookForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

Bool SSWR::OrganMgr::OrganBookForm::IsChanged()
{
	return this->changed;
}