#include "request_handler.h"

#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

namespace transport_catalogue::service {

    RequestHandler::RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer, const router::TransportRouter& router)
        : db_(db)
        , renderer_(renderer)
        , router_(router)
    {
    }

    std::optional<BusInfo> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
        BusPtr bus = db_.FindBus(bus_name);
        return bus ? std::make_optional(db_.GetBusInfo(bus->B_name)) : std::nullopt;
    }

    const std::set<BusPtr>* RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
        return db_.GetBusesByStop(stop_name);
    }

    svg::Document RequestHandler::RenderMap() const {
        const auto buses = db_.GetBuses();
        svg::Document doc;
        renderer_.RenderMap(buses).Draw(doc);
        return doc;
    }

    std::optional<ReportRouter> RequestHandler::GetReportRouter(const std::string from, const std::string to) const {
        return router_.GetReportRouter(from, to);
    }
}  // namespace transport_catalogue::service