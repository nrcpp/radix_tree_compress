/////////////////////////////////////////////////
// Program.cpp - testing GraphGenerator
// Author: Denis Kazakov

#include <Windows.h>
#include <string>
#include <iostream>
using namespace std;

#include "GraphGenerator.h"


void Help()
{
	cout << "==============================" << endl
		 << "P'x' - denotes 'x' is a parent element of current node" << endl
		 << "Ly   - denotes y is a level (deepness) of current node" << endl
		 << "'z|' - z denotes length of compact node"    << endl
		 << "'-' char relative to empty expanded node" << endl		 
		 << "==============================" << endl << endl;		  
}


// prints compressed nodes starting in position 'curPos' due curPos + nodeLen,
// repeats recursively for each child node of printed
void PrintNode( SG_Y_DigitElement &upElement, GraphGenerator &gg, unsigned &curPos )
{	
	static int level = -1;	
	level++;
		 	
	cout << "P'" << GraphGenerator::DigitToChar(upElement.digit) << "', L" << level << ": " ;
	
	unsigned curOff = curPos;
	SG_Y_CompactNode *const n = (SG_Y_CompactNode *)&gg.compressedGraph[curOff];
	unsigned len = n->nodeLen;
	cout << len  << '|';
	SG_Y_DigitElement *pDig = &n->digits;
	for (unsigned i = 1; i <= len; i++, pDig++)
	{
		cout << GraphGenerator::DigitToChar( reinterpret_cast<SG_Y_DigitElement *>(pDig)->digit );
		
		curPos += i == 1 ? sizeof(SG_Y_CompactNode) : sizeof(SG_Y_DigitElement);
	}

	cout << endl;
	
	pDig = &n->digits;
	for (unsigned i = 1; i <= len; i++, pDig++)
	{		
		if( unsigned off = pDig->nextNodeOffset )
		{			
			PrintNode(n->digits, gg, curPos);
		}
	}
	
	level--;	
}

void PrintGraph( GraphGenerator &gg, unsigned rulesRead )
{
	cout << endl << "==============================" << endl;	
	cout << rulesRead << " rules was read. Graph elements=" << gg.GetTotalElements() << ", bytes=" << gg.GetCompressedGraphSize() << ":" << endl << endl;
	SG_Y_DigitElement dummy = { 0 };
	unsigned pos = 0;
	PrintNode(dummy, gg, pos);
}

#include <fstream>
int main()
{
	//cout << sizeof(SG_Y_DigitElement) << ' ' << sizeof(SG_Y_CompactNode) << endl;	return 0;
	Help();

	// Invoke constructors
	GraphGenerator gg;

	// Read the ini file and generate a temporary graph
	if( unsigned rulesRead = gg.ReadRules() )
	{
		gg.Generate();		
		PrintGraph(gg, rulesRead);

		//ofstream f("dump.bin", ios::binary); f.write((char*)gg.GetGraph(), gg.GetCompressedGraphSize());

		gg.Free();
	}
	
	getchar();
	return 0;
}
