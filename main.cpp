// Ecom.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cassert>
#include "wdp.hpp"


int main()
{
    int n_items = 10;
    int n_biders = 10;

    auto prices = wdp::generate_prices(n_items);
    auto bundles = wdp::generate_bundles(n_biders, n_items, prices);
    auto subsets = wdp::enumerate_subsets(bundles);



    std::cout << "Hello World!\n";
}