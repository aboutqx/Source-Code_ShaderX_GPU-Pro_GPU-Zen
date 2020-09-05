#ifndef RENDER_STATES_H
#define RENDER_STATES_H

#define NUM_UNIFORM_BUFFER_BP        3 // number of uniform-buffer binding points, used in this demo
#define NUM_CUSTOM_UNIFORM_BUFFER_BP 2 // number of custom uniform-buffer binding points, used in this demo
#define NUM_TEXTURE_BP             	 9 // number of texture binding points, used in this demo
#define NUM_STRUCTURED_BUFFER_BP   	 3 // number of structured-buffer binding points, used in this demo
#define NUM_SHADER_TYPES           	 4 // number of shader types, used in this demo

// uniform-buffer binding points
enum uniformBufferBP
{
  CAMERA_UB_BP=0,
  CUSTOM0_UB_BP,
  CUSTOM1_UB_BP
};

// texture binding points
enum textureBP
{
  COLOR_TEX_BP=0,
  NORMAL_TEX_BP,
  SPECULAR_TEX_BP,
  CUSTOM0_TEX_BP,
  CUSTOM1_TEX_BP,
  CUSTOM2_TEX_BP,
  CUSTOM3_TEX_BP,
  CUSTOM4_TEX_BP,
  CUSTOM5_TEX_BP
};

enum textureID
{
  COLOR_TEX_ID=COLOR_TEX_BP,
  NORMAL_TEX_ID=NORMAL_TEX_BP,
  SPECULAR_TEX_ID=SPECULAR_TEX_BP,
  CUSTOM0_TEX_ID=CUSTOM0_TEX_BP,
  CUSTOM1_TEX_ID=CUSTOM1_TEX_BP,
  CUSTOM2_TEX_ID=CUSTOM2_TEX_BP,
  CUSTOM3_TEX_ID=CUSTOM3_TEX_BP,
  CUSTOM4_TEX_ID=CUSTOM4_TEX_BP,
  CUSTOM5_TEX_ID=CUSTOM5_TEX_BP
};

// structured-buffer binding points
enum structuredBufferBP
{
  CUSTOM0_SB_BP=0,
  CUSTOM1_SB_BP,
  CUSTOM2_SB_BP
};

enum shaderTypes
{
  VERTEX_SHADER=0,
  GEOMETRY_SHADER,
  FRAGMENT_SHADER,
  COMPUTE_SHADER
};

enum blendOptions
{
  ZERO_BLEND=GL_ZERO, 
  ONE_BLEND=GL_ONE,
  SRC_COLOR_BLEND=GL_SRC_COLOR,
  INV_SRC_COLOR_BLEND=GL_ONE_MINUS_SRC_COLOR, 
  DST_COLOR_BLEND=GL_DST_COLOR, 
  INV_DST_COLOR_BLEND=GL_ONE_MINUS_DST_COLOR,
  SRC_ALPHA_BLEND=GL_SRC_ALPHA,
  INV_SRC_ALPHA_BLEND=GL_ONE_MINUS_SRC_ALPHA, 
  DST_ALPHA_BLEND=GL_DST_ALPHA, 
  INV_DST_ALPHA_BLEND=GL_ONE_MINUS_DST_ALPHA, 
  CONST_COLOR_BLEND=GL_CONSTANT_COLOR, 
  INV_CONST_COLOR_BLEND=GL_ONE_MINUS_CONSTANT_COLOR, 
  CONST_ALPHA_BLEND=GL_CONSTANT_ALPHA, 
  INV_CONST_ALPHA_BLEND=GL_ONE_MINUS_CONSTANT_ALPHA,
  SRC_ALPHA_SAT_BLEND=GL_SRC_ALPHA_SATURATE,
  SRC1_COLOR_BLEND=GL_SRC1_COLOR,
  INV_SRC1_COLOR_BLEND=GL_ONE_MINUS_SRC1_COLOR,
  SRC1_ALPHA_BLEND=GL_SRC1_ALPHA,
  INV_SRC1_ALPHA_BLEND=GL_ONE_MINUS_SRC1_ALPHA
};

enum blendOps
{ 
  ADD_BLEND_OP=GL_FUNC_ADD,
  SUBTRACT_BLEND_OP=GL_FUNC_SUBTRACT,
  REV_SUBTRACT_BLEND_OP=GL_FUNC_REVERSE_SUBTRACT,
  MIN_BLEND_OP=GL_MIN,
  MAX_BLEND_OP=GL_MAX
};

