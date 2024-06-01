#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

std::string base64_encode(const std::string& input) {
    std::string output;
    std::string temp;
    int val = 0;
    int bits = 0;
    for (unsigned char c : input) {
        val = (val << 8) | c;
        bits += 8;
        while (bits >= 6) {
            temp.push_back(base64_chars[(val >> (bits - 6)) & 0x3F]);
            bits -= 6;
        }
    }
    if (bits > 0) {
        temp.push_back(base64_chars[((val << (6 - bits)) & 0x3F)]);
    }
    while (temp.size() % 4 != 0) {
        temp.push_back('=');
    }
    output = temp;
    return output;
}

std::string base64_decode(const std::string& input) {
    static const char padding_char = '=';

    size_t num_pad_chars = 0;
    for (char c : input) {
        if (c == padding_char) {
            ++num_pad_chars;
        }
    }

    if (num_pad_chars > input.size()) {
        throw std::runtime_error("Invalid base64 input with padding characters.");
    }

    std::string output;
    output.reserve(((input.size() / 4) * 3) - num_pad_chars);

    int val = 0;
    int valb = -8;
    bool in_data = false;

    for (unsigned char c : input) {
        if (c == padding_char) {
            continue;
        }

        int index = base64_chars.find(c);
        if (index < 0) {
            throw std::runtime_error("Non-base64 character found.");
        }

        val = (val << 6) + index;
        valb += 6;
        in_data = true;

        if (valb >= 0) {
            output.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
            if (valb >= 0) {
                output.push_back(char((val >> valb) & 0xFF));
                valb -= 8;
            }
            in_data = false;
        }
    }

    if (in_data) {
        throw std::runtime_error("Base64 input is not properly padded.");
    }

    return output;
}
//int main() {
//    std::string input = "Hello world!";
//    std::string out = base64_encode(input);
//    std::cout << "Encoded string: " << out << std::endl;//SGVsbG8sIFdvcmxkIQ==
//
//    std::string encoded = "";
//    std::string decoded = base64_decode(encoded);
//    std::cout << "Decoded string: " << decoded << std::endl;
//
//
//    return 0;
//}
/*
*



*/