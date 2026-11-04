#pragma once
#include <DxLib.h>

// ランバート
struct CB_PixelLambert
{
	VECTOR lightDirection; 
};

// 金属
struct CB_PixelMetal
{
	COLOR_F diffuseColor;  
	COLOR_F ambientColor;  
	COLOR_F specularColor; 
	VECTOR  lightDirection;

	float   specularPower; 
	float   cameraPosX;    
	float   cameraPosY;  
	float   cameraPosZ;    
};