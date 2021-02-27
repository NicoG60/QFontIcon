import json
import sys

import helpers

def main():
    icons = helpers.parse_bootstrap()

    helpers.generate('bootstrap', 'bs', icons, ['bootstrap'])
        
if __name__ == '__main__':
    main()
