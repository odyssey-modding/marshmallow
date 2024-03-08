#include <string>

#define Name "Marshmallow"
constexpr const int NameLength = std::char_traits<char>::length(Name);

struct ModuleName {
    int unknown;
    int name_length;
    char name[NameLength + 1] = Name;
};

__attribute__((section(".nx-module-name")))
const struct ModuleName s_ModuleName = {.unknown = 0, .name_length = NameLength, .name = Name};
