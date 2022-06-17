#include <iostream>
#include <cassert>
#include "wdp.hpp"

/* TODO 
    - change random distribution in generating bundles
*/
int main()
{
    int n_items = 50;
    int n_biders = 22;

    auto prices = wdp::generate_prices(n_items);

    auto bundles = wdp::generate_bundles(n_biders, n_items, prices);

    auto subsets = wdp::enumerate_bundle_subsets(bundles);
    auto best = wdp::find_best(subsets, bundles);

    std::cout << "N ITEMS = " << n_items << " \n";
    std::cout << "N BIDERS = " << n_biders << " \n";
    std::cout << "N SUBSETS = " << subsets.size() << "\n";

   // for (auto& s : subsets)
     //   std::cout << s << " feasible: " << s.is_feasible(bundles) << " " << s.get_price() << "$\n";


    std::cout << "BEST = " << best << "\n";
}