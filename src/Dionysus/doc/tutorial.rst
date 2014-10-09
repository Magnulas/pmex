.. _tutorial:

Brief Tutorial
==============

It suffices to import only the necessary commands from module :mod:`dionysus`,
but for the purposes of the tutorial, we import everything::

    from dionysus import *

Read in a points file (for :func:`points_file` the points should appear one per
line with coordinates separated with spaces; of course, one can read in the
points however one wants)::

    points = [p for p in points_file('points_filename')]
    print "Number of points:", len(points)


Complexes
---------

If the points are in :math:`\mathbb{R}^2` or :math:`\mathbb{R}^3`, one can
construct an alphashape filtration::

    simplices = Filtration()
    fill_alpha2D_complex(points, simplices)     # for 2D, or
    fill_alpha3D_complex(points, simplices)     # for 3D


Functions :ref:`fill_alpha*_complex <alphashapes>` fill the ``simplices``
with all the :class:`simplices <Simplex>` of the Delaunay triangulation. 
Each one has its attribute ``data`` set to a pair: the
smallest value of the squared distance function on the dual Voronoi cell and
whether the simplex is critical or not (i.e. whether its dual cell does or
does not contain a critical point of the distance function).
See :ref:`alphashapes` for more details, and :ref:`alpha-shape-example` for a
full example.

As a result, if one wanted only those simplices whose alpha shape value did not
exceed 10, one could obtain them as follows::

    simplices10 = [s for s in simplices if s.data[0] <= 10]

If the point set lies in higher dimensions, one may construct a Rips complex on
it. This complex requires only pairwise distances, which makes it very
versatile. One must first construct an instance of a class providing such
distances (e.g. :class:`PairwiseDistances` for explicit points, see
:ref:`distances` for more details), and then pass it to the :class:`Rips`
complex class::

    distances = PairwiseDistances(points)
    rips = Rips(distances)

Usually, because of space restrictions, generation of a Rips complex has to be
restricted to a :math:`k`-skeleton of the complex and some maximal parameter
:math:`max`. In the following example :math:`k = 3` and :math:`max = 50`::

    simplices = Filtration()
    rips.generate(3, 50, simplices.append)

