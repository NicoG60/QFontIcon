import requests
import json
import sys
import os

cpp_keywords = [
    'alignas',
    'alignof',
    'and',
    'and_eq',
    'asm',
    'atomic_cancel',
    'atomic_commit',
    'atomic_noexcept',
    'auto',
    'bitand',
    'bitor',
    'bool',
    'break',
    'case',
    'catch',
    'char',
    'char8_t',
    'char16_t',
    'char32_t',
    'class',
    'compl',
    'concept',
    'const',
    'consteval',
    'constexpr',
    'constinit',
    'const_cast',
    'continue',
    'co_await',
    'co_return',
    'co_yield',
    'decltype',
    'default',
    'delete',
    'do',
    'double',
    'dynamic_cast',
    'else',
    'enum',
    'explicit',
    'export',
    'extern',
    'false',
    'float',
    'for',
    'friend',
    'goto',
    'if',
    'inline',
    'int',
    'long',
    'mutable',
    'namespace',
    'new',
    'noexcept',
    'not',
    'not_eq',
    'nullptr',
    'operator',
    'or',
    'or_eq',
    'private',
    'protected',
    'public',
    'reflexpr',
    'register',
    'reinterpret_cast',
    'requires',
    'return',
    'short',
    'signed',
    'sizeof',
    'static',
    'static_assert',
    'static_cast',
    'struct',
    'switch',
    'synchronized',
    'template',
    'this',
    'thread_local',
    'throw',
    'true',
    'try',
    'typedef',
    'typeid',
    'typename',
    'union',
    'unsigned',
    'using',
    'virtual',
    'void',
    'volatile',
    'wchar_t',
    'while',
    'xor',
    'xor_eq',
    'final',
    'override',
    'transaction_safe',
    'transaction_safe_dynamic',
    'import',
    'module',
    'if',
    'elif',
    'else',
    'endif',
    'ifdef',
    'ifndef',
    'define',
    'undef',
    'include',
    'line',
    'error',
    'pragma',
    'defined',
    '__has_include',
    '__has_cpp_attribute',
    'export',
    'import',
    'module ',
    '_Pragma ',
    'linux'
]

def main():

    json_file = {}

    if len(sys.argv) == 2:
        with open(sys.argv[1], 'r') as data:
            json_file = json.load(data)
    else:
        # === Extract icons data directly from the source
        json_file = requests.get("https://raw.githubusercontent.com/FortAwesome/Font-Awesome/master/metadata/icons.json").json()

    icons = []
    max_len = 0

    for key, value in json_file.items():

        # sanitize name
        prefix = ''
        if key[0].isdigit() or key in cpp_keywords:
            prefix = '_'

        name = '{}{}'.format(prefix, key.replace('-', '_'))

        max_len = max(max_len, len(name))

        # get unicode
        code = '0x{}'.format(value["unicode"])

        # save
        icons.append((name, code))

    icons.sort(key=lambda e: e[0])

    with open('awesome.h', 'w') as file:
        file.write('#ifndef AWESOME_H\n')
        file.write('#define AWESOME_H\n\n')
        file.write('/**\n')
        file.write(' * This file has been automatically generated.\n')
        file.write(' */\n\n')
        file.write('namespace fa {\n\n')
        file.write('enum fonts { solid, regular, light, duotone, brands };\n\n')
        file.write('enum icons {\n')
        for name, code in icons:
            line = '    {name:<{len}} = {code},\n'.format(name=name, code=code, len=max_len)
            file.write(line)
        file.write('};\n')
        file.write('}\n\n')
        file.write('#endif // AWESOME_H\n')
        
if __name__ == '__main__':
    main()
