.. _examples:

Examples
========

The most basic example and therefore a good place to start getting acquainted
with the library is the :ref:`triangle-example`. It adds simplices of a triangle
one by one, and then (in case of a :ref:`triangle-zigzag-example`), removes them
one by one.

.. toctree::
    
    triangle
    triangle-zigzag
    
The simplest example that instead of specifying the complex explicitly,
constructs it from the input point set is the :ref:`alpha-shape-example`. The
example reads points from a file, determines their dimension dynamically (based
on the number of coordinates in the first line of the file), and then constructs
an alpha shape and outputs its persistence diagram.

.. toctree::

    alphashape

Another example that follows a similar strategy is the computation of the
Vietoris-Rips complex. Since only pairwise distances are required it works with
points in arbitrary dimension. (Of course, in dimensions 2 and 3 the complexes
are much larger than those for the :ref:`alpha-shape-example`).

.. toctree::
    :maxdepth: 1

    rips

One may use persistent cohomology algorithm to extract persistent cocycles,
turn them into harmonic cocycles, and use them to parametrize the input point
set; for details see [dSVJ09]_. The explanation of how to use Dionysus to
achieve this is available.

.. toctree::
    :maxdepth: 1

    cohomology

A simple example of computing persistence of a lower-star filtration is in
:sfile:`examples/pl-functions/lsfiltration.py`.

A C++-only, but useful example is computation of a vineyard of piecewise
straight-line homotopy of piecewise-linear functions.

.. toctree::
    :maxdepth: 1

    pl-vineyard
