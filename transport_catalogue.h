#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <deque>
#include <iostream>
#include <vector>
#include <execution>
#include "geo.h"

namespace transport_catalogue {
	struct Stop
	{
		std::string name = "";
		distance::Coordinates coordinates = { 0, 0 };
		std::unordered_map<Stop*, int> distances;
	};

	struct Bus {
		std::string B_name;
		std::deque<Stop> b_stops;
	};

	struct BInfo {
		bool exists = false;
		size_t stops = 0;
		size_t uniq_stops = 0;
		int length = 0;
		double curvature = 0.0;
	};

	struct SInfo {
		std::string about = "not found";
		std::vector<std::string_view> stopbuses_;
	};

	class TransportCatalogue
	{
	public:
		void AddStop(const std::string_view stop, double lat, double longt);
		void AddBus(const std::string_view bus, std::vector<std::string_view> stops);
		void AddDistances(const std::string_view stop1, const std::string_view stop2, const int distance);
		[[nodiscard]] inline int GetDistances(const std::string_view stop1, const std::string_view stop2) const;
		[[nodiscard]] inline Bus& FindBus(const std::string_view bus) const;
		[[nodiscard]] inline Stop& FindStop(const std::string_view stop) const;
		[[nodiscard]] BInfo BusInfo(const std::string_view bus) const noexcept;
		[[nodiscard]] SInfo StopInfo(const std::string_view stop) const noexcept;
	private:
		std::unordered_map<std::string_view, Stop> stops_;
		std::unordered_map<std::string_view, Bus> buses_;
		std::unordered_map<std::string_view, std::unordered_set<std::string_view>> stop_to_buses_;
	};
}
