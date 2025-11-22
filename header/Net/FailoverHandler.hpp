#ifndef _SM_NET_FAILOVERHANDLER
#define _SM_NET_FAILOVERHANDLER
#include "Data/ArrayListNN.hpp"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Net
{
	enum FailoverType
	{
		FT_MASTER_SLAVE,
		FT_ACTIVE_PASSIVE,
		FT_ROUND_ROBIN
	};

	template<class T> class FailoverHandler
	{
	private:
		Net::FailoverType foType;
		UOSInt lastIndex;
		Data::ArrayListNN<T> channelList;
		Sync::Mutex mut;
	public:
		FailoverHandler(FailoverType foType);
		~FailoverHandler();

		Optional<T> GetCurrChannel();
		void GetOtherChannels(NN<Data::ArrayListNN<T>> chList);
		void SetCurrChannel(NN<T> channel);
		void GetAllChannels(NN<Data::ArrayListNN<T>> chList);
		void AddChannel(NN<T> channel);
	};

	template <class T> FailoverHandler<T>::FailoverHandler(Net::FailoverType foType)
	{
		this->foType = foType;
		this->lastIndex = 0;
	}

	template <class T> FailoverHandler<T>::~FailoverHandler()
	{
		NN<T> channel;
		UOSInt i = this->channelList.GetCount();
		while (i-- > 0)
		{
			channel = this->channelList.GetItemNoCheck(i);
			channel.Delete();
		}
	}

	template <class T> Optional<T> FailoverHandler<T>::GetCurrChannel()
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (this->channelList.GetCount() == 0)
		{
			return 0;
		}
		UOSInt initIndex;
		UOSInt currIndex;
		NN<T> channel;
		switch (this->foType)
		{
		case Net::FT_ACTIVE_PASSIVE:
			initIndex = this->lastIndex;
			break;
		case Net::FT_MASTER_SLAVE:
			initIndex = 0;
			break;
		case Net::FT_ROUND_ROBIN:
			initIndex = (this->lastIndex + 1) % this->channelList.GetCount();
			break;
		default:
			return 0;
		}
		channel = this->channelList.GetItemNoCheck(initIndex);
		if (!channel->ChannelFailure())
		{
			this->lastIndex = initIndex;
			return channel;
		}
		currIndex = (initIndex + 1) % this->channelList.GetCount();
		while (currIndex != initIndex)
		{
			channel = this->channelList.GetItemNoCheck(currIndex);
			if (!channel->ChannelFailure())
			{
				this->lastIndex = currIndex;
				return channel;
			}
			currIndex = (currIndex + 1) % this->channelList.GetCount();
		}
		return 0;
	}

	template <class T> void FailoverHandler<T>::GetOtherChannels(NN<Data::ArrayListNN<T>> chList)
	{
		Sync::MutexUsage mutUsage(this->mut);
		NN<T> channel;
		UOSInt j = this->channelList.GetCount();
		UOSInt i = (this->lastIndex + 1) % j;
		while (i != this->lastIndex)
		{
			channel = this->channelList.GetItemNoCheck(i);
			if (!channel->ChannelFailure())
			{
				chList->Add(channel);
			}
			i = (i + 1) % j;
		}
	}

	template <class T> void FailoverHandler<T>::SetCurrChannel(NN<T> channel)
	{
		Sync::MutexUsage mutUsage(this->mut);
		UOSInt i = this->channelList.IndexOf(channel);
		if (i != INVALID_INDEX)
		{
			this->lastIndex = i;
		}

	}

	template <class T> void FailoverHandler<T>::GetAllChannels(NN<Data::ArrayListNN<T>> chList)
	{
		Sync::MutexUsage mutUsage(this->mut);
		chList->AddAll(this->channelList);
	}

	template <class T> void FailoverHandler<T>::AddChannel(NN<T> channel)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->channelList.Add(channel);
	}
}
#endif