:meth:`Rips.generate` takes a skeleton and a maximum distance cutoffs, and a
function which is called with each of the simplices in the complex (in this
case, Python list `simplices`' ``append`` method).
    

Persistence
-----------

There are two ways of computing persistence in Dionysus. The first *offline*
way, required by :class:`StaticPersistence` (and its derivatives), is to set up
the entire filtration at once, compute its persistence in one operation, and
then examine the pairings. The second way is to feed simplices one by one in an
*online* manner and manually keep track of the pairs that are created.
:class:`ZigzagPersistence` and :class:`CohomologyPersistence` accept their
input this way,


Offline
^^^^^^^

For the first approach, i.e. to use :class:`StaticPersistence`, one must put the
sort the filtration with respect to some ordering
(for example, :func:`data_dim_cmp` for alpha shapes or :meth:`Rips.cmp` for the
Rips complex)::

    simplices.sort(data_dim_cmp)     # for the alpha shapes
    simplices.sort(rips.cmp)         # for the rips complex

Creating an instance of :class:`StaticPersistence` initialized with the
filtration really initializes a boundary matrix. The subsequent call to
:meth:`~StaticPersistence.pair_simplices` reduces the matrix to compute the
persistence of the filtration::

    p = StaticPersistence(simplices)
    p.pair_simplices()

Once the simplices are paired, one may examine the pairings by iterating over
the instance of :class:`StaticPersistence`. We can use an auxilliary map ``smap`` 
to remap the persistence indices into the actual simplices::

    smap = p.make_simplex_map(simplices)
    for i in p:
        if i.sign():
            birth = smap[i]
            if i.unpaired():
                print birth.dimension(), birth.data, "inf"
                continue
            
            death = smap[i.pair()]
            print birth.dimension(), birth.data, death.data

The iterator ``i`` over the instance ``p`` of :class:`StaticPersistence` is of type
:class:`SPNode`, and represents individual simplices taken in the filtration
order. It knows about its own :meth:`~SPNode.sign` and :meth:`~SPNode.pair` as well as
whether it is :math:`~SPNode.unpaired`. :meth:`StaticPersistence.make_simplex_map` creates 
a map that we use to get the actual simplices: ``smap[i]`` and ``smap[i.pair()]``.
The previous code snippet prints out the persistence diagrams of the given
filtration.


Online
^^^^^^

Class :class:`ZigzagPersistence` accepts additions and removals of the simplices
one by one, and returns an internal representation of the simplices.
(:class:`CohomologyPersistence` works in the same way, but accepts only
additions of the simplices.) 
When one
adds a simplex via :meth:`ZigzagPersistence.add`, one must provide its boundary
in this internal representation together with a *birth value* which
:class:`ZigzagPersistence` will store in case a class is born as a result of the
addition. :meth:`~ZigzagPersistence.add` returns a pair ``(i,d)``. ``i`` is the
internal representation of the newly added simplex, which one must record for
future use in the boundaries of its cofaces (below it is recorded in the
dictionary ``complex``). ``d`` is `None` in case of the birth, otherwise, it is the
previously recorded birth value of the class that dies as a result of the
addition. The following code adds all the ``simplices`` to a zigzag::

    simplices.sort(data_dim_cmp)
    complex = {}
    zz = ZigzagPersistence()
    for s in simplices:
        i,d = zz.add([complex[sb] for sb in s.boundary], (s.dimension(), s.data))
        complex[s] = i
        if d is not None:                   # we have a death
            dimension, birth = d            # we previously stored the (dimension, data) pair
            print dimension, birth, s.data       

Similarly, one can remove simplices by calling :meth:`ZigzagPersistence.remove`
with the internal index previously returned by :meth:`~ZigzagPersistence.add`::

    for s in reversed(simplices):
        d = zz.remove(complex[s], (s.dimension() - 1, s.data))
        del complex[s]
        if d is not None:
            dimension, birth = d
            print dimension, birth, s.data

Naturally, :meth:`~ZigzagPersistence.remove` returns only `d`. 

If one wants to add or remove an entire set of simplices "at once" (i.e.  with
all births being assigned the same value), there are two helper functions:
:func:`add_simplices` and :func:`remove_simplices`.

See the :ref:`bindings reference <python-bindings>` for more details, and
:ref:`triangle-zigzag-example` for an example of :class:`ZigzagPersistence`.
See :ref:`rips-pairwise-cohomology` for an example of
:class:`CohomologyPersistence`.


.. _speed-up-suggestions:

Speed-up suggestions
--------------------

Currently, when the choice comes between efficiency and flexibility, the Python
bindings err on the side of flexibility. There is hope that in the future the
choice won't really be necessary. Meanwhile, one can use a few techniques that
speed up computation at the expense of memory. Note, however, that since the
recent switch of :class:`PairwiseDistances` to C++ rather than pure Python, it
is not clear whether these deliver a substantial speed-up:

* To avoid (possibly expensive) computation of distances during Rips complex
  generation, store :class:`ExplicitDistances` (see :ref:`distances`)::

        distances = PairwiseDistances(points)
        distances = ExplicitDistances(distances)

* To avoid the computation of simplex sizes in the Rips complex during the
  initialization of a :class:`Filtration`, store them explicitly in
  :attr:`Simplex.data` attribute (this is not done by default to save memory);
  then use :func:`data_dim_cmp` when sorting the
  :class:`Filtration`::

        rips = Rips(distances)
        simplices = Filtration()
        rips.generate(..., simplices.append)
        for s in simplices: s.data = rips.eval(s)
        simplices.sort(data_dim_cmp)



.. include::    substitutions.aux
