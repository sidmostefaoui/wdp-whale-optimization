#include <iostream>
#include <cassert>
#include "wdp.hpp"
#include "ui.hpp"


int main()
{
    int n_items = 10;
    int n_biders = 10;

    auto prices = wdp::generate_prices(n_items);
    auto bundles = wdp::generate_bundles(n_biders, n_items, prices);
    auto best = wdp::solve::brute_force(bundles);
    auto [bitset, price] = wdp::solve::whale_optimization(bundles, 1000000);

    std::cout << "best brute: " << best << " " << best.get_price() << "$\n";
    std::cout << "best woa: " << bitset << " " << price << "$\n";
    
    ui::draw();
}