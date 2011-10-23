#include "I4ProfileWriterJson.h"

namespace i4core
{

	void I4ProfileWriterJson::write(I4ProfileNode* node, const wchar_t* fname)
	{
		// Node�� Json ����Ÿ�� ��ȯ
		Json::Value root;
		convertToJsonValue(node, root);

		// Json ���ڿ� ����Ÿ�� �����
		Json::StyledStreamWriter writer;
		std::ostringstream os;
		writer.write(os, root);

		// ���Ͽ� ����.
		std::ofstream ofs;
		ofs.open(fname);
		ofs << os.str().c_str();
		ofs.close();
	}

	void I4ProfileWriterJson::convertToJsonValue(I4ProfileNode* node, Json::Value& value)
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
			// �ڽĵ��� ��������� ���� �����Ѵ�.
			convertToJsonValue(child, value["child"][i]);
			child = child->getSibling();
			i++;
		}
	}

}