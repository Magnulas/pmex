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

from pmex.regression.regressionmodel import RegressionModel
import numpy
import itertools
from numpy import cos, sin, pi

class LinearHarmonicRegression(RegressionModel):
    
    def __init__(self, model_complexity=20):
        self.model_complexity = model_complexity
        self.frequencies = range(1, model_complexity + 1)
     
    def positionModel(self,x, coefficients=None):
        if coefficients is None:
            coefficients = numpy.ones(2 * self.model_complexity)
            
        harmonic_part = list(itertools.chain.from_iterable((
               coefficients[2 * i] * numpy.sin(self.frequencies[i] * 2 * pi * x), 
               coefficients[2 * i + 1] * numpy.cos(self.frequencies[i] * 2 * pi * x))
               for i in range(0, self.model_complexity)))

        return list(itertools.chain.from_iterable((harmonic_part,[coefficients[-2]*x,coefficients[-1]])))
    

    def velocityModel(self,x, coefficients=None):
        if coefficients is None:
            coefficients = numpy.ones(2 * self.model_complexity)
        harmonic_part =  list(itertools.chain.from_iterable((
               coefficients[2 * i] * 2 * pi * self.frequencies[i] * numpy.cos(self.frequencies[i] * 2 * pi * x),
               -coefficients[2 * i + 1] * 2 * pi * self.frequencies[i] * numpy.sin(self.frequencies[i] * 2 * pi * x))
               for i in range(0, self.model_complexity)))
        return list(itertools.chain.from_iterable((harmonic_part,[coefficients[-2],0])))
    
 
    def accelerationModel(self,x, coefficients=None):
        if coefficients is None:
            coefficients = numpy.ones(2 * self.model_complexity)
        harmonic_part =  list(itertools.chain.from_iterable((
               -coefficients[2 * i] * pow(2 * pi * self.frequencies[i], 2) * numpy.sin(self.frequencies[i] * 2 * pi * x),
               -coefficients[2 * i + 1] * pow(2 * pi * self.frequencies[i], 2) * numpy.cos(self.frequencies[i] * 2 * pi * x))
               for i in range(0, self.model_complexity))) 
        return list(itertools.chain.from_iterable((harmonic_part,[0,0])))
    
