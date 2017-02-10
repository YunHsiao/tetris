#ifndef CXML_H
#define CXML_H

class XMLDocument;
class XMLElement {
public:
	XMLElement();
	virtual ~XMLElement();
	void SetText(int v);
	void SetText(const char* v);
	XMLElement* InsertEndChild(XMLElement* addThis);
	XMLElement* FirstChildElement(const char* name) const;
	const char* GetText() const;
	XMLElement*	NextSiblingElement();

protected:
	XMLDocument* doc;
	XMLElement* parent;
	XMLElement* prev;
	XMLElement* next;
	XMLElement* firstChild;
	XMLElement* lastChild;
	char* value;
};

class XMLDeclaration : public XMLElement {
public:
	XMLDeclaration();
};

class XMLDocument : public XMLElement {
public:
	XMLDocument();
	~XMLDocument();

	XMLDeclaration* NewDeclaration();
	XMLElement* NewElement(const char* name);
	int SaveFile(const char* filename);
	int LoadFile(const char* filename);
};

#endif