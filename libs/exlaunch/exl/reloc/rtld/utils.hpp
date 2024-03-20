#pragma once

extern "C" unsigned long __rtld_elf_hash(const char *name);

extern "C" void print_unresolved_symbol(const char *name);
