#include "base64.hpp"

const std::string BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_char_to_value(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1; 
}

std::string base64_decode(const std::string& encoded) {
    std::string decoded;
    uint64_t val = 0;
    int valb = -8;
    for (std::string::const_iterator it = encoded.begin(); it != encoded.end(); ++it) {
        char c = *it;
        if (c == '=') break;
        int value = base64_char_to_value(c);
        if (value == -1) {
            throw std::invalid_argument("Invalid Base64 character");
        }
        val = (val << 6) + value;
        valb += 6;
        if (valb >= 0) {
            decoded.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return decoded;
}

