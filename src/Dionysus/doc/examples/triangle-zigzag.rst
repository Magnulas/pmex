.. _triangle-zigzag-example:

Triangle zigzag example
=======================

Simple example of a filtered triangle where simplices are first inserted in a
given order, and then removed in the reverse order is in
:sfile:`examples/triangle/triangle-zigzag.cpp`. Its Python equivalent
(:sfile:`examples/triangle/triangle-zigzag.py`) is described next.

.. literalinclude:: ../../examples/triangle/triangle-zigzag.py
   :language: python

Unlike the :ref:`triangle-example`, here we use :class:`ZigzagPersistence` to
compute the pairings, and therefore need to store the internal representations
of the simplicies used by the class. These representation are stored in the
dictionary ``complex``, which maps the simplices to their representations for
:class:`ZigzagPersistence`.

The first for loop processes the simplices sorted with respect to
:func:`data_cmp`. :meth:`ZigzagPersistence.add` invoked within the loop accepts
the boundary of the newly added cell in its internal representation, which is
computed by looking up each simplex in the dictionary ``complex``:
``[complex[ss] for ss in s.boundary]``. If there is a birth, the value to be
associated with the newly created class is ``b`` (which in this case is simply a
counter).  :meth:`~ZigzagPersistence.add` returns a pair ``(i,d)``. The former
is an internal representation of the newly added cell, which we immediately
record with ``complex[s] = i``. The latter is an indicator of whether a death
has occurred, which happens iff ``d is not None``, in which case ``d`` is the
birth value passed to :meth:`~ZigzagPersistence.add` whenever the class that
just died was born. If the death occurred, then we outut the interval ``(d,
b-1)``.

The second for loop removes simplices in the reverse order of their insertion.
:meth:`~ZigzagPersistence.remove` takes the index of the cells to be removed
(looked up in the ``complex`` dictionary: ``complex[s]``), and takes a birth
value in case a class is born. It return only a death indicator (which again is
``None`` if no death occurred).
