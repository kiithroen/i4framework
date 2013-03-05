#pragma once

namespace i4object
{
	class I4BulletPhysics
	{
	public:
		I4BulletPhysics(void);
		~I4BulletPhysics(void);

		bool init();
		void destroy();
	};
}
