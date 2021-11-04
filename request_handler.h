#pragma once
#include "domain.h"

#include <optional>
#include <string_view>
#include <unordered_set>

namespace svg {
    class Document;
}

namespace transport_catalogue {

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
            RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer, const router::TransportRouter& router);

            std::optional<BusInfo> GetBusStat(const std::string_view& bus_name) const;

            const std::set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

            svg::Document RenderMap() const;

            std::optional<ReportRouter> GetReportRouter(const std::string from, const std::string to) const;

        private:
            const TransportCatalogue& db_;
            const renderer::MapRenderer& renderer_;
            const router::TransportRouter& router_;
        };

    }  // namespace service
}  // namespace transport_catalogue