#ifndef _SM_DB_SQLOBJECTPATH
#define _SM_DB_SQLOBJECTPATH
#include "DB/SQL/SQLValue.h"
#include "Text/String.h"

namespace DB
{
	namespace SQL
	{
		class SQLObjectPath : public SQLValue
		{
		private:
			NN<Text::String> objectName;
			Optional<SQLObjectPath> parentPath;
		public:
			SQLObjectPath(Text::CStringNN objName, Optional<SQLObjectPath> parentPath);
			virtual ~SQLObjectPath();

			virtual ValueType GetValueType() const;
			NN<Text::String> GetObjectName() const;
			Optional<SQLObjectPath> GetParentPath() const;
		};
	}
}
#endif
