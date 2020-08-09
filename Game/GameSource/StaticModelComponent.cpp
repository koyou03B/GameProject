#include "StaticModelComponent.h"
#include ".\LibrarySource\Framework.h"

void StaticModel::Render()
{
	if (!m_instanceData.empty())
	{
		m_staticModel->Render(Framework::GetContext(),m_instanceData, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

int StaticModel::RayPick(const VECTOR3F& startPosition, const VECTOR3F& endPosition, VECTOR3F* outPosition, VECTOR3F* outNormal)
{

	FLOAT4X4 world = m_instanceData.at(0).world;
	FLOAT4X4 convert = m_staticModel->_resource->axisSystemTransform;
	DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&(world * convert));
	DirectX::XMMATRIX inverseTransform = DirectX::XMMatrixInverse(NULL, worldTransform);

	DirectX::XMVECTOR wStart = DirectX::XMLoadFloat3(&startPosition);
	DirectX::XMVECTOR wEnd = DirectX::XMLoadFloat3(&endPosition);

	DirectX::XMVECTOR localStart = DirectX::XMVector3TransformCoord(wStart, inverseTransform);
	DirectX::XMVECTOR localEnd = DirectX::XMVector3TransformCoord(wEnd, inverseTransform);

	float outDistance;
	VECTOR3F start, end;
	DirectX::XMStoreFloat3(&start, localStart);
	DirectX::XMStoreFloat3(&end, localEnd);

	int ret = m_staticModel->RayPick(start, end, outPosition, outNormal, &outDistance);
	if (ret != -1)
	{
		DirectX::XMVECTOR localPos = DirectX::XMLoadFloat3(outPosition);
		DirectX::XMVECTOR localNormal = DirectX::XMLoadFloat3(outNormal);
		DirectX::XMVECTOR worldPos = DirectX::XMVector3TransformCoord(localPos, worldTransform);
		DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(localNormal, worldTransform);

		DirectX::XMStoreFloat3(outPosition, worldPos);
		DirectX::XMStoreFloat3(outNormal, worldNormal);
	}

	return ret;

}
