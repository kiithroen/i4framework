#include "stdafx.h"
#include "I4XmlData.h"
#include "I4Log.h"
#include "rapidxml.hpp"

namespace i4core
{
	XmlData::XmlData()
		: xmlDoc(nullptr)
		, selNode(nullptr)		
		, textBuffer(nullptr)
	{
		xmlDoc = new XmlDocument;
	}

	XmlData::~XmlData()
	{
		delete[] textBuffer;
		delete xmlDoc;
	}

	bool XmlData::parseFromFile(const char* fname)
	{
		ifstream ifs;
		ifs.open(fname, ifstream::in);

		if (ifs.is_open() == false)
		{
			LOG_WARN << L"can't open file. :" << fname;
			return false;
		}

		ifs.seekg(0, ios_base::end);
		int size = (int)ifs.tellg();
		if (size <= 0)
		{
			LOG_WARN << L"incorrect file. : " << fname;
			return false;
		}

		ifs.seekg(0, ios::beg);
		textBuffer = new char[size + 1];
		memset(textBuffer, 0, size + 1);
		ifs.read(textBuffer, size);

		if (strlen(textBuffer) == 0)
		{
			LOG_WARN << L"%s is empty\n" << fname;
			delete[] textBuffer;
			return false;
		}

		if (parseFromMemory(textBuffer) == false)
			return false;

		return true;
	}

	bool XmlData::parseFromMemory(char* text)
	{
		try
		{
			xmlDoc->parse<0>(text);
		}
		catch (rapidxml::parse_error e)
		{
			LOG_WARN << e.where<char>() << ":" << e.what();
			return false;
		}
		catch (...)
		{
			LOG_WARN << "unknown error";
			return false;
		}

		return true;
	}

	bool XmlData::selectNodeByPath(const char* path)
	{
		XmlNode* node = nullptr;
		getNode(node, path);
		if (node == nullptr)
			return false;

		selNode = node;

		return true;
	}

	bool XmlData::selectParentNode()
	{
		if (selNode == nullptr)
			return false;

		XmlNode* node = selNode->parent();
		if (node == nullptr)
			return false;

		selNode = node;

		return true;
	}

	bool XmlData::selectFirstChildNode(const char* name)
	{
		if (selNode == nullptr)
			return false;

		XmlNode* node = selNode->first_node(name);
		if (node == nullptr)
			return false;

		selNode = node;

		return true;
	}

	bool XmlData::selectNextSiblingNode(const char* name)
	{
		if (selNode == nullptr)
			return false;

		XmlNode* node = selNode->next_sibling(name);
		if (node == nullptr)
			return false;

		selNode = node;

		return true;
	}

	bool XmlData::getNodeValue(short& result)
	{
		if (selNode == nullptr)
			return false;

		result = (short)atoi(selNode->value());

		return true;
	}

	bool XmlData::getNodeValue(unsigned short& result)
	{
		if (selNode == nullptr)
			return false;

		result = (unsigned short)atoi(selNode->value());

		return true;
	}

	bool XmlData::getNodeValue(int& result)
	{
		if (selNode == nullptr)
			return false;

		result = atoi(selNode->value());

		return true;
	}

	bool XmlData::getNodeValue(unsigned int& result)
	{
		if (selNode == nullptr)
			return false;

		result = (unsigned int)atoi(selNode->value());

		return true;
	}

	bool XmlData::getNodeValue(float& result)
	{
		if (selNode == nullptr)
			return false;

		result = (float)atof(selNode->value());

		return true;
	}

	bool XmlData::getNodeValue(const char*& result)
	{
		if (selNode == nullptr)
			return false;

		result = selNode->value();

		return true;
	}

	bool XmlData::getAttrValue(short& result, const char* name)
	{
		if (selNode == nullptr)
			return false;

		XmlAttribute* attr = selNode->first_attribute(name);
		if (attr == nullptr)
			return false;

		result = (short)atoi(attr->value());

		return true;
	}

	bool XmlData::getAttrValue(unsigned short& result, const char* name)
	{
		if (selNode == nullptr)
			return false;

		XmlAttribute* attr = selNode->first_attribute(name);
		if (attr == nullptr)
			return false;

		result = (unsigned short)atoi(attr->value());

		return true;
	}

	bool XmlData::getAttrValue(int& result, const char* name)
	{
		if (selNode == nullptr)
			return false;

		XmlAttribute* attr = selNode->first_attribute(name);
		if (attr == nullptr)
			return false;

		result = atoi(attr->value());

		return true;
	}

	bool XmlData::getAttrValue(unsigned int& result, const char* name)
	{
		if (selNode == nullptr)
			return false;

		XmlAttribute* attr = selNode->first_attribute(name);
		if (attr == nullptr)
			return false;

		result = (unsigned int)atoi(attr->value());

		return true;
	}

	bool XmlData::getAttrValue(float& result, const char* name)
	{
		if (selNode == nullptr)
			return false;

		XmlAttribute* attr = selNode->first_attribute(name);
		if (attr == nullptr)
			return false;

		result = (float)atof(attr->value());

		return true;
	}

	bool XmlData::getAttrValue(const char*& result, const char* name)
	{
		if (selNode == nullptr)
			return false;

		XmlAttribute* attr = selNode->first_attribute(name);
		if (attr == nullptr)
			return false;

		result = attr->value();

		return true;
	}
	
	bool XmlData::getAttrValue(string& result, const char* name)
	{
		assert(selNode != nullptr);

		XmlAttribute* attr = selNode->first_attribute(name);
		if (attr == nullptr)
			return false;

		result = attr->value();

		return true;
	}

	bool XmlData::getNode(XmlNode*& result, const char* path)
	{
		vector<string> split;
		splitPath(split, path);

		XmlNode* node = xmlDoc->first_node(split[0].c_str());

		for (unsigned int i = 1; i < split.size(); ++i)
		{
			if (node == nullptr)
			{
				result = nullptr;
				return false;
			}

			node = node->first_node(split[i].c_str());
		}

		result = node;

		return true;
	}

	void XmlData::splitPath(vector<string>& result, const char* path)
	{
		string tempPath = path;
		for (;;)
		{
			size_t idx = tempPath.find('/');
			if (idx > tempPath.length())
			{
				result.push_back(tempPath);
				break;
			}

			result.push_back(tempPath.substr(0, idx));
			tempPath = tempPath.substr(idx + 1);
		}
	}
}