#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Manage/CPUDB.h"
#include "Manage/CPUInfoDetail.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

Manage::CPUInfoDetail::CPUInfoDetail()
{
	IO::FileStream *fs;
	this->cpuModel = 0;

	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/proc/cpuinfo", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		Int32 cpuPart = 0;
		Text::StringBuilderUTF8 sb;
		Text::UTF8Reader *reader;
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		while (reader->ReadLine(&sb, 512))
		{
			if (sb.StartsWith((const UTF8Char*)"CPU part	:"))
			{
				cpuPart = Text::StrToInt32(sb.ToString() + 11);
			}
			sb.ClearStr();
		}
		DEL_CLASS(reader);
		fs->SeekFromBeginning(0);
		this->cpuModel = Manage::CPUDB::ParseCPUInfo(fs);
		if (this->cpuModel == 0)
		{
			if (this->clsData && Text::StrEquals((const UTF8Char*)this->clsData, (const UTF8Char*)"spade"))
			{
				this->cpuModel = (const UTF8Char*)"MSM8255";
			}
			else if (this->clsData && Text::StrEquals((const UTF8Char*)this->clsData, (const UTF8Char*)"Sony Mobile fusion3"))
			{
				this->cpuModel = (const UTF8Char*)"APQ8064";
			}
			else if (IO::Path::GetPathType((const UTF8Char*)"/sys/rk3368_thermal") == IO::Path::PathType::Directory)
			{
				this->cpuModel = (const UTF8Char*)"RK3368";
			}
			else if (IO::Path::GetPathType((const UTF8Char*)"/sys/devices/platform/rk3026-codec") == IO::Path::PathType::Directory)
			{
				this->cpuModel = (const UTF8Char*)"RK3026";
			}
			else if (IO::Path::GetPathType((const UTF8Char*)"/sys/bus/platform/drivers/bcm2835_thermal") == IO::Path::PathType::Directory)
			{
				if (cpuPart == 0xd03)
				{
					this->cpuModel = (const UTF8Char*)"BCM2837";
				}
				else
				{
					this->cpuModel = (const UTF8Char*)"BCM2835";
				}
			}
			else if (IO::Path::GetPathType((const UTF8Char*)"/sys/bus/platform/drivers/rtk129x-cpufreq") == IO::Path::PathType::Directory)
			{
				OSInt threadCnt = Sync::Thread::GetThreadCnt();
				if (threadCnt == 2)
				{
					this->cpuModel = (const UTF8Char*)"RTD1293";
				}
				else
				{
					if (IO::Path::GetPathType((const UTF8Char*)"/sys/bus/platform/drivers/[RTD129x PCIE Slot2]") == IO::Path::PathType::Unknown)
					{
						this->cpuModel = (const UTF8Char*)"RTD1294";
					}
					else if (IO::Path::GetPathType((const UTF8Char*)"/sys/bus/platform/devices/9803f000.sata:sata-port@2") == IO::Path::PathType::Unknown)
					{
						this->cpuModel = (const UTF8Char*)"RTD1295";
					}
					else
					{
						this->cpuModel = (const UTF8Char*)"RTD1296";
					}
				}
			}
			else if (this->brand == Manage::CPUVendor::CB_TI)
			{
				IO::FileStream *fs2;
				UInt8 fileBuff[33];
				NEW_CLASS(fs2, IO::FileStream((const UTF8Char*)"/sys/devices/soc0/machine", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				if (!fs2->IsError())
				{
					UOSInt i = fs2->Read(fileBuff, 32);
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
					if (Text::StrEquals(fileBuff, (const UTF8Char*)"AM335X"))
					{
						Bool hasCAN = (IO::Path::GetPathType((const UTF8Char*)"/proc/net/can") == IO::Path::PathType::Directory);
						Bool hasGraphics = false;
						Bool hasPRUICSS = false;
//						Bool hasEtherCAT = false;
						if (hasPRUICSS)
						{
							if (hasGraphics)
							{
								if (hasPRUICSS)
								{
									this->cpuModel = (const UTF8Char*)"AM3359";
								}
								else
								{
									this->cpuModel = (const UTF8Char*)"AM3358";
								}
							}
							else
							{
								if (hasPRUICSS)
								{
									this->cpuModel = (const UTF8Char*)"AM3357";
								}
								else
								{
									this->cpuModel = (const UTF8Char*)"AM3356";
								}
							}
						}
						else if (hasCAN)
						{
							if (hasGraphics)
							{
								this->cpuModel = (const UTF8Char*)"AM3354";
							}
							else
							{
								this->cpuModel = (const UTF8Char*)"AM3352";
							}
						}
						else
						{
							this->cpuModel = (const UTF8Char*)"AM3351";
						}
					}
				}
				DEL_CLASS(fs2);
			}
			else if (this->brand == Manage::CPUVendor::CB_AMLOGIC)
			{
				OSInt threadCnt = Sync::Thread::GetThreadCnt();
				if (threadCnt == 8)
				{
					this->cpuModel = (const UTF8Char*)"Amlogic S912";
				}
				else if (threadCnt == 4)
				{
					if (IO::Path::GetPathType((const UTF8Char*)"/sys/class/amaudio") != IO::Path::PathType::Directory)
					{
						this->cpuModel = (const UTF8Char*)"Amlogic S905";
					}
					else if (IO::Path::GetPathType((const UTF8Char*)"/sys/class/tsdemux") != IO::Path::PathType::Directory)
					{
						this->cpuModel = (const UTF8Char*)"Amlogic S905D";
					}
					else
					{
						this->cpuModel = (const UTF8Char*)"Amlogic S905X";
					}
				}
			}
			else if (this->brand == Manage::CPUVendor::CB_QUALCOMM)
			{
				if (IO::Path::GetPathType((const UTF8Char*)"/sys/bus/platform/drivers/cpufreq-ipq40xx") == IO::Path::PathType::Directory)
				{
					Bool hasSDCard = (IO::Path::GetPathType((const UTF8Char*)"/sys/class/mmc_host") == IO::Path::PathType::Directory);
					Bool has4_9GHz = false;
					if (!has4_9GHz)
					{
						if (!hasSDCard)
						{
							this->cpuModel = (const UTF8Char*)"IPQ4018";
						}
						else
						{
							this->cpuModel = (const UTF8Char*)"IPQ4019";
						}
					}
					else
					{
						if (!hasSDCard)
						{
							this->cpuModel = (const UTF8Char*)"IPQ4028";
						}
						else
						{
							this->cpuModel = (const UTF8Char*)"IPQ4029";
						}
					}
				}

			}
			else if (this->brand == Manage::CPUVendor::CB_MARVELL)
			{
				//////////////////////////////////
				this->cpuModel = (const UTF8Char*)"88F6281";
			}
		}
	}
	DEL_CLASS(fs);
}

Manage::CPUInfoDetail::~CPUInfoDetail()
{
}

const UTF8Char *Manage::CPUInfoDetail::GetCPUModel()
{
	return this->cpuModel;
}

Int32 Manage::CPUInfoDetail::GetTCC()
{
	return 0;
}

Bool Manage::CPUInfoDetail::GetCPUTemp(UOSInt index, Double *temp)
{
	Bool ret = false;
	UTF8Char sbuff[256];
	Text::StrConcat(Text::StrOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"/sys/class/thermal/thermal_zone"), index), (const UTF8Char*)"/temp");
	Text::StringBuilderUTF8 sb;
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	sb.ClearStr();
	if (reader->ReadLine(&sb, 512))
	{
		Double val = Text::StrToDouble(sb.ToString());
		if (val < 100 && val > 0)
		{
			*temp = val;
		}
		else
		{
			*temp = val * 0.001;
		}
		ret = true;
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	if (ret)
		return true;

	Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"/sys/class/hwmon/hwmon"), index), (const UTF8Char*)"/device/temperature");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	sb.ClearStr();
	if (reader->ReadLine(&sb, 512))
	{
		if (sb.ToString()[0] == '+')
		{
			*temp = Text::StrToDouble(sb.ToString() + 1);
		}
		else
		{
			*temp = Text::StrToDouble(sb.ToString());
		}
		ret = true;
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	if (ret)
		return true;
	return false;
}

