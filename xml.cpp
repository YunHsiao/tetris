#include "Utility.h"
#include "xml.h"

XMLElement::XMLElement()
	:value(0)
	,doc(0)
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

int XMLElement::GetLength(const char* str) const
{
	int len = 0;
	for (; *str != '\0'; str++) len++;
	return len;
}

char* XMLElement::GenerateString(int v) const
{
	char str[16];
	char* r;
	unsigned p = 0, i = 0, o = 0;
	if (!v) {
		r = new char[2];
		r[0] = '0';
		r[1] = 0;
		return r;
	}

	if (v < 0) {
		str[p++] = '-';
		v = -v;
	}
	while (v != 0) {
		str[p++] = '0' + v % 10;
		v /= 10;
	}
	r = new char[p + 1];
	if (str[i] == '-') 
		r[i++] = '-';
	else 
		o++;
	for (; i < p; i++) 
		r[i] = str[p - i - o];
	r[i] = 0;
	return r;
}

char* XMLElement::GenerateString(const char* str) const
{
	int i, len = GetLength(str);
	char* r = new char[len + 1];
	for (i = 0; i <= len; i++)
		r[i] = str[i];
	return r;
}

char* XMLElement::GenerateString(const char* str, int len) const
{
	int i;
	char* r = new char[len + 1];
	for (i = 0; i < len; i++)
		r[i] = str[i];
	r[i] = 0;
	return r;
}

bool XMLElement::Identical(const char* s1, const char* s2) const
{
	int l1 = GetLength(s1);
	int l2 = GetLength(s2);
	if (l1 - l2) return false;

	for (int i = 0; i < l1; i++) 
		if (s1[i] - s2[i]) 
			return false;

	return true;
}

void XMLElement::CopyString(char*& dst, const char* src) const
{
	for (; *src != 0; src++, dst++)
		*dst = *src;
}

void XMLElement::SetText(int v)
{
	char* str = GenerateString(v);
	if (firstChild && firstChild->ToText()) {
		firstChild->SetValue(str);
	} else {
		XMLText* theText = new XMLText();
		theText->SetValue(str);
		theText->SetDocument(doc);
		doc->UpdateLength(GetLength(str));
		InsertFirstChild(theText);
	}
}

void XMLElement::SetText(const char* v)
{
	char* str = GenerateString(v);
	if (firstChild && firstChild->ToText()) {
		firstChild->SetValue(str);
	} else {
		XMLText* theText = new XMLText();
		theText->SetValue(str);
		theText->SetDocument(doc);
		doc->UpdateLength(GetLength(str));
		InsertFirstChild(theText);
	}
}

void XMLElement::SetText(const char* v, int len)
{
	char* str = GenerateString(v, len);
	if (firstChild && firstChild->ToText()) {
		firstChild->SetValue(str);
	} else {
		XMLText* theText = new XMLText();
		theText->SetValue(str);
		theText->SetDocument(doc);
		doc->UpdateLength(GetLength(str));
		InsertFirstChild(theText);
	}
}

const char* XMLElement::GetText() const 
{
	if (firstChild && firstChild->ToText())
		return firstChild->GetValue();
	return 0;
}

XMLElement* XMLElement::InsertFirstChild(XMLElement* addThis)
{
	addThis->parent = this;
	if (!lastChild) lastChild = addThis;
	if (!firstChild) firstChild = addThis;
	else {
		firstChild->prev = addThis;
		addThis->next = firstChild;
		firstChild = addThis;
	}
	return addThis;
}

XMLElement* XMLElement::InsertEndChild(XMLElement* addThis)
{
	addThis->parent = this;
	if (!firstChild) firstChild = addThis;
	if (!lastChild) lastChild = addThis;
	else {
		lastChild->next = addThis;
		addThis->prev = lastChild;
		lastChild = addThis;
	}
	return addThis;
}

XMLElement* XMLElement::FirstChildElement(const char* name) const
{
	XMLElement* child = firstChild;
	if (!name) return child;
	while (child) {
		if (Identical(name, child->GetValue())) return child;
		child = child->next;
	}
	return 0;
}

XMLElement*	XMLElement::NextSiblingElement()
{
	return next;
}

XMLDeclaration::XMLDeclaration()
{
	SetValue(GenerateString("xml version=\"1.0\" encoding=\"ANSI\""));
}

XMLDocument::XMLDocument()
	:length(0)
{

}

XMLDocument::~XMLDocument()
{

}


XMLDeclaration* XMLDocument::NewDeclaration()
{
	XMLDeclaration* dec = new XMLDeclaration();
	dec->SetDocument(this);
	UpdateLength(38);
	return dec;
}

XMLElement* XMLDocument::NewElement(const char* name)
{
	XMLElement* r = new XMLElement();
	r->SetValue(GenerateString(name));
	r->SetDocument(this);
	UpdateLength(GetLength(name) * 2 + 7); // '<'2 '>'2 '\n'2 '/'1
	return r;
}

