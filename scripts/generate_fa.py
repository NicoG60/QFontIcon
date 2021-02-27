import json
import sys

import helpers

def main():

    json_file = None

    if len(sys.argv) == 2:
        with open(sys.argv[1], 'r') as data:
            json_file = json.load(data)
    
    icons = helpers.parse_awesome(json_file)

    helpers.generate('awesome', 'fa', icons, ['solid', 'regular', 'light', 'brands', 'duotone'])
        
if __name__ == '__main__':
    main()
