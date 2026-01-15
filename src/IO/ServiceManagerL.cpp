#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/ServiceManager.h"
#include "Manage/Process.h"

#include <stdio.h>

IntOS IO::ServiceManager::ServiceComparator::Compare(NN<ServiceItem> a, NN<ServiceItem> b) const
{
	return a->name->CompareTo(b->name);
}

IO::ServiceManager::ServiceManager()
{
}

IO::ServiceManager::~ServiceManager()
{

}

Bool IO::ServiceManager::ServiceCreate(Text::CStringNN svcName, Text::CString svcDesc, Text::CStringNN cmdLine, IO::ServiceInfo::ServiceState stype)
{
	if (svcName.leng == 0 || svcName.IndexOf('.') != INVALID_INDEX || cmdLine.leng == 0)
	{
		return false;
	}
	ServiceDetail svcDetail;
	if (this->ServiceGetDetail(svcName, svcDetail))
	{
		return false;
	}
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("/etc/systemd/system/"));
	sb.Append(svcName);
	sb.AppendC(UTF8STRC(".service"));
	{
		IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (fs.IsError())
		{
			return false;
		}
		sb.ClearStr();
		sb.AppendC(UTF8STRC("[Unit]\n"));
		sb.AppendC(UTF8STRC("Description="));
		Text::CStringNN nnsvcDesc;
		if (!svcDesc.SetTo(nnsvcDesc) || nnsvcDesc.leng == 0)
		{
			sb.Append(svcName);
		}
		else
		{
			sb.Append(nnsvcDesc);
		}
		sb.AppendC(UTF8STRC("\n\n[Service]\n"));
		sb.AppendC(UTF8STRC("ExecStart="));
		sb.Append(cmdLine);
		sb.AppendC(UTF8STRC("\n\n[Install]\n"));
		sb.AppendC(UTF8STRC("WantedBy=multi-user.target\n"));
		fs.Write(sb.ToByteArray());
	}
	sb.ClearStr();
	Manage::Process::ExecuteProcess(CSTR("systemctl daemon-reload"), sb);
	if (stype == IO::ServiceInfo::ServiceState::Active)
	{
		this->ServiceEnable(svcName);
	}
	return true;
}

Bool IO::ServiceManager::ServiceDelete(Text::CStringNN svcName)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbCmd;
	sbCmd.AppendC(UTF8STRC("systemctl status "));
	sbCmd.Append(svcName);
	sbCmd.AppendC(UTF8STRC(" --no-pager"));
	Int32 ret = Manage::Process::ExecuteProcess(sbCmd.ToCString(), sb);
	if (ret == 4 || ret == -1)
	{
		return false;
	}
	Text::CString svcFile = nullptr;
	Text::CStringNN nnsvcFile;
	Text::PString lines[2];
	UIntOS lineCnt;
	UIntOS valIndex;
	Text::PString name;
	Text::PString val;
	UIntOS i;
	lineCnt = Text::StrSplitLineP(lines, 2, sb);
	if (lineCnt == 2)
	{
		valIndex = lines[1].IndexOf(UTF8STRC(": "));
		if (valIndex == INVALID_INDEX)
		{
			return false;
		}
		while (lineCnt == 2)
		{
			lineCnt = Text::StrSplitLineP(lines, 2, lines[1]);
			if (lines[0].leng > valIndex && lines[0].v[valIndex] == ':' && lines[0].v[valIndex + 1] == ' ')
			{
				name.v = lines[0].v;
				name.leng = valIndex;
				name.v[name.leng] = 0;
				name = name.TrimAsNew();
				if (name.Equals(UTF8STRC("Loaded")))
				{
					val = lines[0].Substring(valIndex + 2);
					if (val.StartsWith(UTF8STRC("masked ")))
					{
					}
					else
					{
						i = val.IndexOf('(');
						if (i != INVALID_INDEX)
						{
							val = val.Substring(i + 1);
							i = val.LastIndexOf(')');
							if (i != INVALID_INDEX)
							{
								val.TrimToLength(i);
							}
							Text::PString sarr[4];
							i = Text::StrSplitTrimP(sarr, 4, val, ';');
							svcFile = sarr[0].ToCString();
							break;
						}
					}
				}
			}
		}
		if (svcFile.SetTo(nnsvcFile) && nnsvcFile.leng > 8 && nnsvcFile.EndsWith(UTF8STRC(".service")))
		{
			if (IO::Path::DeleteFile(nnsvcFile.v))
			{
				sb.ClearStr();
				Manage::Process::ExecuteProcess(CSTR("systemctl daemon-reload"), sb);
				return true;
			}
		}
	}
	return false;
}

