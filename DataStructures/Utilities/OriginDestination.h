#pragma once

#include <cassert>
#include <string>
#include <vector>

#include <csv.h>

#include "Tools/Constants.h"

// An origin-destination (OD) pair, representing a travel demand or a query.
struct OriginDestination {
	// Constructs an OD-pair from o to d.
	OriginDestination(const int o, const int d, const int v)
		: origin(o), destination(d), volume(v) {}

	// Compares this OD-pair with rhs lexicographically.
	bool operator<(const OriginDestination& rhs) const {
		return origin < rhs.origin || (origin == rhs.origin && destination < rhs.destination);
	}

	int origin;
	int destination;
	int volume;
};

// An origin-destination (OD) pair that additionally stores an origin zone and a destination zone.
// Zones or traffic cells represent for example residential or commercial areas.
struct ClusteredOriginDestination : public OriginDestination {
	// Constructs a clustered OD-pair from o to d.
	ClusteredOriginDestination(const int o, const int d, const int oZone, const int dZone, const int v)
		: OriginDestination(o, d, v), originZone(oZone), destinationZone(dZone) {}

	// Compares this clustered OD-pair with rhs lexicographically.
	bool operator<(const ClusteredOriginDestination& rhs) const {
		if (originZone < rhs.originZone)
			return true;
		if (originZone > rhs.originZone)
			return false;
		if (destinationZone < rhs.destinationZone)
			return true;
		if (destinationZone > rhs.destinationZone)
			return false;
		return OriginDestination::operator<(rhs);
	}

	// Returns true if the OD-pair has the same origin and destination zone as the specified one.
	bool hasSameZones(const ClusteredOriginDestination& other) const {
		return originZone == other.originZone && destinationZone == other.destinationZone;
	}

	int originZone;
	int destinationZone;
};

// Reads the specified file into a vector of OD-pairs.
std::vector<OriginDestination> importODPairsFrom(const std::string& infile) {
	std::vector<OriginDestination> pairs;
	int origin, destination, volume;
	using TrimPolicy = io::trim_chars<>;
	using QuotePolicy = io::no_quote_escape<','>;
	using OverflowPolicy = io::throw_on_overflow;
	using CommentPolicy = io::single_line_comment<'#'>;
	io::CSVReader<3, TrimPolicy, QuotePolicy, OverflowPolicy, CommentPolicy> in(infile);
	in.read_header(io::ignore_extra_column, "origin", "destination", "volume");
	while (in.read_row(origin, destination, volume)) {
		assert(origin >= 0);
		assert(destination >= 0);
		assert(volume >= 0);
		pairs.emplace_back(origin, destination, volume);
	}
	return pairs;
}

// Reads the specified file into a vector of clustered OD-pairs.
std::vector<ClusteredOriginDestination> importClusteredODPairsFrom(const std::string& infile) {
	std::vector<ClusteredOriginDestination> pairs;
	int origin, destination, volume, originZone = INVALID_ID, destinationZone = INVALID_ID;
	using TrimPolicy = io::trim_chars<>;
	using QuotePolicy = io::no_quote_escape<','>;
	using OverflowPolicy = io::throw_on_overflow;
	using CommentPolicy = io::single_line_comment<'#'>;
	io::CSVReader<5, TrimPolicy, QuotePolicy, OverflowPolicy, CommentPolicy> in(infile);
	const io::ignore_column ignore = io::ignore_extra_column | io::ignore_missing_column;
	in.read_header(ignore, "origin", "destination", "origin_zone", "destination_zone", "volume");
	while (in.read_row(origin, destination, originZone, destinationZone, volume)) {
		assert(origin >= 0);
		assert(destination >= 0);
		pairs.emplace_back(origin, destination, originZone, destinationZone, volume);
	}
	return pairs;
}
