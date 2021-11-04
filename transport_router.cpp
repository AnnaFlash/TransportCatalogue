#include "transport_router.h"

transport_catalogue::router::TransportRouter::TransportRouter(const TransportCatalogue& catalogue, const Settings& settings)
    : catalogue_(catalogue), settings_(settings) {
    graph_ = Graph(AddGraphVertexes());
    AddWaitEdges();
    AddBusEdges();
    router_ = std::make_unique<Router>(graph_);
}

std::optional<ReportRouter> transport_catalogue::router::TransportRouter::GetReportRouter(const std::string from, const std::string to) const
{
    graph::VertexId from_vertex = vertexes.at(from).in;
    graph::VertexId to_vertex = vertexes.at(to).in;

    std::optional<graph::Router<double>::RouteInfo> route_info = router_->BuildRoute(from_vertex, to_vertex);

    if (!route_info) {
        return {};
    }
    ReportRouter output;
    output.total_minutes = route_info->weight;
    auto& items = output.information;
    for (const auto& edge_id : route_info->edges) {
        Info info;
        if (road_edges_.count(edge_id) > 0) {
            info.bus = road_edges_.at(edge_id);
        }
        else if (wait_edges_.count(edge_id) > 0) {
            info.wait = wait_edges_.at(edge_id);
        }
        else {
            assert(false);
        }
        items.push_back(info);
    }
    return { std::move(output) };
}

double transport_catalogue::router::TransportRouter::CalculateTimeBetweenStations(StopPtr from, StopPtr to) const
{
    return 60.0 * catalogue_.GetDistanceBetweenStops(from, to) / (1000.0 * settings_.bus_velocity);
}

void transport_catalogue::router::TransportRouter::AddWaitEdges()
{
    for (const auto& stop : catalogue_.GetStops()) {
        wait_edges_.insert({
            graph_.AddEdge({
                vertexes.at(stop.name).in,
                vertexes.at(stop.name).out,
                static_cast<double>(settings_.bus_wait_time)
            }),
            {
                static_cast<double>(settings_.bus_wait_time),
                stop.name
            }
            });
    }
}

void transport_catalogue::router::TransportRouter::AddBusEdges()
{
    for (const BusPtr& bus : catalogue_.GetBuses()) {
        AddEdgesGraph(bus->b_stops.begin(), bus->b_stops.end(), bus->B_name);

        if (!(bus->end_points_.size() == 1)) {
            AddEdgesGraph(bus->b_stops.rbegin(), bus->b_stops.rend(), bus->B_name);
        }
    }
}

size_t transport_catalogue::router::TransportRouter::AddGraphVertexes()
{
    for (const auto& stop : catalogue_.GetStops()) {
        vertexes_count++;
        vertexes[stop.name].in = vertexes_count;
        vertexes_count++;
        vertexes[stop.name].out = vertexes_count;
    }
    return vertexes_count;
}
