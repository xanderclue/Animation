#include "stdafx.h"
#include "RenderWindowHelper.h"
#include "GraphicsSystem.h"
extern GraphicsSystem g_graphicsSystem;
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
TriangleMesh PositionTrianglesToMesh( const std::vector<PositionUvTriangle>& _triangles )
{
	TriangleMesh outMesh_;
	Triangle tempTriangle_;
	tempTriangle_.m_vertexA.m_color =
		tempTriangle_.m_vertexB.m_color =
		tempTriangle_.m_vertexC.m_color = RGBAColor( 0.2f, 0.2f, 0.2f, 1.0f ).m_rgba;
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
		tempTriangle_.m_vertexB.m_position.x = _triangles[ i ].m_posB.x;
		tempTriangle_.m_vertexB.m_position.y = _triangles[ i ].m_posB.y;
		tempTriangle_.m_vertexB.m_position.z = _triangles[ i ].m_posB.z;
		tempTriangle_.m_vertexB.m_color.x = _triangles[ i ].m_uvB.x;
		tempTriangle_.m_vertexB.m_color.y = _triangles[ i ].m_uvB.y;
		tempTriangle_.m_vertexC.m_position.x = _triangles[ i ].m_posC.x;
		tempTriangle_.m_vertexC.m_position.y = _triangles[ i ].m_posC.y;
		tempTriangle_.m_vertexC.m_position.z = _triangles[ i ].m_posC.z;
		tempTriangle_.m_vertexC.m_color.x = _triangles[ i ].m_uvC.x;
		tempTriangle_.m_vertexC.m_color.y = _triangles[ i ].m_uvC.y;
		outMesh_.AddTriangle( tempTriangle_ );
	}
	return outMesh_;
}
void DrawJoint( DirectX::XMFLOAT4X4 _transformationMatrix, const DirectX::XMFLOAT4X4& _worldMatrix )
{
	XMStoreFloat4x4( &_transformationMatrix, XMMatrixMultiply( XMLoadFloat4x4( &_transformationMatrix ), XMLoadFloat4x4( &_worldMatrix ) ) );
	PositionColorVertex xAxis_, yAxis_, zAxis_, position_;
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

	position_.m_color = xAxis_.m_color = RGBAColor::Red.m_rgba;
	g_graphicsSystem.AddDebugLine( position_, xAxis_ );
	position_.m_color = yAxis_.m_color = RGBAColor::Green.m_rgba;
	g_graphicsSystem.AddDebugLine( position_, yAxis_ );
	position_.m_color = zAxis_.m_color = RGBAColor::Blue.m_rgba;
	g_graphicsSystem.AddDebugLine( position_, zAxis_ );
}
void DrawBone( DirectX::XMFLOAT4X4 _transformA, DirectX::XMFLOAT4X4 _transformB, const DirectX::XMFLOAT4X4& _worldMatrix )
{
	XMStoreFloat4x4( &_transformA, XMMatrixMultiply( XMLoadFloat4x4( &_transformA ), XMLoadFloat4x4( &_worldMatrix ) ) );
	XMStoreFloat4x4( &_transformB, XMMatrixMultiply( XMLoadFloat4x4( &_transformB ), XMLoadFloat4x4( &_worldMatrix ) ) );
	g_graphicsSystem.AddDebugLine( PositionColorVertex( _transformA._41, _transformA._42, _transformA._43, RGBAColor::Cyan ),
								   PositionColorVertex( _transformB._41, _transformB._42, _transformB._43, RGBAColor::Cyan ) );
}
void DrawSkeleton( const std::vector<JointTransform>& _joints, const DirectX::XMFLOAT4X4& _worldMatrix )
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
JointTransform JointInterpolation( const JointTransform& _a, const JointTransform& _b, double _t )
{
	JointTransform outJoint_;
	outJoint_.m_parent = _a.m_parent;
	outJoint_.m_transform = TransformInterpolation( _a.m_transform, _b.m_transform, ( float )_t );
	return outJoint_;
}
std::vector<JointTransform> FrameInterpolation( const std::vector<JointTransform>& _a,
												const std::vector<JointTransform>& _b, double _t )
{
	std::vector<JointTransform> outJoints_;
	for ( unsigned int i = 0u; i < _a.size(); ++i )
		outJoints_.push_back( JointInterpolation( _a[ i ], _b[ i ], _t ) );
	return outJoints_;
}
void DrawSkeletonAnimation( const AnimClip& _anim, const DirectX::XMFLOAT4X4& _world, double _time, bool _tween )
{
	std::vector<JointTransform> currFrame_;
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
