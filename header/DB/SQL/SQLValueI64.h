#ifndef _SM_DB_SQLVALUEI64
#define _SM_DB_SQLVALUEI64
#include "DB/SQL/SQLValue.h"

namespace DB
{
	namespace SQL
	{
		class SQLValueI64 : public SQLValue
		{
		private:
			Int64 value;
		public:
			SQLValueI64(Int64 value);
			virtual ~SQLValueI64();

			virtual ValueType GetValueType() const;
			Int64 GetValue() const;
		};
	}
}
#endif
