#pragma once
#include <string>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <deque>
#include <iostream>
#include "geo.h"

namespace transport_catalogue {
	struct Stop;
	using StopPtr = const Stop*;

	struct Stop
	{
		std::string name = "";
		distance::Coordinates coordinates = { 0, 0 };
	};

	struct Bus {
		std::string B_name;
		std::deque<StopPtr> b_stops;
		int length_ = 0;
		double curvature_ = 0.0;
		std::vector<StopPtr> end_points_;
	};

	struct HashPairStopPtr {
	public:
		size_t operator() (const std::pair<StopPtr, StopPtr>& pair) const
		{
			size_t p_hash_first = p_hasher(pair.first);
			size_t p_hash_second = p_hasher(pair.second);
			return p_hash_first + p_hash_second * 29;
		}
		std::hash<const void*> p_hasher;
	};

	struct BusInfo {
		bool exists = false;
		int stops = 0;
		int uniq_stops = 0;
		int length = 0;
		double curvature = 0.0;
	};

	struct StopInfo {
		std::string about = "not found";
		std::set<std::string_view> stop_buses_;
	};
	using BusPtr = const Bus*;
}
namespace detail {
	struct Bus_to_Add {
		std::string name;
		std::vector<std::string> stops;
		bool is_round;
		std::vector<std::string> end_points_;
	};
	struct StopDistances {
		std::string stop;
		std::unordered_map<std::string, int> distances;
	};
}


struct Settings {
	int bus_wait_time = 1;
	double bus_velocity = 1;
};

struct Info {
	struct Wait {
		double minutes = 0;
		std::string stop_name;
	};

	struct Bus {
		std::string number;
		int span_count = 0;
		double minutes = 0.0;
	};

	Wait wait;
	Bus bus;
};

struct ReportRouter {
	std::vector<Info>  information;
	double             total_minutes = 0;
};
/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */