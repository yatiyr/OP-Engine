set(PROJECT_NAME Opium)

################################################################################
# Source groups
################################################################################
set(external__glm__glm
    "external/glm/glm/common.hpp"
    "external/glm/glm/exponential.hpp"
    "external/glm/glm/ext.hpp"
    "external/glm/glm/fwd.hpp"
    "external/glm/glm/geometric.hpp"
    "external/glm/glm/glm.hpp"
    "external/glm/glm/integer.hpp"
    "external/glm/glm/mat2x2.hpp"
    "external/glm/glm/mat2x3.hpp"
    "external/glm/glm/mat2x4.hpp"
    "external/glm/glm/mat3x2.hpp"
    "external/glm/glm/mat3x3.hpp"
    "external/glm/glm/mat3x4.hpp"
    "external/glm/glm/mat4x2.hpp"
    "external/glm/glm/mat4x3.hpp"
    "external/glm/glm/mat4x4.hpp"
    "external/glm/glm/matrix.hpp"
    "external/glm/glm/packing.hpp"
    "external/glm/glm/trigonometric.hpp"
    "external/glm/glm/vec2.hpp"
    "external/glm/glm/vec3.hpp"
    "external/glm/glm/vec4.hpp"
    "external/glm/glm/vector_relational.hpp"
)
source_group("external\\glm\\glm" FILES ${external__glm__glm})

set(external__glm__glm__detail
    "external/glm/glm/detail/_features.hpp"
    "external/glm/glm/detail/_fixes.hpp"
    "external/glm/glm/detail/_noise.hpp"
    "external/glm/glm/detail/_swizzle.hpp"
    "external/glm/glm/detail/_swizzle_func.hpp"
    "external/glm/glm/detail/_vectorize.hpp"
    "external/glm/glm/detail/compute_common.hpp"
    "external/glm/glm/detail/compute_vector_relational.hpp"
    "external/glm/glm/detail/func_common.inl"
    "external/glm/glm/detail/func_common_simd.inl"
    "external/glm/glm/detail/func_exponential.inl"
    "external/glm/glm/detail/func_exponential_simd.inl"
    "external/glm/glm/detail/func_geometric.inl"
    "external/glm/glm/detail/func_geometric_simd.inl"
    "external/glm/glm/detail/func_integer.inl"
    "external/glm/glm/detail/func_integer_simd.inl"
    "external/glm/glm/detail/func_matrix.inl"
    "external/glm/glm/detail/func_matrix_simd.inl"
    "external/glm/glm/detail/func_packing.inl"
    "external/glm/glm/detail/func_packing_simd.inl"
    "external/glm/glm/detail/func_trigonometric.inl"
    "external/glm/glm/detail/func_trigonometric_simd.inl"
    "external/glm/glm/detail/func_vector_relational.inl"
    "external/glm/glm/detail/func_vector_relational_simd.inl"
    "external/glm/glm/detail/qualifier.hpp"
    "external/glm/glm/detail/setup.hpp"
    "external/glm/glm/detail/type_float.hpp"
    "external/glm/glm/detail/type_half.hpp"
    "external/glm/glm/detail/type_half.inl"
    "external/glm/glm/detail/type_mat2x2.hpp"
    "external/glm/glm/detail/type_mat2x2.inl"
    "external/glm/glm/detail/type_mat2x3.hpp"
    "external/glm/glm/detail/type_mat2x3.inl"
    "external/glm/glm/detail/type_mat2x4.hpp"
    "external/glm/glm/detail/type_mat2x4.inl"
    "external/glm/glm/detail/type_mat3x2.hpp"
    "external/glm/glm/detail/type_mat3x2.inl"
    "external/glm/glm/detail/type_mat3x3.hpp"
    "external/glm/glm/detail/type_mat3x3.inl"
    "external/glm/glm/detail/type_mat3x4.hpp"
    "external/glm/glm/detail/type_mat3x4.inl"
    "external/glm/glm/detail/type_mat4x2.hpp"
    "external/glm/glm/detail/type_mat4x2.inl"
    "external/glm/glm/detail/type_mat4x3.hpp"
    "external/glm/glm/detail/type_mat4x3.inl"
    "external/glm/glm/detail/type_mat4x4.hpp"
    "external/glm/glm/detail/type_mat4x4.inl"
    "external/glm/glm/detail/type_mat4x4_simd.inl"
    "external/glm/glm/detail/type_quat.hpp"
    "external/glm/glm/detail/type_quat.inl"
    "external/glm/glm/detail/type_quat_simd.inl"
    "external/glm/glm/detail/type_vec1.hpp"
    "external/glm/glm/detail/type_vec1.inl"
    "external/glm/glm/detail/type_vec2.hpp"
    "external/glm/glm/detail/type_vec2.inl"
    "external/glm/glm/detail/type_vec3.hpp"
    "external/glm/glm/detail/type_vec3.inl"
    "external/glm/glm/detail/type_vec4.hpp"
    "external/glm/glm/detail/type_vec4.inl"
    "external/glm/glm/detail/type_vec4_simd.inl"
)
source_group("external\\glm\\glm\\detail" FILES ${external__glm__glm__detail})

