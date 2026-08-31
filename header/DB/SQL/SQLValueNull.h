#ifndef _SM_DB_SQLVALUENULL
#define _SM_DB_SQLVALUENULL
#include "DB/SQL/SQLValue.h"

namespace DB
{
	namespace SQL
	{
		class SQLValueNull : public SQLValue
		{
		public:
			SQLValueNull();
			virtual ~SQLValueNull();

			virtual ValueType GetValueType() const;
		};
	}
}
#endif
