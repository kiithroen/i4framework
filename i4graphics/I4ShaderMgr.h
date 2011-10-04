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
		I4SHADER_MASK_VERTEX_COLOR		= 1 << 1,
	};


	typedef std::map<std::wstring, I4ShaderMgr*>		I4ShaderMgrMap;

	class I4GRAPHICS_API I4ShaderMgr
	{
		typedef std::map<unsigned int, I4ShaderProgram*>	I4ShaderProgramMap;
	public:
		virtual ~I4ShaderMgr();

		bool 				begin(unsigned int mask, const I4INPUT_ELEMENT* inputElements, unsigned int numElements);
		void 				end();

		void				setVector(ShaderVector sv, float* v);
		void				setVectorArray(ShaderVectorArray sva, float* v, unsigned int offset, unsigned int count);
		void				setMatrix(ShaderMatrix sm, float* v);
		void				setMatrixArray(ShaderMatrixArray sva, float* v, unsigned int offset, unsigned int count);
		void				setTexture(ShaderTexture st, I4Texture* tex);

	protected:
		bool				load(const wchar_t* fname);
		I4ShaderProgram*	createShaderProgram(unsigned int mask, const I4INPUT_ELEMENT* inputElements, unsigned int numElements);
		
	public:
		static bool			addShaderMgr(const std::wstring& fx);
		static I4ShaderMgr*	findShaderMgr(const std::wstring& fx);
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
