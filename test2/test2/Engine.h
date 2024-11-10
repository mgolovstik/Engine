#pragma once
#include "EngineConstants.h"

struct Vector2d {
	double x, y;
	Vector2d(double x = 0, double y = 0) : x(x), y(y) {}

	void operator-=(Vector2d vec) {
		x -= vec.x;
		y -= vec.y;
	}
	void operator+=(Vector2d vec) {
		x += vec.x;
		y += vec.y;
	}
	void operator*=(double k) {
		x *= k;
		y *= k;
	}
	Vector2d operator-(Vector2d vec) {
		return Vector2d(x - vec.x, y - vec.y);
	}
	Vector2d operator+(Vector2d vec) {
		return Vector2d(x + vec.x, y + vec.y);
	}
	Vector2d operator*(double k) {
		return Vector2d(x * k, y * k);
	}
	Vector2d operator/(double k) {
		return Vector2d(x / k, y / k);
	}

	int getDirection() {
		if (x > EPS)
			return Direction::Right;
		if (x < -EPS)
			return Direction::Left;
		if (y > EPS)
			return Direction::Up;
		if (y < -EPS)
			return Direction::Down;
		return Direction::Zero;
	}
	
	// Get length of vector
	double getLength() {
		return sqrt(x * x + y * y);
	}

	// Convert vector to vector with length = 1
	void normalize() {
		double len = getLength();
		if (len <= EPS) {
			x = 1;
			y = 0;
			return;
		}
		x /= len;
		y /= len;
	}

	// Get normal of vector
	Vector2d getNorm() {
		return Vector2d(-y, x);
	}

	// Decompose on basis (vec) ans (normal of vec)
	// length of vec would be 1
	Vector2d decompose(Vector2d vec) {
		return Vector2d(vec.x * x + vec.y * y, vec.x * y - vec.y * x);
	}
};

class Space;
class Circle;

struct Circle {
	// Radius of Cricle
	double radius;

	Circle(double radius) : radius(radius) {}
	Circle() : Circle(0) {}
};

class Space final {
public:
	class PartOfSpace;
	class PhysicalObject;
	class SceneObject;
	class Player;
	
//	double getDistance(PhysicalObject* obj1, PhysicalObject* obj2);
//	Vector2d getDirectedDistance(PhysicalObject* obj1, PhysicalObject* obj2);
//
//private:
//	int makeNewPartOfSpace(int x, int y);
//	void updatePhysicalObjects(PhysicalObject* obj1, PhysicalObject* obj2);

public:
	class PhysicalObject {
		//friend class Space;

	private:
		// position in PartOfSpace
		double x, y;

		// mass of physical object
		// would be >= 1
		// if mass == MaxMass: object would be static
		const double mass;

		// maximum speed of object
		const double maxspeed;

		// vector of speed
		Vector2d speed;

		// Iterator on PartOfSpace
		PartOfSpace* part;

		// Circle of physical body
		Circle* body;


	protected:
		// experemental: set radius of circle
		void setSize(double newRadius) {
			body->radius = newRadius;
		}

	public:
		PhysicalObject(int type, double x, double y, PartOfSpace* part) : x(x), y(y), part(part), maxspeed(MaxSpeedOfObject[type]), mass(MassOfObject[type]) {
			body = new Circle();
			speed = Vector2d();
			if (part == nullptr)
				throw ErrorObjectWithoutPartOfSpace;
		}
		PhysicalObject(int type, PartOfSpace* part) : PhysicalObject(type, 0, 0, part) {}

		~PhysicalObject() {
			delete body;
		}

		virtual void moveDynamic(Vector2d speedVector, double time) {
			return;
		}

		// Setting new position
		void setPosition(double newX, double newY) {
			x = newX;
			y = newY;
		}
		// Addition vec to position
		void move(Vector2d vec) {
			x += vec.x;
			y += vec.y;
		}

		// Setting new speed
		void setSpeed(Vector2d newSpeed) {
			speed = newSpeed;
			if (speed.getLength() > maxspeed) {
				speed.normalize();
				speed *= maxspeed;
			}
		}
		// Addition vec to speed
		void addSpeed(Vector2d vec) {
			speed += vec;
			if (speed.getLength() > maxspeed) {
				speed.normalize();
				speed *= maxspeed;
			}
		}

		void setPartOfSpace(PartOfSpace* newPart) {
			if (newPart == nullptr)
				throw ErrorObjectWithoutPartOfSpace;
			part = newPart;
		}

		Vector2d getPosition() {
			return Vector2d(x, y);
		}
		Vector2d getSpeed() {
			return speed;
		}
		// Returning iterator on PartOfSpace
		PartOfSpace* getPartOfSpace() {
			return part;
		}

