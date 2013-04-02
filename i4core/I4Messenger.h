#pragma once

#include "I4Variant.h"

namespace i4core {

	typedef vector<Variant>		MessageArgs;

	typedef function<void (MessageArgs& args)>		MessageCallback;

	template <typename T>
	class Messenger
	{
	public:
		typedef vector<pair<T*, MessageCallback>>			MessageCallbackVector;
		typedef map<unsigned int, MessageCallbackVector*>	MessageCallbackTable;
	public:
		Messenger(void)		{}
		~Messenger(void)		{}
		
		void subscribe(unsigned int msgID, T* receiver, MessageCallback cb)
		{
			MessageCallbackVector* v = nullptr;

			auto itr = tableMessageCallback.find(msgID);
			if (itr == tableMessageCallback.end())
			{
				v = new MessageCallbackVector;
				tableMessageCallback.insert(make_pair(msgID, v));
			}
			else
			{
				v = itr->second;
			}

			// 이미 등록했으면 그만둔다.
			auto i = find_if(v->begin(), v->end(), [&receiver](const pair<T*, MessageCallback>& p) { return p.first == receiver; });
			if (i != v->end())
				return;

			v->push_back(make_pair(receiver, cb));
		}

		void unsubscribe(unsigned int msgID, T* receiver)
		{
			auto itr = tableMessageCallback.find(msgID);
			if (itr == tableMessageCallback.end())
				return;

			MessageCallbackVector* v = itr->second;

			// 등록한거 지워버린다.
			v->erase(remove_if(v->begin(), v->end(), [&receiver](const pair<T*, MessageCallback>& p) { return p.first == receiver; }), v->end());

			if (v->size() == 0)
			{
				delete v;
				tableMessageCallback.erase(itr);
			}
		}

		void send(unsigned int msgID, MessageArgs& args)
		{
			auto itr = tableMessageCallback.find(msgID);
			if (itr == tableMessageCallback.end())
				return;

			// 등록되있는 모든 receiver들에게 전송.
			MessageCallbackVector* v = itr->second;
			for (auto& i : *v)
			{
				i.second(args);
			}
		}

	private:
		MessageCallbackTable		tableMessageCallback;
	};
}


	

