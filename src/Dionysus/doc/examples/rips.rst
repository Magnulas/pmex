.. _rips-example:

Rips complex example
====================

.. todo::
   Explain `Vietoris-Rips complex`_.

There is an elementary example in :sfile:`examples/rips/rips.py` that computes a
Rips complex of a few points with integer coordinates on a line. It illustrates
the use of Rips complexes and in particular of defining your own notion of a
:ref:`Distance <Distances>` based on which the Rips complex is constructed.

A more useful example is given in :sfile:`examples/rips/rips-pairwise.py` (and
its C++ counterpart in :sfile:`examples/rips/rips-pairwise.cpp`). The example
takes on the command line the filename of a file with points in Euclidean space
(one point per line), and a cut off parameters for the skeleton and the
:math:`\epsilon` parameter for the Rips complex construction. It then constructs
the Rips complex up to these cutoff parameters, computes its persistence, and
outputs the persistence diagram (one point per line).

.. literalinclude:: ../../examples/rips/rips-pairwise.py

The bit that sets up the Rips complex is::

    distances = PairwiseDistances(points)
    rips = Rips(distances)
    simplices = Filtration()
    rips.generate(skeleton, max, simplices.append)

The computation of persistence and output of the persistence diagram are the
same as in the :ref:`alpha-shape-example`. The example also incorporates 
the :ref:`speed-up-suggestions` given in the :ref:`tutorial`.



C++ sketch
----------
.. highlight:: cpp

.. warning:: This section is not finished.

The example given in :sfile:`examples/rips/rips.cpp` illustrates how one can use
the library to compute persistence of a `Vietoris-Rips complex`_ for a given set of
distances. At the top of the file a `struct Distances` is defined. The
particular distances in the example are trivial (they are points with integer
coordinates on a real line), however, the `struct` illustrates the basic
requirements of any such class to be passed to the `Rips<Distances>` class.

.. _`Vietoris-Rips complex`:        http://en.wikipedia.org/wiki/Vietoris-Rips_complex

The Rips complex itself is generated in the line::

    rips.generate(2, 50, make_push_back_functor(complex));

which tells it to generate a 2-skeleton of the Rips complex up to 
distance value of 50, and insert the simplices into the previously defined
vector `complex`. 

Subsequent sort is unnecessary since Bron-Kerbosch algorithm that generates the
complex will actually generate the simplices in lexicographic order; it's there
for illustration purposes only (the simplices must be sorted
lexicographically). 

The following "paragraph" sets up the filtration with respect to simplex sizes
(specified by `Generator::Comparison(distances)`), and computes its persistence::

    // Generate filtration with respect to distance and compute its persistence
    Fltr f(complex.begin(), complex.end(), Generator::Comparison(distances));
    Persistence p(f);
    p.pair_simplices();
