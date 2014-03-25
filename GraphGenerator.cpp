///////////////////////////////////////////////////////////////////////////////////////////////
// GraphGenerator.cpp - implementation of expanded graph builder and compressed graph generator
// Author: Denis Kazakov

#include <Windows.h>

#include <iomanip>
#include <iostream>
#include <string>
using namespace std;

#include "GraphGenerator.h"


// free previous results, makes new expanded graph, fills it with 0
void GraphGenerator::Init()
{
	delete [] expandGraph;		
	delete [] compressedGraph;
	compressedGraph = NULL;

	expandGraph = new SG_Y_ExpandedNode[EXPAND_GRAPH_SIZE];		

	// NOTE: zero all the graph at once gives and error: heap corrupted
	// init with zero's each node
	for( unsigned i = 0; i < EXPAND_GRAPH_SIZE; i++ )
		memset(expandGraph[i], 0, sizeof(SG_Y_ExpandedNode));		

	totalNodes = 0;
	totalDigitElements = 0;
}


// returns full path to the .ini file with rules
string GraphGenerator::GetIniFullPath()
{
	static const char *INI_FILE_NAME = "\\rules.ini";
	
	char temp[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, temp);
	string path = temp;
	path += INI_FILE_NAME;

	return path;
}


// reads numbers from .ini file and add it to expanded graph
unsigned GraphGenerator::ReadRules()
{
	Init();
	
	string iniPath = GetIniFullPath();	// get full path to .ini file with rules

	unsigned ruleIdx = 0;	
	for( ;; ruleIdx++ )
	{
		RuleEntry ruleEntry;
		if( !RuleEntry::ReadRule(ruleEntry, ruleIdx, iniPath) )
		{			
			break;
		}

		bool success = AddToExpandedGraph(ruleEntry);
		if( !success )
		{
			return 0;
		}
	}
	
	return ruleIdx;
}


// adds 'number' in rule to expanded graph, sets attributes of last digit element using rule attributes 
bool GraphGenerator::AddToExpandedGraph( RuleEntry &ruleEntry )
{
	string number = ruleEntry.number;						// number to add to graph, for example "012345"
	SG_Y_ExpandedNode *curExpandedNode = &expandGraph[0];			// init current node with root node
	SG_Y_DigitElement *prvDigit = NULL;						// previous added digit, to link it with next digit element that will be added
	for( unsigned i = 0; i < number.length(); i++ )	
	{
		unsigned digit = CharToDigit(number[i]);			// get next number from string, for example '#' equals SHARP_DIGIT, '1' equals 1
		bool isLeaf = i == number.length() - 1;				// last digit in number is leaf, for example "012345" - '5' is leaf

		SG_Y_DigitElement &expandedDigitElement = (*curExpandedNode)[digit];	// reference to element in expanded node that represents digit		
		totalDigitElements += IsDigitElementEmpty(expandedDigitElement);
		
		expandedDigitElement.digit = digit;
		expandedDigitElement.leafNode = expandedDigitElement.leafNode | isLeaf;		// element should be already in graph, so if it was marked a leaf node already, it must remain a leaf node.
		if( isLeaf )
		{
			ruleEntry.SetAttributesOf(expandedDigitElement);				// copy attributes from
		}
		if( prvDigit )
		{
			prvDigit->nextNodeOffset = &expandedDigitElement - &expandGraph[0][0];			// link previous added digit element with new one,
		}
		prvDigit = &expandedDigitElement;													

		// move to next node:
		// get existing expanded node which contains child digit element 
		if( unsigned nextOffset = expandedDigitElement.nextNodeOffset )						// 'nextNodeOffset' is an address from first byte of expanded graph
		{
			curExpandedNode = &expandGraph[nextOffset / K_MAX_EXPANDED_NODES];
		}

		// get new expanded node from array. For leaf it doesn't make sense, all digits was added to graph
		else if( !isLeaf )
		{	
			if( ++totalNodes == EXPAND_GRAPH_SIZE )
			{
				return false;		// too many rules to store in graph
			}
			
			curExpandedNode = &expandGraph[totalNodes];			
		}		
	}

	return true;
}



// generates compressed graph from expanded graph by removing empty nodes
void GraphGenerator::Generate()
{
	assert(compressedGraph == NULL);
	assert(expandGraph != NULL);	

	unsigned size = this->GetCompressedGraphSize();
	compressedGraph = new BYTE[ size ];	
	memset(compressedGraph, 0, size);

	SG_Y_DigitElement dummy = { 0 };	
	unsigned curPos = 0;
	CompressNode(dummy, expandGraph[0], curPos);	
}


// recursive function that compress expanded node and makes compact nodes (digits) elements
//
// upElement - parent digit element in Compressed graph
// expandedNode - expand node to compress 
// curPos    - current position in compressedGraph
void GraphGenerator::CompressNode( SG_Y_DigitElement &upElement, SG_Y_ExpandedNode expandedNode, unsigned &curPos )
{	

	// put non-empty elements of expanded node to compact graph, returns  first added element,
	// 'curPos' is offset in compressed graph, changes after call to size of added elements
	
	SG_Y_CompactNode *first = MakeCompactNode(expandedNode, curPos);
	upElement.nextNodeOffset = (BYTE *)first - compressedGraph;		// replace previous offset in expanded graph with address in compressed																	

	// compress children of each element that was put to compact graph, and reset nextNodeOffset value relative to compressedGraph	
	SG_Y_DigitElement *pCompressedGraphElement = &first->digits;
	unsigned nodeLen = first->nodeLen;
	for( unsigned i = 0; i < nodeLen; i++ )
	{
		SG_Y_DigitElement &digitElement = *(SG_Y_DigitElement *)pCompressedGraphElement;
		if( unsigned childOffset = digitElement.nextNodeOffset )
		{			
			// For example child element, in position 23 relative to the address of expanded graph, 
			// then 23/12=1st expanded node

			CompressNode(digitElement, expandGraph[childOffset / K_MAX_EXPANDED_NODES], curPos);
		}

		pCompressedGraphElement += 1;		// move to next compressed element
	}
}


// makes compact node from expanded node, compactNode element of 'expandedNode' has type SG_Y_CompactNode,
// the others are SG_Y_DigitElement
SG_Y_CompactNode *GraphGenerator::MakeCompactNode( SG_Y_ExpandedNode expandedNode, unsigned &curPos )
{
	unsigned len = 0;
	SG_Y_CompactNode *compactNode = NULL;
	for( unsigned i = 0; i < K_MAX_EXPANDED_NODES; i++ )
	{
		SG_Y_DigitElement &digitElement = expandedNode[i];
		if( !IsDigitElementEmpty(digitElement) )							// 'x1xxxxxx9x*x' - [1, 9, *] are non empty elements of expanded node
		{
			if( !compactNode )													// first element is compact node
			{
				compactNode = (SG_Y_CompactNode *)&compressedGraph[curPos];
				compactNode->digits = digitElement;				

				curPos += sizeof(SG_Y_CompactNode);
			}
			else																// the others are digit elements in compressed graph
			{
				SG_Y_DigitElement *digInCG = (SG_Y_DigitElement *)&compressedGraph[curPos];				
				*digInCG = digitElement;

				curPos += sizeof(SG_Y_DigitElement);
			}

			len++;
		}
	}

	compactNode->nodeLen = len;		// number of added elements
	return compactNode;
}
