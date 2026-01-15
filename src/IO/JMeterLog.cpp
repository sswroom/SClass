#include "Stdafx.h"
#include "DB/ColDef.h"
#include "DB/CSVFile.h"
#include "IO/JMeterLog.h"

void IO::JMeterLog::FreeGroup(NN<JMeterGroup> group)
{
	group->label->Release();
	group->url->Release();
	group.Delete();
}

void IO::JMeterLog::FreeThread(NN<JMeterThread> thread)
{
	thread->name->Release();
	thread.Delete();
}

void IO::JMeterLog::FreeLog(NN<JMeterLogItem> log)
{
	log->responseMessage->Release();
	log->failureMessage->Release();
	log.Delete();
}

NN<IO::JMeterGroup> IO::JMeterLog::GetOrCreateGroup(Text::CStringNN label, Text::CStringNN url)
{
	NN<JMeterGroup> group;
	if (this->groups.GetC(label).SetTo(group))
		return group;
	NEW_CLASSNN(group, JMeterGroup());
	group->label = Text::String::New(label);
	group->url = Text::String::New(url);
	this->groups.Put(group->label, group);
	return group;
}

NN<Text::String> IO::JMeterLog::NewResponseCode(Text::CStringNN responseCode)
{
	IntOS i = this->responseCodes.SortedIndexOfC(responseCode);
	if (i >= 0)
		return this->responseCodes.GetItemNoCheck((UIntOS)i);
	NN<Text::String> s = Text::String::New(responseCode);
	this->responseCodes.Insert((UIntOS)~i, s);
	return s;
}

NN<IO::JMeterThread> IO::JMeterLog::GetOrCreateThread(Text::CStringNN threadName)
{
	NN<JMeterThread> t;
	if (this->threads.GetC(threadName).SetTo(t))
		return t;
	NEW_CLASSNN(t, JMeterThread());
	t->name = Text::String::New(threadName);
	this->threads.Put(t->name, t);
	return t;
}

IO::JMeterLog::JMeterLog(Text::CStringNN fileName)
{
	DB::CSVFile csv(fileName, 65001);
	NN<DB::DBReader> r;
	this->minTime = 0;
	this->maxTime = 0;
	if (csv.QueryTableData(nullptr, CSTR("csv"), nullptr, 0, 0, nullptr, nullptr).SetTo(r))
	{
		Bool succ = true;
		DB::ColDef colDef(CSTR(""));
		if (!r->GetColDef(0, colDef) || !colDef.GetColName()->Equals(CSTR("timeStamp"))) succ = false;
		if (!r->GetColDef(1, colDef) || !colDef.GetColName()->Equals(CSTR("elapsed"))) succ = false;
		if (!r->GetColDef(2, colDef) || !colDef.GetColName()->Equals(CSTR("label"))) succ = false;
		if (!r->GetColDef(3, colDef) || !colDef.GetColName()->Equals(CSTR("responseCode"))) succ = false;
		if (!r->GetColDef(4, colDef) || !colDef.GetColName()->Equals(CSTR("responseMessage"))) succ = false;
		if (!r->GetColDef(5, colDef) || !colDef.GetColName()->Equals(CSTR("threadName"))) succ = false;
		if (!r->GetColDef(6, colDef) || !colDef.GetColName()->Equals(CSTR("dataType"))) succ = false;
		if (!r->GetColDef(7, colDef) || !colDef.GetColName()->Equals(CSTR("success"))) succ = false;
		if (!r->GetColDef(8, colDef) || !colDef.GetColName()->Equals(CSTR("failureMessage"))) succ = false;
		if (!r->GetColDef(9, colDef) || !colDef.GetColName()->Equals(CSTR("bytes"))) succ = false;
		if (!r->GetColDef(10, colDef) || !colDef.GetColName()->Equals(CSTR("sentBytes"))) succ = false;
		if (!r->GetColDef(11, colDef) || !colDef.GetColName()->Equals(CSTR("grpThreads"))) succ = false;
		if (!r->GetColDef(12, colDef) || !colDef.GetColName()->Equals(CSTR("allThreads"))) succ = false;
		if (!r->GetColDef(13, colDef) || !colDef.GetColName()->Equals(CSTR("URL"))) succ = false;
		if (!r->GetColDef(14, colDef) || !colDef.GetColName()->Equals(CSTR("Latency"))) succ = false;
		if (!r->GetColDef(15, colDef) || !colDef.GetColName()->Equals(CSTR("IdleTime"))) succ = false;
		if (!r->GetColDef(16, colDef) || !colDef.GetColName()->Equals(CSTR("Connect"))) succ = false;
		if (succ)
		{
			Text::StringBuilderUTF8 sb1;
			Text::StringBuilderUTF8 sb2;
			NN<JMeterGroup> group;
			NN<JMeterThread> thread;
			NN<JMeterLogItem> log;
			while (r->ReadNext())
			{
				sb1.ClearStr();
				r->GetStr(2, sb1);
				sb2.ClearStr();
				r->GetStr(13, sb2);
				group = this->GetOrCreateGroup(sb1.ToCString(), sb2.ToCString());
				NEW_CLASSNN(log, JMeterLogItem());
				log->startTime = r->GetInt64(0);
				log->endTime = log->startTime + r->GetInt64(1);
				log->group = group;
				sb1.ClearStr();
				r->GetStr(3, sb1);
				log->responseCode = this->NewResponseCode(sb1.ToCString());
				log->responseMessage = r->GetNewStrNN(4);
				sb1.ClearStr();
				r->GetStr(5, sb1);
				thread = this->GetOrCreateThread(sb1.ToCString());
				log->threadName = thread->name;
				sb1.ClearStr();
				r->GetStr(7, sb1);
				log->success = sb1.Equals(CSTR("true"));
				log->failureMessage = r->GetNewStrNN(8);
				log->bytes = (UInt32)r->GetInt32(9);
				log->sentBytes = (UInt32)r->GetInt32(10);
				log->grpThreads = (UInt32)r->GetInt32(11);
				log->allThreads = (UInt32)r->GetInt32(12);
				log->latency = (UInt32)r->GetInt32(14);
				log->idleTime = (UInt32)r->GetInt32(15);
				log->connect = (UInt32)r->GetInt32(16);
				group->logs.Add(log);
				thread->logs.Add(log);
				this->logs.Add(log);
				if (this->maxTime == 0)
				{
					this->minTime = log->startTime;
					this->maxTime = log->endTime;
				}
				else
				{
					if (this->minTime > log->startTime) this->minTime = log->startTime;
					if (this->maxTime < log->endTime) this->maxTime = log->endTime;
				}
			}
		}
		csv.CloseReader(r);
	}
}

IO::JMeterLog::~JMeterLog()
{
	this->logs.FreeAll(FreeLog);
	this->groups.FreeAll(FreeGroup);
	this->threads.FreeAll(FreeThread);
	this->responseCodes.FreeAll();
}

UIntOS IO::JMeterLog::GetConcurrCnt(Int64 ts) const
{
	NN<JMeterLogItem> log;
	UIntOS cnt = 0;
	UIntOS i = this->logs.GetCount();
	while (i-- > 0)
	{
		log = this->logs.GetItemNoCheck(i);
		if (log->startTime <= ts && log->endTime >= ts)
		{
			cnt++;
		}
	}
	return cnt;
}
