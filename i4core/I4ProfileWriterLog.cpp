#include "I4ProfileWriterLog.h"
#include "I4Log.h"
#include "I4Profile.h"

namespace i4core
{
	void I4ProfileWriterLog::write(I4ProfileNode* node)
	{
		writeRecursive(node, 0);
	}

	void I4ProfileWriterLog::writeRecursive(I4ProfileNode* node, int depth)
	{
		if (depth != 0)
		{
			std::wstring depthStr = L"";
			for (int i = 0; i < depth; ++i)
			{
				depthStr += L"\t";
			}

			I4LOG_INFO << depthStr << node->getName() << L" : " << node->getTotalTime() << L" sec : " << node->getTotalCalls();
		}

		I4ProfileNode* child = node->getChild();
		while (child != NULL)
		{
			writeRecursive(child, depth + 1);
			child = child->getSibling();
		}
	}

}