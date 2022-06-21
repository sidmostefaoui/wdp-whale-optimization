#pragma once

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <tuple>
#include <utility>


namespace wdp {

	using Bitset = boost::dynamic_bitset<>;
	using Bundle = std::pair<Bitset, int64_t>;
	using SearchResult = std::tuple<Bitset, int64_t, double>;

	namespace solve {
		SearchResult brute_force(std::vector<Bundle>& bundles);
		SearchResult whale_optimization(std::vector<Bundle>& bundles, int MAX_ITERATIONS);
	}

	namespace generate {
		std::vector<Bundle> random_bundles(int n_biders, int n_items);
	}
}