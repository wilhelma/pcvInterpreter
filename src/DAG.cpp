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
#define SCALING_FACTOR 1000

DAG::DAG() {

	std::shared_ptr<Dag> directed_asymmetric_graph(new Dag());
	dag = directed_asymmetric_graph;
	vertex_descr_type first_vertex = bgl::add_vertex(*dag);
	std::vector<vertex_descr_type> strt_vertex_stack;
	start_vertex_stack = strt_vertex_stack;
	start_vertex_stack.push_back(first_vertex);
}

DAG::~DAG() {}

void DAG::add_thread_create(TIME length) {

	vertex_descr_type start_vertex = bgl::add_vertex(*dag);
	EdgeInfo props;
	props.length = static_cast<double>(length) / 
				   static_cast<double>(SCALING_FACTOR);
	bgl::add_edge(start_vertex_stack.back(), start_vertex, 
			 	  props, *dag);
	start_vertex_stack.push_back(start_vertex);
	start_vertex_stack.push_back(start_vertex);
}

void DAG::add_thread_end(TIME length) {

	vertex_descr_type end_vertex = bgl::add_vertex(*dag);
	EdgeInfo props;
	props.length = static_cast<double>(length) / 
				   static_cast<double>(SCALING_FACTOR);
	bgl::add_edge(start_vertex_stack.back(), end_vertex,
				  props, *dag);
	start_vertex_stack.pop_back();
}