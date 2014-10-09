:class:`Rips` class
======================

.. class:: Rips

    .. method:: __init__(distances)
    
        Initializes :class:`Rips` with the given `distances` whose main purpose
        is to return the distance of two points given their indices. See
        Distances_ below.

    .. method:: generate(k, max, functor[, seq])
     
        Calls `functor` with every simplex in the `k`-skeleton of the Rips
        complex :math:`VR` (`max`). If `seq` is provided, then the complex is
        restricted to the vertex indices in the sequence.

    .. method:: vertex_cofaces(v, k, max, functor[, seq])
     
        Calls `functor` with every coface of the vertex `v` in the `k`-skeleton
        of the Rips complex :math:`VR` (`max`). If `seq` is provided, then the
        complex is restricted to the vertex indices in the sequence.

    .. method:: edge_cofaces(u, v, k, max, functor[, seq])
     
        Calls `functor` with every coface of the edge (`u`, `v`) in the
        `k`-skeleton of the Rips complex :math:`VR` (`max`). If `seq` is
        provided, then the complex is restricted to the vertex indices in the
        sequence.

    .. method:: cmp(s1, s2)

        Compares simplices `s1` and `s2` with respect to their ordering in the
        Rips complex.  Note that like Python's built in `cmp` this is a three
        possible outsome comparison (-1,0,1) for (:math:`\leq, =, \geq`,
        respectively).

    .. method:: eval(s)

        Returns the size of simplex `s`, i.e. the length of its longest edge.


.. _distances:

Distances
---------

An instance of `distances` passed to the constructor of :class:`Rips` should
know its length and the distances between the points. The length should be
retrievable via ``len(distance)`` and it determines how many points the complex
is built on. The distances between the points are inferred by the class
:class:`Rips` by calling `distances` with a pair of vertices as arguments.

For example, the following class represents 10 points on an integer lattice::

    class Distances:
        def __len__(self): 
            return 10

        def __call__(self, x, y):
            return math.fabs(y-x)

The bindings expose a C++ class as a Python class :class:`PairwiseDistances` to deal with
explicit points in a Euclidean space. In pure Python it could be defined as
follows (in fact it used to be a pure Python class, and one may still find it in 
:sfile:`bindings/python/dionysus/distances.py`; its performance is much slower
than its pure C++ analog)::

    class PairwiseDistances:
        def __init__(self, points, norm = l2):
            self.points = points
            self.norm = norm

        def __len__(self):
            return len(self.points)

        def __call__(self, p1, p2):
            return self.norm([x - y for (x,y) in zip(self.points[p1], self.points[p2])])

Another distances class is available that speeds up the computation of the Rips
complex at the expense of the memory usage: :class:`ExplicitDistances`. It is
initialized with an instance of any class that behaves like a distances class,
and it stores all of its distances explicitly to not have to recompute them in
the future::

    distances = PairwiseDistances(points)
    distances = ExplicitDistances(distances)

With :class:`PairwiseDistances` being a C++ class, and
:class:`ExplicitDistances` being pure Python, the speed-up seems minor.


Example
-------

The following example reads in points from a file, and fills the list
`simplices` with the simplices of the 2-skeleton of the Rips complex built on
those vertices with distance cutoff parameter 50. Subsequently it computes the
persistence of the resulting filtration (defined by ``rips.cmp``)::

    points = [for p in points_file('...')]
    distances = PairwiseDistances(points)
    rips = Rips(distances)
    simplices = Filtration()
    rips.generate(2, 50, simplices.append)
    
    simplices.sort(rips.cmp)
    p = StaticPersistence(simplices)
    p.pair_simplices()

Essentially the same example is implemented in
:sfile:`examples/rips/rips-pairwise.py`, although it reads the `k` and `max`
parameters for the Rips complex on the command line, and uses a trick to speed
up the computation.
