#!/usr/bin/env python

from    pylab           import scatter, show, cm, colorbar, axes, savefig
from    itertools       import izip
from    sys             import argv, exit
import  os.path         as     osp


def plot(val1_fn, val2_fn, outfn = None):
    values1 = []
    with open(val1_fn) as fp:
        for line in fp.xreadlines():
            values1.append(float(line.split()[1]))
    
    values2 = []
    with open(val2_fn) as fp:
        for line in fp.xreadlines():
            values2.append(float(line.split()[1]))
    
    values1 = [v % 1. for v in values1]
    values2 = [v % 1. for v in values2]
    print min(values1), max(values2), min(values1), min(values2)

    scatter(values1, values2, s=10)
    axes().set_aspect('equal')
    if not outfn:
        show()
    else:
        savefig(outfn)

if __name__ == '__main__':
    if len(argv) < 3:
        print "Usage: %s VALUES1 VALUES2 [OUTPUT]" % argv[0]
        exit()

    val1_fn = argv[1]
    val2_fn  = argv[2]
    
    outfn = None
    if len(argv) > 3: 
        outfn = argv[3]

    plot(val1_fn, val2_fn, outfn)
