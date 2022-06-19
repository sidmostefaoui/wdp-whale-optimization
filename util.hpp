#pragma once

#include<chrono>
#include<string>
#include<iostream>


namespace util {
    class Timer {
        std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
        std::string m_Text;

    public:
        Timer(const std::string& text) : m_Text(text) {
            m_Start = std::chrono::high_resolution_clock::now();
        }

        ~Timer() {
            auto end = std::chrono::high_resolution_clock::now();
            auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_Start).count();
            auto us = ns * 0.001;
            auto ms = us * 0.001;
            std::cout << m_Text << ":\n" << "ns: "
                << ns << " us: " << us << " ms: " << ms << std::endl;
        }
    };
}