#pragma once

#include "i4core.h"
#include "I4Framework.h"

namespace i4core
{
	class FrameCallback;

	enum InputStateType
	{
		INPUT_KEY_DOWN = 0,
		INPUT_KEY_UP,
		INPUT_LEFT_MOUSE_DOWN,
		INPUT_LEFT_MOUSE_UP,
		INPUT_RIGHT_MOUSE_DOWN,
		INPUT_RIGHT_MOUSE_UP,
	};

	struct InputState
	{
		static void initialize();

		InputStateType	type;
		int					key;

		static int			MouseX;
		static int			MouseY;
		static bool			KeyPressed[256];
		static bool			LeftMousePressed;
		static bool			RightMousePressed;
		static bool			MoveMouseCenter;
	};

	class Framework
	{
	public:
		Framework();
		virtual ~Framework();

		bool				create(unsigned int width, unsigned int height, const string& title);
		void				destroy();
		void				run();
		
		void				setFrameCallback(FrameCallback* callback);

		void*				getWindowID() const						{ return windowID; }
		string				getTitle() const						{ return title; }
		unsigned int		getWidth() const						{ return width; }
		unsigned int		getHeight() const						{ return height; }
		bool				isActivated() const						{ return activated; }
		float				getFps()								{ return fps; }
	public:
		static Framework*	getFramework()		{ return framework; }

		static void			createFramework();
		static void			destroyFramework();

	protected:	
		virtual	bool		onCreate()								{ return true; }
		virtual void		onDestroy()								{}
		virtual bool		onMessagePump()							{ return true; }

	private:
		bool				mainLoop(float deltaMs);
	protected:
		void*				windowID;
		string				title;
		unsigned int		width;
		unsigned int		height;
		bool				activated;
		float				fps;
		FrameCallback*	frameCallback;

	private:
		static Framework*	framework;
	};
}