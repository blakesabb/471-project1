#pragma once

#include <msxml2.h>

/*! This function accepts a reference to a node pointer
*  and advances it to the next sibling node.
*/
inline void NextNode(CComPtr<IXMLDOMNode> &node)
{
	CComPtr<IXMLDOMNode> next;
	node->get_nextSibling(&next);
	node = next;
}