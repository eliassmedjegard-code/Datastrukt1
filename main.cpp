// main.cpp
// Drivrutin för labb 1 - Grafer.
// Läser indatafil, bygger grafen, kör DFS/BFS för
// sammanhängandetest, och svarar med Dijkstra på de tre frågorna.

#include <iostream>
#include <iomanip>
#include <string>
#include "reader.h"
#include "graph.h"

// Skriver ut en väg på formen "Nod1 -> Nod2 -> Nod3 (total: X)"
static void print_path(const Graph& g,
                       const std::vector<node_id_t>& path,
                       weight_t total) {
    if (path.empty()) {
        std::cout << "  Ingen väg hittades.\n";
        return;
    }
    std::cout << "  Total väglängd: " << total << "\n";
    std::cout << "  Promenad: ";
    for (size_t i = 0; i < path.size(); ++i) {
        std::string name = g.get_node_name(path[i]);
        if (name.empty()) name = "(nod " + std::to_string(path[i]) + ")";
        std::cout << name;
        if (i + 1 < path.size()) std::cout << " -> ";
    }
    std::cout << "\n";
}

// Kör Dijkstra mellan två noder beskrivna med namn och skriver ut svaret
static void answer_question(const Graph& g,
                            const std::string& from,
                            const std::string& to) {
    std::cout << "\nFråga: kortaste väg från \"" << from
              << "\" till \"" << to << "\"\n";

    node_id_t start = g.find_node_by_name(from);
    node_id_t end   = g.find_node_by_name(to);
    if (start == -1) {
        std::cout << "  Hittade inte start-noden.\n";
        return;
    }
    if (end == -1) {
        std::cout << "  Hittade inte slut-noden.\n";
        return;
    }

    auto [length, path] = g.dijkstra(start, end);
    print_path(g, path, length);
}

int main(int argc, char** argv) {
    // Indata-fil. Default är "input.txt", men man kan ange annan via kommandorad.
    std::string filename = "input.txt";
    if (argc > 1) filename = argv[1];

    std::cout << "Läser indata från: " << filename << "\n";

    // Läs filen och bygg grafen
    auto adj = parse_file(filename);
    Graph g(adj);

    std::cout << "Antal noder i grafen: " << g.nodes().size() << "\n";

    // ----- Sammanhängande-test med DFS och BFS -----
    // Vi väljer ett startnod-id som finns i grafen
    node_id_t start = -1;
    for (node_id_t id : g.nodes()) { start = id; break; }
    if (start == -1) {
        std::cout << "Grafen är tom.\n";
        return 0;
    }

    bool dfs = g.is_connected_dfs(start);
    bool bfs = g.is_connected_bfs(start);

    std::cout << "\n--- Sammanhängande-test ---\n";
    std::cout << "DFS säger att grafen är "
              << (dfs ? "SAMMANHÄNGANDE" : "INTE sammanhängande") << "\n";
    std::cout << "BFS säger att grafen är "
              << (bfs ? "SAMMANHÄNGANDE" : "INTE sammanhängande") << "\n";

    if (!dfs || !bfs) {
        // Rapportera bekymmer och föreslå åtgärd
        auto bad = g.unreachable_from(start);
        std::cout << "\nBekymmer i grafen! Följande noder kan inte nås"
                  << " från startnoden \"" << g.get_node_name(start) << "\":\n";
        for (node_id_t id : bad) {
            std::cout << "  Nod " << id << ": " << g.get_node_name(id) << "\n";
        }
        std::cout << "\nFörslag på åtgärd:\n"
                  << "  Lägg till en kant från en nåbar nod till någon av\n"
                  << "  noderna ovan. Exempel-format för indatafilen:\n"
                  << "      <nod1> <nod2> <vikt> <beskrivning>\n"
                  << "  Alternativt: kontrollera att indatafilen inte saknar rader.\n";
    }

    // ----- Besvara labbens tre frågor med Dijkstra -----
    std::cout << "\n--- Dijkstras algoritm: kortaste vägar ---\n";
    answer_question(g, "Nackstavägen",  "Förrådet");
    answer_question(g, "L319",          "D025");
    answer_question(g, "Universitetet", "Bite Line Västra");

    return 0;
}
