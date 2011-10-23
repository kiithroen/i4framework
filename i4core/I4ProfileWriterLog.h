#pragma once

#include "i4core.h"

namespace i4core
{
	class I4ProfileNode;

	class I4CORE_API I4ProfileWriterLog
	{
	public:
		void write(I4ProfileNode* node);

	private:
		static void writeRecursive(I4ProfileNode* node, float parentTotalTime, int depth);
	};

}