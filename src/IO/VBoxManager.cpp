#include "Stdafx.h"
#include "IO/VBoxManager.h"
#include "Manage/Process.h"

#include <stdio.h>

UOSInt IO::VBoxManager::GetVMList(NN<Data::ArrayListNN<VMId>> vms)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(this->progPath);
	sb.AppendC(UTF8STRC(" list vms"));
	Text::StringBuilderUTF8 sbResult;
	Manage::Process::ExecuteProcess(sb.ToCString(), sbResult);
	UOSInt lineCnt;
	Text::PString sarr[2];
	NN<VMId> vm;
	UOSInt ret = 0;
	UOSInt i;
	sarr[1] = sbResult;
	while (true)
	{
		lineCnt = Text::StrSplitLineP(sarr, 2, sarr[1]);
		if (sarr[0].v[0] == '"' && (i = sarr[0].IndexOf(UTF8STRC("\" {"), 1)) >= 0)
		{
			NEW_CLASSNN(vm, VMId());
			vm->name = Text::String::New(&sarr[0].v[1], i - 1);
			vm->uuid.SetValue(sarr[0].ToCString().Substring(i + 2));
			vms->Add(vm);
			ret++;
		}
		if (lineCnt != 2)
			break;
	}
	return ret;
}

IO::VBoxManager::VBoxManager()
{
	this->progPath = nullptr;
	this->version = nullptr;
	Text::StringBuilderUTF8 sb;
	Manage::Process::ExecuteProcess(CSTR("vboxmanage --version"), sb);
	if (sb.leng > 0)
	{
		sb.TrimWSCRLF();
		this->progPath = Text::String::New(UTF8STRC("vboxmanage"));
		this->version = Text::String::New(sb.ToCString());
		this->GetVMList(this->vms);
	}
}

IO::VBoxManager::~VBoxManager()
{
	OPTSTR_DEL(this->progPath);
	OPTSTR_DEL(this->version);
	UOSInt i = this->vms.GetCount();
	NN<VMId> vm;
	while (i-- > 0)
	{
		vm = this->vms.GetItemNoCheck(i);
		vm->name->Release();
		vm.Delete();
	}
}

Optional<Text::String> IO::VBoxManager::GetVersion() const
{
	return this->version;
}

NN<const Data::ArrayListNN<IO::VBoxManager::VMId>> IO::VBoxManager::GetVMS() const
{
	return this->vms;
}

Optional<IO::VBoxVMInfo> IO::VBoxManager::GetVMInfo(NN<VMId> vm) const
{
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(this->progPath);
	sb.AppendC(UTF8STRC(" showvminfo "));
	vm->uuid.ToString(sb);
	Text::StringBuilderUTF8 sbResult;
	printf("Cmd: %s\r\n", sb.v.Ptr());
	Manage::Process::ExecuteProcess(sb.ToCString(), sbResult);
	UOSInt lineCnt;
	Text::PString sarr[2];
	VBoxVMInfo *info = 0;
	Text::CStringNN name;
	Text::CStringNN value;
	UOSInt i;
	sarr[1] = sbResult;
	while (true)
	{
		lineCnt = Text::StrSplitLineP(sarr, 2, sarr[1]);
		i = sarr[0].IndexOf(UTF8STRC(": "));
		if (i >= 0)
		{
			value = sarr[0].ToCString().Substring(i + 2).LTrim();
			sarr[0].TrimToLength(i);
			name = sarr[0].ToCString();
			if (name.Equals(UTF8STRC("Name")) && info == 0)
			{
				NEW_CLASS(info, VBoxVMInfo(value));
				if (vm->name->Equals(UTF8STRC("<inaccessible>")))
				{
					vm->name->Release();
					vm->name = info->GetName()->Clone();
				}
			}
			else if (info)
			{
				if (name.Equals(UTF8STRC("State")))
				{
					if (value.StartsWith(UTF8STRC("powered off (since ")))
					{
						info->SetState(VBoxVMInfo::State::PoweredOff);
						info->SetStateSince(Data::Timestamp::FromStr(Text::CStringNN(&value.v[19], value.leng - 20), 0));
					}
					else
					{
						printf("Unknown state: %s\r\n", value.v.Ptr());
					}
				}
			}
		}
		if (lineCnt != 2)
			break;
	}
	return info;
}
