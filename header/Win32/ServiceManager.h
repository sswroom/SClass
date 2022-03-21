#ifndef _SM_WIN32_SERVICEMANAGER
#define _SM_WIN32_SERVICEMANAGER

namespace Win32
{
	class ServiceManager
	{
	public:
		typedef enum
		{
			ST_AUTO,
			ST_MANUAL,
			ST_DISABLE
		} StartType;
	private:
		void *hand;

	public:
		ServiceManager();
		~ServiceManager();

	};
};
#endif
