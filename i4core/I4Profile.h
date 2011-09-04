#pragma once

#include "i4core.h"
#include "I4StopWatch.h"
#include "I4Log.h"
#include <json/json.h>

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
			// ù��° �ڽĵ��߿� �ִ��� ã�Ƽ� ������ ����
			I4ProfileNode* _child = child;
			while (_child)
			{
				if (_child->name == name)
					return _child;

				_child = _child->sibling;
			}

			// ������ ���ο� �ڽ��� ����� ù��° �ڽ����� �߰��ϰ� ����
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

		static I4ProfileIterator	getIterator()									{ return I4ProfileIterator(&root); }
		static I4ProfileNode*		getRoot()										{ return &root; }

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

	class I4CORE_API I4ProfileWriter
	{
	public:
		static void writeJson(I4ProfileNode* node, const wchar_t* fname)
		{
			// Node�� Json ����Ÿ�� ��ȯ
			Json::Value root;
			getJson(node, root);

			// Json ���ڿ� ����Ÿ�� �����
			Json::StyledStreamWriter writer;
			std::ostringstream os;
			writer.write(os, root);

			// ���Ͽ� ����.
			std::wofstream ofs;
			ofs.open(fname);
			ofs << os.str().c_str();
			ofs.close();
		}

		static void getJson(I4ProfileNode* node, Json::Value& value)
		{
			// ���� ����� ���� �����ϰ�
			value["name"] = node->getName();
			value["totalTime"] = node->getTotalTime();
			value["totalCalls"] = node->getTotalCalls();

			// �ڽĳ����� �迭�� �߰��Ѵ�.
			I4ProfileNode* child = node->getChild();
			int i = 0;
			while (child != NULL)
			{
				// �ڽĵ��� ��������� ���� ���´�.
				getJson(child, value["child"][i]);
				child = child->getSibling();
				i++;
			}
		}
	};

#define PROFILE(name)	I4ProfileSample __profile(name)

}