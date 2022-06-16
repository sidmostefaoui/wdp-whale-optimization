#pragma once
#include <random>
#include <vector>

namespace wdp {
    class Random {
        std::mt19937 generator;
        std::uniform_int_distribution<int> dist;

    public:
        Random(int min, int max) : generator(std::random_device()()), dist(min, max + 1) {}

        int next() { return dist(generator); }
    };

    class Bundle {
    public:
        int price = 0;
        std::vector<bool> items;

    private:
        Bundle() {}

        void random(int n_items, auto& prices) {
            assert(n_items == prices.size());

            items.reserve(n_items);

            auto r = Random(0, 1);
            for (size_t i = 0; i < items.size(); i++)
                items[i] = r.next();

            for (size_t i = 0; i < items.size(); i++)
                if (items[i]) price += prices[i];
        }

        friend auto generate_bundles(int n_biders, int n_items, auto& prices);
    };

    auto generate_prices(int n_items) {
        std::vector<int> prices;
        prices.reserve(n_items);

        auto r = Random(0, 1000);
        for (int& p : prices)
            p = r.next();

        return prices;
    }

    auto generate_bundles(int n_biders, int n_items, auto& prices) {
        std::vector<Bundle> bundles;
        bundles.reserve(n_biders);

        for (auto& b : bundles)
            b.random(n_items, prices);

        return bundles;
    }

    class Subset {
        std::vector<bool> bundles;
        int price = 0;


    };


    // implement one of these algorithms
    // https://math.stackexchange.com/a/349225
    // https://math.stackexchange.com/a/349253
    // https://math.stackexchange.com/a/89453
    auto enumerate_subsets(auto& bundles) {
    }

}
