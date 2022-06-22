#ifndef _SM_MANAGE_ENVIRONMENTVAR
#define _SM_MANAGE_ENVIRONMENTVAR
#include "Data/ICaseStringMap.h"

namespace Manage
{
	class EnvironmentVar
	{
	private:
		void *envs;
		Data::ICaseStringMap<const UTF8Char *> names;
	public:
		EnvironmentVar();
		~EnvironmentVar();

		const UTF8Char *GetValue(Text::CString name);
		void SetValue(Text::CString name, Text::CString val);

		static UTF8Char *GetEnvValue(UTF8Char *buff, Text::CString name);
	};
}
#endif
