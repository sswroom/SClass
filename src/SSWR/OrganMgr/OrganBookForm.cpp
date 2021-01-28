#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "SSWR/OrganMgr/OrganBookForm.h"
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"

SSWR::OrganMgr::BookArrayList::BookArrayList() : Data::SortableArrayList<OrganBook*>()
{
}

SSWR::OrganMgr::BookArrayList::~BookArrayList()
{
}

void SSWR::OrganMgr::BookArrayList::Sort()
{
	ArtificialQuickSort_SortCmp((void**)arr, (Data::IComparable::CompareFunc)CompareBook, 0, this->objCnt - 1);
}

OSInt __stdcall SSWR::OrganMgr::BookArrayList::CompareBook(OrganBook *book1, OrganBook *book2)
{
	OSInt ret = book1->GetPublishDate()->CompareTo(book2->GetPublishDate());
	if (ret == 0)
	{
		ret = Text::StrCompareICase(book1->GetDispAuthor(), book2->GetDispAuthor());
		if (ret == 0)
		{
			ret = Text::StrCompareICase(book1->GetTitle(), book2->GetTitle());
		}
	}
	return ret;
}

UOSInt SSWR::OrganMgr::BookArrayList::SortedInsert(OrganBook *Val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	i = 0;
	j = objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = CompareBook(arr[k], Val);
		if (l > 0)
		{
			j = k - 1;
		}
		else if (l < 0)
		{
			i = k + 1;
		}
		else
		{
			i = k + 1;
			break;
		}
	}

	if (objCnt == this->capacity)
	{
		OrganBook **newArr = MemAlloc(OrganBook*, this->capacity << 1);
		k = this->objCnt;
		while (k-- > 0)
		{
			newArr[k] = arr[k];
		}
		this->capacity = this->capacity << 1;
		MemFree(arr);
		arr = newArr;
	}
	j = objCnt;
	while (j > i)
	{
		arr[j] = arr[j - 1];
		j--;
	}
	objCnt++;
	arr[i] = Val;
	return i;
}

OSInt SSWR::OrganMgr::BookArrayList::SortedIndexOf(OrganBook *Val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	i = 0;
	j = objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = CompareBook(arr[k], Val);
		if (l > 0)
		{
			j = k - 1;
		}
		else if (l < 0)
		{
			i = k + 1;
		}
		else
		{
			return k;
		}
	}
	return -i - 1;
}

void __stdcall SSWR::OrganMgr::OrganBookForm::OnBookPublishChg(void *userObj, Data::DateTime *newDate)
{
	OrganBookForm *me = (OrganBookForm*)userObj;
	Data::DateTime currTime(newDate->GetYear(), 1, 1, 0, 0, 0);
	OSInt i = 0;
	OSInt j = me->bookList->GetCount() - 1;
	OSInt k;
	OrganBook *book;
	while (i <= j)
	{
		k = (i + j) >> 1;
		book = me->bookList->GetItem(k);
		if (book->GetPublishDate()->CompareTo(&currTime) >= 0)
		{
			j = k - 1;
		}
		else
		{
			i = k + 1;
		}
	}
	if (i >= (OSInt)me->bookList->GetCount())
	{
		if (me->bookList->GetCount() > 0)
		{
			me->lvBook->EnsureVisible(me->bookList->GetCount() - 1);
			me->lvBook->SetSelectedIndex(me->bookList->GetCount() - 1);
		}
	}
	else
	{
		UOSInt w;
		UOSInt h;
		me->lvBook->GetSizeP(&w, &h);
		j = h >> 1;
		Int32 rect[4];
		me->lvBook->GetItemRectP(i, rect);
		if (rect[1] > j)
		{
			k = i + j / rect[3];
			if (k >= (OSInt)me->bookList->GetCount())
				k = me->bookList->GetCount() - 1;
		}
		else
		{
			k = i - j / rect[3];
			if (k < 0)
				k = 0;
		}
		me->lvBook->EnsureVisible(k);
		me->lvBook->SetSelectedIndex(i);
	}
}

void __stdcall SSWR::OrganMgr::OrganBookForm::OnBookAddClicked(void *userObj)
{
	OrganBookForm *me = (OrganBookForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sb3;
	Text::StringBuilderUTF8 sb4;
	me->txtBookAuthor->GetText(&sb);
	me->txtBookTitle->GetText(&sb2);
	me->txtBookSource->GetText(&sb3);
	me->txtBookURL->GetText(&sb4);
	if (sb.GetLength() <= 0)
	{
		UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"BookFormInputAuthor"), (const UTF8Char*)"Error", me);
		return;
	}
	if (sb2.GetLength() <= 0)
	{
		UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"BookFormInputName"), (const UTF8Char*)"Error", me);
		return;
	}
	if (sb3.GetLength() <= 0)
	{
		UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"BookFormInputSource"), (const UTF8Char*)"Error", me);
		return;
	}
	Data::DateTime publishDate;
	me->dtpBookPublish->GetSelectedTime(&publishDate);
	if (!me->env->NewBook(sb2.ToString(), sb.ToString(), sb3.ToString(), &publishDate, sb4.ToString()))
	{
		UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"BookFormDBError"), (const UTF8Char*)"Error", me);
		return;
	}

	me->changed = true;
	me->bookList->Clear();
	me->env->GetBooksAll(me->bookList);
	me->bookList->Sort();
	me->UpdateBookList();
	me->txtBookAuthor->SetText((const UTF8Char*)"");
	me->txtBookTitle->SetText((const UTF8Char*)"");
	me->txtBookSource->SetText((const UTF8Char*)"");
	me->txtBookURL->SetText((const UTF8Char*)"");
}

