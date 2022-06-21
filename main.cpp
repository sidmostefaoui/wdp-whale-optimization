#include <iostream>
#include <cassert>
#include "wdp.hpp"


int64_t memory = 0;

void* operator new(size_t size) {
    memory += size;
    return malloc(size);
}


int main() {

    int n_items = 20;
    int n_biders = 15;

    auto prices = wdp::generate_prices(n_items);
    auto bundles = wdp::generate_bundles(n_biders, n_items, prices);

    auto [best_brute, brute_price, brute_ms] = wdp::solve::brute_force(bundles);
    std::cout << "best brute: " << best_brute << " " << brute_price << "$ " << brute_ms << " ms "
              << memory / (1024 * 1024) << " mb\n";

    memory = 0;

    auto [best_woa, woa_price, woa_ms] = wdp::solve::whale_optimization(bundles, 300000);
    std::cout << "best woa:   " << best_woa << " " << woa_price << "$ " << woa_ms << " ms "
              << memory / (1024 * 1024) << " mb\n";

   
}