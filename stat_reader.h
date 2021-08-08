#pragma once
#include "transport_catalogue.h"
#include <iomanip>
namespace output {
	void OutpuInfo(transport_catalogue::TransportCatalogue& catalogue, std::istream& input, std::ostream& output);
}