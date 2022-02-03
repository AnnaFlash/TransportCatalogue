#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include <transport_catalogue.pb.h>
#include <map_renderer.pb.h>
namespace serialization {

class Serialization {
public:
    using Path = std::filesystem::path;
    
    Serialization(transport_catalogue::TransportCatalogue& transport_catalogue,
        renderer::MapRenderer& map_renderer, 
        router::TransportRouter& transport_router);
    void SetSetting(const Path& path_to_base);
    void CreateBase();
    void AccessBase();
private:
    // TransportCatalogue requests
    transport_catalogue_proto::Stop SaveStop(const transport_catalogue::Stop& stop) const;
    transport_catalogue_proto::FromToDistance SaveFromToDistance(transport_catalogue::StopPtr from, transport_catalogue::StopPtr to, uint64_t distance) const;
    transport_catalogue_proto::Bus SaveBus(const transport_catalogue::Bus& bus) const;
    void SaveStops();
    void SaveFromToDistance();
    void SaveBuses();
    void LoadStop(const transport_catalogue_proto::Stop& stop);
    void LoadFromToDistance(const transport_catalogue_proto::FromToDistance& from_to_distance);
    void LoadBus(const transport_catalogue_proto::Bus& bus);
    void LoadStops();
    void LoadFromToDistances();
    void LoadBuses();  

    //Map Requests
private:
    void SaveMap() const;
public:
    proto_renderer::Map LoadMap() const;

    //Route Request
private:
    void SaveRouteSettings() const;
    void LoadRouteSettings();
private:
    Path  path_to_base_;
    transport_catalogue::TransportCatalogue& transport_catalogue_;
    renderer::MapRenderer& map_renderer_;
    router::TransportRouter& transport_router_;
    mutable transport_catalogue_proto::TransportCatalogue base_;
};
    
}