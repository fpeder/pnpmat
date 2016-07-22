#!/usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np
import subprocess
import os

NNTIMES = 3
BIN_GH = "./build/gh"
BIN_CLGH = "./build/clgh"
BIN_PEDEGH = "./build/pedegh"
IMG_DB = "./img/dat"
IMG_EXT = ".dat"
#IMG_FILES = ["320x240", "640x480", "1024x768",
#             "1280x1024", "1600x1200", "2048x1536"]
IMG_FILES = ['d1', 'd2', 'd3']


res = []
for i, imgf in enumerate(IMG_FILES):
    imgf = os.path.join(IMG_DB, imgf + IMG_EXT)
    print imgf
    assert(os.path.exists(imgf))
    res_img = []
    for j in range(NNTIMES):
        r1 = subprocess.check_output([BIN_GH, imgf]).strip('\n')
        r2 = subprocess.check_output([BIN_CLGH, imgf]).strip('\n')
        r3 = subprocess.check_output([BIN_PEDEGH, imgf]).strip('\n')
        tmp = (r1 + r2 + r3).split(" ")[:-1]
        tmp = [i] + [float(x) for x in tmp]
        print tmp
        res_img.append(tmp)
    res += res_img[1:]

res = np.array(res)
np.save('res/gh', res)
    

 