set(external__glm__glm__ext
    "external/glm/glm/ext/matrix_clip_space.hpp"
    "external/glm/glm/ext/matrix_clip_space.inl"
    "external/glm/glm/ext/matrix_common.hpp"
    "external/glm/glm/ext/matrix_common.inl"
    "external/glm/glm/ext/matrix_double2x2.hpp"
    "external/glm/glm/ext/matrix_double2x2_precision.hpp"
    "external/glm/glm/ext/matrix_double2x3.hpp"
    "external/glm/glm/ext/matrix_double2x3_precision.hpp"
    "external/glm/glm/ext/matrix_double2x4.hpp"
    "external/glm/glm/ext/matrix_double2x4_precision.hpp"
    "external/glm/glm/ext/matrix_double3x2.hpp"
    "external/glm/glm/ext/matrix_double3x2_precision.hpp"
    "external/glm/glm/ext/matrix_double3x3.hpp"
    "external/glm/glm/ext/matrix_double3x3_precision.hpp"
    "external/glm/glm/ext/matrix_double3x4.hpp"
    "external/glm/glm/ext/matrix_double3x4_precision.hpp"
    "external/glm/glm/ext/matrix_double4x2.hpp"
    "external/glm/glm/ext/matrix_double4x2_precision.hpp"
    "external/glm/glm/ext/matrix_double4x3.hpp"
    "external/glm/glm/ext/matrix_double4x3_precision.hpp"
    "external/glm/glm/ext/matrix_double4x4.hpp"
    "external/glm/glm/ext/matrix_double4x4_precision.hpp"
    "external/glm/glm/ext/matrix_float2x2.hpp"
    "external/glm/glm/ext/matrix_float2x2_precision.hpp"
    "external/glm/glm/ext/matrix_float2x3.hpp"
    "external/glm/glm/ext/matrix_float2x3_precision.hpp"
    "external/glm/glm/ext/matrix_float2x4.hpp"
    "external/glm/glm/ext/matrix_float2x4_precision.hpp"
    "external/glm/glm/ext/matrix_float3x2.hpp"
    "external/glm/glm/ext/matrix_float3x2_precision.hpp"
    "external/glm/glm/ext/matrix_float3x3.hpp"
    "external/glm/glm/ext/matrix_float3x3_precision.hpp"
    "external/glm/glm/ext/matrix_float3x4.hpp"
    "external/glm/glm/ext/matrix_float3x4_precision.hpp"
    "external/glm/glm/ext/matrix_float4x2.hpp"
    "external/glm/glm/ext/matrix_float4x2_precision.hpp"
    "external/glm/glm/ext/matrix_float4x3.hpp"
    "external/glm/glm/ext/matrix_float4x3_precision.hpp"
    "external/glm/glm/ext/matrix_float4x4.hpp"
    "external/glm/glm/ext/matrix_float4x4_precision.hpp"
    "external/glm/glm/ext/matrix_int2x2.hpp"
    "external/glm/glm/ext/matrix_int2x2_sized.hpp"
    "external/glm/glm/ext/matrix_int2x3.hpp"
    "external/glm/glm/ext/matrix_int2x3_sized.hpp"
    "external/glm/glm/ext/matrix_int2x4.hpp"
    "external/glm/glm/ext/matrix_int2x4_sized.hpp"
    "external/glm/glm/ext/matrix_int3x2.hpp"
    "external/glm/glm/ext/matrix_int3x2_sized.hpp"
    "external/glm/glm/ext/matrix_int3x3.hpp"
    "external/glm/glm/ext/matrix_int3x3_sized.hpp"
    "external/glm/glm/ext/matrix_int3x4.hpp"
    "external/glm/glm/ext/matrix_int3x4_sized.hpp"
    "external/glm/glm/ext/matrix_int4x2.hpp"
    "external/glm/glm/ext/matrix_int4x2_sized.hpp"
    "external/glm/glm/ext/matrix_int4x3.hpp"
    "external/glm/glm/ext/matrix_int4x3_sized.hpp"
    "external/glm/glm/ext/matrix_int4x4.hpp"
    "external/glm/glm/ext/matrix_int4x4_sized.hpp"
    "external/glm/glm/ext/matrix_integer.hpp"
    "external/glm/glm/ext/matrix_integer.inl"
    "external/glm/glm/ext/matrix_projection.hpp"
    "external/glm/glm/ext/matrix_projection.inl"
    "external/glm/glm/ext/matrix_relational.hpp"
    "external/glm/glm/ext/matrix_relational.inl"
    "external/glm/glm/ext/matrix_transform.hpp"
    "external/glm/glm/ext/matrix_transform.inl"
    "external/glm/glm/ext/matrix_uint2x2.hpp"
    "external/glm/glm/ext/matrix_uint2x2_sized.hpp"
    "external/glm/glm/ext/matrix_uint2x3.hpp"
    "external/glm/glm/ext/matrix_uint2x3_sized.hpp"
    "external/glm/glm/ext/matrix_uint2x4.hpp"
    "external/glm/glm/ext/matrix_uint2x4_sized.hpp"
    "external/glm/glm/ext/matrix_uint3x2.hpp"
    "external/glm/glm/ext/matrix_uint3x2_sized.hpp"
    "external/glm/glm/ext/matrix_uint3x3.hpp"
    "external/glm/glm/ext/matrix_uint3x3_sized.hpp"
    "external/glm/glm/ext/matrix_uint3x4.hpp"
    "external/glm/glm/ext/matrix_uint3x4_sized.hpp"
    "external/glm/glm/ext/matrix_uint4x2.hpp"
    "external/glm/glm/ext/matrix_uint4x2_sized.hpp"
    "external/glm/glm/ext/matrix_uint4x3.hpp"
    "external/glm/glm/ext/matrix_uint4x3_sized.hpp"
    "external/glm/glm/ext/matrix_uint4x4.hpp"
    "external/glm/glm/ext/matrix_uint4x4_sized.hpp"
    "external/glm/glm/ext/quaternion_common.hpp"
    "external/glm/glm/ext/quaternion_common.inl"
    "external/glm/glm/ext/quaternion_common_simd.inl"
    "external/glm/glm/ext/quaternion_double.hpp"
    "external/glm/glm/ext/quaternion_double_precision.hpp"
    "external/glm/glm/ext/quaternion_exponential.hpp"
    "external/glm/glm/ext/quaternion_exponential.inl"
    "external/glm/glm/ext/quaternion_float.hpp"
    "external/glm/glm/ext/quaternion_float_precision.hpp"
    "external/glm/glm/ext/quaternion_geometric.hpp"
    "external/glm/glm/ext/quaternion_geometric.inl"
    "external/glm/glm/ext/quaternion_relational.hpp"
    "external/glm/glm/ext/quaternion_relational.inl"
    "external/glm/glm/ext/quaternion_transform.hpp"
    "external/glm/glm/ext/quaternion_transform.inl"
    "external/glm/glm/ext/quaternion_trigonometric.hpp"
    "external/glm/glm/ext/quaternion_trigonometric.inl"
    "external/glm/glm/ext/scalar_common.hpp"
    "external/glm/glm/ext/scalar_common.inl"
    "external/glm/glm/ext/scalar_constants.hpp"
    "external/glm/glm/ext/scalar_constants.inl"
    "external/glm/glm/ext/scalar_int_sized.hpp"
    "external/glm/glm/ext/scalar_integer.hpp"
    "external/glm/glm/ext/scalar_integer.inl"
    "external/glm/glm/ext/scalar_packing.hpp"
    "external/glm/glm/ext/scalar_packing.inl"
    "external/glm/glm/ext/scalar_reciprocal.hpp"
    "external/glm/glm/ext/scalar_reciprocal.inl"
    "external/glm/glm/ext/scalar_relational.hpp"
    "external/glm/glm/ext/scalar_relational.inl"
    "external/glm/glm/ext/scalar_uint_sized.hpp"
    "external/glm/glm/ext/scalar_ulp.hpp"
    "external/glm/glm/ext/scalar_ulp.inl"
    "external/glm/glm/ext/vector_bool1.hpp"
    "external/glm/glm/ext/vector_bool1_precision.hpp"
    "external/glm/glm/ext/vector_bool2.hpp"
    "external/glm/glm/ext/vector_bool2_precision.hpp"
    "external/glm/glm/ext/vector_bool3.hpp"
    "external/glm/glm/ext/vector_bool3_precision.hpp"
    "external/glm/glm/ext/vector_bool4.hpp"
    "external/glm/glm/ext/vector_bool4_precision.hpp"
    "external/glm/glm/ext/vector_common.hpp"
    "external/glm/glm/ext/vector_common.inl"
    "external/glm/glm/ext/vector_double1.hpp"
    "external/glm/glm/ext/vector_double1_precision.hpp"
    "external/glm/glm/ext/vector_double2.hpp"
    "external/glm/glm/ext/vector_double2_precision.hpp"
    "external/glm/glm/ext/vector_double3.hpp"
    "external/glm/glm/ext/vector_double3_precision.hpp"
    "external/glm/glm/ext/vector_double4.hpp"
    "external/glm/glm/ext/vector_double4_precision.hpp"
    "external/glm/glm/ext/vector_float1.hpp"
    "external/glm/glm/ext/vector_float1_precision.hpp"
    "external/glm/glm/ext/vector_float2.hpp"
    "external/glm/glm/ext/vector_float2_precision.hpp"
    "external/glm/glm/ext/vector_float3.hpp"
    "external/glm/glm/ext/vector_float3_precision.hpp"
    "external/glm/glm/ext/vector_float4.hpp"
    "external/glm/glm/ext/vector_float4_precision.hpp"
    "external/glm/glm/ext/vector_int1.hpp"
    "external/glm/glm/ext/vector_int1_sized.hpp"
    "external/glm/glm/ext/vector_int2.hpp"
    "external/glm/glm/ext/vector_int2_sized.hpp"
    "external/glm/glm/ext/vector_int3.hpp"
    "external/glm/glm/ext/vector_int3_sized.hpp"
    "external/glm/glm/ext/vector_int4.hpp"
    "external/glm/glm/ext/vector_int4_sized.hpp"
    "external/glm/glm/ext/vector_integer.hpp"
    "external/glm/glm/ext/vector_integer.inl"
    "external/glm/glm/ext/vector_packing.hpp"
    "external/glm/glm/ext/vector_packing.inl"
    "external/glm/glm/ext/vector_reciprocal.hpp"
    "external/glm/glm/ext/vector_reciprocal.inl"
    "external/glm/glm/ext/vector_relational.hpp"
    "external/glm/glm/ext/vector_relational.inl"
    "external/glm/glm/ext/vector_uint1.hpp"
    "external/glm/glm/ext/vector_uint1_sized.hpp"
    "external/glm/glm/ext/vector_uint2.hpp"
    "external/glm/glm/ext/vector_uint2_sized.hpp"
    "external/glm/glm/ext/vector_uint3.hpp"
    "external/glm/glm/ext/vector_uint3_sized.hpp"
    "external/glm/glm/ext/vector_uint4.hpp"
    "external/glm/glm/ext/vector_uint4_sized.hpp"
    "external/glm/glm/ext/vector_ulp.hpp"
    "external/glm/glm/ext/vector_ulp.inl"
)
source_group("external\\glm\\glm\\ext" FILES ${external__glm__glm__ext})