		double getCircleRadius() {
			return body->radius;
		}

		double getMass() {
			return mass;
		}
	};
	class SceneObject : public PhysicalObject {
		//friend class Space;

	private:
		// Type of Object
		int typeOfObject;
		// Direction for Sprite
		int direction;
		// texture index for Sprite
		int textureIndex;
		// Time of direction for Sprite
		double directionTime;
		// Sprite of Object
		sf::Sprite spriteOfObject;

		void setOrigin(double x, double y) {
			spriteOfObject.setOrigin(Textures[typeOfObject][Direction::Zero][0].getSize().x * x, Textures[typeOfObject][Direction::Zero][0].getSize().y * y);
		}

	protected:

	public:
		SceneObject(int type, double x, double y, PartOfSpace* part) : PhysicalObject(type, x, y, part) {
			directionTime = 0;
			textureIndex = 0;
			direction = Direction::Zero;

			typeOfObject = type;
			spriteOfObject.setTexture(Textures[type][Direction::Zero][0]);
			spriteOfObject.setOrigin(Textures[type][Direction::Zero][0].getSize().x * SpriteOriginsX[type], Textures[type][Direction::Zero][0].getSize().y * SpriteOriginsY[type]);

			setSize(RadiusOfObject[type]);
		}
		SceneObject(int type, PartOfSpace* part) : SceneObject(type, 0, 0, part) {}
		SceneObject(PartOfSpace* part) : SceneObject(TypesOfObject::Point, part) {}

		// Move with animation
		void moveDynamic(Vector2d speedVector, double time) {
			move(speedVector * time);

			if (speedVector.getDirection() != direction) {
				direction = speedVector.getDirection();
				directionTime = time;
				textureIndex = 0;

				spriteOfObject.setTexture(Textures[typeOfObject][direction][textureIndex]);
			}
			else {
				directionTime += time;
			}

			if (directionTime > TexturesPerSecond[typeOfObject][direction]) {
				directionTime -= TexturesPerSecond[typeOfObject][direction];
				textureIndex++;
				if (textureIndex >= TexturesCount[typeOfObject][direction])
					textureIndex -= TexturesCount[typeOfObject][direction];
				spriteOfObject.setTexture(Textures[typeOfObject][direction][textureIndex]);
			}
		}

		void setSpritePosition(double newX, double newY) {
			spriteOfObject.setPosition(newX, newY);
		}

		void draw(sf::RenderWindow* window) {
			window->draw(spriteOfObject);
		}

		sf::Sprite* getSprite() {
			return &spriteOfObject;
		}

		virtual void updateConclusion() {}
	};
	class PartOfSpace {
		friend class Space;

		// Position
		int x, y;

		// Index of adjacent PartOfSpace
		int left, right, up, down;

		// Vector of objects in PartOfSpace
		std::vector<SceneObject*> objects;

	public:
		PartOfSpace(int x, int y) : x(x), y(y) {
			left = right = up = down = -1;
			objects = {};
		}
		//~PartOfSpace() {
		//	for (auto i : objects)
		//		delete i;
		//}

		std::pair<int, int> getPosition() {
			return { x, y };
		}

		void deleteObjects() {
			for (auto i : objects)
				delete i;
		}
	};
	class Player : public SceneObject {
	private:
		double a;

	public:
		Player(double x, double y, PartOfSpace* part) : SceneObject(TypesOfObject::Player, x, y, part) {
			a = 0.1;
		}

		/*Vector2f getPositionOnWindow() {
			return positionOnWindow;
		}*/

		void updateConclusion(double time) {
			Vector2d dir;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				dir.x += 1;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				dir.x -= 1;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				dir.y += 1;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				dir.y -= 1;
			if (dir.x != 0 && dir.y != 0)
				dir.normalize();

			addSpeed(dir * a);
		}
	};

private:
	const double partSize;
	// Map for getting index of part with position
	std::map<std::pair<int, int>, int> indexOfPart;
	// Vector with parts of Space
	std::vector<PartOfSpace> parts;

