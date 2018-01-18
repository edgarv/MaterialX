#include "stdlib/impl/shadergen/source/glsl/aastep.glsl"

void mx_splittb_vector3(vec3 valuet, vec3 valueb, float center, vec2 texcoord, out vec3 result)
{
    result = mix(valuet, valueb, aastep (center, texcoord.y));
}
