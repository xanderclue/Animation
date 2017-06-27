#include "stdafx.h"
#include "GraphicsSystem.h"

const RGBAColor RGBAColor::Red = RGBAColor( 1.0f, 0.0f, 0.0f );
const RGBAColor RGBAColor::Green = RGBAColor( 0.0f, 1.0f, 0.0f );
const RGBAColor RGBAColor::Blue = RGBAColor( 0.0f, 0.0f, 1.0f );
const RGBAColor RGBAColor::Cyan = RGBAColor( 0.0f, 1.0f, 1.0f );
const RGBAColor RGBAColor::Magenta = RGBAColor( 1.0f, 0.0f, 1.0f );
const RGBAColor RGBAColor::Yellow = RGBAColor( 1.0f, 1.0f, 0.0f );
const RGBAColor RGBAColor::Black = RGBAColor( 0.0f, 0.0f, 0.0f );
const RGBAColor RGBAColor::White = RGBAColor( 1.0f, 1.0f, 1.0f );
RGBAColor::RGBAColor( void )
{
	// TODO
}
RGBAColor::RGBAColor( float, float, float, float )
{
	// TODO
}
PositionColorVertex::PositionColorVertex( void )
{
	// TODO
}
PositionColorVertex::PositionColorVertex( float, float, float, const RGBAColor& )
{
	// TODO
}
GraphicsSystem::DebugRenderer::DebugRenderer( void )
{
	// TODO
}
GraphicsSystem::DebugRenderer::~DebugRenderer( void )
{
	// TODO
}
void GraphicsSystem::AddDebugLine( void )
{
	// TODO
}
void GraphicsSystem::FlushDebugRenderer( void )
{
	// TODO
}
GraphicsSystem::GraphicsSystem( void )
{
	// TODO
}
GraphicsSystem::~GraphicsSystem( void )
{
	// TODO
}
void GraphicsSystem::InitializeViewport( void )
{
	// TODO
}
void GraphicsSystem::SetPipelineStages( PipelineState* )
{
	// TODO
}
void GraphicsSystem::SetupDefaultBuffer( void )
{
	// TODO
}
void GraphicsSystem::InitializeGraphicsSystem( void )
{
	// TODO
}
void GraphicsSystem::CreateDeviceContextSwapChain( void )
{
	// TODO
}
void GraphicsSystem::InitializeDepthStencilBuffer( void )
{
	// TODO
}
void GraphicsSystem::InitializeDepthStencilState( void )
{
	// TODO
}
void GraphicsSystem::InitializeDepthStencilView( void )
{
	// TODO
}
void GraphicsSystem::InitializeRasterizerState( void )
{
	// TODO
}
void GraphicsSystem::InitializeShadersAndInputLayout( void )
{
	// TODO
}
void GraphicsSystem::DrawFrame( void )
{
	// TODO
}