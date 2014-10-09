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

import bpy.types

from math import floor, sqrt, ceil,isnan
from numpy import linspace,round, var, sin, cos, arcsin, arccos

#import importlib
import os
import re
import time
import numpy

from pmex.core.motionextractor import MotionExtractor
from pmex.regression.harmonicregression import HarmonicRegression 
from pmex.regression.linearharmonicregression import LinearHarmonicRegression
from pmex.view.persistendiagramwidget import PersistenDiagramWidget
from pmex.core.utilitary import PCAPlot
        
class ExtractionOperator(bpy.types.Operator):
    """The operation tries to detect periodic motions in motion capture data and constructs a set of keyframes describing the found periodic motion."""
    bl_idname = "pmex.extractionoperator"
    bl_label = "Periodic Motion Extraction"
    bl_options = {'REGISTER', 'PRESET'}

    def execute(self, context):
        
        options = context.scene.extractionProperties
        
        for blend_object in context.selected_objects:
            action = blend_object.animation_data.action
            print("Finding periodic motion for action",action.name)
            
            root_bone, bones = selectBones(blend_object,context)
            
            points,data_paths,locations,loc_paths,loc_models = createPointCloud(root_bone,bones,action,options)
            
            paths = data_paths + loc_paths

            delay_embedding = 0
            if options.enable_advanced:
                delay_embedding = options.delay_embedding

            print(time.asctime(),"Constructing simplicial complex and cocycels.")
            motext = MotionExtractor(points,options.dmax,delay_embedding,locations=locations,prime=options.prime)
            print(time.asctime(),"Complex constructed.")
            if options.enable_advanced and options.manual_cocycle_selection:
                
                ccls = motext.getSCO().getCocycels()
                if len(ccls) > 0:
                    wid = PersistenDiagramWidget([b[1] for b in ccls],[b[2] for b in ccls])
                    print(time.asctime(),"Waiting for input. Select one or more cocycle.")  
                    indices = wid.execute()
                else:
                    indices = []
            else:
                print(time.asctime(),"Selecting",options.cycels,"most persistent cocycels.")
                indices = range(0,min(motext.getCocycleCount(),options.cycels))

            if len(indices) == 0:
                print("No cocycels found at distance",options.dmax)

            skeletons = []
            
            for i in range(0,len(indices)):
                print(time.asctime(),"Constructing action",i+1,"of",len(indices))
                #try:
                skeleton, loc = motext.getPeriodicMotion(indices[i],action.name,options.use_velocity,options.use_acceleration,HarmonicRegression(options.model_complexity),loc_models)
                for j in range(0,len(skeleton)):
                    skeletons.append(skeleton[j] + loc[j])
                    if options.plot_pca:
                        print(time.asctime(),"Plotting pca projection of action",i+1,"of",len(indices))
                        PCAPlot(motext.getPoints(),MotionExtractor.uniformlyDistributeSamples(motext.getSCO().getCircularMapping(indices[i])),skeleton[j],options.plot_output_folder, action.name + "." + str(i) + "_" + str(j))
                        PCAPlot(motext.getPoints(),MotionExtractor.uniformlyDistributeSamples(motext.getSCO().getCircularMapping(indices[i])),[],options.plot_output_folder, action.name + ".empty" + str(i))
                #except Exception as err:
                    #print(err)
                    #print("Failed to lift prime cocycle to integer cocycle. Try running the operation again with a different prime number.")
                    
            #skeletons.append(skeleton + loc)
                
            
            print(time.asctime(),"Outputing actions")
                
            createActions(blend_object,skeletons,paths,options.sampling_density)
            
            print("Finished extracting periodic motion for action ",action.name)
            
        return {'FINISHED'}       # this lets blender know the operator finished successfully
    
    def invoke(self, context, event):
        return self.execute(context)

#def importFunction(name):
#    strs = name.rsplit(".",1)
#    moduleName = strs[0]
#    functionName = strs[1]
#    module = importlib.import_module(moduleName)
#    func = getattr(module, functionName)
#    return module, func

def selectBones(blend_object,context):
    bones = []
    root_bone = None

    if bpy.context.mode == 'POSE':
        for b in context.selected_pose_bones:
            bones.append(b)
            if b.parent == None:
                root_bone = b
    else:
        for b in blend_object.data.bones:
            bones.append(b)
            if b.parent == None:
                root_bone = b

    return root_bone, bones

def createPointCloud(root_bone,bones,action,options):
    
    #Create lookup dictionary for bone names
    index_dictionary = {}

    for i in range(0,len(action.fcurves)):
        index_dictionary[action.fcurves[i].data_path] = []
        
    for i in range(0,len(action.fcurves)):
        index_dictionary[action.fcurves[i].data_path].append(i)
    
    start_frame = action.frame_range[0]
    end_frame = action.frame_range[1]
    
    if options.limit_input_range:
        end_frame = min(end_frame, options.frame_range + options.start_frame)
        start_frame = min(end_frame,options.start_frame)

    frame_indices = range(int(start_frame-action.frame_range[0]),
                           int(end_frame-action.frame_range[0]),options.sampling_density)


    curves = []
