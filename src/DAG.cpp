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

// std::vector<double> DAG::shortest_paths() {
	
// 	const int vert_number = num_vertices(*dag);
// 	std::vector<vertex_descr_type> parent(vert_number);
// 	for (int i = 0; i < vert_number; ++i)
//     	parent.at(i) = i;
// 	std::vector<double> distance(vert_number);
// 	distance[0] = 0;
// 	bgl::bellman_ford_shortest_paths(*dag, 
// 								     vert_number,
// 								     weight_map(
// 							     	 	get(bgl::edge_weight_t(), *dag)).
// 						 	         	distance_map(&distance[0]).
// 						             	predecessor_map(&parent[0]));

// 	std::cout << "shortest paths" << std::endl;
// 	for (int i = 0; i < distance.size(); i++)
//     	std::cout << distance.at(i) << ' '<< std::endl;

// 	return distance;
// }

// std::vector<double> DAG::longest_paths() {
	
// 	Dag negative_dag;
// 	bgl::copy_graph(*dag, negative_dag);
// 	const int vert_number = num_vertices(*dag);
// 	std::vector<vertex_descr_type> parent(vert_number);
// 	std::vector<double> distance(vert_number);
// 	distance[0] = 0;
// 	edge_it_type  ei, ei_end;
// 	typedef bgl::property_map<Dag, bgl::edge_weight_t>::type  WeightAcc;
// 	WeightAcc weightInfo(bgl::get(bgl::edge_weight_t(), negative_dag));

// 	for (bgl::tie(ei, ei_end) = edges(negative_dag); ei != ei_end; ++ei) {
// 		double weight = bgl::get(weightInfo, *ei);
// 		weight = -weight;
// 		bgl::put(weightInfo, *ei, weight);
// 	}

// 	bgl::bellman_ford_shortest_paths(negative_dag, 
// 								     vert_number,
// 								     weight_map(
// 							     	 	get(bgl::edge_weight_t(), negative_dag)).
// 						 	         	distance_map(&distance[0]).
// 						             	predecessor_map(&parent[0]));

// 	for (int i = 0; i < distance.size(); i++) {
// 		distance.at(i) = -distance.at(i);
// 	}

// 	std::cout << "longest paths" <<std::endl;
// 	for (int i = 0; i < distance.size(); i++)
//     	std::cout << distance.at(i) << ' '<< std::endl;

// 	return distance;
// }

// double DAG::span() {
// 	return longest_paths().back();
// }

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