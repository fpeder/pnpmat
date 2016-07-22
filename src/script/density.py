#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
usage: ./density.py <img>
"""

import struct

MAGIC = 666

def main(fil):
    with open(fil, 'rb') as f:
        magic = struct.unpack('1i', f.read(4))[0]
        assert magic == MAGIC
        H, W = struct.unpack('2i', f.read(8))
        count = 0
        for i in range(H*W):
            count += struct.unpack('1b', f.read(1))[0]
    return float(count)/H/W


if __name__ == '__main__':
    from docopt import docopt

    args = docopt(__doc__)
    print main(args['<img>'])

