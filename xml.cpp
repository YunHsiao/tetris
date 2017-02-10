#include "Utility.h"
#include "xml.h"

XMLElement::XMLElement()
	:doc(0)
	,parent(0)
	,prev(0)
	,next(0)
	,firstChild(0)
	,lastChild(0)
{

}

XMLElement::~XMLElement()
{
	Safe_Delete_Array(value);
	XMLElement *node = firstChild, *next = node;
	while (node != NULL) {
		next = node->NextSiblingElement();
		delete node;
		node = next;
	}
}

void XMLElement::SetText(int v)
{

}

void XMLElement::SetText(const char* v)
{

}

XMLElement* XMLElement::InsertEndChild(XMLElement* addThis)
{

	return 0;
}

XMLElement* XMLElement::FirstChildElement(const char* name) const
{

	return 0;
}

const char* XMLElement::GetText() const 
{

	return 0;
}

XMLElement*	XMLElement::NextSiblingElement()
{

	return 0;
}

XMLDeclaration::XMLDeclaration()
{
	value = new char[35];
	const char* str = "xml version=\"1.0\" encoding=\"UTF-8\"";
}

XMLDocument::XMLDocument()
{

}

XMLDocument::~XMLDocument()
{

}


XMLDeclaration* XMLDocument::NewDeclaration()
{

	return 0;
}

XMLElement* XMLDocument::NewElement(const char* name)
{

	return 0;
}

int XMLDocument::SaveFile(const char* filename)
{

	return 0;
}

int XMLDocument::LoadFile(const char* filename)
{

	return 0;
}
