#include "stdafx.h"
#include "I4Profile.h"
using namespace i4core;

void foo()
{
	PROFILE("foo()");
}

void funcA()
{
	PROFILE("funcA()");

	for (int i = 0; i < 3; i++)
	{
		foo();
	}
}

void funcB()
{
	PROFILE("funcB()");
}

// �������Ϸ����� ���������� �ݼ��� �׽�Ʈ�Ѵ�.
// �׿� ���(�ð�, ...) �� �ý��ۿ� �������� �κ��̶� �׽�Ʈ���� �ʴ´�.
TEST(I4Profile, Test)
{	
	{
		PROFILE("A");

		for (int i = 0; i < 2; i++)
		{
			{
				PROFILE("a");

				for (int j = 0; j < 2; j++)
				{
					PROFILE("1");
				}
			}

			{
				PROFILE("b");
			}

			{
				PROFILE("c");
			}
		}
	}

	{
		PROFILE("B");

		for (int j = 0; j < 2; j++)
		{
			funcA();
		}
			
		for (int i = 0; i < 2; i++)
		{
			funcB();
		}
	}

	I4ProfileIterator itr = I4ProfileManager::getRootIterator();
	
	EXPECT_STREQ("root", itr.getCurName());

	itr.firstChild();
	EXPECT_STREQ("B", itr.getCurChildName());
	EXPECT_EQ(1, itr.getCurChildTotalCalls());

	itr.nextChild();
	EXPECT_STREQ("A", itr.getCurChildName());
	EXPECT_EQ(1, itr.getCurChildTotalCalls());

	itr.nextChild();
	EXPECT_FALSE(itr.isCurChildValid());

	itr.enterChild(0);
	EXPECT_STREQ("B", itr.getCurName());

	itr.firstChild();
	EXPECT_STREQ("funcB()", itr.getCurChildName());
	EXPECT_EQ(2, itr.getCurChildTotalCalls());

	itr.nextChild();
	EXPECT_STREQ("funcA()", itr.getCurChildName());
	EXPECT_EQ(2, itr.getCurChildTotalCalls());

	itr.enterChild(1);
	EXPECT_STREQ("funcA()", itr.getCurName());

	itr.firstChild();
	EXPECT_STREQ("foo()", itr.getCurChildName());
	EXPECT_EQ(6, itr.getCurChildTotalCalls());

	itr.nextChild();
	EXPECT_FALSE(itr.isCurChildValid());

	itr.enterParent();
	itr.enterParent();
	EXPECT_STREQ("root", itr.getCurName());
	
	itr.enterChild(1);
	EXPECT_STREQ("A", itr.getCurName());

	itr.enterChild(2);
	EXPECT_STREQ("a", itr.getCurName());
	EXPECT_EQ(2, itr.getCurTotalCalls());

	itr.enterChild(0);
	EXPECT_STREQ("1", itr.getCurName());
	EXPECT_EQ(4, itr.getCurTotalCalls());
}