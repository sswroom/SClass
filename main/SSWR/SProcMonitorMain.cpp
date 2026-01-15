#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/CRC32RC.h"
#include "Data/ArrayListObj.hpp"
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "IO/Path.h"
#include "Manage/Process.h"
#include "Net/OSSocketFactory.h"
#include "Net/UDPServer.h"
#include "Sync/Thread.h"
#include "Text/UTF8Reader.h"

class ProcMonitorCore
{
private:
	struct ProgInfo
	{
		NN<Text::String> progName;
		Text::String *progPath;
		UOSInt procId;
		Data::Timestamp lastSent;
	};

	IO::LogTool myLog;
	Data::ArrayListObj<ProgInfo*> progList;
	Net::OSSocketFactory sockf;
	Net::UDPServer udp;
	Crypto::Hash::CRC32RC crc;

	void NotifyServer(ProgInfo *prog, UInt8 type)
	{
		UInt8 buff[256];
		Text::CStringNN host = CSTR("sswroom.no-ip.org");
		Net::SocketUtil::AddressInfo addr;
		UnsafeArray<UTF8Char> sptr;

		Data::Timestamp currTime = Data::Timestamp::UtcNow();
		if (currTime.DiffSec(prog->lastSent) >= 60 && this->sockf.DNSResolveIP(host, addr))
		{
			buff[0] = 'S';
			buff[1] = 'm';
			buff[2] = 'P';
			buff[3] = 'M';
			WriteInt64(&buff[4], currTime.ToUnixTimestamp());
			WriteUInt32(&buff[12], currTime.inst.nanosec);
			buff[16] = type;
			WriteUInt32(&buff[17], (UInt32)prog->procId);
			sptr = prog->progName->ConcatTo(&buff[21]);
			crc.Clear();
			crc.Calc(host.v, 7);
			crc.Calc(buff, (UOSInt)(sptr - buff));
			WriteMUInt32(&sptr[0], crc.GetValueU32());
			this->udp.SendTo(addr, 5080, buff, (UOSInt)(sptr - buff + 4));
		}
		prog->lastSent = currTime;
	}

	Bool SearchProcId(ProgInfo *prog)
	{
		NN<Text::String> progPath;
		if (!progPath.Set(prog->progPath))
			return false;

		UTF8Char sbuff[512];
		UOSInt i;
		Bool ret = false;
		Manage::Process::ProcessInfo info;
		i = Text::StrLastIndexOfCharC(progPath->v, progPath->leng, IO::Path::PATH_SEPERATOR);
		NN<Manage::Process::FindProcSess> sess;
		if (Manage::Process::FindProcess(progPath->ToCString().Substring(i + 1)).SetTo(sess))
		{
			Text::StringBuilderUTF8 sb;
			while (Manage::Process::FindProcessNext(sbuff, sess, info).NotNull())
			{
				Manage::Process proc(info.processId, false);
				sb.ClearStr();
				if (proc.GetTrueProgramPath(sb))
				{
					if (sb.Equals(progPath))
					{
						prog->procId = info.processId;
						NotifyServer(prog, 1);
						Text::StringBuilderUTF8 sb;
						ret = true;
						sb.AppendC(UTF8STRC("Prog "));
						sb.Append(prog->progName);
						sb.AppendC(UTF8STRC(": Updated procId as "));
						sb.AppendUOSInt(prog->procId);
						this->myLog.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
						break;
					}
				}
			}
			Manage::Process::FindProcessClose(sess);
		}
		return ret;
	}

