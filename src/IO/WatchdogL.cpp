#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Watchdog.h"
#include "Math/Unit/Temperature.h"
#include "Text/MyString.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>

namespace IO
{
	class WatchdogLinux  : public IO::Watchdog
	{
	private:
		Int32 fd;

		void Init(const Char *path);
	public:
		WatchdogLinux();
		WatchdogLinux(Int32 devNum);
		virtual ~WatchdogLinux();

		virtual Bool IsError();

		virtual Bool Keepalive() ;
		virtual Bool Enable();
		virtual Bool Disable();
		virtual Bool SetTimeoutSec(Int32 timeoutSec);
		virtual Bool GetTimeoutSec(Int32 *timeoutSec);
		virtual Bool GetTemperature(Double *temp);
	};
}

IO::Watchdog *IO::Watchdog::Create()
{
	IO::Watchdog *wd;
	NEW_CLASS(wd, IO::WatchdogLinux());
	if (!wd->IsError())
		return wd;
	DEL_CLASS(wd);

	return 0;
}

IO::Watchdog *IO::Watchdog::Create(Int32 devNum)
{
	IO::Watchdog *wd;
	NEW_CLASS(wd, IO::WatchdogLinux(devNum));
	if (!wd->IsError())
		return wd;
	DEL_CLASS(wd);

	return 0;
}


void IO::WatchdogLinux::Init(const Char *path)
{
	this->fd = open(path, O_RDWR);
}

IO::WatchdogLinux::WatchdogLinux()
{
	Init("/dev/watchdog");
}

IO::WatchdogLinux::WatchdogLinux(Int32 devNum)
{
	Char sbuff[32];
	Text::StrInt32(Text::StrConcat(sbuff, "/dev/watchdog"), devNum);
	Init(sbuff);
}

IO::WatchdogLinux::~WatchdogLinux()
{
	if (this->fd >= 0)
	{
		close(this->fd);
		this->fd = -1;
	}
}

Bool IO::WatchdogLinux::IsError()
{
	if (this->fd < 0)
	{
		return true;
	}
	return false;
}

Bool IO::WatchdogLinux::Keepalive()
{
	if (this->fd < 0)
		return false;

	return ioctl(this->fd, WDIOC_KEEPALIVE, 0) == 0;
}

Bool IO::WatchdogLinux::Enable()
{
	if (this->fd < 0)
		return false;

	int options = WDIOS_ENABLECARD;
	return ioctl(this->fd, WDIOC_SETOPTIONS, &options) == 0;
}

Bool IO::WatchdogLinux::Disable()
{
	if (this->fd < 0)
		return false;

	int options = WDIOS_DISABLECARD;
	return ioctl(this->fd, WDIOC_SETOPTIONS, &options) == 0;
}

Bool IO::WatchdogLinux::SetTimeoutSec(Int32 timeoutSec)
{
	if (this->fd < 0)
		return false;

	return ioctl(this->fd, WDIOC_SETTIMEOUT, &timeoutSec) == 0;
}

Bool IO::WatchdogLinux::GetTimeoutSec(Int32 *timeoutSec)
{
	if (this->fd < 0)
		return false;

	return ioctl(this->fd, WDIOC_GETTIMEOUT, timeoutSec) == 0;
}

Bool IO::WatchdogLinux::GetTemperature(Double *temp)
{
	if (this->fd < 0)
		return false;

	int t;
	if (ioctl(this->fd, WDIOC_GETTEMP, &t) != 0)
		return false;

	*temp = Math::Unit::Temperature::Convert(Math::Unit::Temperature::TU_FAHRENHEIT, Math::Unit::Temperature::TU_CELSIUS, t);
	return true;
}
