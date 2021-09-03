#pragma once
#include "transport_catalogue.h"
#include <iomanip>
namespace output {
	void OutputInfo(transport_catalogue::TransportCatalogue& catalogue, std::istream& input, std::ostream& output);
}