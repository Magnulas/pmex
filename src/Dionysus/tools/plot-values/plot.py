#!/usr/bin/env python

from    pylab           import scatter, show, cm, colorbar, savefig, axis, \
                               figure, xlim, axes, hsv, subplots_adjust as adjust
from    itertools       import izip
from    sys             import argv, exit
import  os.path         as     osp


def plot(val_fn, pts_fn, output_fn):
    points = []
    with open(pts_fn) as fp:
        for line in fp.xreadlines():
            points.append(map(float, line.split()))
    
    values = []
    with open(val_fn) as fp:
        for line in fp.xreadlines():
            values.append(float(line.split()[1]))

    xx = [pt[0] for pt in points]
    yy = [pt[1] for pt in points]
    print "X:", min(xx), max(xx)
    print "Y:", min(yy), max(yy)

    m = min(values)
    values = [(v-m) % 1. for v in values]
    print "V:", min(values), max(values)

    # hsv()
    fig = figure()
    scatter(xx,yy,s=10,c=values)
    colorbar()
    
    # ax = fig.add_axes([-.05,-.1,1.1,1.1])
    ax = axes()
    ax.set_axis_off()
    ax.set_aspect('equal', 'box')
    # adjust(0,0,1,1,0,0)

    fig.savefig(output_fn)

if __name__ == '__main__':
    if len(argv) < 3:
        print "Usage: %s VALUES POINTS" % argv[0]
        exit()

    val_fn = argv[1]
    pts_fn  = argv[2]
    output_fn, ext = osp.splitext(val_fn)
    output_fn += '.pdf'
    plot(val_fn, pts_fn, output_fn)
