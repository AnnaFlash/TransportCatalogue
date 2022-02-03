#include "json_reader.h"

#include "request_handler.h"
#include "transport_catalogue.h"
#include "transport_router.h"
#include <algorithm>
#include <sstream>

namespace reader {
    struct Stop_to_Add {
        std::string name;
        double lat;
        double longt;
    };
    using namespace std::literals;

    void AddingBuses(transport_catalogue::TransportCatalogue& catalogue, std::vector<detail::Bus_to_Add>& buses) {
        using namespace transport_catalogue;
        for (auto& bus : buses) {
            std::vector<std::string_view> stops(begin(bus.stops), end(bus.stops));
            catalogue.AddBus(std::move(bus.name), std::move(stops), std::move(bus.end_points_));
        }
    }

    void AddingDistances(transport_catalogue::TransportCatalogue& catalogue, std::vector<detail::StopDistances>&& distances, std::vector<std::string>&& buses_names) {
        for (auto& stop_distance : distances) {
            for (auto& [s_name, distance] : stop_distance.distances) {
                catalogue.AddDistances(stop_distance.stop, s_name, distance);
            }
        }
        for (auto& name : buses_names) {
            catalogue.AddLenth(name);
        }
    }

    namespace {

        detail::Bus_to_Add ParsingBuses(const json::Dict& bus_json, std::vector<std::string>& buses_names) {
            detail::Bus_to_Add req;
            req.name = bus_json.at("name"s).AsString();
            req.is_round = bus_json.at("is_roundtrip"s).AsBool();
            buses_names.push_back(req.name);
            const auto& stops_json = bus_json.at("stops"s).AsArray();
            req.stops.reserve(stops_json.size());
            for (const auto& stop_json : stops_json) {
                req.stops.emplace_back(stop_json.AsString());
            }
            if (req.is_round == false) {
                req.end_points_.push_back(req.stops[0]);
                req.end_points_.push_back(req.stops[req.stops.size() - 1]);
                if (req.end_points_.size() == 2 && req.end_points_[0] == req.end_points_[1]) {
                    req.end_points_.pop_back();
                }
                req.stops.reserve(req.stops.size() * 2 - 1);
                for (auto it = ++std::rbegin(req.stops); it != std::rend(req.stops); ++it) {
                    req.stops.emplace_back(*it);
                }
            }
            else {
                req.end_points_.push_back(req.stops[0]);
            }
            return req;
        }

        Stop_to_Add ParsingStops(std::vector<detail::StopDistances>& dist, const json::Dict& stop_json) {
            Stop_to_Add req;
            req.name = stop_json.at("name"s).AsString();
            req.lat = stop_json.at("latitude"s).AsDouble();
            req.longt = stop_json.at("longitude"s).AsDouble();
            if (const auto distances_it = stop_json.find("road_distances"s);
                distances_it != stop_json.end()) {
                const auto& neighbors_json = stop_json.at("road_distances"s).AsMap();
                detail::StopDistances distances = { req.name, {} };
                for (const auto& [name, distance] : neighbors_json) {
                    distances.distances.emplace(name, distance.AsInt());
                }
                dist.emplace_back(std::move(distances));
            }
            return req;
        }

        uint8_t ReadByte(const json::Node& json) {
            int int_value = json.AsInt();
            uint8_t byte = static_cast<uint8_t>(int_value);
            if (static_cast<int>(byte) == int_value) {
                return byte;
            }
            throw std::out_of_range(std::to_string(int_value) + " is out of byte range"s);
        }

        svg::Color ReadColor(const json::Node& json) {
            if (json.IsArray()) {
                const auto& arr = json.AsArray();
                if (arr.size() == 3) {  // Rgb
                    return svg::Rgb(ReadByte(arr[0]), ReadByte(arr[1]), ReadByte(arr[2]));
                }
                else if (arr.size() == 4) {  // Rgba
                    return svg::Rgba(ReadByte(arr[0]), ReadByte(arr[1]), ReadByte(arr[2]),
                        arr[3].AsDouble());
                }
            }
            else if (json.IsString()) {
                return json.AsString();
            }
            else if (json.IsNull()) {
                return svg::NoneColor;
            }
            throw  reader::InvalidRequestError("invalid color format");
        }

        svg::Point ReadPoint(const json::Array& json) {
            if (json.size() != 2) {
                throw std::invalid_argument("Point array must have exactly 2 elements");
            }
            return svg::Point{ json[0].AsDouble(), json[1].AsDouble() };
        }

        std::vector<svg::Color> ReadColors(const json::Array& json) {
            std::vector<svg::Color> colors;
            colors.reserve(json.size());

            for (const auto& item : json) {
                colors.emplace_back(ReadColor(item));
            }

            return colors;
        }

        // -------------

