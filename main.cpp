#include <iostream>
#include <cassert>
#include "wdp.hpp"

/* TODO 
    - implement Subset::is_feasible()
*/
int main()
{
    int n_items = 04;
    int n_biders = 10;

    auto prices = wdp::generate_prices(n_items);
    auto bundles = wdp::generate_bundles(n_biders, n_items, prices);
    auto subsets = wdp::enumerate_bundle_subsets(bundles);
    auto best = wdp::find_best(subsets);

    std::cout << "N ITEMS = " << n_items << " \n";
    std::cout << "N BIDERS = " << n_biders << " \n";
    std::cout << "N SUBSETS = " << subsets.size() << "\n";


    std::cout << "BEST = " << best << "\n";
}