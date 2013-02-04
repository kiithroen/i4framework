#pragma once

#include "i4graphics.h"
#include "I4ShaderProgram.h"

namespace i4graphics
{
	class I4ShaderProgram;
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
		I4SHADER_MASK_TEX_NORMAL			= 1 << 3,
	};


	typedef std::map<std::string, I4ShaderMgr*>		I4ShaderMgrMap;

	class I4GRAPHICS_API I4ShaderMgr
	{
		typedef std::map<unsigned int, I4ShaderProgram*>	I4ShaderProgramMap;
	public:
		virtual ~I4ShaderMgr();

		bool 				begin(unsigned int mask, const I4INPUT_ELEMENT* inputElements, unsigned int numElements);
		void 				end();

		void				setConstantBuffer(I4ShaderProgramType type, unsigned int slot, const char* name, unsigned int size, void* buffer);
		void				setTexture(unsigned int slot, const I4Texture* tex);
		void				setRenderTarget(unsigned int slot, const I4RenderTarget* tex);

	protected:
		bool				load(const char* fname);
		I4ShaderProgram*	createShaderProgram(unsigned int mask, const I4INPUT_ELEMENT* inputElements, unsigned int numElements);
		
	public:
		static bool			addShaderMgr(const std::string& fxName);
		static I4ShaderMgr*	findShaderMgr(const std::string& fxName);
		static void			destroyShaderMgr();

	protected:
		I4ShaderMgr();

	protected:
		 std::string		baseShaderCode;
		 I4ShaderProgram*	activeShaderProgram;
		 I4ShaderProgramMap	mapShaderProgram;

	private:
		static I4ShaderMgrMap		mapShaderMgr;
	};
}