	int makeNewPartOfSpace(int x, int y) {
		int ind = indexOfPart[{x, y}] = parts.size();
		parts.push_back(PartOfSpace(x, y));

		if (indexOfPart.find({ x - 1, y }) != indexOfPart.end()) {
			parts[ind].left = indexOfPart[{x - 1, y}];
			parts[indexOfPart[{x - 1, y}]].right = ind;
		}
		if (indexOfPart.find({ x + 1, y }) != indexOfPart.end()) {
			parts[ind].right = indexOfPart[{x + 1, y}];
			parts[indexOfPart[{x + 1, y}]].left = ind;
		}
		if (indexOfPart.find({ x, y - 1 }) != indexOfPart.end()) {
			parts[ind].down = indexOfPart[{x, y - 1}];
			parts[indexOfPart[{x, y - 1}]].up = ind;
		}
		if (indexOfPart.find({ x, y + 1 }) != indexOfPart.end()) {
			parts[ind].up = indexOfPart[{x, y + 1}];
			parts[indexOfPart[{x, y + 1}]].down = ind;
		}
		return ind;
	}
	int getPartOfSpace(int x, int y) {
		if (indexOfPart.find({ x, y }) == indexOfPart.end())
			return makeNewPartOfSpace(x, y);
		return indexOfPart[{x, y}];
	}
	PartOfSpace* getLeftPartOfSpace(PartOfSpace* part) {
		if (part->left != -1)
			return &parts[part->left];
		return &parts[makeNewPartOfSpace(part->x - 1, part->y)];
	}
	PartOfSpace* getRightPartOfSpace(PartOfSpace* part) {
		if (part->right != -1)
			return &parts[part->right];
		return &parts[makeNewPartOfSpace(part->x + 1, part->y)];
	}
	PartOfSpace* getDownPartOfSpace(PartOfSpace* part) {
		if (part->down != -1)
			return &parts[part->down];
		return &parts[makeNewPartOfSpace(part->x, part->y - 1)];
	}
	PartOfSpace* getUpPartOfSpace(PartOfSpace* part) {
		if (part->up != -1)
			return &parts[part->up];
		return &parts[makeNewPartOfSpace(part->x, part->y + 1)];
	}


	// obj1, obj2 != nullptr
	void updatePhysicalObjects(PhysicalObject* obj1, PhysicalObject* obj2) {
		Vector2d direction = getDirectedDistance(obj1, obj2);
		if (direction.getLength() > obj1->getCircleRadius() + obj2->getCircleRadius())
			return;

		direction.normalize();

		// perpendicular vector
		Vector2d ndirection = Vector2d(-direction.y, direction.x);
		
		Vector2d speed1 = obj1->getSpeed().decompose(direction);
		Vector2d speed2 = obj2->getSpeed().decompose(direction);
		double m1 = obj1->getMass();
		double m2 = obj2->getMass();

		if (m1 == MaxMass) {
			if (m2 == MaxMass) {
				speed1.x = speed2.x = (speed1.x + speed2.x) / 2;
			}
			else {
				speed2.x = 0;
			}
		}
		else {
			if (m2 == MaxMass) {
				speed1.x = 0;
			}
			else {
				speed1.x = speed2.x = (m1 * speed1.x + m2 * speed2.x) / 2;
				speed1.x /= m1;
				speed2.x /= m2;
			}
		}

		speed1 = direction * speed1.x + ndirection * speed1.y;
		speed2 = direction * speed2.x + ndirection * speed2.y;
		obj1->setSpeed(speed1);
		obj2->setSpeed(speed2);
	}
	void updatePhysicalObjectPosition(PhysicalObject* obj, double time) {
		Vector2d position = obj->getPosition() + obj->getSpeed();
		PartOfSpace* part = obj->getPartOfSpace();
		while (position.x < 0) {
			position.x += partSize;
			part = getLeftPartOfSpace(part);
		}
		while (position.x > partSize) {
			position.x -= partSize;
			part = getRightPartOfSpace(part);
		}
		while (position.y < 0) {
			position.y += partSize;
			part = getDownPartOfSpace(part);
		}
		while (position.y > partSize) {
			position.y -= partSize;
			part = getUpPartOfSpace(part);
		}
		obj->setPartOfSpace(part);
		obj->moveDynamic(position - obj->getPosition(), time);
	}

public:
	Space(double partSizeCreating) : partSize(partSizeCreating) {
		indexOfPart = {};
		parts = {};
	}

	// obj1, obj2 != nullptr
	Vector2d getDirectedDistance(PhysicalObject* obj1, PhysicalObject* obj2) {
		PartOfSpace* part1 = obj1->getPartOfSpace();
		PartOfSpace* part2 = obj2->getPartOfSpace();
		Vector2d vec = Vector2d(partSize * (part1->x - part2->x), partSize * (part1->y - part2->y));
		vec += obj2->getPosition() - obj1->getPosition();
		return vec;
	}
	// obj1, obj2 != nullptr
	double getDistance(PhysicalObject* obj1, PhysicalObject* obj2) {
		PartOfSpace* part1 = obj1->getPartOfSpace();
		PartOfSpace* part2 = obj2->getPartOfSpace();
		Vector2d vec = Vector2d(partSize * (part1->x - part2->x), partSize * (part1->y - part2->y));
		vec += obj2->getPosition() - obj1->getPosition();
		return sqrt(vec.x * vec.x + vec.y * vec.y);
	}