Bool IO::ServiceManager::ServiceSetDesc(Text::CStringNN svcName, Text::CStringNN svcDesc)
{
	return false;
}

Bool IO::ServiceManager::ServiceStart(Text::CStringNN svcName)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbCmd;
	sbCmd.AppendC(UTF8STRC("systemctl start "));
	sbCmd.Append(svcName);
	sbCmd.AppendC(UTF8STRC(" --no-pager"));
	Int32 ret = Manage::Process::ExecuteProcess(sbCmd.ToCString(), sb);
	printf("Start ret = %d\r\n", ret);
	printf("Start msg = %s\r\n", sb.ToPtr());
	if (ret != 0)
	{
		return false;
	}
	return true;

}

Bool IO::ServiceManager::ServiceStop(Text::CStringNN svcName)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbCmd;
	sbCmd.AppendC(UTF8STRC("systemctl stop "));
	sbCmd.Append(svcName);
	sbCmd.AppendC(UTF8STRC(" --no-pager"));
	Int32 ret = Manage::Process::ExecuteProcess(sbCmd.ToCString(), sb);
	printf("Stop ret = %d\r\n", ret);
	printf("Stop msg = %s\r\n", sb.ToPtr());
	if (ret != 0)
	{
		return false;
	}
	return true;
}

Bool IO::ServiceManager::ServiceEnable(Text::CStringNN svcName)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbCmd;
	sbCmd.AppendC(UTF8STRC("systemctl enable "));
	sbCmd.Append(svcName);
	sbCmd.AppendC(UTF8STRC(" --no-pager"));
	Int32 ret = Manage::Process::ExecuteProcess(sbCmd.ToCString(), sb);
	printf("Enable ret = %d\r\n", ret);
	printf("Enable msg = %s\r\n", sb.ToPtr());
	if (ret != 0)
	{
		return false;
	}
	return true;
}

Bool IO::ServiceManager::ServiceDisable(Text::CStringNN svcName)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbCmd;
	sbCmd.AppendC(UTF8STRC("systemctl disable "));
	sbCmd.Append(svcName);
	sbCmd.AppendC(UTF8STRC(" --no-pager"));
	Int32 ret = Manage::Process::ExecuteProcess(sbCmd.ToCString(), sb);
	printf("Disable ret = %d\r\n", ret);
	printf("Disable msg = %s\r\n", sb.ToPtr());
	if (ret != 0)
	{
		return false;
	}
	return true;
}

