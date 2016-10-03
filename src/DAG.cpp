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

DAG::DAG(std::string _name_) : vertex_count(1), name(_name_){

	std::shared_ptr<Dag> directed_asymmetric_graph(new Dag());
	dag = directed_asymmetric_graph;
	last_vertex = bgl::add_vertex(VertexNameProp("TS_0"), *dag);
	source = last_vertex;
}

DAG::~DAG() {}

void DAG::add_edge(vertex_descr_type start, TIME length,
								std::string end_vertex_label) {

	if (GRAPH) {
		vertex_count += 1;
		vertex_descr_type end = bgl::add_vertex(VertexNameProp(end_vertex_label),
																		*dag); 
		double lngth = static_cast<double>(length) / 
				   static_cast<double>(SCALING_FACTOR);
		bgl::add_edge(start, end, EdgeWeightProp(lngth), *dag);
		last_vertex = end;
	}
}

void DAG::add_join_edge(vertex_descr_type start, vertex_descr_type end) {

	if (GRAPH)
		bgl::add_edge(start, end, EdgeWeightProp(0), *dag);
}


void DAG::write_dot_file() {

	if (GRAPH) {
		std::string dir = "./";
		std::string file_name = (dir + "parasite_output") + ".dot";
		std::ofstream dot(file_name);
		bgl::dynamic_properties dp;
    	dp.property("label", get(bgl::edge_weight_t(), *dag));
    	dp.property("node_id", get(bgl::vertex_name_t(), *dag));
    	dp.property("weight", get(bgl::edge_weight_t(), *dag));
		write_graphviz_dp(dot, *dag, dp);
	}
}