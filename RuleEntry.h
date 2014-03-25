//////////////////////////////////////////////////////////////////////////
// RuleEntry.h - declaration of RuleEntry class 
// Author: Denis Kazakov

#pragma once

#include <string>
using std::string;

#include "Structures.h"


// represents rule in [Rules] section of .ini file
//
// Examples:
// R0000=12345,0,0,0,0,0,0,0,0,0,0
// R0001=123764465,0,0,0,0,1,0,1,0,0,0
// ...
// R0765=...
struct RuleEntry
{
	static const unsigned RULE_ATTRIBUTES_NUMBER = 11u;		// increase this value on adding new attributes

	string 	number  			;
	bool 	attCalledNumber		;
	bool 	attCallingNumber	;
	bool 	attInboundCall		;
	bool 	attOutboundCall		;
	bool 	attWorkingHours		;
	bool 	attNonWorkingHours	;
	bool 	attBlocked			;
	bool 	attAllowed			;
	bool 	attPrefix			;
	bool 	attAutoAnswer		;

		
	// reads rule with number 'ruleIdx' from [Rules] section to ruleEntry,
	// returns true if success
	static bool ReadRule(RuleEntry &ruleEntry, unsigned ruleIdx, const string &iniPath);

	// sets rules attributes to digit element
	void SetAttributesOf(SG_Y_DigitElement &dig);
};

