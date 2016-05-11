///**
// *
// *    @file  dotFuncTracker.cpp
// *   @brief  
// *
// *    @date  03/11/16
// *  @author  Paolo Di Giglio (github.com/pdigiglio),
// *           <p.digiglio91@gmail.com>
// *
// */
//
//#include <iostream>
//#include <vector>
//#include <string>
//#include <map>
//
//
//
////class Graph {
////	public:
////		void addEntry(std::string funcName, unsigned int funcSegment) {
////			funcNameStack.push_back(funcName);
////			funcSegmentStack.push_back(funcSegment);
////		};
////
////		void generateGraph(std::string graphName="graph0") {
////
////		};
////
////	private:
////		std::vector<std::string>  funcNameStack;
////		std::vector<unsigned int> funcSegmentStack;
////};
//
//int main () {
//
//	//-------------------------------------------------------------------------
//	//
//	// From Function Table
//	//
//	// associate (function id) [Id] to (function name) [Signature]
//	std::map<unsigned int, std::string> idToName;
//	idToName.insert( std::pair<unsigned, std::string>( 9, "foo") );
//	idToName.insert( std::pair<unsigned, std::string>(10, "bar") );
//	idToName.insert( std::pair<unsigned, std::string>(11, "main") );
//
//	//-------------------------------------------------------------------------
//	//
//	// From Call Table
//	//
//	// associate (segment) [Id] to (function id) [Function]
//	std::map<unsigned int, unsigned int> segmentToId;
//	segmentToId.insert( std::pair<unsigned,unsigned>(4, 9) );
//	segmentToId.insert( std::pair<unsigned,unsigned>(3, 10) );
//	segmentToId.insert( std::pair<unsigned,unsigned>(5, 9) );
//	segmentToId.insert( std::pair<unsigned,unsigned>(2, 11) );
//
//	// From Segment Table
//	std::vector<unsigned int> segmentStack({2,3,4,3,2,5,2});
//	std::cerr << "Seg." << "\t" << "F. ID" << "\t" << "F. Name" << std::endl;
//	for (auto it = segmentStack.begin(); it != segmentStack.end(); ++it) {
//		std::cerr << *it << "\t" << segmentToId[*it] << "\t" << idToName[segmentToId[*it]] << std::endl;
//	}
//
//
//	std::cout << "graph name {" << std::endl;
//	auto it = segmentStack.begin();
//	do {
//		unsigned oldSegment = *(it++);
//		unsigned newSegment = *(it);
//		if( oldSegment < newSegment )
//			std::cout << idToName[segmentToId[oldSegment]] << "_" << oldSegment << " -- ";
//		else if( oldSegment > newSegment )
//			std::cout << idToName[segmentToId[oldSegment]] << "_" << oldSegment << ";" << std::endl;
//		else
//			return 1;
//
//	} while( it != segmentStack.end() );
//	std::cout << "}" << std::endl;
//
//
//	return 0;
//}
