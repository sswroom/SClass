#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

void __stdcall UI::ListBoxLogger::TimerTick(AnyType userObj)
{
	NN<UI::ListBoxLogger> me = userObj.GetNN<UI::ListBoxLogger>();

	if (me->logCnt > 0)
	{
		UIntOS curr;
		UIntOS cnt;
		UIntOS i;
		NN<Text::String> s;
		Sync::MutexUsage mutUsage(me->mut);
		cnt = me->logCnt;
		curr = me->logIndex - cnt;
		if ((IntOS)curr < 0)
		{
			curr += me->maxLog;
		}
		i = 0;
		while (i < cnt)
		{
			me->tmpLogArr[i] = me->logArr[curr];
			me->logArr[curr] = nullptr;
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
				UIntOS lbCnt = me->lb->GetCount();
				while (lbCnt + cnt > me->maxLog)
				{
					lbCnt--;
					me->lb->RemoveItem(lbCnt);
				}
			}
			
			i = 0;
			while (cnt-- > 0)
			{
				if (me->tmpLogArr[cnt].SetTo(s))
				{
					me->lb->InsertItem(i, s->ToCString(), 0);
					s->Release();
					i++;
				}
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
				UIntOS lbCnt = me->lb->GetCount();
				while (lbCnt + cnt > me->maxLog)
				{
					me->lb->RemoveItem(0);
					lbCnt--;
				}
			}
			i = 0;
			while (i < cnt)
			{
				if (me->tmpLogArr[i].SetTo(s))
				{
					me->lb->AddItem(s, 0);
					s->Release();
					i++;
				}
			}
		}
//		this->lb->EndUpdate();
	}
}

void __stdcall UI::ListBoxLogger::OnListBoxSelChg(AnyType userObj)
{
	NN<UI::ListBoxLogger> me = userObj.GetNN<UI::ListBoxLogger>();
	NN<UI::GUITextBox> txt;
	if (me->txt.SetTo(txt))
	{
		NN<Text::String> s;
		if (me->lb->GetSelectedItemTextNew().SetTo(s))
		{
			txt->SetText(s->ToCString());
			s->Release();
		}
	}
}

UI::ListBoxLogger::ListBoxLogger(NN<UI::GUIForm> frm, NN<UI::GUIListBox> lb, UIntOS maxLog, Bool reverse)
{
	UIntOS i;
	this->lb = lb;
	this->maxLog = maxLog;
	this->reverse = reverse;
	this->frm = frm;
	this->logArr = MemAllocArr(Optional<Text::String>, this->maxLog);
	this->tmpLogArr = MemAllocArr(Optional<Text::String>, this->maxLog);
	this->logIndex = 0;
	this->logCnt = 0;
	this->timeFormat = nullptr;
	this->txt = nullptr;
	i = this->maxLog;
	while (i-- > 0)
	{
		this->logArr[i] = nullptr;
	}
	this->tmr = frm->AddTimer(500, TimerTick, this);
}

UI::ListBoxLogger::~ListBoxLogger()
{
	this->frm->RemoveTimer(this->tmr);

	UIntOS i = this->maxLog;
	while (i-- > 0)
	{
		OPTSTR_DEL(this->logArr[i]);
	}
	MemFreeArr(this->logArr);
	MemFreeArr(this->tmpLogArr);
	SDEL_TEXTC(this->timeFormat);
	
}

void UI::ListBoxLogger::LogClosed()
{
}

void UI::ListBoxLogger::LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, IO::LogHandler::LogLevel logLev)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Sync::MutexUsage mutUsage(this->mut);
	UnsafeArray<const Char> timeFormat;
	if (this->timeFormat.SetTo(timeFormat))
	{
		sptr = logTime.ToString(sbuff, timeFormat);
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
	OPTSTR_DEL(this->logArr[this->logIndex]);
	this->logArr[this->logIndex] = Text::String::New(sb.ToCString());
	this->logIndex = (this->logIndex + 1) % this->maxLog;
}

void UI::ListBoxLogger::SetTimeFormat(UnsafeArray<const Char> timeFormat)
{
	Sync::MutexUsage mutUsage(this->mut);
	SDEL_TEXTC(this->timeFormat);
	this->timeFormat = Text::StrCopyNewCh(timeFormat);
}

NN<UI::ListBoxLogger> UI::ListBoxLogger::CreateUI(NN<UI::GUIForm> frm, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, UIntOS maxLog, Bool reverse)
{
	NN<UI::GUITextBox> txt;
	NN<UI::GUIListBox> lb;
	NN<UI::ListBoxLogger> logger;
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
