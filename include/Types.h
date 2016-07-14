/**
 *
 *    @file  Types.h
 *   @brief  Collects all `typedef`s and `BOOST_STRONG_TYPEDEF()`s.
 *
 *    @date  04/19/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <boost/serialization/strong_typedef.hpp>
#include <string>

// Define ID to be unsigned
typedef unsigned ID;
typedef ID ACC_ID;
typedef ID CAL_ID;
typedef ID FIL_ID;
typedef ID FUN_ID;
typedef ID INS_ID;
typedef ID LOP_ID;
typedef ID LOE_ID;
typedef ID LOI_ID;
typedef ID REF_ID;
typedef ID SEG_ID;
typedef ID TRD_ID;

//// All others IDs should have the same type as ID
//BOOST_STRONG_TYPEDEF( ID, ACC_ID )
//BOOST_STRONG_TYPEDEF( ID, CAL_ID )
//BOOST_STRONG_TYPEDEF( ID, FIL_ID )
//BOOST_STRONG_TYPEDEF( ID, FUN_ID )
//BOOST_STRONG_TYPEDEF( ID, INS_ID )
//BOOST_STRONG_TYPEDEF( ID, LOP_ID )
//BOOST_STRONG_TYPEDEF( ID, LOE_ID )
//BOOST_STRONG_TYPEDEF( ID, LOI_ID )
//BOOST_STRONG_TYPEDEF( ID, REF_ID )
//BOOST_STRONG_TYPEDEF( ID, SEG_ID )
//BOOST_STRONG_TYPEDEF( ID, TRD_ID )

// Access Types
BOOST_STRONG_TYPEDEF( unsigned, POS )

// Call Types
BOOST_STRONG_TYPEDEF( uint64_t, TIME )
BOOST_STRONG_TYPEDEF( size_t, CALLSITE )

// File Types
typedef std::string FIL_PT;

// Function Types
typedef std::string FUN_SG; 
//BOOST_STRONG_TYPEDEF( uint32_t, FUN_TYP ) // Already uses FunctionType
BOOST_STRONG_TYPEDEF( unsigned, LIN_NO )

// Instruction Types
//BOOST_STRONG_TYPEDEF( unsigned, INS_TYP ) // Already uses InstructionType

// TODO loops

// Reference Types
BOOST_STRONG_TYPEDEF( size_t, REF_SIZE )
//BOOST_STRONG_TYPEDEF( unsigned, REF_MTYP ) // Already uses ReferenceType
typedef std::string REF_NAME;

// Segment Types
//BOOST_STRONG_TYPEDEF( bool, SEG_TYP )     // Already uses SegmentType 

// Thread Types
BOOST_STRONG_TYPEDEF( unsigned, PID )
//BOOST_STRONG_TYPEDEF( std::string, TIME_STRING )
// XXX using strong typedefs with strings prevents from using string methods
typedef std::string TIME_STRING;
BOOST_STRONG_TYPEDEF( unsigned, NUM_CYCLES )

#endif /* TYPES_H_ */
