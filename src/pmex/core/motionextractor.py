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

from pmex.core.sco import SimplicialComplexOperator
from pmex.core.utilitary import velocityAcceleration
from pmex.regression.harmonicregression import HarmonicRegression 
from pmex.regression.linearharmonicregression import LinearHarmonicRegression
import math

from numpy import pi, sin
import numpy
import numpy.linalg

import sys
import os

class MotionExtractor():
    
    compop = None

    positions_radians = None
    positions = None
    
    locations= None
    
    def __init__(self, points_radians,distance,delay_embedding=0, locations=None,prime=11):
           
        points = points_radians
#        points = len(points_radians)*[None]
#        for i in range(0,len(points)):
#            points[i] = 2*len(points_radians[0])*[0]
#            for j in range(0,len(points_radians[i])):
#                points[i][2*j] = math.cos(points_radians[i][j])
#                points[i][2*j+1] = math.sin(points_radiPersistenDiagramWidgetans[i][j])
                
        if delay_embedding > 0:
            delay_embedding = delay_embedding + 1
            delay_embedded_point = MotionExtractor.delay_embbed(points,delay_embedding)
            points = points[:len(points)-delay_embedding]
        else:
            delay_embedded_point = points
            
        #delay_embedded_point = [delay_embedded_point[i] for i in range(0,len(delay_embedded_point),subsampling)]
        
        self.positions_radians = [points_radians[i] for i in range(0,len(delay_embedded_point))]
        self.positions = [points[i] for i in range(0,len(delay_embedded_point))]

        self.compop = SimplicialComplexOperator(delay_embedded_point,dmax=distance,prime=prime)
        self.locations = [locations[i] for i in range(0,len(delay_embedded_point))]
   
    def getPoints(self):
        return self.positions

    def getSCO(self):
        return self.compop
    
    def getCocycleCount(self):
        return self.compop.getCocycleCount()

    def getCocycleLength(self,index):
        return self.compop.getCocycleLength(index)
    
    def getPeriodicMotion(self, cocycle_index, cloud_label,useVelocity=True,useAcceleration=True,jointRegressionModel = HarmonicRegression(20), translationRegressionModels=None):
        cycle_map = self.compop.getCircularMapping(cocycle_index)
        V = MotionExtractor.uniformlyDistributeSamples(cycle_map)
        ccluw = MotionExtractor.unwrap_simple(V)
        (ar,vr) = numpy.polyfit(range(len(ccluw)),ccluw,1)
        
        if ar < 0:
            V = numpy.array([1 - c for c in V])
            cycle_map = numpy.array([1 - c for c in cycle_map])
        
        npoints = 1/abs(ar)
        n_cycels = len(cycle_map)/npoints
        n_cycels = max(1.0,n_cycels)
        npoints = round(len(cycle_map)/n_cycels)

        D = len(self.positions_radians[0])
        N = len(self.positions)
        
        positions = self.positions
        
        time_steps = numpy.linspace(0,n_cycels,len(ccluw))
        velocities, accelerations = velocityAcceleration(positions,time_steps)
         
        skeletons = [D * [None] for i in range(0,1)] 
        out_locations = [] if self.locations == None else len(self.locations[0])*[None]
        tList = numpy.arange(0, 1, 1.0 / npoints)

        t = ccluw
        dt = ccluw
        ddt = ccluw
        
        for d in range(0, D):
            percentage = d/(D+len(out_locations))
            sys.stdout.write('\r {:.2f}%'.format(100*percentage))
            sys.stdout.flush()       
            
            X = [p[d] for p in positions]
            XDelta = []
            XAcc = []
            TDelta = []
            TAcc = []
                
            if useVelocity:
                XDelta = [p[d] for p in velocities]
                TDelta = dt
                
            if useAcceleration:
                XAcc = [p[d] for p in accelerations]
                TAcc = ddt
    
            if(numpy.var(X) > 0):
                
                s, resP, _, _  = MotionExtractor.fitPoints(tList, t, X, [], [], [], [],jointRegressionModel, 1, 1,1)#velWeight, accWeight)
                _, _, resV, _  = MotionExtractor.fitPoints(tList, [], [], TDelta, XDelta, [], [],jointRegressionModel, 1, 1,1)#velWeight, accWeight)
                _, _, _, resA  = MotionExtractor.fitPoints(tList, [],[], [], [], TAcc, XAcc,jointRegressionModel, 1, 1,1)#velWeight, accWeight)

                sigmaP = numpy.sqrt(numpy.sum(numpy.power(resP,2))/len(t))
                sigmaV = numpy.sqrt(numpy.sum(numpy.power(resV,2))/len(TDelta))
                sigmaA = numpy.sqrt(numpy.sum(numpy.power(resA,2))/len(TAcc))
    
                s, resP, resV, resA  = MotionExtractor.fitPoints(tList,t, X, TDelta, XDelta, TAcc, XAcc,jointRegressionModel, 1, sigmaP/sigmaV, sigmaP/sigmaA)#velWeight, accWeight)

                skeletons[0][d] = s
            else:
                skeletons[0][d] = numpy.array(len(tList)*[numpy.mean(X)])
        
        velocities, accelerations = velocityAcceleration(self.locations,time_steps)
        
        for d in range(0, len(out_locations)):
            percentage = (d+D)/(D+len(out_locations))
            sys.stdout.write('\r {:.2f}%'.format(100*percentage))
            sys.stdout.flush()
            
            X = [p[d] for p in self.locations]
            XDelta = []
            XAcc = []
            TDelta = []
            TAcc = []
            
            if useVelocity:
                XDelta = [p[d] for p in velocities]
                TDelta = dt
                
            if useAcceleration:
                XAcc = [p[d] for p in accelerations]
                TAcc = ddt

            if(numpy.var(X) > 0):
                s, resP, _, _  = MotionExtractor.fitPoints(tList, t, X, [], [], [], [],translationRegressionModels[d], 1, 1,1)#velWeight, accWeight)
                _, _, resV, _  = MotionExtractor.fitPoints(tList, [], [], TDelta, XDelta, [], [],translationRegressionModels[d], 1, 1,1)#velWeight, accWeight)
                _, _, _, resA  = MotionExtractor.fitPoints(tList, [],[], [], [], TAcc, XAcc,translationRegressionModels[d], 1, 1,1)#velWeight, accWeight)

                sigmaP = numpy.sqrt(numpy.sum(numpy.power(resP,2))/len(t))
                sigmaV = numpy.sqrt(numpy.sum(numpy.power(resV,2))/len(TDelta))
                sigmaA = numpy.sqrt(numpy.sum(numpy.power(resA,2))/len(TAcc))
                
                s, resP, resV, resA = MotionExtractor.fitPoints(tList, t, X,TDelta, XDelta, TAcc, XAcc, translationRegressionModels[d], 1, sigmaP/sigmaV, sigmaP/sigmaA) 
                
                out_locations[d] = s #- min(s)
            else:
                s = numpy.array(len(tList)*[numpy.mean(X)])
                out_locations[d] = s 

        print('\r {:.2f}%'.format(100))
        
        return skeletons,len(skeletons)*[out_locations]
   
    @staticmethod 
    def unwrap_simple(vals):
        lastP = vals[0]
        np = [lastP]
        offset = 0.0
        for p in vals[1:]:
            offset = math.floor(lastP-p+0.5)
            lastP = p+offset
            np.append(lastP)        
        
        return np
             
    @staticmethod 
    def uniformlyDistributeSamples(cycle_map):
        V = [(i, cycle_map[i]) for i in range(0, len(cycle_map))]
        V.sort(key=lambda tup : tup[1])
        V = [(V[i][0], i / len(V)) for i in range(0, len(V))]
        V.sort(key=lambda tup : tup[0])
        V = numpy.array([c[1] for c in V])
        return V

    @staticmethod
    def fitPoints(sampledT, tList, yList, dTList, dYList, ddTList, ddYList, regressionModel, posWeights, velWeights, accWeights):

        #import statsmodels.stats.diagnostic as smdiag
        
        #import statsmodels.api as sm
        #import statsmodels.robust.norms as smnorm
        pos = [yList[i] * posWeights for i in range(0, len(yList))]
        
        vel = [dYList[i] * velWeights for i in range(0, len(dYList))]
        acc = [ddYList[i] * accWeights for i in range(0, len(ddYList))]
        b = numpy.array(pos + vel + acc)
        
        rows = [ posWeights * numpy.array(regressionModel.positionModel(tList[i])) for i in range(0, len(tList))]
        deltaRows = [ velWeights * numpy.array(regressionModel.velocityModel(dTList[i])) for i in range(0, len(dYList))]
        ddRows = [accWeights * numpy.array(regressionModel.accelerationModel(ddTList[i])) for i in range(0, len(ddYList))]
        
        A = numpy.matrix(rows + deltaRows + ddRows)
        
        if len(b) == 0:
            return ([],[],[],[])
        (w, residuals, rank, sing_vals) = numpy.linalg.lstsq(A, b)

        #model = sm.RLM(b, A, M=smnorm.RamsayE())
        #model = sm.OLS(b, A)
        #result = model.fit()

        #w = result.params

        positions = [ numpy.sum(regressionModel.positionModel(t, w)) for t in sampledT]
        
        residuals1 = posWeights*(numpy.array(yList) - numpy.array([ numpy.sum(regressionModel.positionModel(t, w)) for t in tList]))
        residuals2 = velWeights*(numpy.array(dYList) - numpy.array([ numpy.sum(regressionModel.velocityModel(t, w)) for t in dTList]))
        residuals3 = accWeights*(numpy.array(ddYList) - numpy.array([ numpy.sum(regressionModel.accelerationModel(t, w)) for t in ddTList]))

        return (positions, residuals1,residuals2,residuals3)            
        
    @staticmethod
    def delay_embbed(points,n):
        D = len(points[0])
        N = len(points)
        
        delay_embedded = [(n*D)*[0] for i in range(0,N-n)]
        
        for i in range(0,N-n):
            for k in range(0,n):
                for j in range(0,D):
                    delay_embedded[i][j+k*D] = points[i+k][j]
                    
        return delay_embedded
