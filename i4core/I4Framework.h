#pragma once

#include "i4core.h"
#include "I4Framework.h"

namespace i4core
{
	class I4FrameCallback;

	class I4CORE_API I4Framework
	{
	public:
		I4Framework();
		virtual ~I4Framework();

		bool				create(unsigned int width, unsigned int height, const string& title);
		void				destroy();
		void				run();
		
		void				setFrameCallback(I4FrameCallback* callback);

		void*				getWindowID() const						{ return windowID; }
		string				getTitle() const						{ return title; }
		unsigned int		getWidth() const						{ return width; }
		unsigned int		getHeight() const						{ return height; }
		bool				isActivated() const						{ return activated; }
		bool				isKeyPressed(unsigned int key) const	{ return keyPressed[key]; }

		virtual void		getMousePos(int& x, int& y)				{}
		virtual void		moveMouseCenter()						{}

	public:
		static I4Framework*	getFramework()		{ return framework; }

		static void			createFramework();
		static void			destroyFramework();

	protected:	
		virtual	bool		onCreate()								{ return true; }
		virtual void		onDestroy()								{}
		virtual bool		onRun()									{ return true; }
		virtual void		onYield()								{}

	private:
		bool				mainLoop(float deltaMs);

	protected:
		void*				windowID;
		string				title;
		unsigned int		width;
		unsigned int		height;
		bool				activated;
		bool				keyPressed[256];

		I4FrameCallback*	frameCallback;

	private:
		static I4Framework*	framework;
	};
}