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

namespace transport_catalogue {

    namespace router {

        struct VertexIds {
            graph::VertexId in{};
            graph::VertexId out{};
        };

        class TransportRouter {
        public:
            using Graph = graph::DirectedWeightedGraph<double>;
            using Router = graph::Router<double>;
            TransportRouter(const TransportCatalogue& catalogue, const Settings& settings);
            std::optional<ReportRouter> GetReportRouter(const std::string from, const std::string to) const;
            double CalculateTimeBetweenStations(StopPtr from, StopPtr to) const;
            void AddWaitEdges();
            void AddBusEdges();
        private:
            size_t AddGraphVertexes();
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
                double sum_minute = 0;
                int span_count = 0;
                for (auto to_it = std::next(from_it); to_it != end; ++to_it) {
                    std::string from_name = (*from_it)->name;
                    graph::VertexId from = vertexes.at(from_name).out;
                    std::string to_name = (*to_it)->name;
                    graph::VertexId to = vertexes.at(to_name).in;
                    sum_minute += CalculateTimeBetweenStations(*prev(to_it), *(to_it));
                    ++span_count;
                    auto bus_edge_id = graph_.AddEdge({ from, to, sum_minute });
                    road_edges_[bus_edge_id] = { name, span_count, sum_minute };
                }
            }
        }
    }
}