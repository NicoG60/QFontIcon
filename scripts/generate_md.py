import json
import sys

import helpers

def main():

    variant = [
        ('regular',  ''),
        ('outlined', 'Outlined'),
        ('round',    'Round'),
        ('sharp',    'Sharp'),
        ('twotone',  'TwoTone')
    ]

    for name, filename in variant:
        icons = helpers.parse_material(filename)

        helpers.generate('material_' + name, 'md::' + name, icons, ['font'])
        
if __name__ == '__main__':
    main()
