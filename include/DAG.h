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
#include <boost/graph/properties.hpp>
#include <vector>

#include "Types.h"

namespace bgl = boost;

struct EdgeInfo
{
    double length;
};

struct EdgeInfoPropertyTag
{
    typedef bgl::edge_property_tag kind;
};

typedef bgl::property<EdgeInfoPropertyTag, EdgeInfo> edge_info_prop_type;
typedef bgl::adjacency_list<bgl::vecS, bgl::vecS, bgl::directedS,
    	bgl::no_property, edge_info_prop_type> Dag;
typedef bgl::graph_traits<Dag>::vertex_descriptor vertex_descr_type;
typedef bgl::graph_traits<Dag>::edge_descriptor edge_descr_type;


class DAG {

 public:
	DAG();
	~DAG();
	void add_thread_create(TIME length);
	void add_thread_end(TIME length);

	std::shared_ptr<Dag> dag;
	std::vector<vertex_descr_type> start_vertex_stack;
};

#endif /* DAG_H */