	void AddProg(UnsafeArray<const UTF8Char> progName, UOSInt progNameLen, UnsafeArrayOpt<const UTF8Char> progPath, UOSInt progPathLen)
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		ProgInfo *prog;
		prog = MemAlloc(ProgInfo, 1);
		prog->progName = Text::String::New(progName, progNameLen);
		prog->procId = 0;
		prog->lastSent = 0;
		UnsafeArray<const UTF8Char> nns;
		if (progPath.SetTo(nns))
		{
			sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, Text::CStringNN(nns, progPathLen));
			prog->progPath = Text::String::NewP(sbuff, sptr).Ptr();
		}
		else
		{
			prog->progPath = 0;
		}
		this->progList.Add(prog);

		if (progPath.SetTo(nns))
		{
			this->SearchProcId(prog);
		}
	}

	void LoadProgList()
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		Text::PString sarr[2];
		Text::StringBuilderUTF8 sb;

		IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("prg"));
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			Text::UTF8Reader reader(fs);
			while (true)
			{
				sb.ClearStr();
				if (!reader.ReadLine(sb, 4096))
					break;
				if (Text::StrSplitP(sarr, 2, sb, ',') == 2)
				{
					if (sarr[1].leng > 0)
					{
						AddProg(sarr[0].v, sarr[0].leng, UnsafeArray<const UTF8Char>(sarr[1].v), sarr[1].leng);
					}
					else
					{
						AddProg(sarr[0].v, sarr[0].leng, UnsafeArray<const UTF8Char>(sarr[1].v), sarr[1].leng);
					}
				}
			}
		}
	}

	static void __stdcall OnTimerTick(AnyType userObj)
	{
		NN<ProcMonitorCore> me = userObj.GetNN<ProcMonitorCore>();
		UOSInt i;
		ProgInfo *prog;
		i = me->progList.GetCount();
		while (i-- > 0)
		{
			prog = me->progList.GetItem(i);
			if (prog->progPath != 0)
			{
				if (prog->procId != 0)
				{
					Manage::Process proc(prog->procId, false);
					if (!proc.IsRunning())
					{
						prog->procId = 0;
						Text::StringBuilderUTF8 sb;
						sb.AppendC(UTF8STRC("Prog "));
						sb.Append(prog->progName);
						sb.AppendC(UTF8STRC(" stopped"));
						me->myLog.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
					}
				}
				if (prog->procId == 0)
				{
					if (!me->SearchProcId(prog))
					{
						Manage::Process proc(prog->progPath->v);
						if (proc.IsRunning())
						{
							prog->procId = proc.GetProcId();
							me->NotifyServer(prog, 0);
							Text::StringBuilderUTF8 sb;
							sb.AppendC(UTF8STRC("Prog "));
							sb.Append(prog->progName);
							sb.AppendC(UTF8STRC(" restarted, procId = "));
							sb.AppendUOSInt(prog->procId);
							me->myLog.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
						}
					}
				}
			}
		}
	}

	static void __stdcall CheckThread(NN<Sync::Thread> thread)
	{
		while (!thread->IsStopping())
		{
			OnTimerTick(thread->GetUserObj());
			thread->Wait(30000);
		}
	}

	static void __stdcall OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
	{

	}

public:
	ProcMonitorCore() : sockf(false), udp(sockf, nullptr, 0, nullptr, OnUDPPacket, this, myLog, CSTR(""), 1, true)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Log"));
		sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
		sb.AppendC(UTF8STRC("ProgLog"));
		this->myLog.AddFileLog(sb.ToCString(), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
		this->LoadProgList();
	}

	~ProcMonitorCore()
	{
		ProgInfo *prog;
		UOSInt i = this->progList.GetCount();
		while (i-- > 0)
		{
			prog = this->progList.GetItem(i);
			SDEL_STRING(prog->progPath);
			prog->progName->Release();
			MemFree(prog);
		}
	}

	void Run(NN<Core::ProgControl> progCtrl)
	{
		if (this->progList.GetCount() > 0)
		{
			Sync::Thread chkThread(CheckThread, this, CSTR("CheckThread"));
			chkThread.Start();

			progCtrl->WaitForExit(progCtrl);

			chkThread.Stop();
		}
	}
};

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	ProcMonitorCore core;
	core.Run(progCtrl);
	return 0;
}
