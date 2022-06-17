#pragma once

#include <boost/dynamic_bitset.hpp>
#include <random>
#include <vector>
#include <iostream>

namespace wdp {

    using Bitset = boost::dynamic_bitset<>;

    class Random {
        std::mt19937 generator;
        std::uniform_int_distribution<int> dist;

    public:
        Random(int min, int max) : generator(std::random_device()()), dist(min, max) {}

        int next() { return dist(generator); }
    };

    class Bundle {
        Bitset items;
        int price = 0;

        Bundle() {}

        void random(int n_items, auto& prices) {
            assert(n_items == prices.size());
            items = Bitset(n_items, 0);

            // set bits to random values
            auto r = Random(0, 1);
            for (size_t i = 0; i < n_items; ++i)
                items[i] = r.next();

            // calculate total bundle price
            for (size_t i = 0; i < n_items; ++i)
                if (items[i]) price += prices[i];
        };

        friend std::ostream& operator<<(std::ostream& os, const Bundle& b);
        friend auto generate_bundles(int n_biders, int n_items, auto& prices);

    public: 
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

        auto r = Random(10, 1000);
        for (size_t i = 0; i < n_items; i++)
            prices.emplace_back(r.next());

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

        Subset() : bundles(0) {}
        Subset(size_t size, uint64_t bits, auto& full_set) : bundles(size, bits) {
            // calculate subset price
            for (int i = 0; i < size; ++i)
                if (bundles[i]) price += full_set[i].get_price();
        }

        // implement this
        bool is_feasible() {
            for 
        }
        
        friend auto enumerate_bundle_subsets(auto& bundles);
        friend std::ostream& operator<<(std::ostream& os, const Subset& s);
        friend Subset find_best(auto& subsets);

    public:
        int get_price() { return price; }
    };

    std::ostream& operator<<(std::ostream& os, const Subset& s)
    {
        os << s.bundles;
        return os;
    }
    
    


    // implement one of these algorithms
    // https://math.stackexchange.com/a/349225
    // https://math.stackexchange.com/a/349253
    // https://math.stackexchange.com/a/89453
    auto enumerate_bundle_subsets(auto& bundles) {
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


    Subset find_best(auto& subsets) {
        Subset* best = nullptr;
        int best_price = 0;

        for (auto& s : subsets) {
            if (s.is_feasible() && s.get_price() > best_price) {
                best = &s;
                best_price = s.get_price();
            }
        }

        return *best;
    }
}