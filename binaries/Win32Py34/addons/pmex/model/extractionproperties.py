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

import bpy.props

class ExtractionProperties(bpy.types.PropertyGroup):
    dmax = bpy.props.FloatProperty(name="Maximum Distance", default=1.0, min=0)
    #in_frame_rate = bpy.props.IntProperty(name="Frame rate", default=120, min=1)
    
    limit_input_range = bpy.props.BoolProperty( name="Limit input size", default=False)
    start_frame = bpy.props.IntProperty(name="Start Frame", default=0, min=0)
    frame_range = bpy.props.IntProperty(name="Frame Range", default=300, min=1)
    
    sampling_density = bpy.props.IntProperty(name="Sampling density", default=1, min=1)
        
    use_custom_functions = bpy.props.BoolProperty(name="Use custom model", default=False)
    regression_model = bpy.props.StringProperty(name="Regression Model",default="cohomology.regression.HarmonicRegression")

    #out_frame_rate = bpy.props.IntProperty(name="Frame rate", default=120, min=1)
    cycels = bpy.props.IntProperty(name="Number of output actions", default=1, min=1)
    prime = bpy.props.IntProperty(name="Prime", default=11, min=3)
    
    enable_advanced = bpy.props.BoolProperty(name="Advanced options", default=False)
    delay_embedding =  bpy.props.IntProperty(name="Delay embedding", default=2, min=0)
    
    use_positions = bpy.props.BoolProperty(name="Compute translation", default=True)
    use_positions_X = bpy.props.BoolProperty(name="X", default=True)
    use_positions_Y = bpy.props.BoolProperty(name="Y", default=True)
    use_positions_Z = bpy.props.BoolProperty(name="Z", default=True)
    
    
    #e1 = bpy.types.EnumPropertyItem()
    #e2 = bpy.types.EnumPropertyItem(name="2")
    #model_enum = [e1,e2]
    
    #model_enum = [bpy.types.EnumPropertyItem(name="1"),bpy.types.EnumPropertyItem(name="2")]
    
    positions_X_model = bpy.props.EnumProperty(name="Regression Model",items=[("Linear_Harmonic", "Periodic with offset", "A harmonic regression model with an added linear component"),("Harmonic", "Periodic", "A harmonic regression model")])
    positions_Y_model = bpy.props.EnumProperty(name="Regression Model",items=[("Linear_Harmonic", "Periodic with offset", "A harmonic regression model with an added linear component"),("Harmonic", "Periodic", "A harmonic regression model")])
    positions_Z_model = bpy.props.EnumProperty(name="Regression Model",items=[("Linear_Harmonic", "Periodic with offset", "A harmonic regression model with an added linear component"),("Harmonic", "Periodic", "A harmonic regression model")])
    
    #positions_Y_model = bpy.props.EnumProperty(enum_items=model_enum)
    #positions_Z_model = bpy.props.EnumProperty(enum_items=model_enum)
    
    use_velocity = bpy.props.BoolProperty(name="Use joint velocity", default=True)
    use_acceleration = bpy.props.BoolProperty(name="Use joint acceleration", default=True)
    
    model_complexity = bpy.props.IntProperty(name="Joint model complexity", default=20, min=1)
    translation_model_complexity = bpy.props.IntProperty(name="Translation model complexity", default=20, min=1)
    
    plot_output_folder = bpy.props.StringProperty(name="Plot output folder",default="plots")
    plot_pca = bpy.props.BoolProperty(name="PCA plot motion", default=False)
    manual_cocycle_selection = bpy.props.BoolProperty(name="Select cocycels manually", default=False)