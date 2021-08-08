#include "input_reader.h"
namespace detail {
    struct Bus_to_Add {
        std::string name;
        std::vector<std::string> stops;
    };

    struct StopDistances {
        std::string stop;
        std::unordered_map<std::string, int> distances;
    };
}
void input::FillCatalogue(transport_catalogue::TransportCatalogue& catalogue, std::istream& input)
{
    using namespace transport_catalogue;
    int request = 0;
    input >> request;
    std::vector<detail::Bus_to_Add> buses;
    std::vector<detail::StopDistances> dist;
    for (size_t i = 0; i < request; i++) {
        std::string req;
        input >> req;
        std::string line;
        std::getline(input, line);
        std::istringstream line_stream(std::move(line));
        if (req == "Bus") {
            std::string b_name;
            getline(line_stream, b_name, ':');
            b_name.erase(b_name.find_last_not_of(' ')+1);
            b_name.erase(0, b_name.find_first_not_of(' '));
            detail::Bus_to_Add b = { move(b_name), {} };
            std::string stops;
            getline(line_stream, stops);
            //ring < , dash - 
            char dash_or_ring = (stops.find('>') != std::string::npos) ? '>' : '-';
            auto start = stops.find(' ');
            while (true) {
                auto end = stops.find(dash_or_ring, start);
                auto stop = stops.substr(start, end - start);
                stop.erase(stop.find_last_not_of(' ') + 1);
                stop.erase(0, stop.find_first_not_of(' '));
                b.stops.emplace_back(move(stop));

                if (end == std::string::npos) {
                    break;
                }
                start = end + 1;
            }
            if (dash_or_ring == '-') {
                b.stops.reserve(b.stops.size() * 2 - 1);
                for (auto it = ++std::rbegin(b.stops); it != std::rend(b.stops); ++it) {
                    b.stops.emplace_back(*it);
                }
            }
            buses.emplace_back(std::move(b));
        }
        else if (req == "Stop") {
            std::string s_name, comma;
            getline(line_stream, s_name, ':');
            s_name.erase(s_name.find_last_not_of(' ') + 1);
            s_name.erase(0, s_name.find_first_not_of(' '));
            double lat, longt;
            line_stream >> lat >> comma >> longt;
            catalogue.AddStop(s_name, lat, longt);
            line_stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
            if (!line_stream.eof()) {
                detail::StopDistances distances = { std::move(s_name), {} };

                while (!line_stream.eof()) {
                    int distance;
                    line_stream >> distance;

                    {
                        std::string unused;
                        line_stream >> unused;
                        line_stream >> unused;
                    }

                    std::string s_name2;
                    getline(line_stream, s_name2, ',');
                    s_name2.erase(s_name2.find_last_not_of(' ') + 1);
                    s_name2.erase(0, s_name2.find_first_not_of(' '));
                    distances.distances.emplace(std::move(s_name2), distance);
                }

                dist.emplace_back(std::move(distances));
            }
        }
        else {
            throw std::invalid_argument("Invalid query");
        }
    }

    for (auto& bus : buses) {
        std::vector<std::string_view> stops(begin(bus.stops), end(bus.stops));
        catalogue.AddBus(std::move(bus.name), std::move(stops));
    }

    for (auto& stop_distance : dist) {
        for (auto& [s_name, distance] : stop_distance.distances) {
            catalogue.AddDistances(stop_distance.stop, s_name, distance);
        }
    }
}