set(external__glm__glm__gtc
    "external/glm/glm/gtc/bitfield.hpp"
    "external/glm/glm/gtc/bitfield.inl"
    "external/glm/glm/gtc/color_space.hpp"
    "external/glm/glm/gtc/color_space.inl"
    "external/glm/glm/gtc/constants.hpp"
    "external/glm/glm/gtc/constants.inl"
    "external/glm/glm/gtc/epsilon.hpp"
    "external/glm/glm/gtc/epsilon.inl"
    "external/glm/glm/gtc/integer.hpp"
    "external/glm/glm/gtc/integer.inl"
    "external/glm/glm/gtc/matrix_access.hpp"
    "external/glm/glm/gtc/matrix_access.inl"
    "external/glm/glm/gtc/matrix_integer.hpp"
    "external/glm/glm/gtc/matrix_inverse.hpp"
    "external/glm/glm/gtc/matrix_inverse.inl"
    "external/glm/glm/gtc/matrix_transform.hpp"
    "external/glm/glm/gtc/matrix_transform.inl"
    "external/glm/glm/gtc/noise.hpp"
    "external/glm/glm/gtc/noise.inl"
    "external/glm/glm/gtc/packing.hpp"
    "external/glm/glm/gtc/packing.inl"
    "external/glm/glm/gtc/quaternion.hpp"
    "external/glm/glm/gtc/quaternion.inl"
    "external/glm/glm/gtc/quaternion_simd.inl"
    "external/glm/glm/gtc/random.hpp"
    "external/glm/glm/gtc/random.inl"
    "external/glm/glm/gtc/reciprocal.hpp"
    "external/glm/glm/gtc/round.hpp"
    "external/glm/glm/gtc/round.inl"
    "external/glm/glm/gtc/type_aligned.hpp"
    "external/glm/glm/gtc/type_precision.hpp"
    "external/glm/glm/gtc/type_precision.inl"
    "external/glm/glm/gtc/type_ptr.hpp"
    "external/glm/glm/gtc/type_ptr.inl"
    "external/glm/glm/gtc/ulp.hpp"
    "external/glm/glm/gtc/ulp.inl"
    "external/glm/glm/gtc/vec1.hpp"
)
source_group("external\\glm\\glm\\gtc" FILES ${external__glm__glm__gtc})