enum colorMaskBits
{	
  RED_COLOR_MASK=1,
  GREEN_COLOR_MASK=2,
  BLUE_COLOR_MASK=4,
  ALPHA_COLOR_MASK=8,
  ALL_COLOR_MASK = (RED_COLOR_MASK | GREEN_COLOR_MASK | BLUE_COLOR_MASK | ALPHA_COLOR_MASK)
};

enum clearBits
{
  COLOR_CLEAR_BIT=GL_COLOR_BUFFER_BIT,
  DEPTH_CLEAR_BIT=GL_DEPTH_BUFFER_BIT,
  STENCIL_CLEAR_BIT=GL_STENCIL_BUFFER_BIT
};

enum comparisonFuncs
{
  NEVER_COMP_FUNC=GL_NEVER,
  LESS_COMP_FUNC=GL_LESS,
  LEQUAL_COMP_FUNC=GL_LEQUAL,
  EQUAL_COMP_FUNC=GL_EQUAL,
  GREATER_COMP_FUNC=GL_GREATER,
  NOTEQUAL_COMP_FUNC=GL_NOTEQUAL,
  GEQUAL_COMP_FUNC=GL_GEQUAL,
  ALWAYS_COMP_FUNC=GL_ALWAYS
};

enum stencilOps
{
  KEEP_STENCIL_OP=GL_KEEP,
  ZERO_STENICL_OP=GL_ZERO,
  REPLACE_STENCIL_OP=GL_REPLACE,
  INCR_SAT_STENCIL_OP=GL_INCR,
  DECR_SAT_STENCIL_OP=GL_DECR,
  INVERT_STENICL_OP=GL_INVERT,
  INCR_STENCIL_OP=GL_INCR_WRAP,
  DECR_STENCIL_OP=GL_DECR_WRAP
};

enum fillModes
{
  SOLID_FILL=GL_FILL,
  WIREFRAME_FILL=GL_LINE
};

enum cullModes
{
  NONE_CULL=0,
  FRONT_CULL=GL_FRONT, 
  BACK_CULL=GL_BACK 
};

enum filterModes
{
  MIN_MAG_POINT_FILTER=0,      
  MIN_POINT_MAG_LINEAR_FILTER,
  MIN_LINEAR_MAG_POINT_FILTER, 
  MIN_MAG_LINEAR_FILTER,  
  MIN_MAG_MIP_POINT_FILTER,
  MIN_MAG_POINT_MIP_LINEAR_FILTER,                
  MIN_POINT_MAG_LINEAR_MIP_POINT_FILTER,
  MIN_POINT_MAG_MIP_LINEAR_FILTER,
  MIN_LINEAR_MAG_MIP_POINT_FILTER, 
  MIN_LINEAR_MAG_POINT_MIP_LINEAR_FILTER,
  MIN_MAG_LINEAR_MIP_POINT_FILTER,  
  MIN_MAG_MIP_LINEAR_FILTER,  
  ANISOTROPIC_FILTER,
  COMP_MIN_MAG_POINT_FILTER,
  COMP_MIN_POINT_MAG_LINEAR_FILTER,
  COMP_MIN_LINEAR_MAG_POINT_FILTER,
  COMP_MIN_MAG_LINEAR_FILTER,
  COMP_MIN_MAG_MIP_POINT_FILTER,
  COMP_MIN_MAG_POINT_MIP_LINEAR_FILTER,  
  COMP_MIN_POINT_MAG_LINEAR_MIP_POINT_FILTER,
  COMP_MIN_POINT_MAG_MIP_LINEAR_FILTER,
  COMP_MIN_LINEAR_MAG_MIP_POINT_FILTER,
  COMP_MIN_LINEAR_MAG_POINT_MIP_LINEAR_FILTER,
  COMP_MIN_MAG_LINEAR_MIP_POINT_FILTER,
  COMP_MIN_MAG_MIP_LINEAR_FILTER,
  COMP_ANISOTROPIC_FILTER
};

enum texAdressModes
{
  REPEAT_TEX_ADDRESS=GL_REPEAT,
  MIRROR_TEX_ADDRESS=GL_MIRRORED_REPEAT,
  CLAMP_TEX_ADRESS=GL_CLAMP_TO_EDGE,
  BORDER_TEX_ADDRESS=GL_CLAMP_TO_BORDER
};

enum primitiveTypes
{
  POINTS_PRIMITIVE=GL_POINTS, 
  LINES_PRIMITIVE=GL_LINES,
  LINE_STRIP_PRIMITIVE=GL_LINE_STRIP, 
  TRIANGLES_PRIMITIVE=GL_TRIANGLES,
  TRIANGLE_STRIP_PRIMITIVE=GL_TRIANGLE_STRIP
};

enum timerQueryModes
{
  BEGIN_TIMER_QUERY=0,
  END_TIMER_QUERY
};

#endif