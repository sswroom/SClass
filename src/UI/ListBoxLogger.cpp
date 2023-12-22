#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

void __stdcall UI::ListBoxLogger::TimerTick(void *userObj)
{
	UI::ListBoxLogger *me = (UI::ListBoxLogger*)userObj;

	if (me->logCnt > 0)
	{
		UOSInt curr;
		UOSInt cnt;
		UOSInt i;
		Sync::MutexUsage mutUsage(me->mut);
		cnt = me->logCnt;
		curr = me->logIndex - cnt;
		if ((OSInt)curr < 0)
		{
			curr += me->maxLog;
		}
		i = 0;
		while (i < cnt)
		{
			me->tmpLogArr[i] = me->logArr[curr];
			me->logArr[curr] = 0;
			i++;
			curr = (curr + 1) % me->maxLog;
		}
		me->logCnt = 0;
		mutUsage.EndUse();

//		this->lb->BeginUpdate();
		if (me->reverse)
		{
			if (cnt >= me->maxLog)
			{
				me->lb->ClearItems();
			}
			else
			{
				UOSInt lbCnt = me->lb->GetCount();
				while (lbCnt + cnt > me->maxLog)
				{
					lbCnt--;
					me->lb->RemoveItem(lbCnt);
				}
			}
			
			i = 0;
			while (cnt-- > 0)
			{
				me->lb->InsertItem(i, me->tmpLogArr[cnt], 0);
				me->tmpLogArr[cnt]->Release();
				i++;
			}
		}
		else
		{
			if (cnt >= me->maxLog)
			{
				me->lb->ClearItems();
			}
			else
			{
				UOSInt lbCnt = me->lb->GetCount();
				while (lbCnt + cnt > me->maxLog)
				{
					me->lb->RemoveItem(0);
					lbCnt--;
				}
			}
			i = 0;
			while (i < cnt)
			{
				me->lb->AddItem(Text::String::OrEmpty(me->tmpLogArr[i]), 0);
				me->tmpLogArr[i]->Release();
				i++;
			}
		}
//		this->lb->EndUpdate();
	}
}

void __stdcall UI::ListBoxLogger::OnListBoxSelChg(void *userObj)
{
	UI::ListBoxLogger *me = (UI::ListBoxLogger*)userObj;
	NotNullPtr<UI::GUITextBox> txt;
	if (me->txt.SetTo(txt))
	{
		NotNullPtr<Text::String> s;
		if (me->lb->GetSelectedItemTextNew().SetTo(s))
		{
			txt->SetText(s->ToCString());
			s->Release();
		}
	}
}

UI::ListBoxLogger::ListBoxLogger(NotNullPtr<UI::GUIForm> frm, NotNullPtr<UI::GUIListBox> lb, UOSInt maxLog, Bool reverse)
{
	UOSInt i;
	this->lb = lb;
	this->maxLog = maxLog;
	this->reverse = reverse;
	this->frm = frm;
	this->logArr = MemAlloc(Text::String *, this->maxLog);
	this->tmpLogArr = MemAlloc(Text::String *, this->maxLog);
	this->logIndex = 0;
	this->logCnt = 0;
	this->timeFormat = 0;
	this->txt = 0;
	i = this->maxLog;
	while (i-- > 0)
	{
		this->logArr[i] = 0;
	}
	this->tmr = frm->AddTimer(500, TimerTick, this);
}

UI::ListBoxLogger::~ListBoxLogger()
{
	this->frm->RemoveTimer(this->tmr);

	UOSInt i = this->maxLog;
	while (i-- > 0)
	{
		if (this->logArr[i])
		{
			this->logArr[i]->Release();
		}
	}
	MemFree(this->logArr);
	MemFree(this->tmpLogArr);
	SDEL_TEXT(this->timeFormat);
	
}

void UI::ListBoxLogger::LogClosed()
{
}

void UI::ListBoxLogger::LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, IO::LogHandler::LogLevel logLev)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Sync::MutexUsage mutUsage(this->mut);
	if (this->timeFormat)
	{
		sptr = logTime.ToString(sbuff, this->timeFormat);
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.AppendTSNoZone(logTime);
	}
	sb.AppendUTF8Char('\t');
	sb.Append(logMsg);
	if (this->logCnt < this->maxLog)
	{
		this->logCnt++;
	}
	if (this->logArr[this->logIndex])
	{
		this->logArr[this->logIndex]->Release();
	}
	this->logArr[this->logIndex] = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	this->logIndex = (this->logIndex + 1) % this->maxLog;
}

void UI::ListBoxLogger::SetTimeFormat(const Char *timeFormat)
{
	Sync::MutexUsage mutUsage(this->mut);
	SDEL_TEXT(this->timeFormat);
	this->timeFormat = Text::StrCopyNew(timeFormat);
	mutUsage.EndUse();
}

NotNullPtr<UI::ListBoxLogger> UI::ListBoxLogger::CreateUI(NotNullPtr<UI::GUIForm> frm, NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> ctrl, UOSInt maxLog, Bool reverse)
{
	NotNullPtr<UI::GUITextBox> txt;
	NotNullPtr<UI::GUIListBox> lb;
	NotNullPtr<UI::ListBoxLogger> logger;
	txt = ui->NewTextBox(ctrl, CSTR(""));
	txt->SetReadOnly(true);
	txt->SetRect(0, 0, 100, 23, false);
	txt->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	lb = ui->NewListBox(ctrl, false);
	lb->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASSNN(logger, UI::ListBoxLogger(frm, lb, maxLog, reverse));
	logger->txt = txt;
	lb->HandleSelectionChange(OnListBoxSelChg, logger.Ptr());
	return logger;
}
