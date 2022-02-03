#pragma once
#include "map_renderer.h"
#include "json_builder.h"
#include "serialization.h"
#include <stdexcept>
#include <filesystem>

    namespace service {
        class RequestHandler;
    }

    namespace renderer {
        struct RenderSettings;
    }  // namespace renderer

    class TransportCatalogue;

    namespace reader {

        class JsonReaderError : public std::runtime_error {
        public:
            using runtime_error::runtime_error;
        };

        class InvalidRequestError : public JsonReaderError {
        public:
            using JsonReaderError::JsonReaderError;
        };

        class JsonReader final {
        public:
            using Path = std::filesystem::path;
            JsonReader() = delete;
            explicit JsonReader(serialization::Serialization& ser,
                transport_catalogue::TransportCatalogue& tc,
                renderer::MapRenderer& map_ren, 
                service::RequestHandler& req_hand,
                router::TransportRouter& tr);
            void ReadBase(std::istream& in = std::cin);
            void ReadRequests(std::istream& in = std::cin);
            void Answer(std::ostream& out = std::cout);
        private:
            serialization::Serialization& ser_;
            transport_catalogue::TransportCatalogue& tc_;
            renderer::MapRenderer& map_ren_;
            service::RequestHandler& req_hand_;
            router::TransportRouter& tr_;
            json::Array base_requests_;
            json::Array stat_request_;
            json::Array answer_on_reqiests_;
            void LoadRouter();
            void ReadTransportCatalogue();
            renderer::RenderSettings ReadRenderSettings(const json::Dict& description) const;
            Settings ReadRouterSettings(const json::Dict& description) const;
            Path ReadSerializationSetting(const json::Dict& description) const;
        };
        json::Array HandleRequests(const json::Array& requests_json, const service::RequestHandler& handler);

    }  // namespace reader