Bool IO::ServiceManager::ServiceGetDetail(Text::CStringNN svcName, NN<ServiceDetail> svcDetail)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbCmd;
	sbCmd.AppendC(UTF8STRC("systemctl status "));
	sbCmd.Append(svcName);
	sbCmd.AppendC(UTF8STRC(" --no-pager"));
	Int32 ret = Manage::Process::ExecuteProcess(sbCmd.ToCString(), sb);
	if (ret == 4 || ret == -1)
	{
		return false;
	}
	Text::PString lines[2];
	UIntOS lineCnt;
	UIntOS valIndex;
	Text::PString name;
	Text::PString val;
	UIntOS i;
	lineCnt = Text::StrSplitLineP(lines, 2, sb);
	if (lineCnt == 2)
	{
		valIndex = lines[1].IndexOf(UTF8STRC(": "));
		if (valIndex == INVALID_INDEX)
		{
			return false;
		}
		svcDetail->memoryUsage = 0;
		svcDetail->procId = 0;
		svcDetail->startTime = nullptr;
		svcDetail->status = IO::ServiceInfo::RunStatus::Unknown;
		svcDetail->enabled = IO::ServiceInfo::ServiceState::Unknown;
		while (lineCnt == 2)
		{
			lineCnt = Text::StrSplitLineP(lines, 2, lines[1]);
			if (lines[0].leng > valIndex && lines[0].v[valIndex] == ':' && lines[0].v[valIndex + 1] == ' ')
			{
				name.v = lines[0].v;
				name.leng = valIndex;
				name.v[name.leng] = 0;
				name = name.TrimAsNew();
				if (name.Equals(UTF8STRC("Active")))
				{
					val = lines[0].Substring(valIndex + 2);
					if (val.StartsWith(UTF8STRC("active ")))
					{
						svcDetail->status = IO::ServiceInfo::RunStatus::Running;
						i = val.IndexOf(UTF8STRC(" since "));
						if (i == INVALID_INDEX)
						{
							printf("Start Time = %s\r\n", val.v.Ptr());
						}
						else
						{
							val = val.Substring(i + 7);
							i = val.IndexOf(';');
							if (i != INVALID_INDEX)
							{
								val.TrimToLength(i);
							}
							Data::DateTime dt;
							if (val.leng > 4 && val.v[3] == ' ')
							{
								val = val.Substring(4);
							}
							svcDetail->startTime = Data::Timestamp::FromStr(val.ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
							if (svcDetail->startTime.IsNull())
							{
								printf("Start Time = %s\r\n", val.v.Ptr());
							}
						}
					}
					else if (val.StartsWith(UTF8STRC("inactive (dead)")))
					{
						svcDetail->status = IO::ServiceInfo::RunStatus::Stopped;
					}
					else
					{
						printf("Active = %s\r\n", val.v.Ptr());
					}
				}
				else if (name.Equals(UTF8STRC("Main PID")))
				{
					val = lines[0].Substring(valIndex + 2);
					i = val.IndexOf(' ');
					if (i != INVALID_INDEX)
					{
						val.TrimToLength(i);
					}
					if (!val.ToUInt32(svcDetail->procId))
					{
						printf("Main PID = %s\r\n", val.v.Ptr());
					}
				}
				else if (name.Equals(UTF8STRC("Memory")))
				{
					Double vmul = 1;
					val = lines[0].Substring(valIndex + 2);
					if (val.EndsWith('M'))
					{
						val.RemoveChars(1);
						vmul = 1000000;
					}
					else if (val.EndsWith('K'))
					{
						val.RemoveChars(1);
						vmul = 1000;
					}
					else if (val.EndsWith('G'))
					{
						val.RemoveChars(1);
						vmul = 1000000000;
					}
					else if (val.EndsWith('B'))
					{
						val.RemoveChars(1);
						vmul = 1;
					}
					Double dval;
					if (val.ToDouble(dval))
					{
						svcDetail->memoryUsage = (UInt64)(vmul * dval);
					}
					else
					{
						printf("Memory = %s\r\n", val.v.Ptr());
					}
				}
				else if (name.Equals(UTF8STRC("Loaded")))
				{
					val = lines[0].Substring(valIndex + 2);
					if (val.StartsWith(UTF8STRC("masked ")))
					{
						svcDetail->enabled = IO::ServiceInfo::ServiceState::Masked;
					}
					else
					{
						i = val.IndexOf('(');
						if (i != INVALID_INDEX)
						{
							val = val.Substring(i + 1);
							i = val.LastIndexOf(')');
							if (i != INVALID_INDEX)
							{
								val.TrimToLength(i);
							}
							Text::PString sarr[4];
							i = Text::StrSplitTrimP(sarr, 4, val, ';');
							while (i-- > 0)
							{
								if (sarr[i].Equals(UTF8STRC("enabled")))
								{
									svcDetail->enabled = IO::ServiceInfo::ServiceState::Active;
								}
								else if (sarr[i].Equals(UTF8STRC("disabled")))
								{
									svcDetail->enabled = IO::ServiceInfo::ServiceState::Inactive;
								}
								else if (sarr[i].Equals(UTF8STRC("static")))
								{
									svcDetail->enabled = IO::ServiceInfo::ServiceState::Static;
								}
								else if (sarr[i].Equals(UTF8STRC("indirect")))
								{
									svcDetail->enabled = IO::ServiceInfo::ServiceState::Indirect;
								}
								else if (sarr[i].Equals(UTF8STRC("generated")))
								{
									svcDetail->enabled = IO::ServiceInfo::ServiceState::Generated;
								}
							}
						}
					}
				}
			}		
		}
	}
	else
	{
		return false;
	}
	return true;
}

