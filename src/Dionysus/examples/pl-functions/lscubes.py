#!/usr/bin/env python

from    dionysus        import CohomologyPersistence
from    cube            import Cube
from    sys             import argv, exit


def max_vertex(s, vertices):
    return max((vertices[v] for v in s.vertices))

def max_vertex_cmp(s1, s2, vertices):
    m1 = max_vertex(s1, vertices)
    m2 = max_vertex(s2, vertices)
    return cmp(m1, m2) or cmp(s1.dimension(), s2.dimension())

def lsf(values_filename, cubes_filename, prime = 11):
    # Read vertices
    vertices = []
    with open(values_filename) as f:
        for line in f:
            if line.startswith('#'): continue
            vertices.append(float(line.split()[0]))

    # Read cubes
    fltr = []
    with open(cubes_filename) as f:
        for line in f:
            if line.startswith('#'): continue
            fltr.append(Cube(map(int, line.split())))
    fltr.sort(lambda x,y: max_vertex_cmp(x,y,vertices))
    for i,c in enumerate(fltr): c.data = i

    ch = CohomologyPersistence(prime)
    complex = {}

    for c in fltr:
        # print "%s: %s" % (c, " + ".join(map(str, c.boundary())))
        # print complex
        i,d,_ = ch.add([complex[cb] for cb in c.boundary()], c.data)
        complex[c] = i
        if d:
            birth = d
            print c.dimension() - 1, max_vertex(fltr[birth], vertices), max_vertex(c, vertices)

    for ccl in ch:
        birth = ccl.birth
        c = fltr[birth]
        print c.dimension(), max_vertex(c, vertices), 'inf'

if __name__ == '__main__':
    if len(argv) < 3:
        print "Usage: %s VERTICES CUBES" % argv[0]
        print 
        print "Computes persistence of the lower star filtration of the cubical "
        print "complex explicitly listed out in CUBES with vertex values given in VERTICES."
        exit()

    values = argv[1]
    cubes = argv[2]

    lsf(values, cubes)
