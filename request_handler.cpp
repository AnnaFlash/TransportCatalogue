#include "request_handler.h"

#include "map_renderer.h"
#include "transport_catalogue.h"

namespace transport_catalogue::service {

    RequestHandler::RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer)
        : db_(db)
        , renderer_(renderer) {
    }

    std::optional<BusInfo> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
        const Bus* bus = db_.FindBus(bus_name);
        return bus ? std::make_optional(db_.GetBusInfo(bus->B_name)) : std::nullopt;
    }

    const std::unordered_set<Bus*>* RequestHandler::GetBusesByStop(
        const std::string_view& stop_name) const {
        Stop* stop = db_.FindStop(stop_name);
        return stop ? &(db_.GetBusesByStop(stop)) : nullptr;
    }

    svg::Document RequestHandler::RenderMap() const {
        const auto buses = db_.GetBuses();
        svg::Document doc;
        renderer_.RenderMap(buses).Draw(doc);
        return doc;
    }

}  // namespace transport_catalogue::service