UIntOS IO::ServiceManager::QueryServiceList(NN<Data::ArrayListNN<ServiceItem>> svcList)
{
	Text::StringBuilderUTF8 sb;
	Manage::Process::ExecuteProcess(CSTR("systemctl list-unit-files --no-pager"), sb);
	Text::PString lines[2];
	Text::PString svcName;
	Text::PString states[2];
	UIntOS lineCnt;
	UIntOS stateIndex;
	UIntOS ret = 0;
	NN<ServiceItem> svc;
	lineCnt = Text::StrSplitLineP(lines, 2, sb);
	if (!lines[0].StartsWith(UTF8STRC("UNIT FILE")))
	{
		return 0;
	}
	stateIndex = lines[0].IndexOf(UTF8STRC("STATE"), 10);
	if (stateIndex == INVALID_INDEX)
	{
		return 0;
	}
	while (lineCnt == 2)
	{
		lineCnt = Text::StrSplitLineP(lines, 2, lines[1]);
		if (lines[0].leng > stateIndex)
		{
			svcName.v = lines[0].v;
			svcName.leng = stateIndex - 1;
			svcName.v[svcName.leng] = 0;
			svcName.RTrim();
			if (svcName.leng > 0 && svcName.v[0])
			{
				states[1] = lines[0].SubstrTrim(stateIndex);
				Text::StrSplitWSP(states, 2, states[1]);
				svc = MemAllocNN(ServiceItem);
				svc->name = Text::String::New(svcName.ToCString());
				svc->state = IO::ServiceInfo::ServiceState::Unknown;
				svc->runStatus = IO::ServiceInfo::RunStatus::Unknown;
				if (states[0].Equals(UTF8STRC("static")))
				{
					svc->state = IO::ServiceInfo::ServiceState::Static;
				}
				else if (states[0].Equals(UTF8STRC("enabled")))
				{
					svc->state = IO::ServiceInfo::ServiceState::Active;
				}
				else if (states[0].Equals(UTF8STRC("disabled")))
				{
					svc->state = IO::ServiceInfo::ServiceState::Inactive;
				}
				else if (states[0].Equals(UTF8STRC("alias")))
				{
					svc->state = IO::ServiceInfo::ServiceState::Alias;
				}
				else if (states[0].Equals(UTF8STRC("generated")))
				{
					svc->state = IO::ServiceInfo::ServiceState::Generated;
				}
				else if (states[0].Equals(UTF8STRC("masked")))
				{
					svc->state = IO::ServiceInfo::ServiceState::Masked;
				}
				else if (states[0].Equals(UTF8STRC("indirect")))
				{
					svc->state = IO::ServiceInfo::ServiceState::Indirect;
				}
				else if (states[0].Equals(UTF8STRC("enabled-runtime")))
				{
					svc->state = IO::ServiceInfo::ServiceState::EnabledRuntime;
				}
				else if (states[0].Equals(UTF8STRC("transient")))
				{
					svc->state = IO::ServiceInfo::ServiceState::Transient;
				}
				else
				{
//					printf("State0 = %s, len = %d\r\n", states[0].v, (UInt32)states[0].leng);
//					printf("Name: %s (%d), State: %s\r\n", svcName.v, (UInt32)svcName.leng, states[0].v);
				}
				svcList->Add(svc);
				ret++;
			}
		}
	}
	return ret;
}

void IO::ServiceManager::FreeServiceList(NN<Data::ArrayListNN<ServiceItem>> svcList)
{
	UIntOS i = svcList->GetCount();
	NN<ServiceItem> svc;
	while (i-- > 0)
	{
		svc = svcList->GetItemNoCheck(i);
		svc->name->Release();
		MemFreeNN(svc);
	}
	svcList->Clear();
}
