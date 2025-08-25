#ifndef _SM_DATA_INVEST_INVESTMENTMANAGER
#define _SM_DATA_INVEST_INVESTMENTMANAGER
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListTS.h"
#include "Data/FastMapNN.h"
#include "Data/Timestamp.h"
#include "DB/ReadingDB.h"
#include "Text/String.h"
#define CURRENCY(c) (*(UInt32*)(c))
#define CURRENCYSTR(c) Text::CStringNN((UTF8Char*)&c, 3)

namespace Data
{
	namespace Invest
	{
		struct TradeDetail
		{
			Data::Timestamp priceDate;
			Double cost;
			Double amount;
		};

		struct Currency
		{
			UInt32 c;
			Double current;
			Data::ArrayListTS tsList;
			Data::ArrayListDbl valList;
			Data::ArrayListNN<TradeDetail> trades;
		};

		struct Asset
		{
			UOSInt index;
			NN<Text::String> shortName;
			NN<Text::String> fullName;
			UInt32 currency;
			Double current;
			Data::ArrayListTS tsList;
			Data::ArrayListDbl valList;
			Data::ArrayListDbl divList;
			Data::ArrayListNN<TradeDetail> trades;
		};

		enum class DateFormat
		{
			MMDDYYYY,
			DDMMYYYY
		};

		class InvestmentManager
		{
		private:
			NN<Text::String> path;
			UInt32 localCurrency;
			UInt32 refCurrency;
			Data::FastMapNN<UInt32, Currency> currMap;
			Data::ArrayListStringNN accounts;
			Data::ArrayListNN<Asset> assetList;

			NN<Currency> LoadCurrency(UInt32 c);
			Bool LoadAsset(NN<Asset> ass);
			static Data::Timestamp ParseTime(NN<Text::String> s, DateFormat fmt);
		public:
			InvestmentManager(Text::CStringNN path);
			~InvestmentManager();

			Bool IsError() const;
			Bool SaveSettings() const;
			Bool SaveAccounts() const;
			Bool SaveAssets() const;
			Bool SaveCurrency(NN<Currency> curr) const;
			Bool SaveAsset(NN<Asset> ass) const;
			Bool CurrencyImport(NN<Currency> curr, NN<DB::ReadingDB> db, UOSInt timeCol, UOSInt valueCol, DateFormat fmt) const;
			UInt32 GetLocalCurrency() const { return this->localCurrency; }
			UInt32 GetRefCurrency() const { return this->refCurrency; }
			UOSInt GetCurrencyCount() const { return this->currMap.GetCount(); }
			Optional<Currency> GetCurrencyInfo(UOSInt index) const { return this->currMap.GetItem(index); }
			Bool UpdateCurrency(NN<Currency> curr, Data::Timestamp ts, Double value);
			Optional<Text::String> GetAccount(UOSInt index) const { return this->accounts.GetItem(index); }
			UOSInt GetAccountCount() const { return this->accounts.GetCount(); }
			Bool AddAccount(NN<Text::String> accountName);
			UOSInt GetAssetCount() const { return this->assetList.GetCount(); }
			Optional<Asset> GetAsset(UOSInt index) const { return this->assetList.GetItem(index); }
			Optional<Asset> AddAsset(NN<Text::String> shortName, NN<Text::String> fullName, UInt32 currency);
			Bool AssetImport(NN<Asset> ass, NN<DB::ReadingDB> db, UOSInt timeCol, UOSInt valueCol, DateFormat fmt) const;
			Bool AssetImportDiv(NN<Asset> ass, NN<DB::ReadingDB> db, UOSInt timeCol, UOSInt valueCol, DateFormat fmt) const;
		};
	}
}
#endif
