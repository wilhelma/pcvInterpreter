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

DAG::DAG() : vertex_count(1) {

	std::shared_ptr<Dag> directed_asymmetric_graph(new Dag());
	dag = directed_asymmetric_graph;
	last_vertex = bgl::add_vertex(VertexNameProp("1_TS"), *dag);
	source = last_vertex;
}

DAG::~DAG() {}

vertex_descr_type DAG::add_edge(vertex_descr_type start, TIME length,
								std::string end_vertex_label) {

	vertex_count += 1;
	end_vertex_label = "_" + end_vertex_label;
	end_vertex_label = std::to_string(vertex_count) + end_vertex_label;
	vertex_descr_type end = bgl::add_vertex(VertexNameProp(end_vertex_label),
																		*dag); 
	double lngth = static_cast<double>(length) / 
				   static_cast<double>(SCALING_FACTOR);
	bgl::add_edge(start, end, EdgeWeightProp(lngth), *dag);
	last_vertex = end;
	return end;
}

void DAG::add_join_edge(vertex_descr_type start, vertex_descr_type end) {

	bgl::add_edge(start, end, EdgeWeightProp(0), *dag);
}


void DAG::write_dot_file() {

	std::ofstream dot("parasite_graph.dot");
	bgl::dynamic_properties dp;
    dp.property("label", get(bgl::edge_weight_t(), *dag));
    dp.property("node_id", get(bgl::vertex_name_t(), *dag));
    dp.property("weight", get(bgl::edge_weight_t(), *dag));
	write_graphviz_dp(dot, *dag, dp);
}