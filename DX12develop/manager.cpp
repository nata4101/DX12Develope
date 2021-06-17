#include "main.h"
#include "manager.h"
#include "Renderer.h"

Renderer* renderer;


void Manager::Init()
{
	renderer = Renderer::GetInstance();
	renderer->Init();
}


void Manager::Uninit()
{
	renderer->Uninit();
}

void Manager::Update()
{

}

void Manager::Draw()
{
}
