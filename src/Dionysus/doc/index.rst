Welcome to Dionysus' documentation!
===================================

Dionysus is a C++ library for computing persistent homology. It provides
implementations of the following algorithms:

.. sidebar:: Contents
   
   .. toctree::
      :maxdepth: 1
   
      get-build-install
      tutorial
      examples/index
      python/overview
      bibliography

* Persistent homology computation [ELZ02]_ [ZC05]_
* Vineyards [CEM06]_    |cpp-only|
* Persistent cohomology computation (described in [dSVJ09]_)
* Zigzag persistent homology [CdSM09]_
* :ref:`examples` provide useful functionality in and of themselves:
  
  * :ref:`Alpha shape construction <alpha-shape-example>` in 2D and 3D
  * :ref:`Rips complex construction <rips-example>`
  * Cech complex construction       |cpp-only|
  * :ref:`Circle-valued parametrization <cohomology-parametrization>`
  * :ref:`Piecewise-linear vineyards <pl-vineyard>`

.. todo:: 
   Document more examples.

The C++ API is currently very poorly documented. One's best source for its
documentation is its usage in various :ref:`examples` (located in
:sfile:`examples/`).

The :ref:`Python bindings <python-bindings>` provide both a simple interface to
the low-level C++ functionality as well as high-level auxilliary routines. Their
"thinness" is meant to provide the efficiency benefits of C++ together with the
simplicity, elegance, and interactivity of Python. Since they mimick the C++
functionality, their documentation may be a helpful resource for the latter.

:ref:`Download <download>` the software, or read a :ref:`tutorial` to
get acquainted with its structure.

The library is distributed under the GPL_ license.

.. _GPL: http://www.gnu.org/licenses/gpl.html

.. include::    substitutions.aux


..
    Indices and tables
    ==================

    * :ref:`genindex`
    * :ref:`modindex`
    * :ref:`search`