        struct BusRequest {
            std::string name;
            json::Node Execute(const service::RequestHandler& handler, const int id_) const {
                if (const auto bus_stat = handler.GetBusStat(name)) {
                    return json::Builder{}.StartDict().Key("stop_count").Value(json::Node::Value{ static_cast<int>(bus_stat->stops) })
                        .Key("unique_stop_count").Value(json::Node::Value{ static_cast<int>(bus_stat->uniq_stops) })
                        .Key("route_length").Value(json::Node::Value{ bus_stat->length })
                        .Key("curvature").Value(json::Node::Value{ bus_stat->curvature })
                        .Key("request_id").Value(json::Node::Value{ id_ }).EndDict().Build();
                }
                else {
                    return json::Builder{}.StartDict().Key("error_message").Value("not found"s)
                        .Key("request_id").Value(json::Node::Value{ id_ }).EndDict().Build();
                }
            }
        };

        struct StopRequest {
            std::string name;

            json::Node Execute(const service::RequestHandler& handler, const int id_) const {
                if (const auto buses = handler.GetBusesByStop(name)) {
                    if (buses->size() == 0) {
                        json::Builder builder;
                        builder.StartDict().Key("buses").StartArray();
                        builder.EndArray().Key("request_id").Value(json::Node::Value{ id_ }).EndDict();
                        return builder.Build();
                    }
                    else {
                        std::vector<transport_catalogue::BusPtr> bus_vector{ buses->begin(), buses->end() };
                        std::sort(bus_vector.begin(), bus_vector.end(), [](transport_catalogue::BusPtr lhs, transport_catalogue::BusPtr rhs) {
                            return lhs->B_name < rhs->B_name;
                            });
                        json::Builder builder;
                        builder.StartDict().Key("buses").StartArray();
                        for (const auto& bus : bus_vector) {
                            builder.Value(json::Node::Value{ bus->B_name });
                        }
                        builder.EndArray().Key("request_id").Value(json::Node::Value{ id_ }).EndDict();
                        return builder.Build();
                    }
                }
                else {
                    return json::Builder{}.StartDict().Key("error_message").Value("not found"s)
                        .Key("request_id").Value(json::Node::Value{ id_ }).EndDict().Build();
                }
            }
        };

        struct MapRequest {
            json::Node Execute(const service::RequestHandler& handler, const int id_) const {
                json::Builder builder_node;
                builder_node.StartDict()
                    .Key("request_id"s).Value(id_);
                builder_node.Key("map"s).Value(handler.RenderPrMap().description());
                return builder_node.EndDict().Build();
            }
        };

        struct RouteRequest {
            std::string from;
            std::string to;
            json::Node Execute(const service::RequestHandler& handler, const int id_) const {
                using namespace graph;
                json::Builder builder;
                builder.StartDict()
                    .Key("request_id"s).Value(json::Node::Value{ id_ });
                const auto& route_report = handler.GetReportRouter(from, to);
                if (route_report) {
                    builder.Key("total_time"s).Value(route_report->total_minutes)
                        .Key("items"s).StartArray();
                    for (const auto& info : route_report->information) {
                        if (!info.wait.stop_name.empty()) {
                            builder.StartDict()
                                .Key("type"s).Value("Wait"s)
                                .Key("time"s).Value(info.wait.minutes)
                                .Key("stop_name"s).Value(info.wait.stop_name)
                                .EndDict();
                        }
                        if (!info.bus.number.empty()) {
                            builder.StartDict()
                                .Key("type"s).Value("Bus"s)
                                .Key("time"s).Value(info.bus.minutes)
                                .Key("bus"s).Value(std::string(info.bus.number))
                                .Key("span_count"s).Value(info.bus.span_count)
.EndDict();
                        }
                    }
                    builder.EndArray().EndDict();
                }
                else {
                builder.Key("error_message"s).Value("not found"s).EndDict();
                }
                return builder.Build();
            }
        };

        json::Node HandleRequest(const json::Dict& request_json, const service::RequestHandler& handler) {
            const std::string& type = request_json.at("type"s).AsString();
            if (type == "Bus"sv) {
                const auto request = BusRequest{ request_json.at("name"s).AsString() };
                auto response_json = request.Execute(handler, request_json.at("id"s).AsInt());
                return response_json;
            }
            else if (type == "Stop"sv) {
                const auto request = StopRequest{ request_json.at("name"s).AsString() };
                auto response_json = request.Execute(handler, request_json.at("id"s).AsInt());
                return response_json;
            }
            else if (type == "Map"sv) {
                const auto request = MapRequest{};
                auto response_json = request.Execute(handler, request_json.at("id"s).AsInt());
                return response_json;
            }
            else if (type == "Route"sv) {
                const auto request = RouteRequest{ request_json.at("from").AsString(), request_json.at("to").AsString() };
                auto response_json = request.Execute(handler, request_json.at("id"s).AsInt());
                return response_json;
            }
            else {
                throw reader::InvalidRequestError("Invalid request type "s + type);
            }
        }

    }  // namespace

