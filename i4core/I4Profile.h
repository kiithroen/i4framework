#pragma once

#include "I4StopWatch.h"

namespace i4core
{
	class I4ProfileNode
	{
	public:
		I4ProfileNode(const char* _name, I4ProfileNode* _parent, I4ProfileNode* _sibling);
		~I4ProfileNode();

		I4ProfileNode* findSubNode(const char* name)
		{
			// 첫번째 자식들중에 있는지 찾아서 있으면 리턴
			I4ProfileNode* _child = child;
			while (_child)
			{
				if (_child->name == name)
					return _child;

				_child = _child->sibling;
			}

			// 없으면 새로운 자식을 만들고 첫번째 자식으로 추가하고 리턴
			I4ProfileNode* node = new I4ProfileNode(name, this, child);
			child = node;

			return child;
		}

		void reset()
		{
			totalCalls = 0;
			totalTime = 0;

			if (child)
			{
				child->reset();
			}

			if (sibling)
			{
				sibling->reset();
			}
		}

		void begin()
		{
			++totalCalls;
			if (recursionCounter++ == 0)
			{
				stopWatch.reset();
			}
		}

		bool end()
		{
			if (--recursionCounter == 0)
			{				
				totalTime += stopWatch.getElapsedTime();
				return true;
			}

			return false;
		}

		float			getTotalTime()				{ return totalTime; }
		int				getTotalCalls()				{ return totalCalls; }	
		I4ProfileNode*	getParent()					{ return parent; }
		I4ProfileNode*	getChild()					{ return child; }				
		I4ProfileNode*	getSibling()				{ return sibling; }
		const char*		getName()					{ return name; }

	private:
		I4StopWatch		stopWatch;
		float			totalTime;		
		int				totalCalls;
		int				recursionCounter;
		I4ProfileNode*	parent;
		I4ProfileNode*	child;
		I4ProfileNode*	sibling;
		const char*		name;		
	};

	class I4CORE_API I4ProfileIterator
	{
	public:
		I4ProfileIterator(I4ProfileNode* start)
		{
			curNode = start;
			curChildNode = start->getChild();
		}

		void firstChild()
		{
			curChildNode = curNode->getChild();
		}

		void nextChild()
		{
			curChildNode = curChildNode->getSibling();
		}

		bool isCurChildValid()
		{
			return (curChildNode != nullptr);
		}

		void enterChild(int index)
		{
			// 자식들을 순회하면서 인덱스만큼 가던지 아니면 마지막까지 가본다
			curChildNode = curNode->getChild();
			while (isCurChildValid() && index != 0)
			{
				--index;

				curChildNode = curChildNode->getSibling();
			}

			// 찾은 자식이 있으면 자식노드로 들어간다.
			if (curChildNode != nullptr)
			{
				curNode = curChildNode;
				curChildNode = curNode->getChild();
			}
		}

		void enterParent()
		{
			if (curNode->getParent() != nullptr)
			{
				curNode = curNode->getParent();
			}

			curChildNode = curNode->getChild();
		}

		const char*	getCurName()				{ return curNode->getName(); }
		int			getCurTotalCalls()			{ return curNode->getTotalCalls(); }
		float		getCurTotalTime()			{ return curNode->getTotalTime(); }

		const char*	getCurChildName()			{ return curChildNode->getName(); }
		int			getCurChildTotalCalls()		{ return curChildNode->getTotalCalls(); }
		float		getCurChildTotalTime()		{ return curChildNode->getTotalTime(); }

	private:
		I4ProfileNode*		curNode;
		I4ProfileNode*		curChildNode;
	};

	class I4CORE_API I4ProfileManager
	{
	public:
		static void	begin(const char* name)
		{
			if (name != curNode->getName())
			{
				curNode = curNode->findSubNode(name);
			}

			curNode->begin();
		}

		static void	end()
		{
			if (curNode->end())
			{
				curNode = curNode->getParent();
			}
		}

		static void	reset()
		{
			rootNode.reset();
		}

		static I4ProfileIterator	getRootIterator()									{ return I4ProfileIterator(&rootNode); }
		static I4ProfileNode*		getRootNode()										{ return &rootNode; }

	private:
		static I4ProfileNode	rootNode;
		static I4ProfileNode*	curNode;
	};

	class I4CORE_API I4ProfileSample
	{
	public:
		I4ProfileSample(const char* name)
		{
			I4ProfileManager::begin(name);
		}

		~I4ProfileSample()
		{
			I4ProfileManager::end();
		}
	};

	class I4ProfileWriter
	{
	public:
		virtual void write(I4ProfileNode* node, const wchar_t* fname) = 0;
	};
	
#define I4PROFILE_BLOCK(name)	I4ProfileSample __profile(name)
#define I4PROFILE_THISFUNC	I4ProfileSample __profile(__FUNCTION__)

}