//
// Created by Isaiah on 22/09/2026.
//
#include <cstdint>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

auto NEXT_ID = 0;
 
struct ExpendibleObject {
    std::uint32_t id;
    std::string name;
    std::int32_t value;
    bool alive;

};

[[nodiscard]] auto new_(std::string name, int32_t val) noexcept -> ExpendibleObject {
    return ExpendibleObject {
        .id = std::uint32_t(NEXT_ID++), 
        .name = name,
        .value = val, 
        .alive = true
    };
}

[[nodiscard]] auto to_bytes(ExpendibleObject& Eo) noexcept -> std::vector<std::uint8_t> {
    auto name_data = reinterpret_cast<const std::uint8_t*>(Eo.name.data()); //<-- This needs the & i think cause std::string is template class idk
    auto name_len = static_cast<uint8_t>(Eo.name.length());
    std::vector<uint8_t> bytes;

    auto id = reinterpret_cast<std::uint8_t*>(&Eo.id);
    auto value = reinterpret_cast<std::uint8_t*>(&Eo.value);
    auto the_bool = reinterpret_cast<std::uint8_t*>(&Eo.alive);

    for (int i = 0; i < sizeof(std::uint32_t); ++i) {
        bytes.push_back(*(id + i));
    }
    
    bytes.push_back(name_len);

    for (uint32_t i = 0; i < Eo.name.length(); ++i) {
        bytes.push_back(*(name_data + i));
    }

    for (int i = 0; i < sizeof(std::int32_t); ++i) {
        bytes.push_back(*(value + i));
    }

    for (int i = 0; i < sizeof(bool); ++i) {
        bytes.push_back(*(the_bool + i));
    }

    return bytes;
}

[[nodiscard]] auto from_bytes(std::vector<uint8_t> bytes) noexcept -> ExpendibleObject {

    auto id = *reinterpret_cast<std::uint32_t*>(bytes.data());

    auto name_len = bytes[4];

    std::vector<char> name_buf;
    for (char i = 0; i < name_len; ++i) {
        name_buf.push_back(*reinterpret_cast<char*>(bytes.data() + 5 + i));
    }

    auto name = std::string{name_buf.data(), name_len};
    auto name_start = 5;
    auto name_end = name_start + name_len;

    auto value_start = name_start + name_len;

    auto value = *reinterpret_cast<std::int32_t*>(bytes.data() + value_start);
    auto the_bool  = *reinterpret_cast<bool*>(bytes.data() + value_start + 4);

    return ExpendibleObject{id, name, value, the_bool};
}

int main() {
    WSADATA wsadata; // WSAStartup writes into this.
    auto init = WSAStartup(MAKEWORD(2, 2), &wsadata); // gives it a version, and writes data into WSADATA.

    if (init != 0) {
        std::cout << "Failed to Initialized" << std::endl;
        return -1;
    }


    // IPV4 ADDRESSES
    // UDP PORTS (SOCKET DATAGRAM)
    // Default Protocol
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == INVALID_SOCKET) {
        std::cout << "Failed to Initialized/Find Socket" << std::endl;
        return -1;
    }

    sockaddr_in destination {};
    destination.sin_family = AF_INET; // IPV4
    destination.sin_port = htons(7878); // port 7878
    
    // Converts the Human-Readable text into the expected 32-bit format.
    auto pton = inet_pton(AF_INET, "127.0.0.1", &destination.sin_addr);

    if (pton != 1) {
        std::cout << "Failed to convert IP address" << std::endl;
        return -1;
    }


    auto exObj = new_("Name", 12);
    auto exObj_bytes = to_bytes(exObj);

    int sent_code = sendto(sock, reinterpret_cast<const char*>(exObj_bytes.data()), exObj_bytes.size(), 0, reinterpret_cast<sockaddr*>(&destination), sizeof(destination));

    if (sent_code == SOCKET_ERROR) {
        std::cout << "Failed to sendto" << std::endl;
        return -1;
    }

    WSACleanup();
}
