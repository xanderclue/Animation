#include "stdafx.h"
#include "RenderWindowHelper.h"
#include "GraphicsSystem.h"
#include <iostream>
#include <fstream>
extern Renderer::GraphicsSystem g_graphicsSystem;
std::vector<DirectX::XMFLOAT4X4> g_nothing( 64u );
void CheckKeyToggleWireframe( void )
{
	static bool keyDown = false;
	if ( GetAsyncKeyState( '1' ) )
	{
		if ( !keyDown )
		{
			keyDown = true;
			g_graphicsSystem.ToggleWireframe();
#ifndef NDEBUG
			std::cout << "Key pressed: 1 - Toggled Wireframe Mode" << std::endl;
#endif
		}
	}
	else keyDown = false;
}
void CheckKeyToggleTweening( bool& _tween )
{
	static bool keyDown = false;
	if ( GetAsyncKeyState( '2' ) )
	{
		if ( !keyDown )
		{
			keyDown = true;
			_tween = !_tween;
#ifndef NDEBUG
			std::cout << "Key pressed: 2 - Toggled Tweening -> " << ( _tween ? "Tweening enabled" : "Tweening disabled" ) << std::endl;
#endif
		}
	}
	else keyDown = false;
}
Renderer::TriangleMesh PositionTrianglesToMesh( const std::vector<FBXDLL::PositionUvTriangle>& _triangles )
{
	Renderer::TriangleMesh outMesh_;
	Renderer::Triangle tempTriangle_;
	tempTriangle_.m_vertexA.m_color =
		tempTriangle_.m_vertexB.m_color =
		tempTriangle_.m_vertexC.m_color = Renderer::RGBAColor( 0.2f, 0.2f, 0.2f, 1.0f ).m_rgba;
	tempTriangle_.m_vertexA.m_position.w =
		tempTriangle_.m_vertexB.m_position.w =
		tempTriangle_.m_vertexC.m_position.w = 1.0f;
	for ( unsigned int i = 0u; i < _triangles.size(); ++i )
	{
		tempTriangle_.m_vertexA.m_position.x = _triangles[ i ].m_posA.x;
		tempTriangle_.m_vertexA.m_position.y = _triangles[ i ].m_posA.y;
		tempTriangle_.m_vertexA.m_position.z = _triangles[ i ].m_posA.z;
		tempTriangle_.m_vertexA.m_color.x = _triangles[ i ].m_uvA.x;
		tempTriangle_.m_vertexA.m_color.y = _triangles[ i ].m_uvA.y;
		tempTriangle_.m_vertexA.m_weights.x = _triangles[ i ].m_skinnedA.m_jointWeights.x;
		tempTriangle_.m_vertexA.m_weights.y = _triangles[ i ].m_skinnedA.m_jointWeights.y;
		tempTriangle_.m_vertexA.m_weights.z = _triangles[ i ].m_skinnedA.m_jointWeights.z;
		tempTriangle_.m_vertexA.m_weights.w = _triangles[ i ].m_skinnedA.m_jointWeights.w;
		tempTriangle_.m_vertexA.m_indices.x = _triangles[ i ].m_skinnedA.m_jointIndices[ 0u ];
		tempTriangle_.m_vertexA.m_indices.y = _triangles[ i ].m_skinnedA.m_jointIndices[ 1u ];
		tempTriangle_.m_vertexA.m_indices.z = _triangles[ i ].m_skinnedA.m_jointIndices[ 2u ];
		tempTriangle_.m_vertexA.m_indices.w = _triangles[ i ].m_skinnedA.m_jointIndices[ 3u ];
		tempTriangle_.m_vertexB.m_position.x = _triangles[ i ].m_posB.x;
		tempTriangle_.m_vertexB.m_position.y = _triangles[ i ].m_posB.y;
		tempTriangle_.m_vertexB.m_position.z = _triangles[ i ].m_posB.z;
		tempTriangle_.m_vertexB.m_color.x = _triangles[ i ].m_uvB.x;
		tempTriangle_.m_vertexB.m_color.y = _triangles[ i ].m_uvB.y;
		tempTriangle_.m_vertexB.m_weights.x = _triangles[ i ].m_skinnedB.m_jointWeights.x;
		tempTriangle_.m_vertexB.m_weights.y = _triangles[ i ].m_skinnedB.m_jointWeights.y;
		tempTriangle_.m_vertexB.m_weights.z = _triangles[ i ].m_skinnedB.m_jointWeights.z;
		tempTriangle_.m_vertexB.m_weights.w = _triangles[ i ].m_skinnedB.m_jointWeights.w;
		tempTriangle_.m_vertexB.m_indices.x = _triangles[ i ].m_skinnedB.m_jointIndices[ 0u ];
		tempTriangle_.m_vertexB.m_indices.y = _triangles[ i ].m_skinnedB.m_jointIndices[ 1u ];
		tempTriangle_.m_vertexB.m_indices.z = _triangles[ i ].m_skinnedB.m_jointIndices[ 2u ];
		tempTriangle_.m_vertexB.m_indices.w = _triangles[ i ].m_skinnedB.m_jointIndices[ 3u ];
		tempTriangle_.m_vertexC.m_position.x = _triangles[ i ].m_posC.x;
		tempTriangle_.m_vertexC.m_position.y = _triangles[ i ].m_posC.y;
		tempTriangle_.m_vertexC.m_position.z = _triangles[ i ].m_posC.z;
		tempTriangle_.m_vertexC.m_color.x = _triangles[ i ].m_uvC.x;
		tempTriangle_.m_vertexC.m_color.y = _triangles[ i ].m_uvC.y;
		tempTriangle_.m_vertexC.m_weights.x = _triangles[ i ].m_skinnedC.m_jointWeights.x;
		tempTriangle_.m_vertexC.m_weights.y = _triangles[ i ].m_skinnedC.m_jointWeights.y;
		tempTriangle_.m_vertexC.m_weights.z = _triangles[ i ].m_skinnedC.m_jointWeights.z;
		tempTriangle_.m_vertexC.m_weights.w = _triangles[ i ].m_skinnedC.m_jointWeights.w;
		tempTriangle_.m_vertexC.m_indices.x = _triangles[ i ].m_skinnedC.m_jointIndices[ 0u ];
		tempTriangle_.m_vertexC.m_indices.y = _triangles[ i ].m_skinnedC.m_jointIndices[ 1u ];
		tempTriangle_.m_vertexC.m_indices.z = _triangles[ i ].m_skinnedC.m_jointIndices[ 2u ];
		tempTriangle_.m_vertexC.m_indices.w = _triangles[ i ].m_skinnedC.m_jointIndices[ 3u ];
		outMesh_.AddTriangle( tempTriangle_ );
	}
	return outMesh_;
}
void DrawJoint( DirectX::XMFLOAT4X4 _transformationMatrix, const DirectX::XMFLOAT4X4& _worldMatrix )
{
	XMStoreFloat4x4( &_transformationMatrix, XMMatrixMultiply( XMLoadFloat4x4( &_transformationMatrix ), XMLoadFloat4x4( &_worldMatrix ) ) );
	Renderer::PositionColorVertex xAxis_, yAxis_, zAxis_, position_;
	xAxis_.m_position = DirectX::XMFLOAT4(
		_transformationMatrix._11,
		_transformationMatrix._12,
		_transformationMatrix._13,
		_transformationMatrix._14 );
	yAxis_.m_position = DirectX::XMFLOAT4(
		_transformationMatrix._21,
		_transformationMatrix._22,
		_transformationMatrix._23,
		_transformationMatrix._24 );
	zAxis_.m_position = DirectX::XMFLOAT4(
		_transformationMatrix._31,
		_transformationMatrix._32,
		_transformationMatrix._33,
		_transformationMatrix._34 );
	position_.m_position = DirectX::XMFLOAT4(
		_transformationMatrix._41,
		_transformationMatrix._42,
		_transformationMatrix._43,
		_transformationMatrix._44 );
	XMStoreFloat4( &xAxis_.m_position, DirectX::XMVectorAdd( XMLoadFloat4( &position_.m_position ), XMLoadFloat4( &xAxis_.m_position ) ) );
	XMStoreFloat4( &yAxis_.m_position, DirectX::XMVectorAdd( XMLoadFloat4( &position_.m_position ), XMLoadFloat4( &yAxis_.m_position ) ) );
	XMStoreFloat4( &zAxis_.m_position, DirectX::XMVectorAdd( XMLoadFloat4( &position_.m_position ), XMLoadFloat4( &zAxis_.m_position ) ) );

	position_.m_color = xAxis_.m_color = Renderer::RGBAColor::Red.m_rgba;
	g_graphicsSystem.AddDebugLine( position_, xAxis_ );
	position_.m_color = yAxis_.m_color = Renderer::RGBAColor::Green.m_rgba;
	g_graphicsSystem.AddDebugLine( position_, yAxis_ );
	position_.m_color = zAxis_.m_color = Renderer::RGBAColor::Blue.m_rgba;
	g_graphicsSystem.AddDebugLine( position_, zAxis_ );
}
void DrawBone( DirectX::XMFLOAT4X4 _transformA, DirectX::XMFLOAT4X4 _transformB, const DirectX::XMFLOAT4X4& _worldMatrix )
{
	XMStoreFloat4x4( &_transformA, XMMatrixMultiply( XMLoadFloat4x4( &_transformA ), XMLoadFloat4x4( &_worldMatrix ) ) );
	XMStoreFloat4x4( &_transformB, XMMatrixMultiply( XMLoadFloat4x4( &_transformB ), XMLoadFloat4x4( &_worldMatrix ) ) );
	g_graphicsSystem.AddDebugLine( Renderer::PositionColorVertex( _transformA._41, _transformA._42, _transformA._43, Renderer::RGBAColor::Cyan ),
								   Renderer::PositionColorVertex( _transformB._41, _transformB._42, _transformB._43, Renderer::RGBAColor::Cyan ) );
}
void DrawSkeleton( const std::vector<FBXDLL::JointTransform>& _joints, const DirectX::XMFLOAT4X4& _worldMatrix )
{
	const unsigned int size_ = ( unsigned int )_joints.size();
	for ( unsigned int i = 0u; i < size_; ++i )
	{
		DrawJoint( _joints[ i ].m_transform, _worldMatrix );
		if ( _joints[ i ].m_parent >= 0 )
			DrawBone( _joints[ i ].m_transform, _joints[ _joints[ i ].m_parent ].m_transform, _worldMatrix );
	}
}
DirectX::XMFLOAT4X4 TransformInterpolation( const DirectX::XMFLOAT4X4& _a, const DirectX::XMFLOAT4X4& _b, float _t )
{
	DirectX::XMFLOAT4X4 outMat_;
	DirectX::XMVECTOR scaleA_, rotA_, transA_, scaleB_, rotB_, transB_, scaleOut_, rotOut_, transOut_;
	DirectX::XMMatrixDecompose( &scaleA_, &rotA_, &transA_, XMLoadFloat4x4( &_a ) );
	DirectX::XMMatrixDecompose( &scaleB_, &rotB_, &transB_, XMLoadFloat4x4( &_b ) );

	scaleOut_ = DirectX::XMVectorLerp( scaleA_, scaleB_, _t );
	rotOut_ = DirectX::XMQuaternionSlerp( rotA_, rotB_, _t );
	transOut_ = DirectX::XMVectorLerp( transA_, transB_, _t );

	DirectX::XMMatrixAffineTransformation( scaleOut_, DirectX::XMVectorZero(), rotOut_, transOut_ );
	XMStoreFloat4x4( &outMat_, DirectX::XMMatrixAffineTransformation( scaleOut_, DirectX::XMVectorZero(), rotOut_, transOut_ ) );
	return outMat_;
}
FBXDLL::JointTransform JointInterpolation( const FBXDLL::JointTransform& _a, const FBXDLL::JointTransform& _b, double _t )
{
	FBXDLL::JointTransform outJoint_;
	outJoint_.m_parent = _a.m_parent;
	outJoint_.m_transform = TransformInterpolation( _a.m_transform, _b.m_transform, ( float )_t );
	return outJoint_;
}
std::vector<FBXDLL::JointTransform> FrameInterpolation( const std::vector<FBXDLL::JointTransform>& _a,
														const std::vector<FBXDLL::JointTransform>& _b, double _t )
{
	std::vector<FBXDLL::JointTransform> outJoints_;
	for ( unsigned int i = 0u; i < _a.size(); ++i )
		outJoints_.push_back( JointInterpolation( _a[ i ], _b[ i ], _t ) );
	return outJoints_;
}
void DrawSkeletonAnimation( const FBXDLL::AnimClip& _anim, const DirectX::XMFLOAT4X4& _world, double _time, bool _tween, std::vector<DirectX::XMFLOAT4X4>& _joints )
{
	std::vector<FBXDLL::JointTransform> currFrame_;
	_time = fmod( _time, _anim.m_duration );
	unsigned int prevFrameIdx_ = 0u, nextFrameIdx_ = 0u, i;
	double prevFrameTime_ = -DBL_MAX, nextFrameTime_ = DBL_MAX;
	const unsigned int numFrames_ = ( unsigned int )_anim.m_frames.size();
	unsigned int maxFrameIdx_ = 0u, minFrameIdx_ = 0u;
	double maxFrameTime_ = -DBL_MAX, minFrameTime_ = DBL_MAX;
	double frameTime_;
	for ( i = 0u; i < numFrames_; ++i )
	{
		frameTime_ = _anim.m_frames[ i ].m_time;
		if ( frameTime_ >= maxFrameTime_ )
		{
			maxFrameTime_ = frameTime_;
			maxFrameIdx_ = i;
		}
		if ( frameTime_ <= minFrameTime_ )
		{
			minFrameTime_ = frameTime_;
			minFrameIdx_ = i;
		}
	}
	if ( _time < minFrameTime_ || _time > maxFrameTime_ )
	{
		const double duration_ = _anim.m_duration;
		nextFrameIdx_ = minFrameIdx_;
		prevFrameIdx_ = maxFrameIdx_;
		prevFrameTime_ = 0.0;
		nextFrameTime_ = minFrameTime_ + ( duration_ - maxFrameTime_ );
		if ( _time < minFrameTime_ )
			_time += duration_ - maxFrameTime_;
		else if ( _time > maxFrameTime_ )
			_time -= maxFrameTime_;
	}
	else for ( i = 0u; i < numFrames_; ++i )
	{
		frameTime_ = _anim.m_frames[ i ].m_time;
		if ( frameTime_ <= _time && frameTime_ >= prevFrameTime_ )
		{
			prevFrameTime_ = frameTime_;
			prevFrameIdx_ = i;
		}
		if ( frameTime_ >= _time && frameTime_ <= nextFrameTime_ )
		{
			nextFrameTime_ = frameTime_;
			nextFrameIdx_ = i;
		}
	}
	if ( nextFrameIdx_ == prevFrameIdx_ )
		currFrame_ = _anim.m_frames[ prevFrameIdx_ ].m_joints;
	else if ( _tween )
		currFrame_ = FrameInterpolation( _anim.m_frames[ prevFrameIdx_ ].m_joints,
										 _anim.m_frames[ nextFrameIdx_ ].m_joints,
										 ( _time - prevFrameTime_ ) / ( nextFrameTime_ - prevFrameTime_ ) );
	else
		currFrame_ = _anim.m_frames[ _time - prevFrameTime_ < nextFrameTime_ - _time ? prevFrameIdx_ : nextFrameIdx_ ].m_joints;

	for ( unsigned int i = 0u; i < _joints.size(); ++i )
		_joints[ i ] = currFrame_[ i ].m_transform;
	DrawSkeleton( currFrame_, _world );
}
DirectX::XMFLOAT4X4& operator*=( DirectX::XMFLOAT4X4& _a, const DirectX::XMMATRIX& _b )
{
	XMStoreFloat4x4( &_a, XMMatrixMultiply( XMLoadFloat4x4( &_a ), _b ) );
	return _a;
}
DirectX::XMFLOAT4X4 operator*( DirectX::XMFLOAT4X4 _a, const DirectX::XMMATRIX& _b )
{
	XMStoreFloat4x4( &_a, DirectX::XMMatrixMultiply( XMLoadFloat4x4( &_a ), _b ) );
	return _a;
}
DirectX::XMFLOAT4X4 operator*( DirectX::XMFLOAT4X4 _a, const DirectX::XMFLOAT4X4& _b )
{
	return _a * XMLoadFloat4x4( &_b );
}
DirectX::XMFLOAT4X4 GetMatrixInverse( const DirectX::XMFLOAT4X4& _inMat )
{
	DirectX::XMFLOAT4X4 outMat_;
	DirectX::XMStoreFloat4x4( &outMat_, DirectX::XMMatrixInverse( nullptr, DirectX::XMLoadFloat4x4( &_inMat ) ) );
	return outMat_;
}
bool WriteBinFile( const std::string& _filename, const Renderer::TriangleMesh& _mesh, const FBXDLL::AnimClip& _anim )
{
	std::ofstream file_;
	file_.open( _filename.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary );
	if ( !file_.is_open() )
		return false;

	unsigned int tmpUint32, i;
	int j;
	file_.write( ( char* )&( tmpUint32 = _mesh.GetNumTriangles() ), 4u );
	for ( i = 0u; i < _mesh.GetNumTriangles(); ++i )
	{
		file_.write( ( char* )&( _mesh.GetTriangles()[ i ].m_vertices[ 0u ].m_position ), 12u );
		file_.write( ( char* )&( _mesh.GetTriangles()[ i ].m_vertices[ 0u ].m_color ), 8u );
		file_.write( ( char* )&( _mesh.GetTriangles()[ i ].m_vertices[ 0u ].m_weights ), 16u );
		file_.write( ( char* )&( _mesh.GetTriangles()[ i ].m_vertices[ 0u ].m_indices ), 16u );
		file_.write( ( char* )&( _mesh.GetTriangles()[ i ].m_vertices[ 1u ].m_position ), 12u );
		file_.write( ( char* )&( _mesh.GetTriangles()[ i ].m_vertices[ 1u ].m_color ), 8u );
		file_.write( ( char* )&( _mesh.GetTriangles()[ i ].m_vertices[ 1u ].m_weights ), 16u );
		file_.write( ( char* )&( _mesh.GetTriangles()[ i ].m_vertices[ 1u ].m_indices ), 16u );
		file_.write( ( char* )&( _mesh.GetTriangles()[ i ].m_vertices[ 2u ].m_position ), 12u );
		file_.write( ( char* )&( _mesh.GetTriangles()[ i ].m_vertices[ 2u ].m_color ), 8u );
		file_.write( ( char* )&( _mesh.GetTriangles()[ i ].m_vertices[ 2u ].m_weights ), 16u );
		file_.write( ( char* )&( _mesh.GetTriangles()[ i ].m_vertices[ 2u ].m_indices ), 16u );
	}
	file_.write( ( char* )&( _mesh.m_numJoints ), 4u );
	file_.write( ( char* )_mesh.m_invBindJoints, _mesh.m_numJoints * 64u );
	file_.write( ( char* )&( _anim.m_duration ), 8u );
	file_.write( ( char* )&( tmpUint32 = ( unsigned int )_anim.m_frames.size() ), 4u );
	for ( i = 0u; i < ( unsigned int )_anim.m_frames.size(); ++i )
	{
		file_.write( ( char* )&( _anim.m_frames[ i ].m_time ), 8u );
		for ( j = 0; j < _mesh.m_numJoints; ++j )
		{
			file_.write( ( char* )&( _anim.m_frames[ i ].m_joints[ j ].m_parent ), 4u );
			file_.write( ( char* )&( _anim.m_frames[ i ].m_joints[ j ].m_transform ), 64u );
		}
	}

	file_.close();
	return true;
}
bool LoadBinFile( const std::string& _filename, Renderer::TriangleMesh& _mesh, FBXDLL::AnimClip& _anim )
{
	std::ifstream file_;
	file_.open( _filename.c_str(), std::ios_base::in | std::ios_base::binary );
	if ( !file_.is_open() )
		return false;

	// TODO
	( void )_mesh;
	( void )_anim;

	file_.close();
	// return true;
	return false;
}