#ifndef _SM_MANAGE_ENVIRONMENTVAR
#define _SM_MANAGE_ENVIRONMENTVAR
#include "Data/ICaseStringMap.h"

namespace Manage
{
	class EnvironmentVar
	{
	private:
		void *envs;
		Data::ICaseStringMap<Text::String *> names;
	public:
		EnvironmentVar();
		~EnvironmentVar();

		Text::String *GetValue(Text::CString name);
		void SetValue(Text::CString name, Text::CString val);

		static UTF8Char *GetEnvValue(UTF8Char *buff, Text::CString name);
	};
}
#endif