set(external__glm__glm__gtx
    "external/glm/glm/gtx/associated_min_max.hpp"
    "external/glm/glm/gtx/associated_min_max.inl"
    "external/glm/glm/gtx/bit.hpp"
    "external/glm/glm/gtx/bit.inl"
    "external/glm/glm/gtx/closest_point.hpp"
    "external/glm/glm/gtx/closest_point.inl"
    "external/glm/glm/gtx/color_encoding.hpp"
    "external/glm/glm/gtx/color_encoding.inl"
    "external/glm/glm/gtx/color_space.hpp"
    "external/glm/glm/gtx/color_space.inl"
    "external/glm/glm/gtx/color_space_YCoCg.hpp"
    "external/glm/glm/gtx/color_space_YCoCg.inl"
    "external/glm/glm/gtx/common.hpp"
    "external/glm/glm/gtx/common.inl"
    "external/glm/glm/gtx/compatibility.hpp"
    "external/glm/glm/gtx/compatibility.inl"
    "external/glm/glm/gtx/component_wise.hpp"
    "external/glm/glm/gtx/component_wise.inl"
    "external/glm/glm/gtx/dual_quaternion.hpp"
    "external/glm/glm/gtx/dual_quaternion.inl"
    "external/glm/glm/gtx/easing.hpp"
    "external/glm/glm/gtx/easing.inl"
    "external/glm/glm/gtx/euler_angles.hpp"
    "external/glm/glm/gtx/euler_angles.inl"
    "external/glm/glm/gtx/extend.hpp"
    "external/glm/glm/gtx/extend.inl"
    "external/glm/glm/gtx/extended_min_max.hpp"
    "external/glm/glm/gtx/extended_min_max.inl"
    "external/glm/glm/gtx/exterior_product.hpp"
    "external/glm/glm/gtx/exterior_product.inl"
    "external/glm/glm/gtx/fast_exponential.hpp"
    "external/glm/glm/gtx/fast_exponential.inl"
    "external/glm/glm/gtx/fast_square_root.hpp"
    "external/glm/glm/gtx/fast_square_root.inl"
    "external/glm/glm/gtx/fast_trigonometry.hpp"
    "external/glm/glm/gtx/fast_trigonometry.inl"
    "external/glm/glm/gtx/float_notmalize.inl"
    "external/glm/glm/gtx/functions.hpp"
    "external/glm/glm/gtx/functions.inl"
    "external/glm/glm/gtx/gradient_paint.hpp"
    "external/glm/glm/gtx/gradient_paint.inl"
    "external/glm/glm/gtx/handed_coordinate_space.hpp"
    "external/glm/glm/gtx/handed_coordinate_space.inl"
    "external/glm/glm/gtx/hash.hpp"
    "external/glm/glm/gtx/hash.inl"
    "external/glm/glm/gtx/integer.hpp"
    "external/glm/glm/gtx/integer.inl"
    "external/glm/glm/gtx/intersect.hpp"
    "external/glm/glm/gtx/intersect.inl"
    "external/glm/glm/gtx/io.hpp"
    "external/glm/glm/gtx/io.inl"
    "external/glm/glm/gtx/log_base.hpp"
    "external/glm/glm/gtx/log_base.inl"
    "external/glm/glm/gtx/matrix_cross_product.hpp"
    "external/glm/glm/gtx/matrix_cross_product.inl"
    "external/glm/glm/gtx/matrix_decompose.hpp"
    "external/glm/glm/gtx/matrix_decompose.inl"
    "external/glm/glm/gtx/matrix_factorisation.hpp"
    "external/glm/glm/gtx/matrix_factorisation.inl"
    "external/glm/glm/gtx/matrix_interpolation.hpp"
    "external/glm/glm/gtx/matrix_interpolation.inl"
    "external/glm/glm/gtx/matrix_major_storage.hpp"
    "external/glm/glm/gtx/matrix_major_storage.inl"
    "external/glm/glm/gtx/matrix_operation.hpp"
    "external/glm/glm/gtx/matrix_operation.inl"
    "external/glm/glm/gtx/matrix_query.hpp"
    "external/glm/glm/gtx/matrix_query.inl"
    "external/glm/glm/gtx/matrix_transform_2d.hpp"
    "external/glm/glm/gtx/matrix_transform_2d.inl"
    "external/glm/glm/gtx/mixed_product.hpp"
    "external/glm/glm/gtx/mixed_product.inl"
    "external/glm/glm/gtx/norm.hpp"
    "external/glm/glm/gtx/norm.inl"
    "external/glm/glm/gtx/normal.hpp"
    "external/glm/glm/gtx/normal.inl"
    "external/glm/glm/gtx/normalize_dot.hpp"
    "external/glm/glm/gtx/normalize_dot.inl"
    "external/glm/glm/gtx/number_precision.hpp"
    "external/glm/glm/gtx/number_precision.inl"
    "external/glm/glm/gtx/optimum_pow.hpp"
    "external/glm/glm/gtx/optimum_pow.inl"
    "external/glm/glm/gtx/orthonormalize.hpp"
    "external/glm/glm/gtx/orthonormalize.inl"
    "external/glm/glm/gtx/pca.hpp"
    "external/glm/glm/gtx/pca.inl"
    "external/glm/glm/gtx/perpendicular.hpp"
    "external/glm/glm/gtx/perpendicular.inl"
    "external/glm/glm/gtx/polar_coordinates.hpp"
    "external/glm/glm/gtx/polar_coordinates.inl"
    "external/glm/glm/gtx/projection.hpp"
    "external/glm/glm/gtx/projection.inl"
    "external/glm/glm/gtx/quaternion.hpp"
    "external/glm/glm/gtx/quaternion.inl"
    "external/glm/glm/gtx/range.hpp"
    "external/glm/glm/gtx/raw_data.hpp"
    "external/glm/glm/gtx/raw_data.inl"
    "external/glm/glm/gtx/rotate_normalized_axis.hpp"
    "external/glm/glm/gtx/rotate_normalized_axis.inl"
    "external/glm/glm/gtx/rotate_vector.hpp"
    "external/glm/glm/gtx/rotate_vector.inl"
    "external/glm/glm/gtx/scalar_multiplication.hpp"
    "external/glm/glm/gtx/scalar_relational.hpp"
    "external/glm/glm/gtx/scalar_relational.inl"
    "external/glm/glm/gtx/spline.hpp"
    "external/glm/glm/gtx/spline.inl"
    "external/glm/glm/gtx/std_based_type.hpp"
    "external/glm/glm/gtx/std_based_type.inl"
    "external/glm/glm/gtx/string_cast.hpp"
    "external/glm/glm/gtx/string_cast.inl"
    "external/glm/glm/gtx/texture.hpp"
    "external/glm/glm/gtx/texture.inl"
    "external/glm/glm/gtx/transform.hpp"
    "external/glm/glm/gtx/transform.inl"
    "external/glm/glm/gtx/transform2.hpp"
    "external/glm/glm/gtx/transform2.inl"
    "external/glm/glm/gtx/type_aligned.hpp"
    "external/glm/glm/gtx/type_aligned.inl"
    "external/glm/glm/gtx/type_trait.hpp"
    "external/glm/glm/gtx/type_trait.inl"
    "external/glm/glm/gtx/vec_swizzle.hpp"
    "external/glm/glm/gtx/vector_angle.hpp"
    "external/glm/glm/gtx/vector_angle.inl"
    "external/glm/glm/gtx/vector_query.hpp"
    "external/glm/glm/gtx/vector_query.inl"
    "external/glm/glm/gtx/wrap.hpp"
    "external/glm/glm/gtx/wrap.inl"
)
source_group("external\\glm\\glm\\gtx" FILES ${external__glm__glm__gtx})

