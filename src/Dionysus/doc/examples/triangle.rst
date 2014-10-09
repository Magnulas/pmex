.. _triangle-example:

Triangle example
================

Simple example of a filtered triangle is given in
:sfile:`examples/triangle/triangle.cpp`. Its equivalent in Python appears in
:sfile:`examples/triangle/triangle.py`, and we describe it next.

.. literalinclude:: ../../examples/triangle/triangle.py
   :language: python

After the necessary imports, the ``complex`` is setup explicitly as a list of
simplices. Each :class:`Simplex` constructor takes an iterable sequence of
vertices, and optionally a data value.

A filtration ``f`` is initialized using the :class:`Filtration` class, which
takes a list of simplices (or anything iterable) and a comparison that defines
in what order the simplices should come in the filtration. In this case we use
:func:`data_cmp`, which simply compares simplices' :attr:`~Simplex.data`
attributes.

:class:`StaticPersistence` is initialized with the filtration, and its method
:meth:`~StaticPersistence.pair_simplices` pairs the simplices of the
filtration::

    p = StaticPersistence(f)
    p.pair_simplices()

Subsequently, we iterate over ``p`` to access a representation of each simplex
in the filtration order. We output each simplex, its sign, and its pair. The auxilliary 
``smap = p.make_simplex_map(f)`` remaps the indices of :class:`StaticPersistence` into 
the simplices in the filtration.
Naturally, one could use this to access the
:attr:`~Simplex.data` attribute of the simplices to output the actual
persistence diagram, as is done in the :ref:`alpha-shape-example` and the
:ref:`rips-example`.
