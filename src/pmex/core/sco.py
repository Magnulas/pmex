"""This is part of the Periodic Motion Extractor plugin for Blender,
and is to be used for extracting periodic motions from motion capture data.
Copyright (C) 2014  Magnus Raunio

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

from pmex.dionysus import data_dim_cmp, PairwiseDistances, Rips, Filtration, CohomologyPersistence, ExplicitDistances,DynamicPersistenceChains

import numpy

from    cvxopt          import spmatrix, matrix
from    cvxopt.blas     import copy
from    pmex.core.lsqr  import lsqr

import time
import sys

class SimplicialComplexOperator():
    
    ccls = None
    simplices = None
    prime = 47
    cclOrders = None
    
    def __init__(self, points, skeleton = 2, dmax = float('inf'),prime=47):
        
        self.simplices = Filtration()
        self.prime = prime
        self.ccls = []
        
        distances = PairwiseDistances(points)

        distances = ExplicitDistances(distances)           # speeds up generation of the Rips complex at the expense of memory usage
            
        rips = Rips(distances)
            
        self.prime = prime
            
        rips.generate(skeleton, dmax, self.simplices.append)

        for s in self.simplices: 
            s.data = rips.eval(s)


        self.simplices.sort(data_dim_cmp)

        ch = CohomologyPersistence(prime)
        complex = {}
        internalToExternalMap = {}

        for j, s in enumerate(self.simplices):
            percentage = j/len(self.simplices)
            sys.stdout.write('\r {:.2f}%'.format(100*percentage))
            sys.stdout.flush()
            
            i, d, ccl = ch.add([complex[sb] for sb in s.boundary], (s.dimension(), s.data), store=(s.dimension() < skeleton))
            complex[s] = i

            internalToExternalMap[i.order] = j

            if d != None and d[0] == skeleton - 1 and not((d[1] - s.data) == 0):
                orders = [e.si.order for e in ccl]
                self.ccls.append((ccl, d[1], s.data,orders))

        print('\r {:.2f}%'.format(100))
    
        for ccl in ch:
            if ccl.birth[0] == skeleton - 1:
                orders = [e.si.order for e in ccl]
                self.ccls.append((ccl, ccl.birth[1],dmax, orders))
                            
        self.ccls.sort(key=lambda tup : tup[2] - tup[1] , reverse=True)

    def getCocycleCount(self):
        return len(self.ccls)

    def getCocycleLength(self, cocycle_index):
        if cocycle_index < len(self.ccls):
            birth = self.ccls[cocycle_index][1]
            death = self.ccls[cocycle_index][2]
            return death - birth
        else:
            return -1
        
    def getCocycels(self):
        return self.ccls
    
    def getSimplicalComplex(self):
        return self.simplices
    
    def getCircularMapping(self, cocycle_index):

        ccl = self.ccls[cocycle_index][0]
        death = self.ccls[cocycle_index][2]
        orders = self.ccls[cocycle_index][3]
        coefficients = [self.normalized(e.coefficient) for e in ccl]
        ccl_list = [(coefficients[i],orders[i]) for i in range(0,len(orders))]
       
        cycle_map = self.smooth(death, ccl_list)
        cycle_map = numpy.mod(cycle_map, 1.0)

        return cycle_map

    def normalized(self,coefficient):
        if coefficient > self.prime / 2:
            return coefficient - self.prime
        return coefficient
        
    def smooth(self,death, cocycle):
        coefficient = []
        coface_indices = []
        face_indices = []
        
        for i, s in enumerate(self.simplices):
            #print(s.data, " ", death)
            if s.data < death:
                if s.dimension() <= 2:
                    c = 1
                    for sb in s.boundary:
                        j = self.simplices(sb)
                        coefficient.append(c)
                        coface_indices.append(i)
                        face_indices.append(j)
                        c *= -1
            else:
                break
    
        dimension = max(max(coface_indices), max(face_indices)) + 1
        
        D = spmatrix(coefficient, coface_indices, face_indices, (dimension, dimension))
    
        cocycle = [zz for zz in cocycle if zz[1] < dimension]
    
        z = spmatrix([zz[0] for zz in cocycle],
                     [zz[1] for zz in cocycle],
                     [0     for zz in cocycle], (dimension, 1))
    
        v1 = D * z
     
        if bool(D * D):
            raise Exception('D^2 is not 0')
        if bool(v1):
            raise Exception('Expected a cocycle as input')
        z = matrix(z)
    
        def Dfun(x, y, trans='N'):
            if trans == 'N':
                copy(D * x, y)
            elif trans == 'T':
                copy(D.T * x, y)
            else:
                assert False, "Unexpected trans parameter"
    
        tol = 1e-10
        solution = lsqr(Dfun, matrix(z), show=False, atol=tol, btol=tol, itnlim=None)
        z_smooth = z - D * solution[0]
    
        if not (sum((D * z_smooth) ** 2) < tol and sum((D.T * z_smooth) ** 2) < tol):
            raise Exception("Expected a harmonic cocycle: %f %f" % (sum((D*z_smooth)**2), sum((D.T*z_smooth)**2)))
            
        values = []
        vertices = ((i, s) for (i, s) in enumerate(self.simplices) if s.dimension() == 0)
        for i, s in vertices:
            v = [v for v in s.vertices][0]
            if v >= len(values):
                values.extend((None for i in range(len(values), v + 1)))
            values[v] = solution[0][i]
    
        return values
