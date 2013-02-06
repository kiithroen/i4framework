#include "I4ProfileWriterLog.h"
#include "I4Log.h"
#include "I4Profile.h"

namespace i4core
{
	void I4ProfileWriterLog::write(I4ProfileNode* node)
	{
		writeRecursive(node, 0, 0);
	}

	void I4ProfileWriterLog::writeRecursive(I4ProfileNode* node, float siblingTotalTime, int depth)
	{
		if (depth != 0)
		{
			std::wstring depthStr = L"";
			for (int i = 0; i < depth; ++i)
			{
				depthStr += L"\t";
			}
			
			float nodeTotalTimeSec = node->getTotalTime();
			float nodePercentPerSibling = nodeTotalTimeSec/siblingTotalTime*100.0f;
			float nodeTimePerCalls = nodeTotalTimeSec/(float)node->getTotalCalls();

			I4LOG_INFO << depthStr << node->getName() << L" : " << nodeTotalTimeSec*1000.0f << L"ms/sec : " << nodePercentPerSibling << " % : " << nodeTimePerCalls*1000.0f << L" ms/call";
		}

		float childSiblingTotalTime = 0;
		I4ProfileNode* child = NULL;

		child = node->getChild();
		while (child != NULL)
		{
			childSiblingTotalTime += child->getTotalTime();
			child = child->getSibling();
		}

		child = node->getChild();
		while (child != NULL)
		{
			writeRecursive(child, childSiblingTotalTime, depth + 1);
			child = child->getSibling();
		}
	}

}