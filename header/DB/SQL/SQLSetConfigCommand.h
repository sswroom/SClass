#ifndef _SM_DB_SQLSETCONFIGCOMMAND
#define _SM_DB_SQLSETCONFIGCOMMAND
#include "DB/SQL/SQLCommand.h"

namespace DB
{
	namespace SQL
	{
		class SQLSetConfigCommand : public SQLCommand
		{
		public:
			enum class ConfigLife
			{
				Default,
				Session,
				Local
			};
		private:
			ConfigLife configLife;
			NN<Text::String> configName;
			Optional<Text::String> configValue;
		public:
			SQLSetConfigCommand(ConfigLife configLife, NN<Text::String> configName, Text::CString configValue);
			virtual ~SQLSetConfigCommand();

			virtual CommandType GetCommandType() const;
			ConfigLife GetConfigLife() const;
			NN<Text::String> GetConfigName() const;
			Optional<Text::String> GetConfigValue() const;
		};
	}
}
#endif
