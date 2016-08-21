import pygraphviz as pgv
import unicodedata
import networkx as nx
import matplotlib.pyplot as plt

def longest_path(G):
    dist = {} # stores [node, distance] pair
    for node in nx.topological_sort(G):
        # pairs of dist,node for all incoming edges
        pairs = [(dist[v][0]+1,v) for v in G.pred[node]] 
        if pairs:
            dist[node] = max(pairs)
        else:
            dist[node] = (0, node)
    node,(length,_)  = max(dist.items(), key=lambda x:x[1])
    path = []
    while length > 0:
        path.append(node)
        length,node = dist[node]
    return list(reversed(path))


def longest_path_length(G):

	length = 0
	path = longest_path(G)
	for i in range(0, len(path) - 1):
		edge_data = G.get_edge_data(path[i], path[i+1])
		edge_length = float(edge_data['weight'])
		length += edge_length
	return length 

Gtmp = pgv.AGraph('parasite_graph.dot')
G = nx.DiGraph(Gtmp)

lngest_path = longest_path(G)
lngest_path_length = longest_path_length(G)

print lngest_path
print lngest_path_length

# nx.draw(G)
# plt.show()
