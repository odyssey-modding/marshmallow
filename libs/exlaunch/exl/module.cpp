#include <string>
#include <exl/lib.hpp>

#define Name "Marshmallow"
constexpr const int NameLength = std::char_traits<char>::length(Name);

struct ModuleName {
    u32 unknown;
    u32 name_length;
    char name[NameLength + 1] = Name;
};

__attribute__((section(".nx-module-name")))
extern const struct ModuleName s_ModuleName = {.unknown = 0, .name_length = NameLength, .name = Name};
