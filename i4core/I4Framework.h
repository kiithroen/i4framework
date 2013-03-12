#pragma once

#include "i4core.h"
#include "I4Framework.h"

namespace i4core
{
	class I4FrameCallback;

	enum I4InputStateType
	{
		I4INPUT_KEY_DOWN = 0,
		I4INPUT_KEY_UP,
		I4INPUT_LEFT_MOUSE_DOWN,
		I4INPUT_LEFT_MOUSE_UP,
		I4INPUT_RIGHT_MOUSE_DOWN,
		I4INPUT_RIGHT_MOUSE_UP,
	};

	struct I4InputState
	{
		static void initialize();

		I4InputStateType	type;
		int					key;

		static int			MouseX;
		static int			MouseY;
		static bool			KeyPressed[256];
		static bool			LeftMousePressed;
		static bool			RightMousePressed;
		static bool			MoveMouseCenter;
	};

	class I4Framework
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
		float				getFps()								{ return fps; }
	public:
		static I4Framework*	getFramework()		{ return framework; }

		static void			createFramework();
		static void			destroyFramework();

	protected:	
		virtual	bool		onCreate()								{ return true; }
		virtual void		onDestroy()								{}
		virtual bool		onMessagePump()									{ return true; }
		virtual void		onYield()								{}

	private:
		bool				mainLoop(float deltaMs);
	protected:
		void*				windowID;
		string				title;
		unsigned int		width;
		unsigned int		height;
		bool				activated;
		float				fps;
		I4FrameCallback*	frameCallback;

	private:
		static I4Framework*	framework;
	};
}