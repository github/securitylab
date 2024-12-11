#pragma once

#include <random>
#include <filesystem>
#include <fstream>

inline uint32_t rand_uint32(uint32_t min_value, uint32_t max_value) {

    static std::random_device rd;
    static std::mt19937 gen(rd());

    uint32_t rand_number;

    std::uniform_int_distribution<> dist(min_value, max_value);

    rand_number = dist(gen);

    return rand_number;
}


inline std::string uint32_to_string(uint32_t fourcc){

    std::string output = "";
    
    output += fourcc & 0xFF;
    output += (fourcc >> 8) & 0xFF;
    output += (fourcc >> 16) & 0xFF;
    output += (fourcc >> 24) & 0xFF;
    
    return output;
}


inline std::string uint32_to_string_BE(uint32_t fourcc){

    std::string output = "";

    output += (fourcc >> 24) & 0xFF;
    output += (fourcc >> 16) & 0xFF;
    output += (fourcc >> 8) & 0xFF;
    output += fourcc & 0xFF;
    
    return output;
}


inline bool write_to_file(const std::string &content, std::filesystem::path file){

    std::ofstream ofs(file, std::ios::out | std::ios::binary);

    if (!ofs) {
      return false;
    }

    ofs << content;

    ofs.close();

    return true;
}