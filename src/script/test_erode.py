#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
usage: ./test_erode.py <out>
"""

import numpy as np
import subprocess as sp
import os

NNTIMES = 5
BIN_ERODE = "./build/erode"
BIN_CLERODE = "./build/clerode"
BIN_PEDERODE = "./build/pederode"
IMG_DB = "./img/dat"
IMG_EXT = ".dat"
IMG_FILES = ['d1', 'd2', 'd3']


def run(outfile):
    res = []
    for i, imgf in enumerate(IMG_FILES):
        imgf = os.path.join(IMG_DB, imgf + IMG_EXT)
        print imgf
        assert(os.path.exists(imgf))
        res_img = []
        for j in range(NNTIMES):
            res1 = sp.check_output([BIN_ERODE, imgf, '1']).strip('\n')
            res2 = sp.check_output([BIN_CLERODE, imgf]).strip('\n')
            res3 = sp.check_output([BIN_PEDERODE, imgf, '1']).strip('\n')
            tmp = (res1 + res2 + res3).split(" ")[:-1]
            tmp = [i] + [float(x) for x in tmp]
            print tmp
            res_img.append(tmp)
        res += res_img[1:]
    np.save(os.path.splitext(outfile)[0], res)


if __name__ == '__main__':
    from docopt import docopt

    args = docopt(__doc__)
    out = args['<out>']
    res = run(out)


