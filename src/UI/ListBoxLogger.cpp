#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
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
				me->lb->AddItem(me->tmpLogArr[i], 0);
				me->tmpLogArr[i]->Release();
				i++;
			}
		}
//		this->lb->EndUpdate();
	}
}

UI::ListBoxLogger::ListBoxLogger(UI::GUIForm *frm, UI::GUIListBox *lb, UOSInt maxLog, Bool reverse)
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
	i = this->maxLog;
	while (i-- > 0)
	{
		this->logArr[i] = 0;
	}
	NEW_CLASS(this->mut, Sync::Mutex());
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
	DEL_CLASS(this->mut);
	
}

void UI::ListBoxLogger::LogClosed()
{
}

void UI::ListBoxLogger::LogAdded(Data::DateTime *logTime, Text::CString logMsg, IO::ILogHandler::LogLevel logLev)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Sync::MutexUsage mutUsage(this->mut);
	if (this->timeFormat)
	{
		sptr = logTime->ToString(sbuff, this->timeFormat);
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.AppendDate(logTime);
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
	this->logArr[this->logIndex] = Text::String::New(sb.ToString(), sb.GetLength());
	this->logIndex = (this->logIndex + 1) % this->maxLog;
	mutUsage.EndUse();
}

void UI::ListBoxLogger::SetTimeFormat(const Char *timeFormat)
{
	Sync::MutexUsage mutUsage(this->mut);
	SDEL_TEXT(this->timeFormat);
	this->timeFormat = Text::StrCopyNew(timeFormat);
	mutUsage.EndUse();
}
