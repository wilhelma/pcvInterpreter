/**
 *
 *    @file  Function.h
 *   @brief  
 *
 *    @date  06/16/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  FUNCTION_H_
#define  FUNCTION_H_

#include "Types.h"

typedef struct function_t {
    FUN_ID sql_id;
    FUN_SG signature;
    FunctionType type;
    FIL_ID file_id;
    LIN_NO line_number;

    explicit
    function_t(FUN_ID sqlID,
               FUN_SG fnSignature,
               FunctionType fnType,
               FIL_ID fileId,
               LIN_NO lineNumber)
        : sql_id(sqlID), signature(fnSignature), type(fnType),
          file_id(fileId), line_number(lineNumber)
    {
    }

    static void printFunctionType(unsigned short shift) {
        switch(shift) {
            case 0:
                std::cout << "ENTRY_ROUTINE" << std::endl;
                break;
            case 1:
                std::cout << "EXIT_ROUTINE"  << std::endl;
                break;
            case 2:
                std::cout << "FUNCTION"  << std::endl;
                break;
            case 3:
                std::cout << "METHOD"  << std::endl;
                break;
            case 4:
                std::cout << "FREE"  << std::endl;
                break;
            case 5:
                std::cout << "FORK"  << std::endl;
                break;
            case 6:
                std::cout << "JOIN"  << std::endl;
                break;
            case 7:
                std::cout << "ACQUIRE"  << std::endl;
                break;
            case 8:
                std::cout << "RELEASE"  << std::endl;
                break;
            case 9:
                std::cout << "EXTERNAL"  << std::endl;
                break;
            case 10:
                std::cout << "ALLOC"  << std::endl;
                break;
            case 11:
                std::cout << "BARRIER"  << std::endl;
                break;
            case 12:
                std::cout << "WAIT"  << std::endl;
                break;
            case 13:
                std::cout << "SIGNAL_SINGLE"  << std::endl;
                break;
            case 14:
                std::cout << "SIGNAL_BROADCAST"  << std::endl;
                break;
        }
    }

    /**
     * @brief Decodes the function type
     * @todo Return a proper thing!!
     */
//  static Function::type getFunctionType(FUN_TYP fnType) {
//
//      // shift right functionType till there is no information left
//      unsigned int counter = 0;
//      while( fnType != 0 ){
//          if( fnType & 1 ) {
//              std::cout << "   -- has type " << (1 << counter) << std::endl;
//              std::cout << "    -> ";
//              printFunctionType( counter );
//          }
//
//          counter ++;
//          // shift right by one
//          fnType >>= 1;
//      }
//      return Function::OTHER;
//  }
} function_t;

#endif
