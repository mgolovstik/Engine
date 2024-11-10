#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_map>

const char ErrorCreatingPartOfSpace[] = "Part with a such position already exists!";
const char ErrorObjectWithoutPartOfSpace[] = "Object without PartOfSpace";

const int windowX = 800, windowY = 800;
const int MaxPixelsInTextureX = 50, MaxPixelsInTextureY = 50;
const double PixelsInMeter = 50, EPS = 1e-6;
// Maximum mass of physical object
const double MaxMass = 1e10;

const int MaxTexturesCount = 4;
namespace TypesOfObject { enum { Point, Player, LastTypesOfObject }; }
// if you want to mix directions you could xor it. for ex. if you want WD or WSA you could W^D or W^S^A
namespace Direction { enum { Zero = 0, Up = 2, Down = 3, Right = 8, Left = 12, Last = 16 }; }
// Shape types
namespace ShapeType { enum { Circle }; }

int TexturesCount[TypesOfObject::LastTypesOfObject][Direction::Last];
double TexturesPerSecond[TypesOfObject::LastTypesOfObject][Direction::Last];
double SpriteOriginsX[TypesOfObject::LastTypesOfObject];
double SpriteOriginsY[TypesOfObject::LastTypesOfObject];
double RadiusOfObject[TypesOfObject::LastTypesOfObject];
double MaxSpeedOfObject[TypesOfObject::LastTypesOfObject];
double MassOfObject[TypesOfObject::LastTypesOfObject];
std::string TexturesPaths[TypesOfObject::LastTypesOfObject][Direction::Last][MaxTexturesCount];
sf::Image TexturesImages[TypesOfObject::LastTypesOfObject][Direction::Last][MaxTexturesCount];
sf::Texture Textures[TypesOfObject::LastTypesOfObject][Direction::Last][MaxTexturesCount];

void loadTextures(std::string startPath = "D:\\m_sfml_test\\pictures\\") {
	// Point
	{
		SpriteOriginsX[TypesOfObject::Point] = 0.5;
		SpriteOriginsY[TypesOfObject::Point] = 0.5;
		RadiusOfObject[TypesOfObject::Point] = 0.5;
		MassOfObject[TypesOfObject::Point] = 2;
		MaxSpeedOfObject[TypesOfObject::Point] = 2;

		TexturesCount[TypesOfObject::Point][Direction::Zero] = 1;
		TexturesPaths[TypesOfObject::Point][Direction::Zero][0] = startPath + "point.png";
	}
	// Player
	{
		SpriteOriginsX[TypesOfObject::Player] = 0.5;
		SpriteOriginsY[TypesOfObject::Player] = 0.7;
		RadiusOfObject[TypesOfObject::Player] = 0.5;
		MassOfObject[TypesOfObject::Player] = 1;
		MaxSpeedOfObject[TypesOfObject::Player] = 3;

		TexturesCount[TypesOfObject::Player][Direction::Zero] = 1;
		TexturesPaths[TypesOfObject::Player][Direction::Zero][0] = startPath + "menStaining.png";

		TexturesCount[TypesOfObject::Player][Direction::Up] = 4;
		TexturesPaths[TypesOfObject::Player][Direction::Up][0] = startPath + "menWalkingUp1.png";
		TexturesPaths[TypesOfObject::Player][Direction::Up][1] = startPath + "menWalkingUp2.png";
		TexturesPaths[TypesOfObject::Player][Direction::Up][2] = startPath + "menWalkingUp1.png";
		TexturesPaths[TypesOfObject::Player][Direction::Up][3] = startPath + "menWalkingUp2.png";

		TexturesCount[TypesOfObject::Player][Direction::Down] = 4;
		TexturesPaths[TypesOfObject::Player][Direction::Down][0] = startPath + "menWalkingUp1.png";
		TexturesPaths[TypesOfObject::Player][Direction::Down][1] = startPath + "menWalkingUp2.png";
		TexturesPaths[TypesOfObject::Player][Direction::Down][2] = startPath + "menWalkingUp1.png";
		TexturesPaths[TypesOfObject::Player][Direction::Down][3] = startPath + "menWalkingUp2.png";

		TexturesCount[TypesOfObject::Player][Direction::Left] = 3;
		TexturesPaths[TypesOfObject::Player][Direction::Left][0] = startPath + "menWalkingLeft1.png";
		TexturesPaths[TypesOfObject::Player][Direction::Left][1] = startPath + "menWalkingLeft2.png";
		TexturesPaths[TypesOfObject::Player][Direction::Left][2] = startPath + "menWalkingLeft3.png";

		TexturesCount[TypesOfObject::Player][Direction::Right] = 3;
		TexturesPaths[TypesOfObject::Player][Direction::Right][0] = startPath + "menWalkingRight1.png";
		TexturesPaths[TypesOfObject::Player][Direction::Right][1] = startPath + "menWalkingRight2.png";
		TexturesPaths[TypesOfObject::Player][Direction::Right][2] = startPath + "menWalkingRight3.png";

	}
	//

	for (int type = 0; type < TypesOfObject::LastTypesOfObject; type++) {
		for (int dir = 0; dir < Direction::Last; dir++) {
			if (TexturesCount[type][dir] == 0) {
				TexturesCount[type][dir] = TexturesCount[type][dir & (dir - 1)];
				for (int i = TexturesCount[type][dir] - 1; i >= 0; i--)
					TexturesPaths[type][dir][i] = TexturesPaths[type][dir & (dir - 1)][i];
			}

			TexturesPerSecond[type][dir] = 1.0 / TexturesCount[type][dir];
			for (int i = TexturesCount[type][dir] - 1; i >= 0; i--) {
				TexturesImages[type][dir][i].loadFromFile(TexturesPaths[type][dir][i]);
				Textures[type][dir][i].loadFromImage(TexturesImages[type][dir][i]);
			}
		}
	}
}