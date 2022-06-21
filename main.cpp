#include <iostream>
#include <cassert>
#include "wdp.hpp"

int main() {

    int n_items = 20;
    int n_biders = 20;

    auto bundles = wdp::generate::random_bundles(n_biders, n_items);

    auto [best_brute, brute_price, brute_ms] = wdp::solve::brute_force(bundles);
    std::cout << "best brute: " << best_brute << " " << brute_price << "$ " << brute_ms << " ms\n";


    auto [best_woa, woa_price, woa_ms] = wdp::solve::whale_optimization(bundles, 1000000);
    std::cout << "best woa:   " << best_woa << " " << woa_price << "$ " << woa_ms << " ms\n";

   
}