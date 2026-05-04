// graph.cpp
// Implementation av grafklassen.

#include "graph.h"
#include <queue>
#include <stack>
#include <algorithm>
#include <cctype>

// Hjälpfunktion: ta bort whitespace i början och slutet av en sträng.
// Behövs för att jämföra nodnamn när reader.cpp lämnar kvar inledande mellanslag.
static std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
    return s.substr(start, end - start);
}

Graph::Graph(const adjacency_list_t& adj_list) {
    meta = adj_list.first;
    const edge_list_t& edges = adj_list.second;
    raw_edges = edges; // Spara original för senare felsökning

    // Steg 1: hitta alla nod-id som förekommer i meta eller i kanter,
    // och samtidigt största id för att veta hur stor matrisen ska bli.
    int max_id = -1;
    for (const auto& m : meta) {
        existing_nodes.insert(m.first);
        if (m.first > max_id) max_id = m.first;
    }
    for (const auto& e : edges) {
        existing_nodes.insert(e.n1);
        existing_nodes.insert(e.n2);
        if (e.n1 > max_id) max_id = e.n1;
        if (e.n2 > max_id) max_id = e.n2;
    }
    num_nodes = max_id + 1;

    // Steg 2: initiera matrisen med NO_EDGE överallt.
    matrix.assign(num_nodes, std::vector<weight_t>(num_nodes, NO_EDGE));

    // Steg 3: lägg in kanterna. Grafen är oriktad så vi sätter båda riktningarna.
    // Om samma kant förekommer flera gånger behåller vi den minsta vikten.
    for (const auto& e : edges) {
        if (e.weight < matrix[e.n1][e.n2]) {
            matrix[e.n1][e.n2] = e.weight;
            matrix[e.n2][e.n1] = e.weight;
        }
    }
}

std::string Graph::get_node_name(node_id_t id) const {
    auto it = meta.find(id);
    if (it == meta.end()) return "";
    return trim(it->second);
}

node_id_t Graph::find_node_by_name(const std::string& name) const {
    std::string target = trim(name);
    for (const auto& m : meta) {
        if (trim(m.second) == target) return m.first;
    }
    return -1;
}

// ------------ DFS ------------
// Iterativ djupet-först-sökning med stack.
// Vi besöker varje nod, markerar den som besökt, och stoppar in
// alla obesökta grannar på stacken.
bool Graph::is_connected_dfs(node_id_t start) const {
    std::vector<bool> visited(num_nodes, false);
    std::stack<node_id_t> stack;
    stack.push(start);

    while (!stack.empty()) {
        node_id_t current = stack.top();
        stack.pop();
        if (visited[current]) continue;
        visited[current] = true;

        // Gå igenom raden i matrisen och hitta grannar
        for (node_id_t v = 0; v < num_nodes; ++v) {
            if (matrix[current][v] != NO_EDGE && !visited[v]) {
                stack.push(v);
            }
        }
    }

    // Kontrollera att alla existerande noder besöktes
    for (node_id_t id : existing_nodes) {
        if (!visited[id]) return false;
    }
    return true;
}

// ------------ BFS ------------
// Bredden-först-sökning med kö.
// Skillnaden mot DFS är att vi använder kö istället för stack,
// så att noder besöks i nivå-ordning från startnoden.
bool Graph::is_connected_bfs(node_id_t start) const {
    std::vector<bool> visited(num_nodes, false);
    std::queue<node_id_t> queue;
    queue.push(start);
    visited[start] = true;

    while (!queue.empty()) {
        node_id_t current = queue.front();
        queue.pop();

        for (node_id_t v = 0; v < num_nodes; ++v) {
            if (matrix[current][v] != NO_EDGE && !visited[v]) {
                visited[v] = true;
                queue.push(v);
            }
        }
    }

    for (node_id_t id : existing_nodes) {
        if (!visited[id]) return false;
    }
    return true;
}

// Returnerar alla existerande noder som inte gick att nå från start.
std::vector<node_id_t> Graph::unreachable_from(node_id_t start) const {
    std::vector<bool> visited(num_nodes, false);
    std::queue<node_id_t> queue;
    queue.push(start);
    visited[start] = true;
    while (!queue.empty()) {
        node_id_t current = queue.front();
        queue.pop();
        for (node_id_t v = 0; v < num_nodes; ++v) {
            if (matrix[current][v] != NO_EDGE && !visited[v]) {
                visited[v] = true;
                queue.push(v);
            }
        }
    }
    std::vector<node_id_t> result;
    for (node_id_t id : existing_nodes) {
        if (!visited[id]) result.push_back(id);
    }
    return result;
}

// ------------ Dijkstra ------------
// Klassisk Dijkstra med O(V^2) - enkel och tydlig implementation.
// Lämplig när grafen är liten (vilket den är i denna labb).
//
// Idé:
// 1. Sätt avstånd[start] = 0 och alla andra till oändligheten.
// 2. Upprepa: välj den obesökta nod med minst avstånd, markera som besökt.
// 3. Uppdatera avstånd till varje granne om vi hittade en kortare väg.
// 4. När målet är besökt (eller inga fler nås) -> klart.
// 5. Bygg vägen genom att gå bakåt från målet via prev[].
std::pair<weight_t, std::vector<node_id_t>>
Graph::dijkstra(node_id_t start, node_id_t end) const {

    std::vector<weight_t> dist(num_nodes, NO_EDGE); // NO_EDGE = oändlighet
    std::vector<bool> visited(num_nodes, false);
    std::vector<node_id_t> prev(num_nodes, -1);
    dist[start] = 0;

    for (int i = 0; i < num_nodes; ++i) {
        // Hitta obesökt nod med minst avstånd
        node_id_t u = -1;
        weight_t best = NO_EDGE;
        for (node_id_t v = 0; v < num_nodes; ++v) {
            if (!visited[v] && dist[v] < best) {
                best = dist[v];
                u = v;
            }
        }
        if (u == -1) break;       // Inga fler noder att besöka
        if (u == end) break;       // Vi har nått målet
        visited[u] = true;

        // Slappa kanter från u till alla grannar
        for (node_id_t v = 0; v < num_nodes; ++v) {
            if (matrix[u][v] == NO_EDGE) continue; // ingen kant
            if (visited[v]) continue;
            weight_t alt = dist[u] + matrix[u][v];
            if (alt < dist[v]) {
                dist[v] = alt;
                prev[v] = u;
            }
        }
    }

    // Bygg upp vägen genom att gå bakåt från slutnoden via prev[]
    std::vector<node_id_t> path;
    if (dist[end] == NO_EDGE) {
        return {NO_EDGE, path}; // ingen väg finns
    }
    for (node_id_t at = end; at != -1; at = prev[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());
    return {dist[end], path};
}

// ------------ Asymmetri-kontroll ------------
// Indatat är specificerat som riktade kanter. Om grafen ska kunna gås
// åt båda håll måste varje kant ha en motsvarande omvänd kant.
// Funktionen returnerar de kanter som saknar sin motsats.
std::vector<edge> Graph::asymmetric_edges() const {
    // Lägg in alla riktade kanter (n1 -> n2) i en uppsättning för O(1)-uppslag
    std::set<std::pair<node_id_t, node_id_t>> directed;
    for (const auto& e : raw_edges) {
        directed.insert({e.n1, e.n2});
    }

    // För varje kant: kolla om motsatsen (n2 -> n1) också finns
    std::vector<edge> result;
    for (const auto& e : raw_edges) {
        if (directed.find({e.n2, e.n1}) == directed.end()) {
            result.push_back(e);
        }
    }
    return result;
}
