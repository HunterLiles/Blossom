cbuffer Scene : register(b0, space0) {
  row_major float4x4 mvp;
  float globalLight;
  float3 padding;
};

struct VertexOutput {
  float4 position : SV_Position;
  float3 color : COLOR0;
};

VertexOutput main(uint vertexId : SV_VertexID) {
  static const float3 corners[8] = {
      float3(-0.5, -0.5, -0.5), float3(0.5, -0.5, -0.5),
      float3(0.5, 0.5, -0.5), float3(-0.5, 0.5, -0.5),
      float3(-0.5, -0.5, 0.5), float3(0.5, -0.5, 0.5),
      float3(0.5, 0.5, 0.5), float3(-0.5, 0.5, 0.5)};
  static const uint indices[36] = {
      4, 5, 6, 4, 6, 7, 1, 0, 3, 1, 3, 2,
      0, 4, 7, 0, 7, 3, 5, 1, 2, 5, 2, 6,
      3, 7, 6, 3, 6, 2, 0, 1, 5, 0, 5, 4};
  static const float3 faceColors[6] = {
      float3(1.0, 0.25, 0.35), float3(0.25, 0.7, 1.0),
      float3(0.25, 1.0, 0.5), float3(1.0, 0.8, 0.25),
      float3(0.7, 0.35, 1.0), float3(0.3, 1.0, 0.9)};
  VertexOutput output;
  output.position = mul(float4(corners[indices[vertexId]], 1.0), mvp);
  output.color = faceColors[vertexId / 6] * globalLight;
  return output;
}
