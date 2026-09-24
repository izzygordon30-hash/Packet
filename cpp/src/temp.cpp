#include <iostream>
#include <string>
#include <vector>
#include <cstdint>



struct ExpendibleObject {
    std::uint32_t id;
    std::string name;
    std::int32_t value;
    bool alive;
};

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


int main(void) {
    ExpendibleObject EObj{.id = 1, .name = "SUPERLONGNAMELIKEVERYLONG", .value = 12, .alive = true};

    auto bytes = to_bytes(EObj);
    auto reserved = from_bytes(bytes);
    
    std::cout << &EObj.value << std::endl;

    std::cout << reserved.id << std::endl;
    std::cout << reserved.name << std::endl;
    std::cout << reserved.value << std::endl;
    std::cout << (reserved.alive ? "true" : "false") << std::endl;

}
