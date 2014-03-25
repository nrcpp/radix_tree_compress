////////////////////////////////////////////////////////////////////////////////
// GraphGenerator.h - declaration of class to build expanded graph and generate
// Author: Denis Kazakov

#pragma once

#include "Structures.h"
#include "RuleEntry.h"
#include <cassert>


#define MAX_ADDRESSED		8192
#define EXPAND_GRAPH_SIZE	MAX_ADDRESSED / K_MAX_EXPANDED_NODES

#define DEBUG(s)	s



// - read rules from .ini file
// - add rules to expanded graph 
// - generate compressed graph from expanded
class GraphGenerator
{
	SG_Y_ExpandedNode *expandGraph;						// nodes with digits in format '0123xxxx89#*'	
	unsigned totalNodes;								// index of next free expanded node on Adding to expandGraph
	unsigned totalDigitElements;						// non-empty elements in the graph	
		
	BYTE *compressedGraph;							// '0xxxx5xxxx#*' -> '(4|0)5#*'. Where (4|0) is SG_Y_CompactNode, the others are SG_Y_DigitElement
	
	// free previous results, makes new expanded graph, fills it with 0
	void Init();


	// returns index in expanded node (digit) from char in number, 0-9#*
	int CharToDigit( char c ) 
	{
		assert( isdigit(c) || c == '#' || c == '*' );

		if( isdigit(c) )
			return c - '0';
		else if( c == '#' )
			return SHARP_DIGIT;
		else if( c == '*' )
			return STAR_DIGIT;					
		else
			return -1;
	}
	
	// char for output
	friend void PrintNode(SG_Y_DigitElement &, GraphGenerator &, unsigned &);
	static char DigitToChar( unsigned d ) 
	{
		return d <= 9 ? d + '0' : (d == SHARP_DIGIT ? '#' : '*');
	}
	
	// for output
	string GetNodeAsString( SG_Y_ExpandedNode node ) 
	{
		string rval;
		for( unsigned i = 0; i < K_MAX_EXPANDED_NODES; i++ )
		{
			SG_Y_DigitElement &element = node[i];
			rval += IsDigitElementEmpty(element)		?
					'-'  						:
					DigitToChar(element.digit)  ;
		}

		return rval;
	}

	// A digit element is empty if it has no child node and it is not a leaf node and
	bool IsDigitElementEmpty( SG_Y_DigitElement &digitElement ) 
	{
		return !digitElement.nextNodeOffset && !digitElement.leafNode && !digitElement.digit;
	}
		

	// returns full path to the .ini file with rules
	string GetIniFullPath();

	
	// adds 'number' in rule to expanded graph, sets attributes of last digit element using rule attributes
	bool AddToExpandedGraph( RuleEntry &ruleEntry );
	

	// makes compact node from expanded node, first element of 'enode' has type SG_Y_CompactNode if it isn't leaf
	// the others are SG_Y_DigitElement,
	// returns first added digit (or 
	SG_Y_CompactNode *MakeCompactNode( SG_Y_ExpandedNode expandedNode, unsigned &curPos );

	// recursive function, compress expanded node and makes compact nodes (digits) elements
	void CompressNode( SG_Y_DigitElement &upElement, SG_Y_ExpandedNode expandedNode, unsigned &curPos );	

public:
	GraphGenerator() 
	{
		expandGraph = NULL;
		compressedGraph = NULL;		
		totalDigitElements = 0;
		totalNodes = 0;		
	}
	
	~GraphGenerator() 
	{
		Free();
	}

	void Free()	
	{
		delete [] expandGraph;	
		expandGraph = NULL;
		delete [] compressedGraph;
		compressedGraph = NULL;
	}

	// get compressed graph size in bytes, 
	unsigned GetCompressedGraphSize() 
	{		
		unsigned total = this->totalNodes + 1;		// we count totalNodes from 0 on adding to expanded graph, therefore increase value to 1

		// For example for sets: "012" "02" = size of (4 compact nodes + 5-4 digit elements)
		return total * sizeof(SG_Y_CompactNode) + (totalDigitElements-total) * sizeof(SG_Y_DigitElement);
	}

	unsigned GetTotalElements() 
	{
		return totalDigitElements;
	}
	
	SG_Y_CompactNode *GetGraph() 
	{
		return (SG_Y_CompactNode *)compressedGraph;
	}

	unsigned ReadRules();
	void Generate();
};

