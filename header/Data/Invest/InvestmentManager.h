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
		enum class TradeType
		{
			ForeignExchange,
			FixedDeposit,
			CashToAsset,
			AssetInterest,
			AccountInterest
		};

		struct TradeDetail
		{
			Data::Timestamp tranBeginDate;
			Data::Timestamp tranEndDate;
			Data::Timestamp priceDate;
			Double cost;
			Double amount;
		};

		struct TradeEntry
		{
			TradeType type;
			UOSInt fromIndex;
			UOSInt toIndex;
			TradeDetail fromDetail;
			TradeDetail toDetail;
			Double refRate;
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
			Data::ArrayListNN<Asset> assetList;
			Data::ArrayListNN<TradeEntry> tradeList;
			Bool inited;

			NN<Currency> LoadCurrency(UInt32 c);
			Bool LoadAsset(NN<Asset> ass);
			static Data::Timestamp ParseTime(NN<Text::String> s, DateFormat fmt);
			void AddTradeEntry(NN<TradeEntry> ent);
		public:
			InvestmentManager(Text::CStringNN path);
			~InvestmentManager();

			Bool IsError() const;
			Bool SaveSettings() const;
			Bool SaveAccounts() const;
			Bool SaveAssets() const;
			Bool SaveCurrency(NN<Currency> curr) const;
			Bool SaveAsset(NN<Asset> ass) const;
			Bool SaveTransactions() const;
			Bool CurrencyImport(NN<Currency> curr, NN<DB::ReadingDB> db, UOSInt timeCol, UOSInt valueCol, DateFormat fmt) const;
			UInt32 GetLocalCurrency() const { return this->localCurrency; }
			UInt32 GetRefCurrency() const { return this->refCurrency; }
			UOSInt GetCurrencyCount() const { return this->currMap.GetCount(); }
			Optional<Currency> GetCurrencyInfo(UOSInt index) const { return this->currMap.GetItem(index); }
			Optional<Currency> FindCurrency(UInt32 c) const { return this->currMap.Get(c); }
			Bool UpdateCurrency(NN<Currency> curr, Data::Timestamp ts, Double value);
			void CurrencyCalcValues(NN<Currency> curr, Data::Date startDate, Data::Date endDate, NN<Data::ArrayListTS> dateList, NN<Data::ArrayList<Double>> valueList);
			Double CurrencyGetRate(NN<Currency> curr, Data::Timestamp ts);
			UOSInt GetAssetCount() const { return this->assetList.GetCount(); }
			Optional<Asset> GetAsset(UOSInt index) const { return this->assetList.GetItem(index); }
			Optional<Asset> AddAsset(NN<Text::String> shortName, NN<Text::String> fullName, UInt32 currency);
			Bool AssetImport(NN<Asset> ass, NN<DB::ReadingDB> db, UOSInt timeCol, UOSInt valueCol, DateFormat fmt) const;
			Bool AssetImportDiv(NN<Asset> ass, NN<DB::ReadingDB> db, UOSInt timeCol, UOSInt valueCol, DateFormat fmt) const;
			Bool UpdateAsset(NN<Asset> ass, Data::Timestamp ts, Double value, Double divValue);
			Double AssetGetPrice(NN<Asset> ass, Data::Timestamp ts) const;
			Double AssetGetAmount(NN<Asset> ass, Data::Timestamp ts) const;
			void AssetCalcValues(NN<Asset> ass, Data::Date startDate, Data::Date endDate, NN<Data::ArrayListTS> dateList, NN<Data::ArrayList<Double>> valueList);

			UOSInt GetTransactionCount() const { return this->tradeList.GetCount(); }
			Optional<TradeEntry> GetTransactionEntry(UOSInt index) const { return this->tradeList.GetItem(index); }
			Bool AddTransactionFX(Data::Timestamp ts, UInt32 curr1, Double value1, UInt32 curr2, Double value2, Double refRate);
			Bool AddTransactionDeposit(Data::Timestamp startTime, Data::Timestamp endTime, UInt32 curr, Double startValue, Double endValue);
			Bool AddTransactionAsset(Data::Timestamp startTime, Data::Timestamp endTime, Data::Timestamp priceTime, UOSInt assetIndex, Double assetAmount, Double currencyValue);
			Bool AddTransactionAInterest(Data::Timestamp startTime, Data::Timestamp endTime, UOSInt assetIndex, Double currencyValue);
			Bool UpdateTransactionAssetTime(NN<TradeEntry> t, Data::Timestamp endTime, Data::Timestamp priceTime);
			Bool AddTransactionCInterest(Data::Timestamp ts, UInt32 curr, Double currencyValue);
			Data::Timestamp GetFirstTradeTime() const;
			
			static Text::CStringNN TradeTypeGetName(TradeType type);
		};
	}
}
#endif
