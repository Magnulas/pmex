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

class ExtractionPanel(bpy.types.Panel):
    bl_label = "Periodic Motion Extraction"
    bl_idname = "pmex.panel"
    bl_space_type = "VIEW_3D"
    bl_region_type = "TOOLS"
    bl_options = {'DEFAULT_CLOSED'}
    bl_category = "Tools"
    
        
    def draw(self, context):
        layout = self.layout
        props = context.scene.extractionProperties
        
        layout.prop(props,'enable_advanced')
        
        inputBox = layout.box()
        inputBox.label(text="Input")
        operator = inputBox.operator("pmex.extractionoperator", text = "Periodic Motion Extraction")
        inputBox.prop(props, 'dmax')
        #inputBox.prop(props, 'in_frame_rate') 
        
        inputBox.prop(props, 'sampling_density')
        
        inputBox.prop(props, 'limit_input_range')
        if props.limit_input_range:
            box = inputBox.box()
            box.prop(props, 'start_frame')
            box.prop(props, 'frame_range')
            
        if props.enable_advanced:
            inputBox.prop(props,'delay_embedding')
            inputBox.prop(props,'prime')

        outputBox = layout.box()
        outputBox.label(text="Output")
        #outputBox.prop(props, 'out_frame_rate') 
        outputBox.prop(props, 'cycels')
        
        if props.enable_advanced:
            outputBox.prop(props, 'model_complexity')

        outputBox.prop(props, 'use_positions')

        if props.use_positions:
            translationBox = outputBox.box()
            if props.enable_advanced:
                translationBox.prop(props, 'translation_model_complexity')
                
            row = translationBox.row() 
            row.alignment = 'LEFT'
            row.label("Axis")
            row.label("Regression Model")
            row = translationBox.row()
            row.alignment = 'LEFT'
            row.prop(props, 'use_positions_X')
            row.prop(props,'positions_X_model')
            row = translationBox.row() 
            row.alignment = 'LEFT'
            row.prop(props, 'use_positions_Y')
            row.prop(props,'positions_Y_model')
            row = translationBox.row() 
            row.alignment = 'LEFT'
            row.prop(props, 'use_positions_Z')
            row.prop(props,'positions_Z_model')
                  
        if props.enable_advanced:
            outputBox.prop(props,'use_velocity')
            outputBox.prop(props,'use_acceleration') 
            if props.use_acceleration or props.use_velocity:
                outputBox.prop(props,'accuracy')
        
        if props.enable_advanced:
            analyticsBox = layout.box()
            analyticsBox.label(text="Analytics")
            analyticsBox.prop(props,'manual_cocycle_selection')
            analyticsBox.prop(props,'plot_pca')
            if props.plot_pca:
                analyticsBox.prop(props,'plot_output_folder')
            
class PoseExtractionPanel(ExtractionPanel):
    bl_idname = "pmex.posepanel"
    bl_context = "posemode"
    
class ObjectExtractionPanel(ExtractionPanel):   
    bl_idname = "pmex.objectpanel"
    bl_context = "objectmode"
