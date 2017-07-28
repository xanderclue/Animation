#pragma once
#include <vector>
#include <DirectXMath.h>

namespace FBXDLL
{
	struct Skinned
	{
		DirectX::XMFLOAT4 m_jointWeights = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		int m_jointIndices[ 4u ] = { -1, -1, -1, -1 };
		int m_cpIndex = -1;
		int m_weightCount = 0;
	};
	struct PositionUvTriangle
	{
		Skinned m_skinnedA, m_skinnedB, m_skinnedC;
		DirectX::XMFLOAT3 m_posA, m_posB, m_posC;
		DirectX::XMFLOAT2 m_uvA, m_uvB, m_uvC;
	};
	struct JointTransform
	{
		DirectX::XMFLOAT4X4 m_transform;
		int m_parent;
	};
	struct Keyframe
	{
		std::vector<FBXDLL::JointTransform> m_joints;
		double m_time;
	};
	struct AnimClip
	{
		std::vector<FBXDLL::Keyframe> m_frames;
		double m_duration;
	};
}