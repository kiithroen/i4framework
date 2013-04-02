#pragma once

#include "I4Shader.h"

namespace i4graphics
{
	class Shader;
	class ShaderMgr;

	enum ShaderMgrType
	{
		SHADER_MGR_TYPE_NULL		= 0,
		SHADER_MGR_TYPE_UBER,
	};

	enum ShaderMask
	{
		SHADER_MASK_NONE				= 1 << 0,
		SHADER_MASK_TEX_DIFFUSE		= 1 << 1,
		SHADER_MASK_TEX_SPECULAR		= 1 << 2,
		SHADER_MASK_TEX_NORMAL		= 1 << 3,
		SHADER_MASK_SKINNING			= 1 << 4,
	};

	

	typedef map<string, ShaderMgr*>		ShaderMgrMap;

	class ShaderMgr
	{
		typedef map<unsigned int, Shader*>	ShaderMap;
	public:
		virtual ~ShaderMgr();

		bool 				begin(unsigned int mask, const INPUT_ELEMENT* inputElements, unsigned int numElements);
		void 				end();

		void				setConstantBuffer(ShaderType type, unsigned int slot, ConstantBuffer* constantBuffer, void* data);
		void				setTexture(unsigned int slot, const Texture* tex);
		void				setRenderTarget(unsigned int slot, const RenderTarget* tex);
		void				setSamplerState(unsigned int slot, SamplerState state);
	protected:
		bool				load(const char* fname);
		Shader*	createShader(unsigned int mask, const INPUT_ELEMENT* inputElements, unsigned int numElements);
		
	public:
		static bool			addShaderMgr(const string& fxName);
		static ShaderMgr*	findShaderMgr(const string& fxName);
		static void			destroyShaderMgr();

	protected:
		ShaderMgr();

	protected:
		 string				baseShaderCode;
		 Shader*	activeShader;
		 ShaderMap	mapShader;

	private:
		static ShaderMgrMap		mapShaderMgr;
	};
}
