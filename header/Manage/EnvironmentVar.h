#ifndef _SM_MANAGE_ENVIRONMENTVAR
#define _SM_MANAGE_ENVIRONMENTVAR
#include "Data/ICaseStringUTF8Map.h"

namespace Manage
{
	class EnvironmentVar
	{
	private:
		void *envs;
		Data::ICaseStringUTF8Map<const UTF8Char *> *names;
	public:
		EnvironmentVar();
		~EnvironmentVar();

		const UTF8Char *GetValue(const UTF8Char *name);
		void SetValue(const UTF8Char *name, const UTF8Char *val);

		static UTF8Char *GetEnvValue(UTF8Char *buff, const UTF8Char *name);
	};
}
#endif
