#ifndef _SM_DB_SQLVALUEBOOL
#define _SM_DB_SQLVALUEBOOL
#include "DB/SQL/SQLValue.h"

namespace DB
{
	namespace SQL
	{
		class SQLValueBool : public SQLValue
		{
		private:
			Bool value;
		public:
			SQLValueBool(Bool value);
			virtual ~SQLValueBool();

			virtual ValueType GetValueType() const;
			Bool GetValue() const;
		};
	}
}
#endif
