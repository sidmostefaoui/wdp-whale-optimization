#pragma once

#include <boost/dynamic_bitset.hpp>
#include <random>
#include <vector>
#include <iostream>
#include "util.hpp"

namespace wdp {

    using Bitset = boost::dynamic_bitset<>;

    class Random {
        std::mt19937 generator;
        std::uniform_int_distribution<> dist_int;
        std::uniform_real_distribution<> dist_real;

    public:
        Random() : generator(std::random_device()()) {}
        
        void set_integer_bounds(int min, int max) {
            dist_int = std::uniform_int_distribution<>(min, max);
        }

        void set_real_bounds(double min, double max) {
            dist_real = std::uniform_real_distribution<>(min, max);
        }

        int integer() { 
            return dist_int(generator); 
        }

        double real() {
            return dist_real(generator);
        }
    };

    class Bundle {
    public:
        Bitset items;
        int price = 0;

        Bundle() {}

        void random(int n_items, auto& prices) {
            assert(n_items == prices.size());
            items = Bitset(n_items, 0);

            // set bits to random values
            auto r = Random();
            r.set_real_bounds(0.0, 1.0);

            static constexpr auto ITEM_PICKED_ODDS = 0.25; // odds for bidder to pick an item

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

        friend std::ostream& operator<<(std::ostream& os, const Bundle& b);
        friend auto generate_bundles(int n_biders, int n_items, auto& prices);
        friend class Subset;

        int get_price() { return price; }
        
    };


    std::ostream& operator<<(std::ostream& os, const Bundle& b)
    {
        os << b.items;
        return os;
    }

    auto generate_prices(int n_items) {
        auto prices = std::vector<int>();
        prices.reserve(n_items);

        auto r = Random();
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

    class Subset {
        Bitset bundles;
        int price = 0;

        Subset(size_t size, uint64_t bits, auto& full_set) : bundles(size, bits) {
            // calculate subset price
            for (int i = 0; i < size; ++i)
                if (bundles[i]) price += full_set[i].get_price();
        }

        Subset(size_t size, uint64_t bits) : bundles(size, bits) {
        }

        public:
            friend auto enumerate_bundle_subsets(auto& bundles);
            friend std::ostream& operator<<(std::ostream& os, const Subset& s);
            friend Subset find_best(auto& subsets, auto& full_set);

            // checks if bundles in subset don't have items in common
            bool is_feasible(auto& full_set) {
                for (size_t i = 0; i < bundles.size(); ++i)
                    for (size_t j = 0; j < bundles.size(); ++j) {
                        if (bundles[i] == false) continue;
                        if (bundles[j] == false) continue;
                        if (i == j) continue;
                        Bundle& a = full_set[i];
                        Bundle& b = full_set[j];
                        if (a.items.intersects(b.items)) return false;
                    }
                return true;
            }

        int get_price() { return price; }
    };

    std::ostream& operator<<(std::ostream& os, const Subset& s)
    {
        os << s.bundles;
        return os;
    }

    // https://math.stackexchange.com/a/349253
    auto enumerate_bundle_subsets(auto& bundles) {
        auto t = util::Timer("enumerate");

        auto n = bundles.size();
        auto subsets = std::vector<Subset>();
        subsets.reserve(n);

        uint64_t raw = 0;
        while (raw < std::pow(2, n)) {
            auto s = Subset(n, raw, bundles);
            subsets.emplace_back(s);
            raw += 1;
        }

        return subsets;
    }

    namespace solve {
        Subset brute_force(auto& bundles) {
            auto subsets = wdp::enumerate_bundle_subsets(bundles);

            auto t = util::Timer("brute force");

            Subset* best = nullptr;
            int best_price = 0;

            for (auto& s : subsets) {
                if (s.is_feasible(bundles) && s.get_price() > best_price) {
                    best = &s;
                    best_price = s.get_price();
                }
            }

            return *best;
        }

        struct Whale {
            uint64_t pos;
            uint64_t fitness;

            void calculate_fitness(std::vector<Bundle>& bundles) {
                auto subset = Bitset(bundles.size(), pos);

                auto is_feasible = [](Bitset subset, auto& bundles) {
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
                };

                auto calc_price = [](Bitset subset, std::vector<Bundle>& bundles) {
                    auto price = 0;
                    for (size_t i = 0; i < subset.size(); ++i)
                        if (subset[i]) price += bundles[i].get_price();
                    return price;
                };

                if (!is_feasible(subset, bundles)) fitness = 0;
                else fitness = calc_price(subset, bundles);
            }
        };

        auto init_population(const int sz_population, const int sz_search_space) {
            auto r = Random();
            r.set_integer_bounds(0, sz_search_space);

            std::vector<Whale> population;
            population.reserve(sz_population);

            for (int i = 0; i < sz_population; i++)
                population.emplace_back(static_cast<uint64_t>(r.integer()), 0);

            return population;
        }

        // https://en.wikiversity.org/wiki/Whale_Optimization_Algorithm
        std::pair<Bitset, int> whale_optimization(std::vector<Bundle>& bundles, int MAX_ITERATIONS) {
            auto timer = util::Timer("whale optimization");

            // Hyper params
            static constexpr auto SZ_POPULATION = 10;
            static constexpr auto A_DECREASE_COEFF = 0.0000000001;
            static constexpr auto B_CONSTANT = 0.000001;
            const uint64_t SZ_SEARCH_SPACE = static_cast<uint64_t>(std::pow(2, bundles.size()));

            auto population = init_population(SZ_POPULATION, SZ_SEARCH_SPACE);

            // random generator for r1, r2 and p (all of them between 0 and 1)
            auto rand = Random();
            rand.set_real_bounds(0.0, 1.0);

            // random generator for picking random whale
            auto rand2 = Random();
            rand2.set_integer_bounds(0, population.size());

            // random generator for t
            auto rand3 = Random();
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

            return { Bitset(bundles.size(), best.pos), best.fitness};
        }

    }
}