#pragma once

#include "I4Shader.h"

namespace i4graphics
{
	class I4Shader;
	class I4ShaderMgr;

	enum I4ShaderMgrType
	{
		I4SHADER_MGR_TYPE_NULL		= 0,
		I4SHADER_MGR_TYPE_UBER,
	};

	enum I4ShaderMask
	{
		I4SHADER_MASK_NONE				= 1 << 0,
		I4SHADER_MASK_TEX_DIFFUSE		= 1 << 1,
		I4SHADER_MASK_TEX_SPECULAR		= 1 << 2,
		I4SHADER_MASK_TEX_NORMAL		= 1 << 3,
		I4SHADER_MASK_SKINNING			= 1 << 4,
	};

	

	typedef map<string, I4ShaderMgr*>		I4ShaderMgrMap;

	class I4ShaderMgr
	{
		typedef map<unsigned int, I4Shader*>	I4ShaderMap;
	public:
		virtual ~I4ShaderMgr();

		bool 				begin(unsigned int mask, const I4INPUT_ELEMENT* inputElements, unsigned int numElements);
		void 				end();

		void				setConstantBuffer(I4ShaderType type, unsigned int slot, I4ConstantBuffer* constantBuffer, void* data);
		void				setTexture(unsigned int slot, const I4Texture* tex);
		void				setRenderTarget(unsigned int slot, const I4RenderTarget* tex);
		void				setSamplerState(unsigned int slot, I4SamplerState state);
	protected:
		bool				load(const char* fname);
		I4Shader*	createShader(unsigned int mask, const I4INPUT_ELEMENT* inputElements, unsigned int numElements);
		
	public:
		static bool			addShaderMgr(const string& fxName);
		static I4ShaderMgr*	findShaderMgr(const string& fxName);
		static void			destroyShaderMgr();

	protected:
		I4ShaderMgr();

	protected:
		 string				baseShaderCode;
		 I4Shader*	activeShader;
		 I4ShaderMap	mapShader;

	private:
		static I4ShaderMgrMap		mapShaderMgr;
	};
}
