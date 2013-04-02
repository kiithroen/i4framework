#pragma once

namespace i4core
{
	class ProfileNode;

	class ProfileWriterLog
	{
	public:
		void write(ProfileNode* node, float fps);

	private:
		static void writeRecursive(ProfileNode* node, float parentTotalTime, int depth, float fps);
	};

}