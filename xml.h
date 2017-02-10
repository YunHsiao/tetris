#ifndef CXML_H
#define CXML_H

#define INITIAL_SIZE 10

class XMLText;
class XMLDocument;
class XMLDeclaration;
class XMLElement {
public:
	XMLElement();
	virtual ~XMLElement();
	void SetText(int text);
	void SetText(const char* text);
	const char* GetText() const;
	XMLElement* InsertFirstChild(XMLElement* addThis);
	XMLElement* InsertEndChild(XMLElement* addThis);
	XMLElement* FirstChildElement(const char* name = 0) const;
	XMLElement*	NextSiblingElement();
	virtual XMLText* ToText() { return 0; }
	virtual XMLDocument* ToDocument() { return 0; }
	virtual XMLDeclaration* ToDeclaration() { return 0; }

	void _setText(char* str);
	void _setDocument(XMLDocument* _doc) { doc = _doc; }
	XMLDocument* _getDocument() { return doc; }
	XMLElement* _getParent() { return parent; }
	void _setValue(char* v) { Safe_Delete_Array(value); value = v; }
	const char* _getValue() const { return value; }
	void _updateLength(int _chars, int _lines) { chars += _chars; lines += _lines; }

protected:
	char* _generateString(int v) const;
	char* _generateString(const char* str) const;
	char* _generateString(const char* str, int len) const;
	bool _identical(const char* s1, const char* s2) const;
	int _getLength(const char* str) const;
	void _copyString(char*& dst, const char* src) const;

	XMLDocument* doc;
	XMLElement* parent;
	XMLElement* prev;
	XMLElement* next;
	XMLElement* firstChild;
	XMLElement* lastChild;
	char* value;
	int chars, lines;
};

class XMLText : public XMLElement {
	XMLText* ToText() { return this; }
};

class XMLDocument : public XMLElement {
public:
	XMLDocument();
	~XMLDocument();

	XMLDeclaration* NewDeclaration();
	XMLElement* NewElement(const char* name);
	XMLText* XMLDocument::NewText(const char* name);
	int SaveFile(const char* filename);
	int LoadFile(const char* filename);
	XMLDocument* ToDocument() { return this; }

	XMLElement* _newElement(char* str);
	XMLText* _newText(char* str);

protected:
	void _write(XMLElement* node, char*& str, int indent);
	void _parse(XMLElement* node, char*& str, int length);

	void _write(XMLElement* node, char*& str);
	void _parse(char*& str);

	void _addToList(XMLElement* element);
	XMLElement** m_elements;
	int _size, _allocated;
};

class XMLDeclaration : public XMLElement {
public:
	XMLDeclaration();
	XMLDeclaration* ToDeclaration() { return this; }
};

#endif
