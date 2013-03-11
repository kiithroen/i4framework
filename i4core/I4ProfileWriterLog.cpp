#include "stdafx.h"
#include "I4ProfileWriterLog.h"
#include "I4Log.h"
#include "I4Profile.h"

namespace i4core
{
	void I4ProfileWriterLog::write(I4ProfileNode* node, float fps)
	{
		I4LOG_INFO << "fps : " << fps;

		writeRecursive(node, 0, 0, fps);
	}

	void I4ProfileWriterLog::writeRecursive(I4ProfileNode* node, float parentTotalTime, int depth, float fps)
	{
		if (depth != 0)
		{
			wstring depthStr = L"";
			for (int i = 0; i < depth; ++i)
			{
				depthStr += L"\t";
			}
			
			float nodeTotalTimeSec = node->getTotalTime();
			float nodePercentPerParent = nodeTotalTimeSec/parentTotalTime*100.0f;
			float nodeTimePerCalls = nodeTotalTimeSec/(float)node->getTotalCalls();

			I4LOG_INFO << depthStr << node->getName() << L" : " << nodeTotalTimeSec*1000.0f/fps << L"ms/frame : " << nodePercentPerParent << " %/parent : " << nodeTimePerCalls*1000.0f << L" ms/call" << " : " << node->getTotalCalls() << " call";
		}

		I4ProfileNode* child = nullptr;

		child = node->getChild();
		while (child != nullptr)
		{
			writeRecursive(child, node->getTotalTime(), depth + 1, fps);
			child = child->getSibling();
		}
	}

}