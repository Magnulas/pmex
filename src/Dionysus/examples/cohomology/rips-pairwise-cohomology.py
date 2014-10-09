#!/usr/bin/env python

from    dionysus        import Simplex, CohomologyPersistence, points_file, PairwiseDistances, ExplicitDistances, Rips, data_dim_cmp
from    sys             import argv, exit
import  time

def main(filename, skeleton, max, prime = 11):
    points = [p for p in points_file(filename)]
    print '#', time.asctime(), "Points read"
    distances = PairwiseDistances(points)
    distances = ExplicitDistances(distances)           # speeds up generation of the Rips complex at the expense of memory usage
    rips = Rips(distances)
    print '#', time.asctime(), "Rips initialized"

    simplices = []
    rips.generate(skeleton, max, simplices.append)
    print '#', time.asctime(), "Generated complex: %d simplices" % len(simplices)

    # While this step is unnecessary (Filtration below can be passed rips.cmp), 
    # it greatly speeds up the running times
    for s in simplices: s.data = rips.eval(s)
    print '#', time.asctime(), simplices[0], '...', simplices[-1]

    simplices.sort(data_dim_cmp)
    print '#', time.asctime(), "Simplices sorted"

    ch = CohomologyPersistence(prime)
    complex = {}

    for s in simplices:
        i,d = ch.add([complex[sb] for sb in s.boundary], (s.dimension(), s.data), store = (s.dimension() < skeleton))
        complex[s] = i
        if d: 
            dimension, birth = d
            print dimension, birth, s.data
        # else birth

    for ccl in ch:
        dimension, birth = ccl.birth
        if dimension >= skeleton: continue
        print dimension, birth, 'inf'         # dimension, simplex data = birth
        print "# Cocycle at (dim=%d, birth=%f)" % ccl.birth
        for e in ccl:
            print "#  ", e.si.order, normalized(e.coefficient, prime)

def normalized(coefficient, prime):
    if coefficient > prime/2:
        return coefficient - prime
    return coefficient

if __name__ == '__main__':
    if len(argv) < 4:
        print "Usage: %s POINTS SKELETON MAX [PRIME=11]" % argv[0]
        exit()

    filename = argv[1]
    skeleton = int(argv[2])
    max = float(argv[3])
    prime = (len(argv) > 4 and argv[4]) or 11

    main(filename, skeleton, max, prime)
