#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <deque>
#include <iostream>
#include <optional>
#include <vector>
#include "domain.h"
#include "geo.h"

namespace transport_catalogue {
	class TransportCatalogue
	{
	public:
		void AddStop(const std::string_view stop, double lat, double longt);
		void AddBus(const std::string_view bus, std::vector<std::string_view> stops, std::vector<std::string> end_points);
		void AddDistances(const std::string_view stop1, const std::string_view stop2, const int distance);
		void AddLenth(const std::string_view bus);
		[[nodiscard]] inline int GetDistances(const std::string_view stop1, const std::string_view stop2) const;
		[[nodiscard]] Bus* FindBus(const std::string_view bus) const;
		[[nodiscard]] Stop* FindStop(const std::string_view stop) const;
		[[nodiscard]] std::vector<Bus> GetBuses() const;
		[[nodiscard]] inline std::vector<Stop> GetStops() const;
		[[nodiscard]] std::unordered_set<Bus*> GetBusesByStop(std::string_view stop_name) const;
		[[nodiscard]] BusInfo GetBusInfo(const std::string_view bus) const noexcept;
		[[nodiscard]] StopInfo GetStopInfo(const std::string_view stop) const noexcept;
	private:
		std::unordered_map<std::string_view, Stop> stops_;
		std::unordered_map<std::string_view, Bus> buses_;
		std::unordered_map<std::string_view, std::set<std::string_view>> stop_to_buses_;
	};
}
