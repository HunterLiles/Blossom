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
  const uint gridLine = vertexId / 2;
  const bool endpoint = (vertexId % 2) != 0;
  VertexOutput output;
  float3 position;
  output.color = float3(0.28, 0.33, 0.40);

  if (gridLine < 21) {
    position = float3(-5.0 + gridLine * 0.5, 0.0, endpoint ? 5.0 : -5.0);
  } else if (gridLine < 42) {
    position = float3(endpoint ? 5.0 : -5.0, 0.0, -5.0 + (gridLine - 21) * 0.5);
  } else if (gridLine == 42) {
    position = float3(endpoint ? 2.0 : 0.0, 0.0, 0.0);
    output.color = float3(1.0, 0.2, 0.2); // X
  } else if (gridLine == 43) {
    position = float3(0.0, endpoint ? 2.0 : 0.0, 0.0);
    output.color = float3(0.2, 1.0, 0.2); // Y
  } else {
    position = float3(0.0, 0.0, endpoint ? 2.0 : 0.0);
    output.color = float3(0.2, 0.45, 1.0); // Z
  }
  output.position = mul(float4(position, 1.0), mvp);
  return output;
}
