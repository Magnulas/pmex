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

from abc import ABCMeta, abstractmethod

class RegressionModel(metaclass=ABCMeta):
    
    @abstractmethod
    def positionModel(self,x, coefficients=None):
        return 0
    
    @abstractmethod
    def velocityModel(self,x, coefficients=None):
        return 0
 
    @abstractmethod
    def accelerationModel(self,x, coefficients=None):
        return 0
