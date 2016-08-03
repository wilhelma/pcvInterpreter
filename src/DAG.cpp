/**
 *
 *    @file  DAG.cpp
 *   @brief  Implementation file for the class `DAG`
 *
 *    @date  02/08/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */


#include "DAG.h"
#define SCALING_FACTOR 1

DAG::DAG() {

	std::shared_ptr<Dag> directed_asymmetric_graph(new Dag());
	dag = directed_asymmetric_graph;
	last_vertex = bgl::add_vertex(*dag);
}

DAG::~DAG() {}

vertex_descr_type DAG::add_edge(vertex_descr_type start, TIME length) {

	vertex_descr_type end = bgl::add_vertex(*dag);
	double lngth = static_cast<double>(length) / 
				   static_cast<double>(SCALING_FACTOR);
	bgl::add_edge(start, end,
				  bgl::property<boost::edge_weight_t, double>(lngth), *dag);
	last_vertex = end;
	return end;
}

void DAG::add_join_edge(vertex_descr_type start, vertex_descr_type end) {

	bgl::add_edge(start, end,
				  bgl::property<boost::edge_weight_t, double>(0.0), *dag);
}


void DAG::write_dot_file() {

	std::ofstream dot("parasite_graph.dot");
	write_graphviz(dot, *dag, bgl::default_writer(), 
					 bgl::make_label_writer(get(boost::edge_weight_t(), *dag)));
}