#pragma once

namespace i4core
{
	class I4ProfileNode;

	class I4ProfileWriterLog
	{
	public:
		void write(I4ProfileNode* node, float fps);

	private:
		static void writeRecursive(I4ProfileNode* node, float parentTotalTime, int depth, float fps);
	};

}