    void JsonReader::LoadRouter()
    {
        tr_.MakeGraph();
    }

    void JsonReader::ReadTransportCatalogue(){ //transport_catalogue::TransportCatalogue& catalogue, const json::Array& base_requests_json) {
        std::vector<detail::Bus_to_Add> buses;
        std::vector<detail::StopDistances> dist;
        std::vector<std::string> buses_names;
        for (const auto& req_json : base_requests_) {
            const auto& props_json = req_json.AsMap();
            const std::string& type = props_json.at("type"s).AsString();
            if (type == "Bus"sv) {
                buses.push_back(ParsingBuses(props_json, buses_names));
            }
            else if (type == "Stop"sv) {
                Stop_to_Add s = ParsingStops(dist, props_json);
                tc_.AddStop(s.name, s.lat, s.longt);
            }
            else {
                throw reader::InvalidRequestError("Invalid type");
            }
        }
        AddingBuses(tc_, buses);
        AddingDistances(tc_, std::move(dist), std::move(buses_names));
    }

    renderer::RenderSettings JsonReader::ReadRenderSettings(const json::Dict& render_settings_json) const
    {
        renderer::RenderSettings rs;
        rs.palette = ReadColors(render_settings_json.at("color_palette"s).AsArray());

        rs.underlayer_width = render_settings_json.at("underlayer_width"s).AsDouble();
        rs.underlayer_color = ReadColor(render_settings_json.at("underlayer_color"s));

        rs.max_width = render_settings_json.at("width"s).AsDouble();
        rs.max_height = render_settings_json.at("height"s).AsDouble();
        rs.padding = render_settings_json.at("padding"s).AsDouble();

        rs.stop_radius = render_settings_json.at("stop_radius"s).AsDouble();
        rs.line_width = render_settings_json.at("line_width"s).AsDouble();

        rs.stop_label_offset = ReadPoint(render_settings_json.at("stop_label_offset"s).AsArray());
        rs.stop_label_font_size = render_settings_json.at("stop_label_font_size"s).AsInt();

        rs.bus_label_font_size = render_settings_json.at("bus_label_font_size").AsInt();
        rs.bus_label_offset = ReadPoint(render_settings_json.at("bus_label_offset").AsArray());
        return rs;
    }

    json::Array HandleRequests(const json::Array& requests_json,
        const service::RequestHandler& handler) {
        json::Array responses_json;
        responses_json.reserve(requests_json.size());
        for (const json::Node& request_json : requests_json) {
            responses_json.emplace_back(HandleRequest(request_json.AsMap(), handler));
        }
        return responses_json;
    }

    JsonReader::JsonReader(serialization::Serialization& ser, transport_catalogue::TransportCatalogue& tc,
        renderer::MapRenderer& map_ren, service::RequestHandler& req_hand, router::TransportRouter& tr) : 
        ser_(ser), tc_(tc), 
        map_ren_(map_ren), req_hand_(req_hand), tr_(tr)
    {}

    void JsonReader::ReadBase(std::istream & in)
    {
        const auto load = json::Load(in).GetRoot().AsMap();
        base_requests_ = load.at("base_requests"s).AsArray();
        if (load.count("serialization_settings"s)) {
            ser_.SetSetting(ReadSerializationSetting(load.at("serialization_settings"s).AsMap()));
        }
        if (load.count("render_settings")) {
            map_ren_.SetRenderSettings(ReadRenderSettings(load.at("render_settings").AsMap()));
        }
        if (load.count("routing_settings"s)) {
            tr_.SetSettings(ReadRouterSettings(load.at("routing_settings"s).AsMap()));
        }
        ReadTransportCatalogue();
        ser_.CreateBase();
    }

    void JsonReader::ReadRequests(std::istream& in)
    {
        const auto load = json::Load(in).GetRoot().AsMap();
        stat_request_ = load.at("stat_requests").AsArray();
        ser_.SetSetting(ReadSerializationSetting(load.at("serialization_settings"s).AsMap()));
        ser_.AccessBase();
        LoadRouter();
        
    }

    void JsonReader::Answer(std::ostream& out)
    {
        json::Array responses_json;
        responses_json.reserve(stat_request_.size());
        for (const json::Node& request_json : stat_request_) {
            responses_json.emplace_back(HandleRequest(request_json.AsMap(), req_hand_));
        }
        const json::Document report(responses_json);
        Print(report, out);
    }

    Settings JsonReader::ReadRouterSettings(const json::Dict& description) const
    {
        return { description.at("bus_wait_time"s).AsInt(),
                  description.at("bus_velocity"s).AsDouble() };
    }

    JsonReader::Path JsonReader::ReadSerializationSetting(const json::Dict& description) const
    {
        return Path(description.at("file"s).AsString());
    }
}  // namespace reader
