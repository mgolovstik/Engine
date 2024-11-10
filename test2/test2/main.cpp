#include "Constants.h"
#include "Engine.h"

//sf::Sprite* createNewSprite(std::string imagePath) {
//	sf::Image* image = new sf::Image();
//	image->loadFromFile(imagePath);
//	sf::Texture* texture = new sf::Texture;
//	texture->loadFromImage(*image);
//	sf::Sprite* sprite = new sf::Sprite(*texture);
//	sprite->setOrigin(texture->getSize().x / 2, texture->getSize().y / 2);
//	return sprite;
//}
//
//SceneObject* createPoint(double x, double y) {
//	return new SceneObject(TypesOfObject::Point, x, y);
//}

// D:/m_sfml_test/pictures
int main() {
	loadTextures();
	
	//sf::Sprite* sprite = new sf::Sprite();
	//sprite->setTexture(Textures[TypesOfObject::Player][Direction::Zero][0]);
	//delete sprite;

	sf::RenderWindow window(sf::VideoMode(windowX, windowY), "Test", sf::Style::Close | sf::Style::Titlebar);
	sf::Image imageOfIcon;
	imageOfIcon.loadFromFile("D:\\m_sfml_test\\pictures\\icon.png");
	window.setIcon(imageOfIcon.getSize().x, imageOfIcon.getSize().y, imageOfIcon.getPixelsPtr());
	
	Scene scene = Scene(&window, windowX, windowY);
	while (scene.updateScene()) {}
	window.close();
	return 0;
}