XMLElement* XMLDocument::NewElement(const char* name, int len)
{
	XMLElement* r = new XMLElement();
	r->SetValue(GenerateString(name, len));
	r->SetDocument(this);
	UpdateLength(len * 2 + 7); // '<'2 '>'2 '\n'2 '/'1
	return r;
}

int XMLDocument::SaveFile(const char* filename)
{
	char* str = new char[length * 2]; // '\t's
	char* beg = str;
	int len = GetLength(filename);
	if (filename[len - 1] == 'l')
		Write(firstChild, str, 0);
	else Write(firstChild, str);
	FILE* file;
	fopen_s(&file, filename, "w");
	fwrite(beg, str - beg, 1, file);
	fclose(file);
	Safe_Delete_Array(beg);
	return 0;
}

int XMLDocument::LoadFile(const char* filename)
{
	FILE* file = 0;
	int len = 0;
	char *str, *beg;
	fopen_s(&file, filename, "r");
	fseek (file, 0, SEEK_END);
	len = ftell(file);
	fseek (file, 0, SEEK_SET);
	str = new char[len];
	fread_s(str, len, len, 1, file);
	fclose(file);
	beg = str;
	
	len = GetLength(filename);
	if (filename[len - 1] == 'l') Parse(this, str, 0);
	else Parse(str);
	Safe_Delete_Array(beg);
	return 0;
}

// 自定义格式写入
void XMLDocument::Write(XMLElement* node, char*& dst) 
{
	const char* str = node->GetValue();
	int len = GetLength(str);
	if (node->ToDeclaration()) {
	} else if (node->ToText()) {
		CopyString(dst, str);
		CopyString(dst, "\n");
	} else { 
		if (node->GetParent() == this) {
			CopyString(dst, "[");
			CopyString(dst, str);
			CopyString(dst, "]\n");
		} else {
			CopyString(dst, "\t");
			CopyString(dst, str);
			CopyString(dst, ": ");
		}
		Write(node->FirstChildElement(), dst);
	}
	if (node->GetParent() == this)
		CopyString(dst, "\n");
	node = node->NextSiblingElement();
	if (node) Write(node, dst);
}

// 自定义格式读取
void XMLDocument::Parse(char*& str) 
{
	XMLElement *child = 0, *text = 0;
	int len = 0;
	while (true) {
		while(*str == ' ' || *str == '\n' || *str == '\t') str++;
		switch(*str) {
		case '[':
			while (*(str + len) != ']') len++;
			len++;
			child = NewElement(str + 1, len - 2);
			InsertEndChild(child);
			str += len;
			len = 0;
			break;
		default:
			if (*str < 32) return;
			while (*(str + len) != ':') len++;
			text = NewElement(str, len);
			str += len + 2;
			len = 0;
			while (*(str + len) != '\n') len++;
			text->SetText(str, len);
			child->InsertEndChild(text);
			str += len;
			len = 0;
		}
	}
}

// XML格式写入
void XMLDocument::Write(XMLElement* node, char*& dst, int indent) 
{
	const char* str = node->GetValue();
	int len = GetLength(str);
	if (node->ToDeclaration()) {
		CopyString(dst, "<?");
		CopyString(dst, str);
		CopyString(dst, "?>\n");
	} else if (node->ToText()) {
		CopyString(--dst, str);
	} else { 
		for (int i = 0; i < indent; i++) *dst++ = '\t';
		CopyString(dst, "<");
		CopyString(dst, str);
		CopyString(dst, ">\n");
		Write(node->FirstChildElement(), dst, indent + 1);
		CopyString(dst, "</");
		CopyString(dst, str);
		CopyString(dst, ">\n");
	}
	if (node->GetParent() == this && 
		node->GetDocument()->FirstChildElement() != node && 
		node->GetDocument()->lastChild != node)
		CopyString(dst, "\n");
	node = node->NextSiblingElement();
	if (node) Write(node, dst, indent);
}

// XML格式读取
void XMLDocument::Parse(XMLElement* node, char*& str, int length) 
{
	XMLElement* child = 0;
	int len = 0;
	while (true) {
		while(*str == ' ' || *str == '\n' || *str == '\t') str++;
		switch(*str) {
		case '<':
			if (*(str + 1) == '?') {
				child = NewDeclaration();
				node->InsertEndChild(child);
				while (*str++ != '>');
			} else if (*(str + 1) == '/') {
				str += length + 1;
				return;
			} else {
				while (*(str + len) != '>') len++;
				len++;
				child = NewElement(str + 1, len - 2);
				node->InsertEndChild(child);
				str += len;
				Parse(child, str, len);
				len = 0;
			}
			break;
		default:
			if (node == this) return;
			while (*(str + len) != '<') len++;
			node->SetText(str, len);
			str += len;
		}
	}
}
