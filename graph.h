// graph.h
// Grafklass byggd på en granne-matris (adjacency matrix).
// Innehåller DFS, BFS och Dijkstras algoritm.

#ifndef DOA_LABB1_GRAPH_H
#define DOA_LABB1_GRAPH_H

#include "reader.h"
#include <vector>
#include <string>
#include <set>
#include <limits>

// Sentinel-värde som betyder "ingen kant finns".
// Använder oändligheten för double så att Dijkstra kan jämföra rakt av.
const weight_t NO_EDGE = std::numeric_limits<weight_t>::infinity();

class Graph {
private:
    int num_nodes;                              // Antal noder (max-id + 1)
    std::vector<std::vector<weight_t>> matrix;  // Granne-matris med vikter
    meta_t meta;                                // Beskrivning per nod
    std::set<node_id_t> existing_nodes;         // Alla nod-id som faktiskt finns
    edge_list_t raw_edges;                      // Original-kantlistan (för felsökning)

public:
    // Bygger grafen från det parse_file gav oss
    explicit Graph(const adjacency_list_t& adj_list);

    // Returnerar antal noder i grafen
    int size() const { return num_nodes; }

    // Returnerar alla nod-id som finns i grafen
    const std::set<node_id_t>& nodes() const { return existing_nodes; }

    // Slår upp namn för en nod (eller tom sträng om nod saknas)
    std::string get_node_name(node_id_t id) const;

    // Letar reda på nod-id baserat på beskrivning. -1 om ej hittad.
    // Jämförelsen ignorerar inledande/avslutande whitespace.
    node_id_t find_node_by_name(const std::string& name) const;

    // DFS: returnerar true om alla noder kan nås från startnoden
    bool is_connected_dfs(node_id_t start) const;

    // BFS: returnerar true om alla noder kan nås från startnoden
    bool is_connected_bfs(node_id_t start) const;

    // Returnerar lista över noder som EJ kan nås från start.
    // Användbart för att rapportera bekymmer i grafen.
    std::vector<node_id_t> unreachable_from(node_id_t start) const;

    // Dijkstras algoritm.
    // Returnerar par av (total väglängd, vägen som lista av nod-id).
    // Om ingen väg finns returneras (oändlighet, tom lista).
    std::pair<weight_t, std::vector<node_id_t>>
        dijkstra(node_id_t start, node_id_t end) const;

    // Letar efter "bekymmer" i grafen: riktade kanter som saknar
    // sin motsatsriktning. T.ex. om "19 -> 3" finns men "3 -> 19" saknas
    // hamnar kanten 19 -> 3 i listan. Tom lista = inga bekymmer.
    std::vector<edge> asymmetric_edges() const;
};

#endif //DOA_LABB1_GRAPH_H
