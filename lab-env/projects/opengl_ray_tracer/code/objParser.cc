#include "objParser.h"

ObjParser::ObjParser()
{}

ObjParser::~ObjParser()
{}

GameObject* ObjParser::LoadMesh(const char *filename)
{
	// Open file
	std::ifstream file(filename, std::ios::in);
	if (!file.is_open())
	{
		fprintf(stderr, "Could not read file\n");
		return nullptr;
	}


	Vec3 min;
	Vec3 max;
	float x,y,z;
	int a,b,c,d,tmp;
	std::vector<float> tmpValues;
	GameObject *go = new GameObject();

	// Loop over file
	while (!file.eof())
	{
		std::string line = "";
		std::getline(file, line);


		if (line[0] == 'v' && line[1] == ' ')
		{
			sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);

			tmpValues.push_back(x);
			tmpValues.push_back(y);
			tmpValues.push_back(z);

			// Bounding box
			if (x < min.x)
				min.x = x;
			if (y < min.y)
				min.y = y;
			if (z < min.z)
				min.z = z;

			if (x > max.x)
				max.x = x;
			if (y > max.y)
				max.y = y;
			if (z > max.z)
				max.z = z;
		}
		// else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
		// {}
		// else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
		// {}
		else if (line[0] == 'f' && line[1] == ' ')
		{	
			int success;
			bool done = false;

			// Quads
			{
				if (!done)
				{
					// Position only
					success = sscanf(line.c_str(), "f %i %i %i %i", &a, &b, &c, &d);
					done = (success == 4);
					go->nrVerts = 4;
				}
				if (!done)
				{
					// Position/uv
					success = sscanf(line.c_str(), "f %i/%i %i/%i %i/%i %i/%i",
						&a, &tmp,		&b, &tmp,		&c, &tmp,		&d, &tmp);
					done = (success == 8);
					go->nrVerts = 4;
				}
				if (!done)
				{
					// Position/uv/normal
					success = sscanf(line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i",
						&a, &tmp, &tmp,		&b, &tmp, &tmp,		&c, &tmp, &tmp, 	&d, &tmp, &tmp);
					done = (success == 12);
					go->nrVerts = 4;
				}
				if (!done)
				{
					// Position//normal
					success = sscanf(line.c_str(), "f %i//%i %i//%i %i//%i %i//%i",
						&a, &tmp,		&b, &tmp, 		&c, &tmp, 		&d, &tmp);
					done = (success == 8);
					go->nrVerts = 4;
				}
			}

			// Tris
			{
				if (!done)
				{
					// Position only
					success = sscanf(line.c_str(), "f %i %i %i", &a, &b, &c);
					done = (success == 3);
					go->nrVerts = 3;
				}
				if (!done)
				{
					// Position/uv
					success = sscanf(line.c_str(), "f %i/%i %i/%i %i/%i",
						&a, &tmp,		&b, &tmp,		&c, &tmp);
					done = (success == 6);
					go->nrVerts = 3;
				}
				if (!done)
				{
					// Position/uv/normal
					success = sscanf(line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i",
						&a, &tmp, &tmp,		&b, &tmp, &tmp,		&c, &tmp, &tmp);
					done = (success == 9);
					go->nrVerts = 3;
				}
				if (!done)
				{
					// Position//normal
					success = sscanf(line.c_str(), "f %i//%i %i//%i %i//%i",
						&a, &tmp,		&b, &tmp, 		&c, &tmp);
					done = (success == 6);
					go->nrVerts = 3;
				}
			}


			if (!done)
			{
				fprintf(stderr, "Invalid .obj file!\n");
				file.close();
				return nullptr;
			}
			

			int index = a - 1;
			go->values.push_back(tmpValues[index*3 + 0]);
			go->values.push_back(tmpValues[index*3 + 1]);
			go->values.push_back(tmpValues[index*3 + 2]);

			index = b - 1;
			go->values.push_back(tmpValues[index*3 + 0]);
			go->values.push_back(tmpValues[index*3 + 1]);
			go->values.push_back(tmpValues[index*3 + 2]);

			index = c - 1;
			go->values.push_back(tmpValues[index*3 + 0]);
			go->values.push_back(tmpValues[index*3 + 1]);
			go->values.push_back(tmpValues[index*3 + 2]);

			if (go->nrVerts == 4)
			{
				index = d - 1;
				go->values.push_back(tmpValues[index*3 + 0]);
				go->values.push_back(tmpValues[index*3 + 1]);
				go->values.push_back(tmpValues[index*3 + 2]);
			}
		}
	}


	// Set the AABB for the loaded mesh
	go->SetAABB(min, max);


	// fprintf(stderr, "Done parsing file\n");

	// Close file
	file.close();

	return go;
}

