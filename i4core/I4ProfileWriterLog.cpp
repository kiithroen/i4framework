#include "stdafx.h"
#include "I4ProfileWriterLog.h"
#include "I4Log.h"
#include "I4Profile.h"

namespace i4core
{
	void ProfileWriterLog::write(ProfileNode* node, float fps)
	{
		LOG_INFO << "fps : " << fps;

		writeRecursive(node, 0, 0, fps);
	}

	void ProfileWriterLog::writeRecursive(ProfileNode* node, float parentTotalTime, int depth, float fps)
	{
		if (depth != 0)
		{
			wstring depthStr = L"";
			for (int i = 1; i < depth; ++i)
			{
				depthStr += L"    ";
			}
			
			float nodeTotalTimeSec = node->getTotalTime();
			float nodePercentPerParent = (nodeTotalTimeSec*100.0f)/parentTotalTime;
			float nodeTimePerCalls = nodeTotalTimeSec/(float)node->getTotalCalls();

			LOG_INFO << depthStr << node->getName() << L" : " << nodeTotalTimeSec*1000.0f/fps << L" ms/frame : " << nodePercentPerParent << " %/parent : " << nodeTimePerCalls*1000.0f << L" ms/call" << " : " << node->getTotalCalls() << " call";
		}

		ProfileNode* child = nullptr;

		child = node->getChild();
		while (child != nullptr)
		{
			writeRecursive(child, node->getTotalTime(), depth + 1, fps);
			child = child->getSibling();
		}
	}

}