#pragma once

#include "i4core.h"

namespace rapidxml
{
	template <typename T> class xml_document;
	template <typename T> class xml_node;
	template <typename T> class xml_attribute;
}

namespace i4core
{
	typedef rapidxml::xml_document<char>		I4XmlDocument;
	typedef rapidxml::xml_node<char>			I4XmlNode;
	typedef rapidxml::xml_attribute<char>		I4XmlAttribute;

	class I4CORE_API I4XmlData
	{		
	public:
		I4XmlData();
		~I4XmlData();

		bool 		parseFromFile(const char* fname);
		bool 		parseFromMemory(char* text);

		bool		selectNodeByPath(const char* path);

		bool		selectParentNode();

		bool		selectFirstChildNode(const char* name);
		bool		selectNextSiblingNode(const char* name);		
		
		bool		getNodeValue(short& result);
		bool		getNodeValue(unsigned short& result);
		bool		getNodeValue(int& result);
		bool		getNodeValue(unsigned int& result);
		bool		getNodeValue(float& result);
		bool		getNodeValue(const char*& result);

		bool		getAttrValue(short& result, const char* name);
		bool		getAttrValue(unsigned short& result, const char* name);		
		bool		getAttrValue(int& result, const char* name);
		bool		getAttrValue(unsigned int& result, const char* name);
		bool		getAttrValue(float& result, const char* name);
		bool		getAttrValue(const char*& result, const char* name);

		template<typename T>
		bool getNodeValueByPath(T& result, const char* path)
		{
			if (selectNodeByPath(path) == false)
				return false;
			
			getNodeValue(result);

			return true;
		}

		template<typename T>
		bool getAttrValueByPath(T& result, const char* name, const char* path)
		{
			if (selectNodeByPath(path) == false)
				return false;

			return getAttrValue(result, name);
		}

	private:		
		bool	getNode(I4XmlNode*& result, const char* path);
		void	splitPath(std::vector<std::string>& result, const char* path);

	private:
		I4XmlDocument*	xmlDoc;
		I4XmlNode*		selNode;
		char*			textBuffer;
	};

}