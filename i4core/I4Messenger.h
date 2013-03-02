#pragma once

#include "I4Variant.h"

namespace i4core {

	typedef vector<I4Variant>		I4MessageArgs;

	typedef function<void (const I4MessageArgs& args)>		I4MessageCallback;

	template <typename T>
	class I4Messenger
	{
	public:
		typedef vector<pair<T*, I4MessageCallback>>			I4MessageCallbackVector;
		typedef map<unsigned int, I4MessageCallbackVector*>	I4MessageCallbackTable;
	public:
		I4Messenger(void)		{}
		~I4Messenger(void)		{}

		void bind(unsigned int msgID, T* receiver, I4MessageCallback cb)
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

			v->push_back(make_pair(receiver, cb));
		}

		void unbind(unsigned int msgID, T* receiver)
		{
			auto itr = tableMessageCallback.find(msgID);
			if (itr == tableMessageCallback.end())
				return;

			I4MessageCallbackVector* v = itr->second;
			v->erase(remove_if(v->begin(), v->end(), [&receiver](const pair<T*, I4MessageCallback>& p) { return p.first == receiver; }), v->end());

			if (v->size() == 0)
			{
				tableMessageCallback.erase(itr);
			}
		}

		void send(unsigned int msgID, const I4MessageArgs& args)
		{
			auto itr = tableMessageCallback.find(msgID);
			if (itr == tableMessageCallback.end())
				return;

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


	

