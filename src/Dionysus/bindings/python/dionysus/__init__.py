from    _dionysus   import *
from    distances   import l2, ExplicitDistances, points_file
from    zigzag      import *
from    adaptor     import *
import  circular

def init_with_none(self, iter, data = None):        # convenience: data defaults to None
    self._cpp_init_(iter, data)

def repr_with_data(self):
    str = self._cpp_repr_()
    if type(self.data) == float:
        str += ' %f' % self.data
    return str

Simplex._cpp_init_ =    Simplex.__init__
Simplex.__init__   =    init_with_none
Simplex._cpp_repr_ =    Simplex.__repr__
Simplex.__repr__   =    repr_with_data

def Simplex_getinitargs(self):
    return ([v for v in self.vertices], self.data)

Simplex.__getinitargs__ = Simplex_getinitargs

def data_cmp(s1, s2):
    return (s1.data > s2.data) - (s1.data < s2.data)
    #cmp(s1.data, s2.data)

def data_dim_cmp(s1,s2):
    return ((s1.dimension() > s2.dimension()) - (s1.dimension() < s2.dimension())) or data_cmp(s1,s2)
    #cmp(s1.dimension(), s2.dimension()) or data_cmp(s1,s2)

def dim_data_cmp(s1,s2):
    return data_cmp(s1,s2) or cmp(s1.dimension(), s2.dimension())

def vertex_dim_cmp(s1, s2):
    return cmp(s1.dimension(), s2.dimension()) or vertex_cmp(s1, s2)

def dim_cmp(s1, s2):
    return cmp(s1.dimension(), s2.dimension())

def fill_alpha_complex(points, simplices):
    if   len(points[0]) == 2:           # 2D
        fill_alpha2D_complex(points, simplices)
    elif len(points[0]) == 3:           # 3D
        fill_alpha3D_complex(points, simplices)

def closure(simplices, k):
    """Compute the k-skeleton of the closure of the list of simplices."""

    res = set()

    from    itertools   import combinations
    for s in simplices:
        for kk in xrange(1, k+2):
            for face in combinations(s.vertices, min(s.dimension() + 1, kk)):
                res.add(Simplex(face, s.data))

    return list(res)

_init_diagrams = init_diagrams

def init_diagrams(p, f, evaluator = None, data = None):
    if isinstance(p, StaticCohomologyPersistence):
        return init_diagrams_from_adaptor(p,f, evaluator, data)

    return _init_diagrams(p,f, evaluator, data)

def read_points(filename):
    return [p for p in points_file(filename)]
