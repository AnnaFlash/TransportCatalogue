#pragma once
#include "json.h"
#include "map_renderer.h"

#include <stdexcept>

namespace transport_catalogue {

    namespace service {
        class RequestHandler;
    }

    namespace renderer {
        struct RenderSettings;
    }  // namespace renderer

    class TransportCatalogue;

    namespace json_reader {

        class JsonReaderError : public std::runtime_error {
        public:
            using runtime_error::runtime_error;
        };

        class InvalidRequestError : public JsonReaderError {
        public:
            using JsonReaderError::JsonReaderError;
        };

        void ReadTransportCatalogue(transport_catalogue::TransportCatalogue& catalogue, const json::Array& base_requests_json);

        void ReadRenderSettings(renderer::RenderSettings& rs,const json::Dict& render_settings_json);

        json::Array HandleRequests(const json::Array& requests_json,
            const service::RequestHandler& handler);

    }  // namespace json_reader
}  // namespace transport_catalogue