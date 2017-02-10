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
	,chars(0)
	,lines(0)
{

}

XMLElement::~XMLElement()
{
	Safe_Delete_Array(value);
}

int XMLElement::_getLength(const char* str) const
{
	int len = 0;
	for (; *str != '\0'; str++) len++;
	return len;
}

char* XMLElement::_generateString(int v) const
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

char* XMLElement::_generateString(const char* str) const
{
	int i, len = _getLength(str);
	char* r = new char[len + 1];
	for (i = 0; i <= len; i++)
		r[i] = str[i];
	return r;
}

char* XMLElement::_generateString(const char* str, int len) const
{
	int i;
	char* r = new char[len + 1];
	for (i = 0; i < len; i++)
		r[i] = str[i];
	r[i] = 0;
	return r;
}

bool XMLElement::_identical(const char* s1, const char* s2) const
{
	int l1 = _getLength(s1);
	int l2 = _getLength(s2);
	if (l1 - l2) return false;

	for (int i = 0; i < l1; i++) 
		if (s1[i] - s2[i]) 
			return false;

	return true;
}

void XMLElement::_copyString(char*& dst, const char* src) const
{
	for (; *src != 0; src++, dst++)
		*dst = *src;
}

void XMLElement::SetText(int text)
{
	_setText(_generateString(text));
}

void XMLElement::SetText(const char* text)
{
	_setText(_generateString(text));
}

void XMLElement::_setText(char* str)
{
	if (firstChild && firstChild->ToText()) {
		firstChild->_setValue(str);
	} else {
		XMLText* theText = doc->_newText(str);
		InsertFirstChild(theText);
	}
}

const char* XMLElement::GetText() const 
{
	if (firstChild && firstChild->ToText())
		return firstChild->_getValue();
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
	chars += addThis->chars + addThis->lines;
	lines += addThis->lines;
	if (lines == 2) lines++;
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
	chars += addThis->chars + addThis->lines;
	lines += addThis->lines;
	if (lines == 2) lines++;
	return addThis;
}

