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
    // Indata-fil. Default är "export.txt" (kursens datafil).
    // Annan fil kan anges som argument: ./labb1 <filnamn>
    std::string filename = "export.txt";
    if (argc > 1) filename = argv[1];

    std::cout << "Läser indata från: " << filename << "\n";

    // Läs filen och bygg grafen
    auto adj = parse_file(filename);
    Graph g(adj);

    std::cout << "Antal noder i grafen: " << g.nodes().size() << "\n";

    // ----- Sammanhängande-test med DFS och BFS -----
    // Vi väljer första nod-id som finns i grafen som startpunkt
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
        auto bad = g.unreachable_from(start);
        std::cout << "\nFöljande noder kan inte nås från \""
                  << g.get_node_name(start) << "\":\n";
        for (node_id_t id : bad) {
            std::cout << "  Nod " << id << ": " << g.get_node_name(id) << "\n";
        }
    }

    // ----- Asymmetri-kontroll -----
    // Indatafilens header säger att kanterna är riktade. För att kunna
    // promenera åt båda håll måste varje kant ha en motsvarande omvänd kant.
    auto asym = g.asymmetric_edges();
    if (!asym.empty()) {
        std::cout << "\n--- Bekymmer i grafen ---\n";
        std::cout << "Följande riktade kanter saknar sin motsatsriktning:\n";
        for (const auto& e : asym) {
            std::cout << "  " << e.n1 << " -> " << e.n2
                      << " (vikt " << e.weight << ")"
                      << "  [" << g.get_node_name(e.n1) << " -> "
                      << g.get_node_name(e.n2) << "]\n";
        }
        std::cout << "\nFörslag på åtgärd: lägg till motsvarande omvända rader,\n"
                  << "till exempel:\n";
        for (const auto& e : asym) {
            std::cout << "  " << e.n2 << " " << e.n1 << " " << e.weight
                      << e.description << "\n";
        }
        std::cout << "\nMatrisen i den här implementationen behandlar redan\n"
                  << "kanter symmetriskt, så Dijkstra fungerar trots felet.\n";
    } else {
        std::cout << "\n(Inga asymmetriska kanter funna i indatat.)\n";
    }

    // ----- Besvara labbens tre frågor med Dijkstra -----
    // Nodnamnen är hämtade direkt ur datafilen.
    std::cout << "\n--- Dijkstras algoritm: kortaste vägar ---\n";
    answer_question(g, "Nackstavägen Axvägen 1",       "Förrådet");
    answer_question(g, "L319",                          "D025");
    answer_question(g, "Storgatan Rondell Universitetet", "Bite Line West");

    return 0;
}
