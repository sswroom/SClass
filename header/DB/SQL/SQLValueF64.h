#ifndef _SM_DB_SQLVALUEF64
#define _SM_DB_SQLVALUEF64
#include "DB/SQL/SQLValue.h"

namespace DB
{
	namespace SQL
	{
		class SQLValueF64 : public SQLValue
		{
		private:
			Double value;
		public:
			SQLValueF64(Double value);
			virtual ~SQLValueF64();

			virtual ValueType GetValueType() const;
			Double GetValue() const;
		};
	}
}
#endif
