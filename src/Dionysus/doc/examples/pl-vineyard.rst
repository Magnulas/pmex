.. _pl-vineyard:

Piecewise-Linear Vineyard
=========================

Given a simplicial complex :math:`K`, and a sequence of values on each one of
its vertices, one may construct a homotopy of PL functions on the complex that
interpolates linearly between the values. For any given time in the homotopy, we
get a function from the simplicial complex to the real line, and we can compute
its persistence diagram. Stacking all such diagrams together we get a
persistence vineyard [CEM06]_. An example that computes such a vineyard is in 
:sfile:`examples/pl-functions/pl-vineyard.cpp`. 

.. program:: pl-vineyard

Once compiled, it takes three files as the input::
    
    pl-vineyard complex values output-prefix

``complex`` lists the simplices of the simplicial complex :math:`K`, one
per-line::

    0
    1
    0 1
    2
    0 2
    ...

``values`` lists the vertex values of the consequtive "frames" of the homotopy.
Each line is a sequence of as many numbers as there are vertices in the complex.
It describes a single frame. :program:`pl-vineayrd` constructs the homotopy over
the interval :math:`[0,k-1]`, where :math:`k` is the number of frames. As an
example of ``values`` input::

    3.3   6    2
    1.2   3    10
    7.5   2.1  0

This input means: :math:`f_0(0) = 3.3, f_1(0) = 1.2, f_2(0) = 7.5`. Similarly, 
:math:`f_0(1) = 6, f_1(1) = 3, f_2(1) = 2.1`; 
:math:`f_0(2) = 2, f_1(2) = 10, f_2(2) = 0`.

The vineyard is saved to the files prefixed with ``output-prefix``, followed by
the dimension and extension, e.g. ``myfunction1.vin`` or ``myfunction1.edg``,
depending on the format. The two formats are vines and edges. The former saves
one vine per line, listed as a stream of triplets BIRTH DEATH TIME::

    4 5 0 3.4 5.6 0.4 3 6 1 ...

The edge format represents the vine as a sequence of edges, each given as a
start and end point. So the above vine would appear as::

    4 5 0
    3.4 5.6 0.4
    3.4 5.6 0.4
    3 6 1
    ...

:program:`pl-vineyard` takes additional options:

.. cmdoption:: -s, --skip-infinite

    Do not output infinite vines.

.. cmdoption:: -v, --save-vines

    Output vines, instead of the default edge format.

.. cmdoption:: -e, --explicit-events

    Go through the events one by one (useful for the debugging).
