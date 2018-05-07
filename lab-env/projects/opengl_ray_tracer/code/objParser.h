#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "mathVec3.h"
#include "gameObject.h"

class ObjParser
{
public:
	ObjParser();
	~ObjParser();
	
	static GameObject* LoadMesh(const char *filename);
	static void LoadScene(const char *filename, std::vector<GameObject*> &objectsInScene);

};