void ObjParser::LoadScene(const char *filename, std::vector<GameObject *> &objectsInScene)
{
	fprintf(stderr, "Reading file %s\n", filename);


	// Open file
	std::ifstream file(filename, std::ios::in);
	if (!file.is_open())
	{
		fprintf(stderr, "Could not read file\n");
		return;
	}


	Vec3 min(10000, 10000, 10000);
	Vec3 max(-10000, -10000, -10000);
	float x,y,z;
	int a,b,c,d,tmp;
	std::vector<float> tmpValues;
	GameObject *go = nullptr;

	// Loop over file
	while (!file.eof())
	{
		std::string line = "";
		std::getline(file, line);


		if (line[0] == 'v' && line[1] == ' ')
		{
			sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);

			tmpValues.push_back(x);
			tmpValues.push_back(y);
			tmpValues.push_back(z);

			// Bounding box
			if (x < min.x)
				min.x = x;
			if (y < min.y)
				min.y = y;
			if (z < min.z)
				min.z = z;

			if (x > max.x)
				max.x = x;
			if (y > max.y)
				max.y = y;
			if (z > max.z)
				max.z = z;
		}
		else if (line[0] == 'f' && line[1] == ' ')
		{	
			int success;
			bool done = false;

			// Quads
			{
				if (!done)
				{
					// Position only
					success = sscanf(line.c_str(), "f %i %i %i %i", &a, &b, &c, &d);
					done = (success == 4);
					go->nrVerts = 4;
				}
				if (!done)
				{
					// Position/uv
					success = sscanf(line.c_str(), "f %i/%i %i/%i %i/%i %i/%i",
						&a, &tmp,		&b, &tmp,		&c, &tmp,		&d, &tmp);
					done = (success == 8);
					go->nrVerts = 4;
				}
				if (!done)
				{
					// Position/uv/normal
					success = sscanf(line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i",
						&a, &tmp, &tmp,		&b, &tmp, &tmp,		&c, &tmp, &tmp, 	&d, &tmp, &tmp);
					done = (success == 12);
					go->nrVerts = 4;
				}
				if (!done)
				{
					// Position//normal
					success = sscanf(line.c_str(), "f %i//%i %i//%i %i//%i %i//%i",
						&a, &tmp,		&b, &tmp, 		&c, &tmp, 		&d, &tmp);
					done = (success == 8);
					go->nrVerts = 4;
				}
			}

			// Tris
			{
				if (!done)
				{
					// Position only
					success = sscanf(line.c_str(), "f %i %i %i", &a, &b, &c);
					done = (success == 3);
					go->nrVerts = 3;
				}
				if (!done)
				{
					// Position/uv
					success = sscanf(line.c_str(), "f %i/%i %i/%i %i/%i",
						&a, &tmp,		&b, &tmp,		&c, &tmp);
					done = (success == 6);
					go->nrVerts = 3;
				}
				if (!done)
				{
					// Position/uv/normal
					success = sscanf(line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i",
						&a, &tmp, &tmp,		&b, &tmp, &tmp,		&c, &tmp, &tmp);
					done = (success == 9);
					go->nrVerts = 3;
				}
				if (!done)
				{
					// Position//normal
					success = sscanf(line.c_str(), "f %i//%i %i//%i %i//%i",
						&a, &tmp,		&b, &tmp, 		&c, &tmp);
					done = (success == 6);
					go->nrVerts = 3;
				}
			}


			if (!done)
			{
				fprintf(stderr, "Invalid .obj file!\n");
				file.close();
				return;
			}
			

			int index = a - 1;
			go->values.push_back(tmpValues[index*3 + 0]);
			go->values.push_back(tmpValues[index*3 + 1]);
			go->values.push_back(tmpValues[index*3 + 2]);

			index = b - 1;
			go->values.push_back(tmpValues[index*3 + 0]);
			go->values.push_back(tmpValues[index*3 + 1]);
			go->values.push_back(tmpValues[index*3 + 2]);

			index = c - 1;
			go->values.push_back(tmpValues[index*3 + 0]);
			go->values.push_back(tmpValues[index*3 + 1]);
			go->values.push_back(tmpValues[index*3 + 2]);

			if (go->nrVerts == 4)
			{
				index = d - 1;
				go->values.push_back(tmpValues[index*3 + 0]);
				go->values.push_back(tmpValues[index*3 + 1]);
				go->values.push_back(tmpValues[index*3 + 2]);
			}
		}

		else if (line[0] == 'o' && line[1] == ' ')
		{
			if (go != nullptr)
				go->SetAABB(min, max);
			
			
			// Reset AABB values
			min = Vec3(10000, 10000, 10000);
			max = Vec3(-10000, -10000, -10000);

			// Create a new GameObject
			go = new GameObject();
			objectsInScene.push_back(go);
			
		}

		else if (line.substr(0, 6) == "usemtl")
		{
			if (line.substr(7) == "Red")
				go->color = Vec3(0.8f, 0.0f, 0.0f);
			else if (line.substr(7) == "Green")
				go->color = Vec3(0.0f, 0.8f, 0.0f);
			else if (line.substr(7) == "Blue")
				go->color = Vec3(0.0f, 0.0f, 0.8f);
			else if (line.substr(7) == "White")
				go->color = Vec3(1.0f, 1.0f, 1.0f);
			else if (line.substr(7) == "Black")
				go->color = Vec3(0.0f, 0.0f, 0.0f);
			else
				go->color = Vec3(1.0f, 1.0f, 1.0f);
			
		}
	}

	// When reaching the end of the file, set the AABB of the last object as well
	go->SetAABB(min, max);


	
	fprintf(stderr, "Done parsing file. Created %li objects\n", objectsInScene.size());

	// Close file
	file.close();
}