set(src
    "src/Opium.h"
)
source_group("src" FILES ${src})

set(src__Config
    "src/Config/Precomp.cpp"
    "src/Config/Precomp.h"
)
source_group("src\\Config" FILES ${src__Config})

set(src__EventSystem
    "src/EventSystem/AppEvent.h"
    "src/EventSystem/Event.h"
    "src/EventSystem/KeyEvent.h"
    "src/EventSystem/MouseEvent.h"
)
source_group("src\\EventSystem" FILES ${src__EventSystem})

set(src__Gui
    "src/Gui/ImGuiBuild.cpp"
    "src/Gui/ImGuiLayer.cpp"
    "src/Gui/ImGuiLayer.h"
)
source_group("src\\Gui" FILES ${src__Gui})

set(src__Input
    "src/Input/Input.h"
    "src/Input/KeyCodes.h"
    "src/Input/MouseButtonCodes.h"
)
source_group("src\\Input" FILES ${src__Input})

set(src__Opium
    "src/Opium/Application.cpp"
    "src/Opium/Application.h"
    "src/Opium/Core.h"
    "src/Opium/EntryPoint.h"
    "src/Opium/Layer.cpp"
    "src/Opium/Layer.h"
    "src/Opium/LayerStack.cpp"
    "src/Opium/LayerStack.h"
    "src/Opium/Logging.cpp"
    "src/Opium/Logging.h"
)
source_group("src\\Opium" FILES ${src__Opium})

