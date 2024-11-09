#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Manage/CPUDB.h"
#include "Manage/CPUInfoDetail.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#if defined(__APPLE__)
#include <sys/sysctl.h>
#endif

#include <stdio.h>

Manage::CPUInfoDetail::CPUInfoDetail()
{
	this->cpuModel = CSTR_NULL;

#if defined(__APPLE__)
	UTF8Char sbuff[256];
	size_t size = sizeof(sbuff);
	if (sysctlbyname("machdep.cpu.brand_string", sbuff, &size, 0, 0) == 0)
	{
		const Manage::CPUDB::CPUSpec *cpu = Manage::CPUDB::GetCPUSpec(Text::CString(sbuff, size - 1));
		if (cpu)
		{
			this->cpuModel = Text::CString(cpu->model, cpu->modelLen);
		}
	}
#else
	IO::FileStream fs(CSTR("/proc/cpuinfo"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Int32 cpuPart = 0;
		Text::StringBuilderUTF8 sb;
		{
			Text::UTF8Reader reader(fs);
			while (reader.ReadLine(sb, 512))
			{
				if (sb.StartsWith(UTF8STRC("CPU part	:")))
				{
					cpuPart = Text::StrToInt32(sb.ToString() + 11);
				}
				sb.ClearStr();
			}
		}
		fs.SeekFromBeginning(0);
		this->cpuModel = Manage::CPUDB::ParseCPUInfo(fs);
		if (this->cpuModel.v == 0)
		{
			if (this->clsData && Text::StrEquals((const UTF8Char*)this->clsData, (const UTF8Char*)"spade"))
			{
				this->cpuModel = CSTR("MSM8255");
			}
			else if (this->clsData && Text::StrEquals((const UTF8Char*)this->clsData, (const UTF8Char*)"Sony Mobile fusion3"))
			{
				this->cpuModel = CSTR("APQ8064");
			}
			else if (IO::Path::GetPathType(CSTR("/sys/rk3368_thermal")) == IO::Path::PathType::Directory)
			{
				this->cpuModel = CSTR("RK3368");
			}
			else if (IO::Path::GetPathType(CSTR("/sys/devices/platform/rk3026-codec")) == IO::Path::PathType::Directory)
			{
				this->cpuModel = CSTR("RK3026");
			}
			else if (IO::Path::GetPathType(CSTR("/sys/bus/platform/drivers/bcm2835_thermal")) == IO::Path::PathType::Directory)
			{
				if (cpuPart == 0xd03)
				{
					this->cpuModel = CSTR("BCM2837");
				}
				else
				{
					this->cpuModel = CSTR("BCM2835");
				}
			}
			else if (IO::Path::GetPathType(CSTR("/sys/bus/platform/drivers/rtk129x-cpufreq")) == IO::Path::PathType::Directory)
			{
				UOSInt threadCnt = Sync::ThreadUtil::GetThreadCnt();
				if (threadCnt == 2)
				{
					this->cpuModel = CSTR("RTD1293");
				}
				else
				{
					if (IO::Path::GetPathType(CSTR("/sys/bus/platform/drivers/[RTD129x PCIE Slot2]")) == IO::Path::PathType::Unknown)
					{
						this->cpuModel = CSTR("RTD1294");
					}
					else if (IO::Path::GetPathType(CSTR("/sys/bus/platform/devices/9803f000.sata:sata-port@2")) == IO::Path::PathType::Unknown)
					{
						this->cpuModel = CSTR("RTD1295");
					}
					else
					{
						this->cpuModel = CSTR("RTD1296");
					}
				}
			}
			else if (this->brand == Manage::CPUVendor::CB_TI)
			{
				UInt8 fileBuff[33];
				IO::FileStream fs2(CSTR("/sys/devices/soc0/machine"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				if (!fs2.IsError())
				{
					UOSInt i = fs2.Read(BYTEARR(fileBuff).WithSize(32));
					fileBuff[i] = 0;
					while (i > 0)
					{
						if (fileBuff[i - 1] == 13 || fileBuff[i - 1] == 10)
						{
							fileBuff[--i] = 0;
						}
						else
						{
							break;
						}
					}
					if (Text::StrEqualsC(fileBuff, i, UTF8STRC("AM335X")))
					{
						Bool hasCAN = (IO::Path::GetPathType(CSTR("/proc/net/can")) == IO::Path::PathType::Directory);
						Bool hasGraphics = false;
						Bool hasPRUICSS = false;
//						Bool hasEtherCAT = false;
						if (hasPRUICSS)
						{
							if (hasGraphics)
							{
								if (hasPRUICSS)
								{
									this->cpuModel = CSTR("AM3359");
								}
								else
								{
									this->cpuModel = CSTR("AM3358");
								}
							}
							else
							{
								if (hasPRUICSS)
								{
									this->cpuModel = CSTR("AM3357");
								}
								else
								{
									this->cpuModel = CSTR("AM3356");
								}
							}
						}
						else if (hasCAN)
						{
							if (hasGraphics)
							{
								this->cpuModel = CSTR("AM3354");
							}
							else
							{
								this->cpuModel = CSTR("AM3352");
							}
						}
						else
						{
							this->cpuModel = CSTR("AM3351");
						}
					}
				}
			}
			else if (this->brand == Manage::CPUVendor::CB_AMLOGIC)
			{
				UOSInt threadCnt = Sync::ThreadUtil::GetThreadCnt();
				if (threadCnt == 8)
				{
					this->cpuModel = CSTR("Amlogic S912");
				}
				else if (threadCnt == 4)
				{
					if (IO::Path::GetPathType(CSTR("/sys/class/amaudio")) != IO::Path::PathType::Directory)
					{
						this->cpuModel = CSTR("Amlogic S905");
					}
					else if (IO::Path::GetPathType(CSTR("/sys/class/tsdemux")) != IO::Path::PathType::Directory)
					{
						this->cpuModel = CSTR("Amlogic S905D");
					}
					else
					{
						this->cpuModel = CSTR("Amlogic S905X");
					}
				}
			}
			else if (this->brand == Manage::CPUVendor::CB_QUALCOMM)
			{
				if (IO::Path::GetPathType(CSTR("/sys/bus/platform/drivers/cpufreq-ipq40xx")) == IO::Path::PathType::Directory)
				{
					Bool hasSDCard = (IO::Path::GetPathType(CSTR("/sys/class/mmc_host")) == IO::Path::PathType::Directory);
					Bool has4_9GHz = false;
					if (!has4_9GHz)
					{
						if (!hasSDCard)
						{
							this->cpuModel = CSTR("IPQ4018");
						}
						else
						{
							this->cpuModel = CSTR("IPQ4019");
						}
					}
					else
					{
						if (!hasSDCard)
						{
							this->cpuModel = CSTR("IPQ4028");
						}
						else
						{
							this->cpuModel = CSTR("IPQ4029");
						}
					}
				}

			}
			else if (this->brand == Manage::CPUVendor::CB_MARVELL)
			{
				//////////////////////////////////
				this->cpuModel = CSTR("88F6281");
			}
		}
	}
#endif
}

Manage::CPUInfoDetail::~CPUInfoDetail()
{
}

Text::CString Manage::CPUInfoDetail::GetCPUModel()
{
	return this->cpuModel;
}

Int32 Manage::CPUInfoDetail::GetTCC()
{
	return 0;
}

Bool Manage::CPUInfoDetail::GetCPUTemp(UOSInt index, OutParam<Double> temp)
{
	Bool ret = false;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("/sys/class/thermal/thermal_zone")), index), UTF8STRC("/temp"));
	Text::StringBuilderUTF8 sb;
	{
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Reader reader(fs);
		sb.ClearStr();
		if (reader.ReadLine(sb, 512))
		{
			Double val = Text::StrToDoubleOr(sb.ToString(), 0);
			if (val < 100 && val > 0)
			{
				temp.Set(val);
			}
			else
			{
				temp.Set(val * 0.001);
			}
			ret = true;
		}
	}
	if (ret)
		return true;

	sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("/sys/class/hwmon/hwmon")), index), UTF8STRC("/device/temperature"));
	{
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Reader reader(fs);
		sb.ClearStr();
		if (reader.ReadLine(sb, 512))
		{
			if (sb.ToString()[0] == '+')
			{
				temp.Set(Text::StrToDoubleOr(sb.ToString() + 1, 0));
			}
			else
			{
				temp.Set(Text::StrToDoubleOr(sb.ToString(), 0));
			}
			ret = true;
		}
	}
	if (ret)
		return true;
	return false;
}

