.. _cohomology-parametrization:

Parametrizing a point set using circle valued functions
=======================================================

The procedure described below is explained in detail in [dSVJ09]_.

.. program:: rips-pairwise-cohomology

One can use :sfile:`examples/cohomology/rips-pairwise-cohomology.cpp` to compute
persistent pairing of the Rips filtration using the persistent cohomology
algorithm. It takes as input a file containing a point set in Euclidean space
(one per line) as well as the following command-line flags:

.. cmdoption:: -p, --prime

    The prime to use in the computation (defaults to 11).

.. cmdoption:: -m, --max-distance

    Maximum cutoff parameter up to which to compute the complex.

.. cmdoption:: -s, --skeleton-dimension

    Skeleton to compute; persistent pairs output will be this number minus 1
    (defaults to 2).

.. cmdoption:: -b, --boundary

    Filename where to output the boundary matrix.

.. cmdoption:: -c, --cocycle

    Prefix of the filenames where to output the 1-dimensional cocycles.

.. cmdoption:: -v, --vertices

    Filename where to output the simplex vertex mapping.

.. cmdoption:: -d, --diagram

    Filename where to output the persistence diagram.


For example::

    rips-pairwise-cohomology points.txt -m 1 -b points.bdry -c points -v points.vrt -d points.dgm

Assuming that at the threshold value of 1 (``-m 1`` above) Rips complex contains
1-dimensional cocycles, they will be output into filenames of the form
``points-0.ccl``, ``points-1.ccl``, etc.

Subsequently one can use :sfile:`examples/cohomology/cocycle.py` to assign to
each vertex of the input point set a circle-valued function. It takes the
boundary matrix, cocycle, and simplex-vertex map as an input (all produced at
the previous step)::

    cocycle.py points.bdry points-0.ccl points.vrt

The above command outputs a file ``points-0.val`` which contains values assigned
to the input points (the lines match the lines of the input file
``points.txt``, but also contains the indices).


Plotting
--------

Two auxilliary tools allow one to visualize the values assigned to the points
(using Matplotlib_): :sfile:`tools/plot-values/plot.py` and
:sfile:`tools/plot-values/scatter.py`::
    
    plot.py points-0.val points.txt scatter.py points-0.val points-1.val

.. _Matplotlib:                             http://matplotlib.sourceforge.net/    


Dependency
----------

The Python `LSQR code`_ (ported from the `Stanford MATLAB implementation`_ to
Python by `Jeffery Kline`_) included with Dionysus, and used in
:sfile:`examples/cohomology/cocycle.py`, requires CVXOPT_.

.. _`LSQR code`:                            http://pages.cs.wisc.edu/~kline/cvxopt/
.. _CVXOPT:                                 http://abel.ee.ucla.edu/cvxopt/
.. _`Stanford MATLAB implementation`:       http://www.stanford.edu/group/SOL/software/lsqr.html
.. _`Jeffery Kline`:                        http://pages.cs.wisc.edu/~kline/


.. _rips-pairwise-cohomology:

Python cohomology computation
-----------------------------

:sfile:`examples/cohomology/rips-pairwise-cohomology.py` gives an example of the
same computation performed in Python (but with the output in a different format).

After the simplicial complex is computed in a list `simplices`, and the list is
sorted with respect to the Rips filtration order, the simplices are inserted
into the :class:`CohomologyPersistence` one by one::

    # list simplices is created

    ch = CohomologyPersistence(prime)
    complex = {}

    for s in simplices:
        i,d = ch.add([complex[sb] for sb in s.boundary], (s.dimension(), s.data))
        complex[s] = i
        if d: 
            dimension, birth = d
            print dimension, birth, s.data
        # else birth

Above dictionary `complex` maintains the map of simplices to indices returned by
:meth:`CohomologyPersistence.add`.  The pair `(dimension, data)` is used as the
birth value. Here `data` is the value associated with the simplex in the Rips
filtration. The pair is returned back if a death occurs, and is printed on the
standard output. After the for loop finishes, one may output infinite
persistence classes with the following for loop::

    for ccl in ch:
        dimension, birth = ccl.birth
        if dimension >= skeleton: continue
        print dimension, birth, 'inf'         # dimension, simplex data = birth
    
Naturally one may iterate over `ccl` which is of type :class:`Cocycle` and
extract more information. For example, this is necessary to get the coefficients
that serve as the input for :sfile:`examples/cohomology/cocycle.py`.
