#pragma once
#include "domain.h"

#include <optional>
#include <string_view>
#include <unordered_set>
#include "serialization.h"
namespace svg {
    class Document;
}

    namespace renderer {
        class MapRenderer;
    }
    namespace router {
        class TransportRouter;
    }
    class TransportCatalogue;
    struct BusInfo;

    namespace service {

        class RequestHandler {
        public:
            RequestHandler() = delete;
            RequestHandler(const serialization::Serialization& ser, const transport_catalogue::TransportCatalogue& db,
                const renderer::MapRenderer& renderer, const router::TransportRouter& router);

            std::optional<transport_catalogue::BusInfo> GetBusStat(const std::string_view& bus_name) const;

            const std::set<transport_catalogue::BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

            svg::Document RenderMap() const;
            proto_renderer::Map RenderPrMap()const;

            std::optional<ReportRouter> GetReportRouter(const std::string from, const std::string to) const;

        private:
            const serialization::Serialization& ser_;
            const transport_catalogue::TransportCatalogue& db_;
            const renderer::MapRenderer& renderer_;
            const router::TransportRouter& router_;
        };

    }  // namespace service