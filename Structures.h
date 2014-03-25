////////////////////////////////////////////////////////////////////////////
// Structures.h - structures that represents expanded node and compact node
// Author: Denis Kazakov

#pragma  once


#define K_MAX_EXPANDED_NODES		12
#define SHARP_DIGIT		0xa
#define STAR_DIGIT		0xb


#pragma  pack(push, 1)
typedef struct
{
	UINT 	digit  				:  4;
	UINT 	nextNodeOffset 		: 13;
	UINT	leafNode			: 1;

	UINT 	attCalledNumber		: 1;
	UINT 	attCallingNumber	: 1;
	UINT 	attInboundCall		: 1;
	UINT 	attOutboundCall		: 1;
	UINT 	attWorkingHours		: 1;
	UINT 	attNonWorkingHours	: 1;
	UINT 	attBlocked			: 1;
	UINT 	attAllowed			: 1;
	UINT 	attPrefix			: 1;
	UINT 	attAutoAnswer		: 1;
	UINT 	attReserved1		: 1;
	UINT 	attReserved2		: 1;
	UINT 	attReserved3		: 1;
	UINT 	attReserved4		: 1;
} SG_Y_DigitElement;


typedef SG_Y_DigitElement SG_Y_ExpandedNode[K_MAX_EXPANDED_NODES];

typedef struct
{
	UINT8	nodeLen 	: 4;
	UINT8	reserved 	: 4;
	/* The first digit in the collection */
	SG_Y_DigitElement	digits;
} SG_Y_CompactNode;

#pragma pack(pop)
