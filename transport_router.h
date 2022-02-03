#pragma once

#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include "domain.h"
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"
namespace router {

    struct VertexIds {
        graph::VertexId in{};
        graph::VertexId out{};
    };

    class TransportRouter {
    public:
        using Graph = graph::DirectedWeightedGraph<double>;
        using Router = graph::Router<double>;
        explicit TransportRouter(const transport_catalogue::TransportCatalogue& catalogue);
        TransportRouter(const transport_catalogue::TransportCatalogue& catalogue, const Settings& settings);
        void SetSettings(const Settings& settings);
        Settings GetSettings() const;
        void MakeGraph();
        void AddWaitEdges();
        void AddBusEdges();
        double CalculateTimeBetweenStations(transport_catalogue::StopPtr from, transport_catalogue::StopPtr to) const;
        std::optional<ReportRouter> GetReportRouter(const std::string from, const std::string to) const;
    private:
        graph::VertexId GenereateNewVertexId();
        size_t AddVertexes();
        template <typename ItStop>
        void AddEdgesGraph(ItStop begin, ItStop end, const std::string name);
        const transport_catalogue::TransportCatalogue& catalogue_;

        Settings settings_;
        Graph graph_;
        std::unique_ptr<Router> router_;
        std::unordered_map<std::string, VertexIds> vertexes;
        graph::VertexId vertexes_count = 0;
        std::unordered_map<graph::EdgeId, Info::Wait> wait_edges_;
        std::unordered_map<graph::EdgeId, Info::Bus> road_edges_;
    };

    template<typename ItStop>
    inline void TransportRouter::AddEdgesGraph(ItStop begin, ItStop end, const std::string name) {
        for (auto from_it = begin; from_it != end; ++from_it) {
            double weight{};
            int span_count{};
            for (auto to_it = std::next(from_it); to_it != end; ++to_it) {
                std::string departure_name = (*from_it)->name;
                graph::VertexId departure = vertexes.at(departure_name).out;
                std::string arrival_name = (*to_it)->name;
                graph::VertexId arrival = vertexes.at(arrival_name).in;
                weight += CalculateTimeBetweenStations(*prev(to_it), *(to_it));
                ++span_count;
                auto bus_edge_id = graph_.AddEdge({ departure, arrival, weight });
                road_edges_[bus_edge_id] = { name, span_count, weight };
            }
        }
    }
}