import requests

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

    'linux',
    'bootstrap'
]

def parse_awesome(json_data = None):
    if json_data is None:
        json_data = requests.get("https://raw.githubusercontent.com/FortAwesome/Font-Awesome/master/metadata/icons.json").json()

    icons = []

    for key, value in json_data.items():

        # sanitize name
        prefix = ''
        if key[0].isdigit() or key in cpp_keywords:
            prefix = '_'

        name = '{}{}'.format(prefix, key.replace('-', '_'))

        # get unicode
        code = '0x{}'.format(value["unicode"])

        # save
        icons.append((key, name, code))

    icons.sort(key=lambda e: e[0])

    return icons

def parse_bootstrap():

    json_data = requests.get("https://raw.githubusercontent.com/twbs/icons/main/font/bootstrap-icons.json").json()

    icons = []

    for key, value in json_data.items():

        # sanitize name
        prefix = ''
        if key[0].isdigit() or key in cpp_keywords:
            prefix = '_'

        name = '{}{}'.format(prefix, key.replace('-', '_'))

        # get unicode
        code = hex(value)

        # save
        icons.append((key, name, code))

    icons.sort(key=lambda e: e[0])

    return icons


def parse_material(filename):
    url = 'https://raw.githubusercontent.com/google/material-design-icons/master/font/MaterialIcons{}-Regular.codepoints'.format(filename)
    req = requests.get(url)
    lines = req.text.split('\n')

    icons = []

    for line in lines:
        if len(line) == 0:
            continue

        key, code = tuple(line.split())

        prefix = ''
        if key[0].isdigit() or key in cpp_keywords:
            prefix = '_'

        name = '{}{}'.format(prefix, key.replace('-', '_'))

        # get unicode
        code = '0x' + code

        item = (key, name, code)

        # save
        icons.append((key, name, code))

    return icons


def generate(file_name,
             namespace,
             icons,
             fonts = []):

    with open('{}.h'.format(file_name), 'w') as file:
        file.write('#ifndef {}_H\n'.format(file_name.upper()))
        file.write('#define {}_H\n\n'.format(file_name.upper()))

        file.write('/**\n'
                   ' * This file has been automatically generated.\n'
                   ' */\n\n')
        
        file.write('namespace ' + namespace + ' {\n\n')

        file.write('namespace fonts {\nenum {\n    ')
        file.write(',\n    '.join(fonts))
        file.write('\n};\n}\n\n')

        file.write('namespace icons {\nenum {\n    ')

        def format_icon(key, name, code):
            return '{} = {}'.format(name, code)

        file.write(',\n    '.join([format_icon(key, name, code) for key, name, code in icons]))
    
        file.write('\n};\n}\n\n')

        file.write('bool register_{}_names();\n'.format(file_name))

        file.write('}\n\n#endif // ' + file_name.upper() + '_H\n')

    with open('{}.cpp'.format(file_name), 'w') as file:
        file.write('#include <{}.h>\n'.format(file_name))
        file.write('#include <qfonticon.h>\n\n')

        file.write('/**\n'
                   ' * This file has been automatically generated.\n'
                   ' */\n\n')

        file.write('namespace ' + namespace + ' {\n\n')

        file.write('bool register_awesome_names()\n'
                    '{\n'
                    '    bool r = true;\n\n'

                    '    r &= QFontIconEngine::registerFontName({\n        ')

        def format_font(f):
            return '{ QStringLiteral("' + f + '"), fonts::' + f + ' }'

        file.write(',\n        '.join([format_font(f) for f in fonts]))

        file.write('\n    });\n\n'

                   '    r &= QFontIconEngine::registerIconName({\n        ')
          
        def format_icon(key, name):
            return '{ QStringLiteral("' + key + '"), icons::cd scr' + name + ' }'

        file.write(',\n        '.join([format_icon(key, name) for key, name, code in icons]))

        file.write(('\n    });\n\n'

                    '    return r;'
                    '}\n'
                    '}\n'))
           