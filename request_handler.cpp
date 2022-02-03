#include "request_handler.h"

#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

namespace service {

    RequestHandler::RequestHandler(const serialization::Serialization& ser, const transport_catalogue::TransportCatalogue& db,
        const renderer::MapRenderer& renderer, const router::TransportRouter& router)
        : ser_(ser)
        ,db_(db)
        , renderer_(renderer)
        , router_(router)
    {
    }

    std::optional<transport_catalogue::BusInfo> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
        transport_catalogue::BusPtr bus = db_.FindBus(bus_name);
        return bus ? std::make_optional(db_.GetBusInfo(bus->B_name)) : std::nullopt;
    }

    const std::set<transport_catalogue::BusPtr>* RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
        return db_.GetBusesByStop(stop_name);
    }

    svg::Document RequestHandler::RenderMap() const {
        const auto buses = db_.GetBuses();
        svg::Document doc;
        renderer_.RenderMap(buses).Draw(doc);
        return doc;
    }

    proto_renderer::Map RequestHandler::RenderPrMap() const
    {
        return ser_.LoadMap();
    }

    std::optional<ReportRouter> RequestHandler::GetReportRouter(const std::string from, const std::string to) const {
        return router_.GetReportRouter(from, to);
    }
}  // namespace transport_catalogue::service