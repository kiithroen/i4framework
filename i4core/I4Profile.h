#pragma once

#include "i4core.h"
#include "I4StopWatch.h"

namespace i4core
{
	class I4ProfileNode
	{
	public:
		I4ProfileNode(const char* _name, I4ProfileNode* _parent, I4ProfileNode* _sibling)
		: totalTime(0)
		, totalCalls(0)
		, recursionCounter(0)
		, parent(_parent)
		, child(NULL)		
		, sibling(_sibling)
		, name(_name)
		{
		};

		~I4ProfileNode()
		{
			delete child;
			delete sibling;
		}

		I4ProfileNode*	findSubNode(const char* name)
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

		void			reset()
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

		void			begin()
		{
			++totalCalls;
			if (recursionCounter++ == 0)
			{
				stopWatch.reset();
			}
		}

		bool			end()
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
			curParentNode = start;
			curChildNode = start->getChild();
		}

		void		first()
		{
			curChildNode = curParentNode->getChild();
		}

		void		next()
		{
			curChildNode = curChildNode->getSibling();
		}

		bool		isDone()
		{
			return (curChildNode == NULL);
		}

		void		enterChild(int index)
		{
			curChildNode = curParentNode->getChild();
			while (!isDone() && index != 0)
			{
				--index;
				curChildNode = curChildNode->getSibling();
			}

			if (curChildNode != NULL)
			{
				curParentNode = curChildNode;
				curChildNode = curParentNode->getChild();
			}
		}

		void		enterParent()
		{
			if (curParentNode->getParent() != NULL)
			{
				curParentNode = curParentNode->getParent();
			}

			curChildNode = curParentNode->getChild();
		}

		const char*	getCurChildName()			{ return curChildNode->getName(); }
		int			getCurChildTotalCalls()		{ return curChildNode->getTotalCalls(); }
		float		getCurChildTotalTime()		{ return curChildNode->getTotalTime(); }

		const char*	getCurParentName()			{ return curParentNode->getName(); }
		int			getCurParentTotalCalls()	{ return curParentNode->getTotalCalls(); }
		float		getCurParentTotalTime()		{ return curParentNode->getTotalTime(); }

	private:
		I4ProfileNode*		curParentNode;
		I4ProfileNode*		curChildNode;
	};

	class I4CORE_API I4ProfileManager
	{
	public:
		static void		begin(const char* name)
		{
			if (name != curNode->getName())
			{
				curNode = curNode->findSubNode(name);
			}

			curNode->begin();
		}

		static void		end()
		{
			if (curNode->end())
			{
				curNode = curNode->getParent();
			}
		}

		static void		reset()
		{
			root.reset();
		}

		static I4ProfileIterator*	getIterator()									{ return new I4ProfileIterator(&root); }
		static void					releaseIterator(I4ProfileIterator* iterator)	{ delete iterator; }

	private:
		static I4ProfileNode	root;
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

#define PROFILE(name)	I4ProfileSample __profile(name)

}