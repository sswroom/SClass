#ifndef _SM_MANAGE_ENVIRONMENTVAR
#define _SM_MANAGE_ENVIRONMENTVAR
#include "Data/ICaseStringMap.h"

namespace Manage
{
	class EnvironmentVar
	{
	private:
		void *envs;
		Data::ICaseStringMap<UnsafeArrayOpt<const UTF8Char>> names;
	public:
		EnvironmentVar();
		~EnvironmentVar();

		UnsafeArrayOpt<const UTF8Char> GetValue(Text::CStringNN name);
		void SetValue(Text::CStringNN name, Text::CStringNN val);

		static UnsafeArrayOpt<UTF8Char> GetEnvValue(UnsafeArray<UTF8Char> buff, Text::CStringNN name);
	};
}
#endif