XMLElement* XMLElement::FirstChildElement(const char* name) const
{
	XMLElement* child = firstChild;
	if (!name) return child;
	while (child) {
		if (_identical(name, child->_getValue())) return child;
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
	_setValue(_generateString("xml version=\"1.0\" encoding=\"ANSI\""));
}

XMLDocument::XMLDocument()
	:m_elements(0)
	,_size(0)
	,_allocated(INITIAL_SIZE)
{
	m_elements = new XMLElement*[INITIAL_SIZE];
}

XMLDocument::~XMLDocument()
{
	for (int i = 0; i < _size; i++)
		Safe_Delete(m_elements[i]);
	Safe_Delete_Array(m_elements);
}


XMLDeclaration* XMLDocument::NewDeclaration()
{
	XMLDeclaration* dec = new XMLDeclaration();
	dec->_setDocument(this);
	dec->_updateLength(38, 1);
	_addToList(dec);
	return dec;
}

XMLElement* XMLDocument::NewElement(const char* name)
{
	return _newElement(_generateString(name));
}

XMLElement* XMLDocument::_newElement(char* str)
{
	XMLElement* ele = new XMLElement();
	ele->_setValue(str);
	ele->_setDocument(this);
	ele->_updateLength(_getLength(str) * 2 + 7, 1); // '<'2 '>'2 '/'1 '\n'2
	_addToList(ele);
	return ele;
}

XMLText* XMLDocument::NewText(const char* name) 
{
	return _newText(_generateString(name));
}

XMLText* XMLDocument::_newText(char* str)
{
	XMLText* theText = new XMLText();
	theText->_setValue(str);
	theText->_setDocument(this);
	theText->_updateLength(_getLength(str), 0);
	_addToList(theText);
	return theText;
}

void XMLDocument::_addToList(XMLElement* element)
{
	if (_size + 1 > _allocated) {
		int newAllocated = (_size + 1) * 2;
		XMLElement** newElements = new XMLElement*[newAllocated];
		for (int i = 0; i < _size; i++)
			newElements[i] = m_elements[i];
		Safe_Delete_Array(m_elements);
		m_elements = newElements;
		_allocated = newAllocated;
	}
	m_elements[_size++] = element;
}

int XMLDocument::SaveFile(const char* filename)
{
	char* str = new char[chars];
	char* beg = str;
	int len = _getLength(filename);
	if (filename[len - 1] == 'l')
		_write(firstChild, str, 0);
	else _write(firstChild, str);
	*str++ = 0;
	FILE* file;
	fopen_s(&file, filename, "w");
	fwrite(beg, str - beg, 1, file);
	fclose(file);
	Safe_Delete_Array(beg);
	return 0;
}

int XMLDocument::LoadFile(const char* filename)
{
	int len = 0;
	char *str, *beg;
	FILE* file = 0;
	fopen_s(&file, filename, "r");
	fseek (file, 0, SEEK_END);
	len = ftell(file);
	fseek (file, 0, SEEK_SET);
	str = new char[len];
	fread_s(str, len, len, 1, file);
	fclose(file);
	beg = str;
	
	len = _getLength(filename);
	if (filename[len - 1] == 'l') _parse(this, str, 0);
	else _parse(str);
	Safe_Delete_Array(beg);
	return 0;
}

// 自定义格式写入
void XMLDocument::_write(XMLElement* node, char*& dst) 
{
	const char* str = node->_getValue();
	int len = _getLength(str);
	if (node->ToDeclaration()) {
	} else if (node->ToText()) {
		_copyString(dst, str);
		_copyString(dst, "\n");
	} else { 
		if (node->_getParent() == this) {
			_copyString(dst, "[");
			_copyString(dst, str);
			_copyString(dst, "]\n");
		} else {
			//_copyString(dst, "\t");
			_copyString(dst, str);
			_copyString(dst, ": ");
		}
		_write(node->FirstChildElement(), dst);
	}
	node = node->NextSiblingElement();
	if (node) {
		if (node->_getParent() == this)
			_copyString(dst, "\n");
		_write(node, dst);
	}
}

// 自定义格式读取
void XMLDocument::_parse(char*& str) 
{
	XMLElement *child = 0, *text = 0;
	int len = 0;
	while (true) {
		while(*str == ' ' || *str == '\n' || *str == '\t') str++;
		switch(*str) {
		case '[':
			while (*(str + len) != ']') len++;
			len++;
			child = _newElement(_generateString(str + 1, len - 2));
			InsertEndChild(child);
			str += len;
			len = 0;
			break;
		default:
			if (*str == 0) return;
			while (*(str + len) != ':') len++;
			text = _newElement(_generateString(str, len));
			str += len + 2;
			len = 0;
			while (*(str + len) != '\n') len++;
			text->_setText(_generateString(str, len));
			child->InsertEndChild(text);
			str += len;
			len = 0;
		}
	}
}

// XML格式写入
void XMLDocument::_write(XMLElement* node, char*& dst, int indent) 
{
	const char* str = node->_getValue();
	int len = _getLength(str);
	if (node->ToDeclaration()) {
		_copyString(dst, "<?");
		_copyString(dst, str);
		_copyString(dst, "?>\n");
	} else if (node->ToText()) {
		_copyString(--dst, str);
	} else { 
		for (int i = 0; i < indent; i++) *dst++ = '\t';
		_copyString(dst, "<");
		_copyString(dst, str);
		_copyString(dst, ">\n");
		_write(node->FirstChildElement(), dst, indent + 1);
		_copyString(dst, "</");
		_copyString(dst, str);
		_copyString(dst, ">\n");
	}
	if (node->_getParent() == this && firstChild != node && lastChild != node)
		_copyString(dst, "\n");
	node = node->NextSiblingElement();
	if (node) _write(node, dst, indent);
}

// XML格式读取
void XMLDocument::_parse(XMLElement* node, char*& str, int length) 
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
				child = _newElement(_generateString(str + 1, len - 2));
				node->InsertEndChild(child);
				str += len;
				_parse(child, str, len);
				len = 0;
			}
			break;
		default:
			if (node == this) return;
			while (*(str + len) != '<') len++;
			node->_setText(_generateString(str, len));
			str += len;
		}
	}
}
