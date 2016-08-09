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
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/dag_shortest_paths.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/properties.hpp>
 
#include <fstream>
#include <string>
#include <vector>

#include "Types.h"

namespace bgl = boost;

typedef bgl::property<bgl::vertex_name_t, std::string> VertexNameProp;
typedef bgl::property<bgl::edge_weight_t, double> EdgeWeightProp;

typedef bgl::adjacency_list
		   <bgl::vecS, 
		    bgl::vecS, 
		    bgl::directedS, 
		    VertexNameProp,
		    EdgeWeightProp > Dag;

typedef bgl::graph_traits<Dag>::vertex_descriptor vertex_descr_type;
typedef bgl::graph_traits<Dag>::edge_descriptor edge_descr_type;
typedef bgl::graph_traits<Dag>::edge_iterator edge_it_type;

class DAG {

 public:
	DAG();
	~DAG();
	vertex_descr_type add_edge(vertex_descr_type start, TIME length,
								std::string end_vertex_label);
	void add_join_edges(vertex_descr_type start, vertex_descr_type end);

	void write_dot_file();

	int vertex_count;
	std::shared_ptr<Dag> dag;
	std::shared_ptr<Dag> tree;
	vertex_descr_type source;
	vertex_descr_type sink;
	vertex_descr_type last_vertex;
};

#endif /* DAG_H */