	void updateSpace(double time) {
		for (int i = (int)parts.size() - 1; i >= 0; i--) {
			for (auto ind : parts[i].objects)
				ind->updateConclusion();
		}

		for (int i = (int)parts.size() - 1; i >= 0; i--) {
			for (int k = (int)parts[i].objects.size() - 1; k >= 0; k--) {
				for (int j = k - 1; j >= 0; j--)
					updatePhysicalObjects(parts[i].objects[k], parts[i].objects[j]);
				for (int j = i - 1; j >= 0; j--)
					for (auto ind : parts[j].objects)
						updatePhysicalObjects(parts[i].objects[k], ind);
			}
		}
		for (int i = (int)parts.size() - 1; i >= 0; i--) {
			for (auto ind : parts[i].objects)
				updatePhysicalObjectPosition(ind, time);
		}

		return;
	}

	// append object into space
	SceneObject* append(int type, Vector2d position, int partPositionX = 0, int partPositionY = 0) {
		while (position.x < 0) {
			position.x += partSize;
			partPositionX--;
		}
		while (position.x > partSize) {
			position.x -= partSize;
			partPositionX++;
		}
		while (position.y < 0) {
			position.y += partSize;
			partPositionY--;
		}
		while (position.y > partSize) {
			position.y -= partSize;
			partPositionX++;
		}
		if (indexOfPart.find({ partPositionX, partPositionY }) == indexOfPart.end())
			makeNewPartOfSpace(partPositionX, partPositionY);

		SceneObject* obj;
		switch (type) {
		case TypesOfObject::Player:
			obj = new Player(position.x, position.y, &parts[indexOfPart[std::make_pair(partPositionX, partPositionY)]]);
			break;
		default:
			obj = new SceneObject(type, position.x, position.y, &parts[indexOfPart[std::make_pair(partPositionX, partPositionY)]]);
		}
		parts[indexOfPart[std::make_pair(partPositionX, partPositionY)]].objects.push_back(obj);
		return obj;
	}

	std::vector<SceneObject*> getObjectInCircle(int partPositionX, int partPositionY, int radius) {
		std::vector<SceneObject*> vec;
		for (int i = partPositionX - radius; i <= partPositionX + radius; i++)
			for (int j = partPositionY - radius; j <= partPositionY + radius; j++)
				for (auto ind : parts[getPartOfSpace(i, j)].objects)
					vec.push_back(ind);
		return vec;
	}
};

class Scene {
	const float windowXL, windowXR, windowYL, windowYR;
	Space* space;
	Space::SceneObject* player;
	sf::RenderWindow* window;
	sf::Clock clock;

public:
	Scene(sf::RenderWindow* window, float windowXL, float windowXR, float windowYL, float windowYR) : window(window), windowXL(windowXL), windowXR(windowXR), windowYL(windowYL), windowYR(windowYR) {
		space = new Space(std::max(windowXR - windowXL, windowYR - windowYL) / PixelsInMeter + PixelsInMeter / MaxPixelsInTextureX + PixelsInMeter / MaxPixelsInTextureY);
		player = space->append(TypesOfObject::Player, Vector2d(40, 40));
		clock.restart();
	}
	Scene(sf::RenderWindow* window, float windowXSize, float windowYSize) : Scene(window, 0, windowXSize, 0, windowYSize) {}
	~Scene() {
		delete space;
	}

	void draw() {
		std::pair<int, int> partPosition = player->getPartOfSpace()->getPosition();
		std::vector<Space::SceneObject*> objects = space->getObjectInCircle(partPosition.first, partPosition.second, 1);

		window->clear(sf::Color(255, 255, 255));
		for (auto ind : objects) {
			Vector2d direction = space->getDirectedDistance(player, ind) * PixelsInMeter;
			if (abs(direction.x) <= windowXR - windowXL && abs(direction.y) <= windowYR - windowYL) {
				ind->setSpritePosition(direction.x + windowXL, direction.y + windowYL);
				ind->draw(window);
			}
		}
		window->display();
	}

	bool updateScene() {
		double time = clock.getElapsedTime().asSeconds();
		time = std::min(time, 0.5);
		clock.restart();
		space->updateSpace(time);

		sf::Event event;
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				return 0;
		}
		
		//std::cout << "+" << std::endl;
		draw();
		return 1;
	}
};

// change moveDynamic in Space::updatePhysicalObjectPosition
// vector ->
// 
// 
//