#    root_bone_indices = []
    locations = []

    curve_paths = []
    loc_paths = []
    loc_models = []
    #root_bone = None

    for b in bones:
        
        d_path = "pose.bones[\"" + b.name + "\"].rotation_euler"

        if d_path in index_dictionary:
            for i in index_dictionary[d_path]:
                curve,path = getCurve(frame_indices,action.fcurves[i])
#                if b == root_bone:
#                    curves.append(cos(curve))
#                    curve_paths.append(path)
#                    
#                    curves.append(sin(curve))
#                    curve_paths.append(path)
#                    
#                    root_bone_indices.append((len(curves)-2,len(curves)-1))
#                else:   
#                    curves.append(curve)
#                    curve_paths.append(path)
#                if b == root_bone:
#                    root_bone_indices.append(len(curves)-1)

                curves.append(curve)
                curve_paths.append(path)
                
        d_path_loc = "pose.bones[\"" + b.name + "\"].location"

        if options.use_positions and d_path_loc in index_dictionary and b == root_bone:

            for i in index_dictionary[d_path_loc]:
                array_ind = action.fcurves[i].array_index
                
                model_enums = [options.positions_X_model,options.positions_Y_model,options.positions_Z_model]
                
                if (array_ind == 0 and options.use_positions_X) or (array_ind == 1 and options.use_positions_Y) or (array_ind == 2 and options.use_positions_Z):
                    curve,path = getCurve(frame_indices,action.fcurves[i])
                    locations.append(curve)                                
                    loc_paths.append(path)
                    
                    if model_enums[array_ind]=="Linear_Harmonic":
                        loc_models.append(LinearHarmonicRegression(options.translation_model_complexity))
                    else:
                        loc_models.append(HarmonicRegression(options.translation_model_complexity))

#    dummy_dim = len(curves)*[False]
#    d = 0
#    for d in range(0,len(curves)):
#        if var(curves[d]) == 0:
#            dummy_dim[d] = True
    
#    dim = len([dummy for dummy in dummy_dim if not(dummy)])
    dim = len(curves)
    point_cloud = [ dim*[0] for i in range(0,len(frame_indices)) ]
    for d in range(0,dim):
        for i in range(0,len(point_cloud)):
            point_cloud[i][d] = curves[d][i]            
    
    
    
    dim = len(locations)
    locations_cloud = [ dim*[0] for i in range(0,len(frame_indices)) ]
    for d in range(0,dim):
        for i in range(0,len(locations_cloud)):
            locations_cloud[i][d] = locations[d][i]   
    
    return point_cloud,curve_paths,locations_cloud,loc_paths, loc_models #,root_bone_indices

def getCurve(frame_indices,fcurve):
    frames = len(frame_indices)
    curve = frames*[0]
    path = (fcurve.data_path,fcurve.array_index)
    j = 0

    for j in range(0,frames):
        time = frame_indices[j]
        curve[j] = fcurve.keyframe_points[time].co[1]
        j = j + 1
        
    return numpy.array(curve), path


def createActions(blend_object,skeletons,data_paths, density=1):

    i = 0
    name = blend_object.animation_data.action.name
    for skeleton_points in skeletons:
        dim = len(skeleton_points)
        new_name = "{0}{1}{2}{3}".format("Periodic",name, ".",i)
        action_index = bpy.data.actions.find(new_name)
                
        if action_index != -1:
            bpy.data.actions.remove(bpy.data.actions[action_index])
                 
        bpy.data.actions.new(name=new_name);
        new_action = bpy.data.actions[new_name]
        new_action.use_fake_user = True
                
        d = 0
#        bone_ind = 0
        while d < dim:
#            if bone_ind < len(root_bone_indices) and d == root_bone_indices[bone_ind][0]:
#                points = coordinates_to_radians(skeleton_points[d],skeleton_points[d+1])
#                d = d+1
#                bone_ind = bone_ind + 1
#            else:   
            points = skeleton_points[d]

            path = data_paths[d][0]
            index = data_paths[d][1]
            fcurve = new_action.fcurves.new(data_path=path, index=index)
            fcurve.keyframe_points.add(len(skeleton_points[d]))
            j = 0

            for time in range(0,len(skeleton_points[d])):#range(0,self.nframes):
                
                value = points[time]
                fcurve.keyframe_points[j].co = j*density, value
                fcurve.keyframe_points[j].interpolation = "LINEAR"
                j = j + 1
                  
            modifier = fcurve.modifiers.new(type="CYCLES")
            fcurve.extrapolation = "LINEAR"
            if not(re.match(".*location$",data_paths[d][0]) == None):
                modifier.mode_after = 'REPEAT_OFFSET'
                modifier.mode_before = 'REPEAT_OFFSET'
            d = d + 1

        if i == 0:
            blend_object.animation_data.action.use_fake_user = True
            blend_object.animation_data.action = new_action
        i = i + 1
        
#def coordinates_to_radians(coss,sins):
#    
#    r = numpy.power(sins,2) + numpy.power(coss,2)
#    x = coss/numpy.sqrt(r)
#    y = sins/numpy.sqrt(r)
#    
#    radians = numpy.arccos(x)
#    offset = numpy.arccos(-x)
#
#    for i in range(0,len(radians)):
#        if y[i] < 0:
#            radians[i] = radians[i] + 2*offset[i]
#   
#    return radians
