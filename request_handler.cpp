#include "request_handler.h"

#include "map_renderer.h"
#include "transport_catalogue.h"

namespace transport_catalogue::service {

    RequestHandler::RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer)
        : db_(db)
        , renderer_(renderer) {
    }

    BusInfo RequestHandler::GetBusStat(const std::string_view& bus_name) const noexcept {
        const Bus* bus = db_.FindBus(bus_name);
        return bus ? db_.GetBusInfo(bus_name) : BusInfo();
    }

    const std::unordered_set<Bus*> RequestHandler::GetBusesByStop(
        const std::string_view& stop_name) const {
        const Stop* stop = db_.FindStop(stop_name);
        std::unordered_set<Bus*> answer;
        if (stop) { answer = (db_.GetBusesByStop(stop->name)); }
        else {
            throw std::out_of_range("");
        }
        return answer;
    }

    svg::Document RequestHandler::RenderMap() const {
        const auto& buses = db_.GetBuses();
        svg::Document doc;
        renderer_.RenderMap(buses.begin(), buses.end()).Draw(doc);
        return doc;
    }

}  // namespace transport_catalogue::service