void __stdcall SSWR::OrganMgr::OrganBookForm::OnBookPasteClicked(void *userObj)
{
	OrganBookForm *me = (OrganBookForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Data::DateTime dt;
	Win32::Clipboard::GetString(me->GetHandle(), &sb);
	if (sb.GetLength() > 0)
	{
		sb.Replace('\n', ' ');
		sb.Replace('\r', ' ');
		sb.Replace((const UTF8Char*)"  ", (const UTF8Char*)" ");

		OSInt digitCnt = 0;
		OSInt i = 1;
		OSInt j;
		OSInt k;
		UTF8Char *chars = sb.ToString();
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
						me->txtBookAuthor->SetText(sb2.ToString());
						sb2.ClearStr();
						sb2.AppendChar(chars[i - 3], 4);
						dt.SetValue((UInt16)sb2.ToInt32(), 1, 1, 0, 0, 0, 0);
						me->dtpBookPublish->SetValue(&dt);
						sb.SetSubstr(i + 2);
						sb.Trim();
					}
					else
					{
						sb2.ClearStr();
						sb2.AppendC(chars, i - 3);
						sb2.Trim();
						me->txtBookAuthor->SetText(sb2.ToString());
						sb2.ClearStr();
						sb2.AppendChar(chars[i - 3], 4);
						dt.SetValue((UInt16)sb2.ToInt32(), 1, 1, 0, 0, 0, 0);
						me->dtpBookPublish->SetValue(&dt);
						sb.SetSubstr(i + 1);
						sb.Trim();
					}

					chars = sb.ToString();
					j = sb.GetLength();
					i = 0;
					while (true)
					{
						i = sb.IndexOf((const UTF8Char*)".", i);
						if (i == -1)
						{
							me->txtBookTitle->SetText(sb.ToString());
							me->txtBookSource->SetText((const UTF8Char*)"");
							me->txtBookURL->SetText((const UTF8Char*)"");
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
							me->txtBookTitle->SetText(sb2.ToString());
							sb.SetSubstr(k);
							sb.Trim();
							k = sb.IndexOf((const UTF8Char*)"http://");
							if (k >= 0)
							{
								sb2.ClearStr();
								sb2.AppendC(sb.ToString(), k);
								sb2.Trim();
								me->txtBookSource->SetText(sb2.ToString());
								sb.SetSubstr(k)->Trim();
								me->txtBookURL->SetText(sb.ToString());
							}
							else
							{
								me->txtBookSource->SetText(sb.ToString());
								me->txtBookURL->SetText((const UTF8Char*)"");
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
	OSInt i = 0;
	OSInt j = this->bookList->GetCount();
	OSInt k;
	this->lvBook->ClearItems();
	while (i < j)
	{
		book = this->bookList->GetItem(i);;
		book->GetPublishDate()->ToString(sbuff, "yyyy-MM");
		k = this->lvBook->AddItem(sbuff, book);
		this->lvBook->SetSubItem(k, 1, book->GetDispAuthor());
		this->lvBook->SetSubItem(k, 2, book->GetTitle());
		this->lvBook->SetSubItem(k, 3, book->GetPress());
		i++;
	}
}

SSWR::OrganMgr::OrganBookForm::OrganBookForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->env = env;
	this->changed = false;
	NEW_CLASS(this->bookList, BookArrayList());

	this->SetText(this->env->GetLang((const UTF8Char*)"BookFormTitle"));
	this->SetFont(0, 10.5, false);

	NEW_CLASS(this->pnlBook, UI::GUIPanel(ui, this));
	this->pnlBook->SetRect(0, 0, 168, 500, false);
	this->pnlBook->SetDockType(UI::GUIControl::DOCK_RIGHT);
	NEW_CLASS(this->pnlBookDet, UI::GUIPanel(ui, this->pnlBook));
	this->pnlBookDet->SetRect(0, 0, 168, 320, false);
	this->pnlBookDet->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->hspBook, UI::GUIHSplitter(ui, this, 3, true));
	NEW_CLASS(this->lvBook, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvBook->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvBook->SetShowGrid(true);
	this->lvBook->SetFullRowSelect(true);
	this->lvBook->AddColumn(this->env->GetLang((const UTF8Char*)"BookFormColDate"), 60);
	this->lvBook->AddColumn(this->env->GetLang((const UTF8Char*)"BookFormColAuthor"), 120);
	this->lvBook->AddColumn(this->env->GetLang((const UTF8Char*)"BookFormColName"), 400);
	this->lvBook->AddColumn(this->env->GetLang((const UTF8Char*)"BookFormColSource"), 60);
	NEW_CLASS(this->btnBookPaste, UI::GUIButton(ui, this->pnlBookDet, this->env->GetLang((const UTF8Char*)"BookFormBookPaste")));
	this->btnBookPaste->SetRect(88, 0, 75, 23, false);
	this->btnBookPaste->HandleButtonClick(OnBookPasteClicked, this);
	NEW_CLASS(this->lblBookPublish, UI::GUILabel(ui, this->pnlBookDet, this->env->GetLang((const UTF8Char*)"BookFormBookPublish")));
	this->lblBookPublish->SetRect(0, 0, 70, 23, false);
	NEW_CLASS(this->dtpBookPublish, UI::GUIDateTimePicker(ui, this->pnlBookDet, UI::GUIDateTimePicker::ST_CALENDAR));
	this->dtpBookPublish->SetRect(0, 24, 88, 19, false);
	this->dtpBookPublish->SetFormat("yyyy/MM/dd");
	this->dtpBookPublish->HandleDateChange(OnBookPublishChg, this);
	NEW_CLASS(this->lblBookAuthor, UI::GUILabel(ui, this->pnlBookDet, this->env->GetLang((const UTF8Char*)"BookFormBookAuthor")));
	this->lblBookAuthor->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->txtBookAuthor, UI::GUITextBox(ui, this->pnlBookDet, (const UTF8Char*)""));
	this->txtBookAuthor->SetRect(0, 72, 168, 19, false);
	NEW_CLASS(this->lblBookTitle, UI::GUILabel(ui, this->pnlBookDet, this->env->GetLang((const UTF8Char*)"BookFormBookTitle")));
	this->lblBookTitle->SetRect(0, 96, 100, 23, false);
	NEW_CLASS(this->txtBookTitle, UI::GUITextBox(ui, this->pnlBookDet, (const UTF8Char*)""));
	this->txtBookTitle->SetRect(0, 120, 168, 19, false);
	NEW_CLASS(this->lblBookSource, UI::GUILabel(ui, this->pnlBookDet, this->env->GetLang((const UTF8Char*)"BookFormBookSource")));
	this->lblBookSource->SetRect(0, 144, 100, 23, false);
	NEW_CLASS(this->txtBookSource, UI::GUITextBox(ui, this->pnlBookDet, (const UTF8Char*)""));
	this->txtBookSource->SetRect(0, 168, 168, 19, false);
	NEW_CLASS(this->lblBookURL, UI::GUILabel(ui, this->pnlBookDet, this->env->GetLang((const UTF8Char*)"BookFormBookURL")));
	this->lblBookURL->SetRect(0, 192, 100, 23, false);
	NEW_CLASS(this->txtBookURL, UI::GUITextBox(ui, this->pnlBookDet, (const UTF8Char*)""));
	this->txtBookURL->SetRect(0, 216, 168, 19, false);
	NEW_CLASS(this->lblBookGroup, UI::GUILabel(ui, this->pnlBookDet, this->env->GetLang((const UTF8Char*)"BookFormBookGroup")));
	this->lblBookGroup->SetRect(0, 240, 100, 23, false);
	NEW_CLASS(this->txtBookGroup, UI::GUITextBox(ui, this->pnlBookDet, (const UTF8Char*)""));
	this->txtBookGroup->SetRect(0, 264, 168, 19, false);
	NEW_CLASS(this->btnBookAdd, UI::GUIButton(ui, this->pnlBookDet, this->env->GetLang((const UTF8Char*)"BookFormBookAdd")));
	this->btnBookAdd->SetRect(16, 288, 75, 23, false);
	this->btnBookAdd->HandleButtonClick(OnBookAddClicked, this);
	NEW_CLASS(this->tcBook, UI::GUITabControl(ui, this->pnlBook));
	this->tcBook->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpBookRef = this->tcBook->AddTabPage(this->env->GetLang((const UTF8Char*)"BookFormTabRef"));
	this->tpBookAuthor = this->tcBook->AddTabPage(this->env->GetLang((const UTF8Char*)"BookFormTabAuthor"));

	this->env->GetBooksAll(this->bookList);
	this->bookList->Sort();
	this->UpdateBookList();
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganBookForm::~OrganBookForm()
{
	DEL_CLASS(this->bookList);
}

void SSWR::OrganMgr::OrganBookForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

Bool SSWR::OrganMgr::OrganBookForm::IsChanged()
{
	return this->changed;
}