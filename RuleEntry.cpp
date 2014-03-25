//////////////////////////////////////////////////////////////////////////
// RuleEntry.cpp - implementation of RuleEntry methods
// Author: Denis Kazakov

#include <string>
#include <cstdio>
#include <cassert>
#include <vector>

#include <Windows.h>
using namespace std;

#include "RuleEntry.h"


// convert input string into vector of string tokens
//
// note: consecutive delimiters will be treated as single delimiter
// note: delimiters are _not_ included in return data
//
// str - input string to be parsed
// tokens  - output vector with tokens.
// delims  - list of delimiters.
static void tokenize_str(const string & str, vector<string> &tokens, const string & delims = ", \t")
{  
	// Skip delims at beginning, find start of first token
	string::size_type lastPos = str.find_first_not_of(delims, 0);
	// Find next delimiter @ end of token
	string::size_type pos     = str.find_first_of(delims, lastPos);

	while (string::npos != pos || string::npos != lastPos)
    {
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delims.  Note the "not_of". this is beginning of token
		lastPos = str.find_first_not_of(delims, pos);
		// Find next delimiter at end of token.
		pos     = str.find_first_of(delims, lastPos);
    }  
}


// reads rule with number 'ruleIdx' from [Rules] section to ruleEntry,
// returns true if success
//
// ruleEntry - entry to read to
// ruleIdx   - index of rule in section starting from 0
// iniPath   - full path to the .ini file
bool RuleEntry::ReadRule( RuleEntry &ruleEntry, unsigned ruleIdx, const string &iniPath )
{
	const unsigned MAX_RULE_LENGTH = 80;

	CHAR key[30], rule[MAX_RULE_LENGTH];
	sprintf_s(key, sizeof(key) / sizeof(CHAR), "R%04d", ruleIdx);
	
	// get rule value from .ini file
	if( !GetPrivateProfileString( "RULES", key, "", rule, MAX_RULE_LENGTH, iniPath.c_str()) )
	{
		return false;
	}

	vector<string> tokens;	
	tokenize_str(rule, tokens);

	assert(tokens.size() == RULE_ATTRIBUTES_NUMBER);
	
	ruleEntry.number = tokens[0]; 
	ruleEntry.attCalledNumber = tokens[1] == "1";
	ruleEntry.attCallingNumber = tokens[2] == "1";
	ruleEntry.attInboundCall = tokens[3] == "1";
	ruleEntry.attOutboundCall = tokens[4] == "1";
	ruleEntry.attWorkingHours = tokens[5] == "1";
	ruleEntry.attNonWorkingHours = tokens[6] == "1";
	ruleEntry.attBlocked = tokens[7] == "1";
	ruleEntry.attAllowed = tokens[8] == "1";
	ruleEntry.attPrefix = tokens[9] == "1";
	ruleEntry.attAutoAnswer = tokens[10] == "1";

	return true;
}

// sets rule entry attributes to digit element
void RuleEntry::SetAttributesOf( SG_Y_DigitElement &dig )
{
	dig.attCalledNumber = this->attCalledNumber;
	dig.attCallingNumber =	this->attCallingNumber;
	dig.attInboundCall	= this->attInboundCall;
	dig.attOutboundCall	= this->attOutboundCall;
	dig.attWorkingHours = this->attWorkingHours;
	dig.attNonWorkingHours = this->attNonWorkingHours;
	dig.attBlocked = this->attBlocked;
	dig.attAllowed = this->attAllowed;
	dig.attPrefix = this->attPrefix;
	dig.attAutoAnswer = this->attAutoAnswer;
}
