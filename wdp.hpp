#pragma once

#include <boost/dynamic_bitset.hpp>
#include <random>
#include <vector>
#include <iostream>
#include "util.hpp"

namespace wdp {

using Bitset = boost::dynamic_bitset<>;

class Bundle {
public:
    Bitset items;
    int price = 0;

    Bundle() {}

    void random(int n_items, auto& prices) {
        assert(n_items == prices.size());
        items = Bitset(n_items, 0);

        // set bits to random values
        auto r = util::Random();
        r.set_real_bounds(0.0, 1.0);

        // odds for bidder to pick an item
        static constexpr auto ITEM_PICKED_ODDS = 0.25; 

        for (size_t i = 0; i < n_items; ++i) {
            auto chance = r.real();
            if (chance <= ITEM_PICKED_ODDS)
                items[i] = true;
            else 
                items[i] = false;
        }

        // calculate total bundle price
        for (size_t i = 0; i < n_items; ++i)
            if (items[i]) price += prices[i];
    };

    friend auto generate_bundles(int n_biders, int n_items, auto& prices);

    int get_price() { return price; }
        
};

auto generate_prices(int n_items) {
    auto prices = std::vector<int>();
    prices.reserve(n_items);

    auto r = util::Random();
    r.set_integer_bounds(10, 1000);
    for (size_t i = 0; i < n_items; i++)
        prices.emplace_back(r.integer());

    return prices;
}

auto generate_bundles(int n_biders, int n_items, auto& prices) {
    auto bundles = std::vector<Bundle>();
    bundles.reserve(n_biders);

    for (size_t i = 0; i < n_biders; i++) {
        auto b = Bundle();
        b.random(n_items, prices);
        bundles.emplace_back(b);
    }

    return bundles;
}

namespace solve {

// https://math.stackexchange.com/a/349253
auto enumerate_bundle_subsets(auto& bundles) {
    const auto SIZE_SEARCH_SPACE = std::pow(2, bundles.size());

    auto subsets = std::vector<std::pair<Bitset, uint64_t>>();
    subsets.reserve(SIZE_SEARCH_SPACE);

    for (uint64_t raw = 0; raw < SIZE_SEARCH_SPACE; ++raw) {
        auto subset = Bitset(bundles.size(), raw);

        // calculate price
        uint64_t price = 0;
        for (int i = 0; i < subset.size(); ++i)
            if (subset[i]) price += bundles[i].get_price();

        subsets.emplace_back(subset, price);
    }

    return subsets;
}

// checks if bundles in subset don't have items in common
bool subset_is_feasible(Bitset subset, auto& bundles) {
    for (size_t i = 0; i < subset.size(); ++i)
        for (size_t j = 0; j < subset.size(); ++j) {
            if (subset[i] == false) continue;
            if (subset[j] == false) continue;
            if (i == j) continue;
            Bundle& a = bundles[i];
            Bundle& b = bundles[j];
            if (a.items.intersects(b.items)) return false;
        }
    return true;
}

auto brute_force(auto& bundles) {
    auto& t = util::Timer().start();

    auto subsets_and_prices = enumerate_bundle_subsets(bundles);

    // find best
    auto* best_subset = &subsets_and_prices[0].first;
    auto* best_price = &subsets_and_prices[0].second;
    for (auto& [subset, price] : subsets_and_prices)
        if (subset_is_feasible(subset, bundles) && price > *best_price) {
            best_subset = &subset;
            best_price = &price;
        }

    t.end();

    return std::tuple(*best_subset, *best_price, t.ms());
}

struct Whale {
    uint64_t pos;
    uint64_t fitness;

    void calculate_fitness(std::vector<Bundle>& bundles) {
        auto subset = Bitset(bundles.size(), pos);

        static auto calc_price = [&]() {
            auto price = 0;
            for (size_t i = 0; i < subset.size(); ++i)
                if (subset[i]) price += bundles[i].get_price();
            return price;
        };

        if (!subset_is_feasible(subset, bundles)) fitness = 0;
        else fitness = calc_price();
    }
};

auto init_population(const int sz_population, const int sz_search_space) {
    auto r = util::Random();
    r.set_integer_bounds(0, sz_search_space);

    std::vector<Whale> population;
    population.reserve(sz_population);

    for (int i = 0; i < sz_population; i++)
        population.emplace_back(static_cast<uint64_t>(r.integer()), 0);

    return population;
}

// https://en.wikiversity.org/wiki/Whale_Optimization_Algorithm
auto whale_optimization(std::vector<Bundle>& bundles, int MAX_ITERATIONS) {
    auto& timer = util::Timer().start();

    // Hyper params
    static constexpr auto SZ_POPULATION = 20;
    static constexpr auto A_DECREASE_COEFF = 0.0000000001;
    static constexpr auto B_CONSTANT = 0.000001;
    const uint64_t SZ_SEARCH_SPACE = std::pow(2, bundles.size());

    auto population = init_population(SZ_POPULATION, SZ_SEARCH_SPACE);

    // random generator for r1, r2 and p (all of them between 0 and 1)
    auto rand = util::Random();
    rand.set_real_bounds(0.0, 1.0);

    // random generator for picking random whale
    auto rand2 = util::Random();
    rand2.set_integer_bounds(0, population.size());

    // random generator for t
    auto rand3 = util::Random();
    rand3.set_real_bounds(-1, 1);
            
    // initializing coefficients
    auto r1 = rand.real();
    auto r2 = rand.real();

    auto iterations = 0;
    Whale best = { 0, 0 };

    while (iterations <= MAX_ITERATIONS) {
        for (auto& w : population)
            w.calculate_fitness(bundles);

        for (auto& w : population)
            if (w.fitness > best.fitness) best = w;

        // update coefficients
        auto p = rand.real();
        auto a = 2.0 - iterations * A_DECREASE_COEFF;
        auto A = (2 * a * r1) - a;
        auto C = 2 * r2;

        // update whale position;
        for (auto& w : population) {   
            auto& Xp = best.pos;
            auto& X = w.pos;
            if (p < 0.5) {
                if (std::abs(A) < 1) {
                    auto D = std::abs(C * Xp - X);
                    X = Xp - A * D;
                } else {
                    auto& X_rand = population[rand2.integer()].pos;
                    auto D = std::abs(C * X_rand - X);
                    X = X_rand - A * D;
                }
            }
            else {
                auto D = std::abs(static_cast<double>(Xp - X));
                auto t = rand3.real();
                auto b = B_CONSTANT;
                static constexpr double pi = 3.14159265358979323846;
                X = D * std::exp(t * b) * std::cos(pi * t) + Xp;
            }
        }

        // keep positions within search space
        for (auto& w : population)
            if (w.pos > SZ_SEARCH_SPACE) w.pos %= SZ_SEARCH_SPACE;

        iterations++;
    }
            
    timer.end();
    return std::tuple(Bitset(bundles.size(), best.pos), best.fitness, timer.ms());
}

}

}