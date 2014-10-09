.. _download:

Get, Build, Install
===================

The most up to date code is available from
`my Mercurial repository`_.
If you have Mercurial_, the easiest way to obtain the code is by cloning it:

.. parsed-literal::

  hg clone |dionysus-url|
  cd Dionysus
  hg up tip

If you don't have time or desire to deal with Mercurial, you can download the
`tarball of the entire repository`_. The advantage of using Mercurial is that it
makes it very easy to keep up with the updates that are periodically committed
to the repository::

  hg pull -u


.. |dionysus-url|   replace:: http://hg.mrzv.org/Dionysus/

.. _Mercurial:      http://www.selenic.com/mercurial/

.. _`tarball of the entire repository`:     http://hg.mrzv.org/Dionysus/archive/tip.tar.gz
.. _`my Mercurial repository`:              http://hg.mrzv.org/Dionysus/


Dependencies
------------
Dionysus requires the following software:

  :CMake_:              for building (version :math:`\geq` 2.6)
  :Boost_:              C++ utilities (version :math:`\geq` 1.36; including Boost.Python used to create
                        Python bindings)

Optional dependencies:

  :CGAL_:               for alpha shapes   (version :math:`\geq` 3.4)
  :CVXOPT_:             for :ref:`circle-valued parametrization <cohomology-parametrization>` using LSQR
  :PyQt4_:              for :mod:`viewer` module
  :PyOpenGL_, NumPy_:   for 3D visualization in :mod:`viewer` module
  :PyX_:                :sfile:`tools/draw-diagram/draw.py` uses `PyX`_ to
                        produce a PDF of the diagram
  :rlog_:               used for logging only (not needed by default)

..  :dsrpdb_:             for reading PDB files
    :SYNAPS_:             for solving polynomials (for kinetic kernel), which in
                        turn requires GMP_

.. _CMake:          http://www.cmake.org
.. _Boost:          http://www.boost.org
.. _CGAL:           http://www.cgal.org
.. _CVXOPT:         http://abel.ee.ucla.edu/cvxopt/
.. _PyQt4:          http://www.riverbankcomputing.co.uk/software/pyqt/intro
.. _PyOpenGL:       http://pyopengl.sourceforge.net/
.. _NumPy:          http://numpy.scipy.org/
.. _PyX:            http://pyx.sourceforge.net/
.. _rlog:           http://www.arg0.net/rlog
.. _dsrpdb:         http://www.salilab.org/~drussel/pdb/
.. _SYNAPS:         http://www-sop.inria.fr/galaad/synaps/
.. _GMP:            http://gmplib.org/


Building
--------
To build the examples as well as the :ref:`Python bindings <python-bindings>`,
create a directory ``build``. Inside that directory run ``cmake`` and ``make``::

  mkdir build
  cd build
  cmake ..
  make

.. tip::

   To use GCC 4.2 on a Mac one can try ``CXX=g++-4.2 cmake ..`` instead of
   ``cmake ..``.

Instead of ``cmake``, one can run ``ccmake`` for a curses interface. The
following configuration options are available. One can set them either through
the curses interface or by passing a flag of the form ``-Doptimize:bool=on`` to
``cmake``.

  :debug:         Turns on debugging compilation
  :optimize:      Turns on compiler optimizations (`on` by default)
  :logging:       Turns on logging facilities
  :counters:      Turns on various built-in counters

Depending on the combination of debugging and optimization, a particular
``CMAKE_CXX_FLAGS*`` is chosen.

.. tip::    The default settings work fine unless you want to dive into the
            library's internals with logging or study the performance of various
            algorithms with counters.

.. todo::       Write sections on logging and counters.

Some parts of Dionysus understand the ``DEBUG_CONTAINERS`` definition which can
be appended to ``CMAKE_CXX_FLAGS``. If set, the library will use GCC STL's
debugging containers (from the ``std::__debug`` namespace defined in ``debug/*``
header files). These containers return safe iterators (the kind that check
whether they are singular when compared, or additionally whether they are out of
bounds when dereferenced).

.. todo:: ``ZIGZAG_CONSISTENCY`` definition


Install
-------

At the moment there are no installation procedures. To run the Python code you
need to have ``.../build/bindings/python`` somewhere in your ``PYTHONPATH``.
I.e. add::

    export PYTHONPATH=.../build/bindings/python

to your ``~/.bashrc`` (assuming you are using Bash_). Alternatively, run the
python examples from within ``.../build/bindings/python``::

    python .../Dionysus/examples/triangle/triangle.py

The C++ examples can be run from anywhere. The C++ library consists only of
header files (no library actually needs to be built), so to compile against it,
it suffices to add ``-I .../Dionysus/include`` to your ``g++`` flags::

    g++ your-code.cpp -o your-code -I .../Dionysus/include

Proper installation procedures (with ``make install``) will be added in the
future.

.. _Bash:       http://www.gnu.org/software/bash/
