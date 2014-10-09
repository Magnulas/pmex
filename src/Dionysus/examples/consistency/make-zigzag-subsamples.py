#!/usr/bin/env python 

# Creates POINTS and SUBSAMPLES files from a list of points file in a format
# suitable for rips-consistency-zigzag.


from    sys     import argv, exit


def create_subsamples(points_fn, subsamples_fn, points_list):
    points = []
    count = []
    for pfn in points_list:
        count.append(0)
        with open(pfn) as f:
            for line in f:
                if line.startswith('#'): continue
                points.append(line)
                count[-1] += 1

    with open(points_fn, 'w') as f:
        for line in points:
            f.write(line)

    cur = 0
    counts = []
    for c in count:
        counts.append(' '.join(map(str, xrange(cur, cur+c))) + '\n')
        cur += c
        # counts.append(' '.join(map(str, xrange(cur-c, cur+c))) + '\n')
    
    with open(subsamples_fn, 'w') as f:
        f.writelines(counts)


if __name__ == '__main__':
    if len(argv) < 4:
        print "Usage: %s POINTS SUBSAMPLES POINTS1 [POINTS2 [POINTS3 [...]]]" % argv[0]
        print
        print "Creates a file POINTS with the union of POINTS* and SUBSAMPLES which lists"
        print "the indices of the points one per line"
        exit()

    points_fn = argv[1]
    subsamples_fn = argv[2]
    points_list = argv[3:]
    create_subsamples(points_fn, subsamples_fn, points_list)
