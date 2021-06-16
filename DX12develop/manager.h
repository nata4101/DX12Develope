#pragma once
#include "Singleton.h"

class Manager : public Singleton<Manager>
{
private:
	 class CScene* m_scene;
public:
	 void Init();
	 void Uninit();
	 void Update();
	 void Draw();

	 class CScene* GetScene(void) { return m_scene; }
};