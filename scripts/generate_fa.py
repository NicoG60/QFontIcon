import requests
import json
import sys
import os

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
        if key[0].isdigit():
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
