def log2(x):
    i = 0
    while x:
        x >>= 1
        i += 1
    return i-1        

class Cube:
    def __init__(self, vertices):
        self.vertices = vertices

    def dimension(self):
        return log2(len(self.vertices))

    def boundary(self):
        for i in xrange(self.dimension()):
            for side in [0,1]:
                vertices = []
                for idx in xrange(len(self.vertices)/2):
                    # Insert i-th bit equal to side
                    v = (idx & ~(2**i-1)) << 1
                    if side: v |= 2**i
                    v |= (idx & (2**i - 1))
                    vertices.append(self.vertices[v])
                yield Cube(vertices)    

    def __hash__(self):
        return hash(tuple(self.vertices))

    def __eq__(self, other):
        return self.vertices == other.vertices

    def __repr__(self):
        return " ".join(map(str, self.vertices))


if __name__ == '__main__':
    c = Cube(['a', 'b', 'c', 'd'])
    print c
    for sb in c.boundary(): print sb
