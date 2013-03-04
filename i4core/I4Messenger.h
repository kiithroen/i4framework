#pragma once

#include "I4Variant.h"

namespace i4core {

	typedef vector<I4Variant>		I4MessageArgs;

	typedef function<void (I4MessageArgs& args)>		I4MessageCallback;

	template <typename T>
	class I4Messenger
	{
	public:
		typedef vector<pair<T*, I4MessageCallback>>			I4MessageCallbackVector;
		typedef map<unsigned int, I4MessageCallbackVector*>	I4MessageCallbackTable;
	public:
		I4Messenger(void)		{}
		~I4Messenger(void)		{}
		
		void subscribe(unsigned int msgID, T* receiver, I4MessageCallback cb)
		{
			I4MessageCallbackVector* v = nullptr;

			auto itr = tableMessageCallback.find(msgID);
			if (itr == tableMessageCallback.end())
			{
				v = new I4MessageCallbackVector;
				tableMessageCallback.insert(make_pair(msgID, v));
			}
			else
			{
				v = itr->second;
			}

			// 이미 등록했으면 그만둔다.
			auto i = find_if(v->begin(), v->end(), [&receiver](const pair<T*, I4MessageCallback>& p) { return p.first == receiver; });
			if (i != v->end())
				return;

			v->push_back(make_pair(receiver, cb));
		}

		void unsubscribe(unsigned int msgID, T* receiver)
		{
			auto itr = tableMessageCallback.find(msgID);
			if (itr == tableMessageCallback.end())
				return;

			I4MessageCallbackVector* v = itr->second;

			// 등록한거 지워버린다.
			v->erase(remove_if(v->begin(), v->end(), [&receiver](const pair<T*, I4MessageCallback>& p) { return p.first == receiver; }), v->end());

			if (v->size() == 0)
			{
				tableMessageCallback.erase(itr);
			}
		}

		void send(unsigned int msgID, I4MessageArgs& args)
		{
			auto itr = tableMessageCallback.find(msgID);
			if (itr == tableMessageCallback.end())
				return;

			// 등록되있는 모든 receiver들에게 전송.
			I4MessageCallbackVector* v = itr->second;
			for (auto& i : *v)
			{
				i.second(args);
			}
		}

	private:
		I4MessageCallbackTable		tableMessageCallback;
	};
}


	

