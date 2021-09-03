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

    class TransportCatalogue;
    struct BusInfo;

    namespace service {

        class RequestHandler {
        public:
            RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);

            std::optional<BusInfo> GetBusStat(const std::string_view& bus_name) const;

            const std::unordered_set<Bus*>* GetBusesByStop(
                const std::string_view& stop_name) const;

            svg::Document RenderMap() const;

        private:
            const TransportCatalogue& db_;
            const renderer::MapRenderer& renderer_;
        };

    }  // namespace service
}  // namespace transport_catalogue
