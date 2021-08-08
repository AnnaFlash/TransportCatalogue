#include "stat_reader.h"

void output::OutpuInfo(transport_catalogue::TransportCatalogue& catalogue, std::istream& input, std::ostream& output)
{
	using namespace transport_catalogue;
	size_t request;
	input >> request;
	std::string req;
	getline(input, req);
	for (size_t i = 0; i < request; i++) {
		getline(input, req);
		if (req.find("Bus ") != std::string::npos) {
			auto b_name = req.substr(sizeof("Bus"));
			b_name.erase(b_name.find_last_not_of(' ') + 1);
			auto inf = catalogue.BusInfo(b_name);
			if (inf.exists == false) {
				output << "Bus " << b_name << ": not found" << std::endl;
			}
			else {
				output << "Bus " << b_name << ": " << inf.stops << " stops on route, " << inf.uniq_stops 
					<< " unique stops, " << inf.length << " route length, " << std::setprecision(6)
					<< inf.curvature << std::defaultfloat << " curvature" << std::endl;
			}
		}
		else if (req.find("Stop ") != std::string::npos) {
			auto s_name = req.substr(sizeof("Stop"));
			s_name.erase(s_name.find_last_not_of(' ') + 1);
			auto s_inf = catalogue.StopInfo(s_name);
			if (s_inf.about.find("not found") != std::string::npos) {
				output << "Stop " << s_name << ": not found" << std::endl;
			}
			else if (s_inf.about.find("no buses") != std::string::npos) {
				output << "Stop " << s_name << ": no buses" << std::endl;
			}
			else {
				output << "Stop " << s_name << ": buses";
				for (auto s : s_inf.stopbuses_) {
					output << " " << s;
				}
				output << std::endl;
			}
		}
		else {
			std::cerr << "incorrect request!";
		}
	}
}
