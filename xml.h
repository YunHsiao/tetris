#ifndef CXML_H
#define CXML_H

class XMLText;
class XMLDocument;
class XMLDeclaration;
class XMLElement {
public:
	XMLElement();
	virtual ~XMLElement();
	void SetText(int v);
	void SetText(const char* v);
	void SetText(const char* v, int len);
	const char* GetText() const;
	XMLElement* InsertFirstChild(XMLElement* addThis);
	XMLElement* InsertEndChild(XMLElement* addThis);
	XMLElement* FirstChildElement(const char* name = 0) const;
	XMLElement*	NextSiblingElement();
	virtual XMLText* ToText() { return 0; }
	virtual XMLDocument* ToDocument() { return 0; }
	virtual XMLDeclaration* ToDeclaration() { return 0; }

	void SetDocument(XMLDocument* _doc) { doc = _doc; }
	XMLDocument* GetDocument() { return doc; }
	XMLElement* GetParent() { return parent; }
	void SetValue(char* v) { Safe_Delete_Array(value); value = v; }
	const char* GetValue() const { return value; }

protected:
	char* GenerateString(int v) const;
	char* GenerateString(const char* str) const;
	char* GenerateString(const char* str, int len) const;
	bool Identical(const char* s1, const char* s2) const;
	int GetLength(const char* str) const;
	void CopyString(char*& dst, const char* src) const;

	XMLDocument* doc;
	XMLElement* parent;
	XMLElement* prev;
	XMLElement* next;
	XMLElement* firstChild;
	XMLElement* lastChild;
	char* value;
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
	XMLElement* NewElement(const char* name, int len);
	int SaveFile(const char* filename);
	int LoadFile(const char* filename);
	XMLDocument* ToDocument() { return this; }
	void UpdateLength(int len) { length += len; }

private:
	void Write(XMLElement* node, char*& str, int indent);
	void Parse(XMLElement* node, char*& str, int length);

	void Write(XMLElement* node, char*& str);
	void Parse(char*& str);

	int length;
};

class XMLDeclaration : public XMLElement {
public:
	XMLDeclaration();
	XMLDeclaration* ToDeclaration() { return this; }
};

#endif
