#include <cstdint>
#include <fstream>
#include <sstream>
#include <utility>
#include <variant>

#include "serialization.h"

namespace serialization {

    Serialization::Serialization(transport_catalogue::TransportCatalogue& transport_catalogue,
        renderer::MapRenderer& map_renderer,
        router::TransportRouter& transport_router)
        : transport_catalogue_(transport_catalogue)
        , map_renderer_(map_renderer)
        , transport_router_(transport_router) {}

    void Serialization::SetSetting(const Path& path_to_base) {
        path_to_base_ = path_to_base;
    }

    void Serialization::CreateBase() {
        std::ofstream out_file(path_to_base_, std::ios::binary);
        SaveStops();
        SaveFromToDistance();
        SaveBuses();
        SaveMap();
        SaveRouteSettings();
        base_.SerializeToOstream(&out_file);
    }

    void Serialization::AccessBase() {
        std::ifstream in_file(path_to_base_, std::ios::binary);
        base_.ParseFromIstream(&in_file);
        LoadStops();
        LoadBuses();
        LoadFromToDistances();
        LoadRouteSettings();
    }

    //Transport Requests

    transport_catalogue_proto::Stop Serialization::SaveStop(const transport_catalogue::Stop& stop) const {
        transport_catalogue_proto::Stop result;
        result.set_name(stop.name);
        result.mutable_coords()->set_lat(stop.coordinates.lat);
        result.mutable_coords()->set_lon(stop.coordinates.lng);
        return result;
    }

    transport_catalogue_proto::FromToDistance Serialization::SaveFromToDistance(transport_catalogue::StopPtr from,
        transport_catalogue::StopPtr to, uint64_t distance) const {
        transport_catalogue_proto::FromToDistance result;
        result.set_from(from->name);
        result.set_to(to->name);
        result.set_distance(distance);
        return result;
    }

    transport_catalogue_proto::Bus Serialization::SaveBus(const transport_catalogue::Bus& bus) const {
        transport_catalogue_proto::Bus result;
        (bus.end_points_.size() == 1) ? result.set_is_ring(true) : result.set_is_ring(false);
        result.set_number(bus.B_name);
        for (const auto& stop : bus.b_stops) {
            result.add_stop_names(stop->name);
        }
        return result;
    }

    void Serialization::SaveStops() {
        transport_catalogue_proto::Stop add_stop;
        transport_catalogue_proto::Coordinates coords;
        for (const auto& stop : transport_catalogue_.GetStops()) {
            add_stop.set_name(stop.name);
            coords.set_lat(stop.coordinates.lat);
            coords.set_lon(stop.coordinates.lng);
            *add_stop.mutable_coords() = coords;
            //add_stop.set_allocated_coords(&coords);
            *base_.add_stops() = add_stop;
        }
    }

    void Serialization::SaveFromToDistance() {
        for (const auto& [from_to, distance] : transport_catalogue_.GetStopsDistance()) {
            *base_.add_stops_distance() = std::move(SaveFromToDistance(from_to.first, from_to.second, distance));
        }
    }

    void Serialization::SaveBuses() {
        for (const auto& bus : transport_catalogue_.GetBuses()) {
            *base_.add_buses() = std::move(SaveBus(*bus));
        }
    }

    void Serialization::LoadStop(const transport_catalogue_proto::Stop& stop) {
        transport_catalogue_.AddStop(stop.name(), stop.coords().lat(), stop.coords().lon());
    }

    void Serialization::LoadFromToDistance(const transport_catalogue_proto::FromToDistance& from_to_distance) {
        const auto& from = transport_catalogue_.FindStop(from_to_distance.from());
        const auto& to = transport_catalogue_.FindStop(from_to_distance.to());
        transport_catalogue_.AddDistances(from->name, to->name, from_to_distance.distance());
    }

    void Serialization::LoadBus(const transport_catalogue_proto::Bus& bus) {
        std::vector<std::string> stops;
        std::vector<std::string> end_points_;
        for (const auto& stop : bus.stop_names()) {
            stops.emplace_back(stop);
        }
        if (bus.is_ring() == false) {
            end_points_.push_back(stops[0]);
            end_points_.push_back(stops[stops.size() / 2]);
        }
        else {
            end_points_.push_back(stops[0]);
        }
        std::vector<std::string_view> stops_(begin(stops), end(stops));
        transport_catalogue_.AddBus(std::move(bus.number()), std::move(stops_), std::move(end_points_));
    }

    void Serialization::LoadStops() {
        for (int i = 0; i < base_.stops_size(); ++i) {
            LoadStop(base_.stops(i));
        }
    }

    void Serialization::LoadFromToDistances() {
        for (int i = 0; i < base_.stops_distance_size(); ++i) {
            LoadFromToDistance(base_.stops_distance(i));
        }
        const auto& buses = transport_catalogue_.GetBuses();
        for (auto& bus : buses) {
            transport_catalogue_.AddLenth(bus->B_name);
        }
    }

    void Serialization::LoadBuses() {
        for (int i = 0; i < base_.buses_size(); ++i) {
            LoadBus(base_.buses(i));
        }
    }
    //Map Requests

    void Serialization::SaveMap() const
    {
        std::ostringstream out;
        const auto buses = transport_catalogue_.GetBuses();
        svg::Document doc;
        map_renderer_.RenderMap(buses).Draw(doc);
        doc.Render(out);
        base_.mutable_map()->set_description(out.str());
    }
    proto_renderer::Map Serialization::LoadMap() const
    {
        return base_.map();
    }
    // Route Requests
    void Serialization::SaveRouteSettings() const {
        Settings settings_ = transport_router_.GetSettings();
        auto& settings = *base_.mutable_settings();
        settings.set_bus_wait_time(settings_.bus_wait_time);
        settings.set_bus_velocity(settings_.bus_velocity);
    }

    void Serialization::LoadRouteSettings() {
        Settings result;
        const auto& parameters = base_.settings();
        result.bus_wait_time = parameters.bus_wait_time();
        result.bus_velocity = parameters.bus_velocity();
        transport_router_.SetSettings(result);
    }
}
