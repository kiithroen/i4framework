#include "I4ProfileWriterJson.h"

namespace i4core
{

	void I4ProfileWriterJson::write(I4ProfileNode* node, const wchar_t* fname)
	{
		// Node를 Json 데이타로 변환
		Json::Value root;
		convertToJsonValue(node, root);

		// Json 문자열 데이타를 만들고
		Json::StyledStreamWriter writer;
		std::ostringstream os;
		writer.write(os, root);

		// 파일에 쓴다.
		std::ofstream ofs;
		ofs.open(fname);
		ofs << os.str().c_str();
		ofs.close();
	}

	void I4ProfileWriterJson::convertToJsonValue(I4ProfileNode* node, Json::Value& value)
	{
		// 현재 노드의 값을 저장하고
		value["name"] = node->getName();
		value["totalTime"] = node->getTotalTime();
		value["totalCalls"] = node->getTotalCalls();

		// 자식노드들을 배열에 추가한다.
		I4ProfileNode* child = node->getChild();
		int i = 0;
		while (child != NULL)
		{
			// 자식들은 재귀적으로 값을 저장한다.
			convertToJsonValue(child, value["child"][i]);
			child = child->getSibling();
			i++;
		}
	}

}