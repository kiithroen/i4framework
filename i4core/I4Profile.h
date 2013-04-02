#pragma once

#include "I4StopWatch.h"

namespace i4core
{
	class ProfileNode
	{
	public:
		ProfileNode(const char* _name, ProfileNode* _parent, ProfileNode* _sibling);
		~ProfileNode();

		ProfileNode* findSubNode(const char* name)
		{
			// ù��° �ڽĵ��߿� �ִ��� ã�Ƽ� ������ ����
			ProfileNode* _child = child;
			while (_child)
			{
				if (_child->name == name)
					return _child;

				_child = _child->sibling;
			}

			// ������ ���ο� �ڽ��� ����� ù��° �ڽ����� �߰��ϰ� ����
			ProfileNode* node = new ProfileNode(name, this, child);
			child = node;

			return child;
		}

		void clear()
		{
			totalCalls = 0;
			totalTime = 0;
			recursionCounter = 0;

			delete child;
			child = nullptr;

			delete sibling;
			sibling = nullptr;

			parent = nullptr;
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
				totalTime += stopWatch.elapsed();
				return true;
			}

			return false;
		}

		float			getTotalTime()				{ return totalTime; }
		int				getTotalCalls()				{ return totalCalls; }	
		ProfileNode*	getParent()					{ return parent; }
		ProfileNode*	getChild()					{ return child; }				
		ProfileNode*	getSibling()				{ return sibling; }
		const char*		getName()					{ return name; }

	private:
		StopWatch		stopWatch;
		float			totalTime;		
		int				totalCalls;
		int				recursionCounter;
		ProfileNode*	parent;
		ProfileNode*	child;
		ProfileNode*	sibling;
		const char*		name;		
	};

	class ProfileIterator
	{
	public:
		ProfileIterator(ProfileNode* start)
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
			// �ڽĵ��� ��ȸ�ϸ鼭 �ε�����ŭ ������ �ƴϸ� ���������� ������
			curChildNode = curNode->getChild();
			while (isCurChildValid() && index != 0)
			{
				--index;

				curChildNode = curChildNode->getSibling();
			}

			// ã�� �ڽ��� ������ �ڽĳ��� ����.
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
		ProfileNode*		curNode;
		ProfileNode*		curChildNode;
	};

	class ProfileManager
	{
	public:
		static void	begin(const char* name)
		{
			if (curNode == &rootNode)
			{
				curNode->begin();
			}

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

			if (curNode == &rootNode)
			{
				curNode->end();
			}
		}
		
		static void	clear()
		{
			rootNode.clear();
			curNode = &rootNode;
		}

		static ProfileIterator	getRootIterator()									{ return ProfileIterator(&rootNode); }
		static ProfileNode*		getRootNode()										{ return &rootNode; }

	private:
		static ProfileNode	rootNode;
		static ProfileNode*	curNode;
	};

	class ProfileSample
	{
	public:
		ProfileSample(const char* name)
		{
			ProfileManager::begin(name);
		}

		~ProfileSample()
		{
			ProfileManager::end();
		}
	};

	class ProfileWriter
	{
	public:
		virtual void write(ProfileNode* node, const wchar_t* fname) = 0;
	};
	
#define PROFILE_BLOCK(name)	ProfileSample __profile(name)
#define PROFILE_THISFUNC	ProfileSample __profile(__FUNCTION__)

}