set(src__Platform__Windows
    "src/Platform/Windows/WinInput.cpp"
    "src/Platform/Windows/WinInput.h"
    "src/Platform/Windows/WinWindow.cpp"
    "src/Platform/Windows/WinWindow.h"
)
source_group("src\\Platform\\Windows" FILES ${src__Platform__Windows})

set(src__Window
    "src/Window/Window.h"
)
source_group("src\\Window" FILES ${src__Window})

set(ALL_FILES
    ${external__glm__glm}
    ${external__glm__glm__detail}
    ${external__glm__glm__ext}
    ${external__glm__glm__gtc}
    ${external__glm__glm__gtx}
    ${src}
    ${src__Config}
    ${src__EventSystem}
    ${src__Gui}
    ${src__Input}
    ${src__Opium}
    ${src__Platform__Windows}
    ${src__Window}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/Config/Precomp.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Opium)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DEBUG   "Opium"
    TARGET_NAME_DIST    "Opium"
    TARGET_NAME_RELEASE "Opium"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../bin/Debug-windows-x86_64/Opium/"
    OUTPUT_DIRECTORY_DIST    "${CMAKE_CURRENT_SOURCE_DIR}/../bin/Dist-windows-x86_64/Opium/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../bin/Release-windows-x86_64/Opium/"
)
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
    $<$<CONFIG:Debug>:
        MultiThreadedDebug
    >
    $<$<CONFIG:Dist>:
        MultiThreaded
    >
    $<$<CONFIG:Release>:
        MultiThreaded
    >
    $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Dist>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
)
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src;"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Config;"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui;"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/glm"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "OP_DEBUG"
    ">"
    "$<$<CONFIG:Dist>:"
        "OP_DIST"
    ">"
    "$<$<CONFIG:Release>:"
        "OP_RELEASE"
    ">"
    "_CRT_SECURE_NO_WARNINGS;"
    "OP_PLATFORM_WINDOWS;"
    "OP_BUILD_DLL;"
    "GLFW_INCLUDE_NONE;"
    "UNICODE;"
    "_UNICODE"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Od;
            /ZI
        >
        $<$<CONFIG:Dist>:
            /Gm-;
            /Ox;
            /Oi;
            /GF;
            /Gy;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
        >
        $<$<CONFIG:Release>:
            /Gm-;
            /Ox;
            /Oi;
            /GF;
            /Gy;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
        >
        /std:c++17;
        /W3;
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Dist>:
            /OPT:REF;
            /OPT:ICF
        >
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF
        >
        /SUBSYSTEM:WINDOWS
    )
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(${PROJECT_NAME} PUBLIC
    GLFW
    Glad
    ImGui
)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "opengl32"
)
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

