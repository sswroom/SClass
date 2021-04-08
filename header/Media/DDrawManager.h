#ifndef _SM_MEDIA_DDRAWMANAGER
#define _SM_MEDIA_DDRAWMANAGER
#include "Data/Int64Map.h"

namespace Media
{
	class DDrawManager
	{
	private:
		Data::Int64Map<void*> *monMap;
		void *defDD;

		static Int32 __stdcall DDEnumMonCall(void *guid, Char *driverDesc, Char *driverName, void *context, void *hMonitor);
		void ReleaseAll();
	public:
		DDrawManager();
		~DDrawManager();

		Bool IsError();
		void *GetDD7(void *hMonitor);
		void ReleaseDD7(void *hMonitor);
		void RecheckMonitor();
		void Reinit();
	};
};
#endif
