struct CAMERA
{
	float4 position;
	float4 direction;
	row_major float4x4 view;
	row_major float4x4 projection;
	row_major float4x4 view_projection;
	row_major float4x4 inverse_view;
	row_major float4x4 inverse_view_projection;
	row_major float4x4 previous_view_projection;
};

struct DIRECTIONAL_LIGHT
{
	float4 direction;
	float4 color;
};

struct POINT_LIGHT
{
	float4 position;
	float4 color;
};

struct SPOT_LIGHT
{
	float4 position;
	float4 color;
	float4 dir;
};

cbuffer CAMERA_CONSTANTS : register(b1)
{
	CAMERA camera;
};

cbuffer SCENE_CONSTANTS : register(b2)
{
	DIRECTIONAL_LIGHT directional_light;
	POINT_LIGHT point_light;
	SPOT_LIGHT spot_light;
	float4 ambient_color;
};