/**
 *
 *    @file  DAG.h
 *   @brief  Definitions for the class `DAG`
 *
 *    @date  02/08/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef DAG_H
#define DAG_H

#include <boost/graph/adjacency_list.hpp> 
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/properties.hpp>

#include <fstream>
#include <string>
#include <vector>

#include "Types.h"

namespace bgl = boost;


typedef bgl::adjacency_list
		   <bgl::vecS, bgl::vecS, bgl::directedS,
    		bgl::no_property, 
    		bgl::property<bgl::edge_weight_t, double> > Dag;
typedef bgl::graph_traits<Dag>::vertex_descriptor vertex_descr_type;
typedef bgl::graph_traits<Dag>::edge_descriptor edge_descr_type;
typedef std::map<edge_descr_type, double> LengthMap;

class DAG {

 public:
	DAG();
	~DAG();
	void add_thread_create(TIME length);
	void add_thread_end(TIME length);
	void write_dot_file();

	std::shared_ptr<Dag> dag;
	std::vector<vertex_descr_type> start_vertex_stack;
};

#endif /* DAG_H */





