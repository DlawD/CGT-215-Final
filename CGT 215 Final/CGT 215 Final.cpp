#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <cstdlib>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.4;
void MoveBoard(PhysicsRectangle& board, int elapsedMS) {
	if (Keyboard::isKeyPressed(Keyboard::Right)) {
		Vector2f newPos(board.getCenter());
		newPos.x = newPos.x + (KB_SPEED * elapsedMS);
		board.setCenter(newPos);
	}
	if (Keyboard::isKeyPressed(Keyboard::Left)) {
		Vector2f newPos(board.getCenter());
		newPos.x = newPos.x - (KB_SPEED * elapsedMS);
		board.setCenter(newPos);
	}
	if (board.getCenter().x >= 730) {
		board.setCenter(Vector2f(730, 550));
	}
	if (board.getCenter().x <= 70) {
		board.setCenter(Vector2f(70, 550));
	}
}

int main()
{
	// Create our window and world with gravity 0,1
	RenderWindow window(VideoMode(800, 600), "Breakout clone");
	World world(Vector2f(0, 0));
	int score = 0;

	// Create the ball
	PhysicsCircle ball;
	ball.setCenter(Vector2f(400, 450));
	ball.setRadius(10);
	ball.applyImpulse(Vector2f(0, 0.3));
	world.AddPhysicsBody(ball);
	bool ballexist(true);

	// Create the floor
	PhysicsRectangle floor;
	floor.setSize(Vector2f(800, 20));
	floor.setCenter(Vector2f(400, 590));
	floor.setStatic(true);
	world.AddPhysicsBody(floor);

	// Create the leftside
	PhysicsRectangle leftside;
	leftside.setSize(Vector2f(20, 560));
	leftside.setCenter(Vector2f(10, 300));
	leftside.setStatic(true);
	world.AddPhysicsBody(leftside);

	// Create the rightside
	PhysicsRectangle rightside;
	rightside.setSize(Vector2f(20, 560));
	rightside.setCenter(Vector2f(790, 300));
	rightside.setStatic(true);
	world.AddPhysicsBody(rightside);

	// Create the ceiling
	PhysicsRectangle ceiling;
	ceiling.setSize(Vector2f(800, 20));
	ceiling.setCenter(Vector2f(400, 10));
	ceiling.setStatic(true);
	world.AddPhysicsBody(ceiling);

	// Create the board
	PhysicsRectangle board;
	board.setSize(Vector2f(100, 10));
	board.setCenter(Vector2f(400, 550));
	board.setStatic(true);
	world.AddPhysicsBody(board);

	Font font;
	if (!font.loadFromFile("arial.ttf")) {
		cout << "Couldn't load font arial.ttf" << endl;
		exit(1);
	}

	Text scoreText;
	scoreText.setFont(font);

	// Create the clone
	PhysicsShapeList<PhysicsRectangle> clones;
	for (int i = 30; i < 300; i = i + 20) {
		for (int j = 40; j < 780; j = j + 40) {
			PhysicsRectangle& clone = clones.Create();
			clone.setSize(Vector2f(40, 20));
			clone.setCenter(Vector2f(j, i));
			clone.setStatic(true);
			int x = 0 + (rand() % 256);
			int y = 0 + (rand() % 256);
			int z = 0 + (rand() % 256);
			clone.setFillColor(Color(x, y, z));
			world.AddPhysicsBody(clone);
			clone.onCollision = [&ball, &clones, &clone, &world, &score]
			(PhysicsBodyCollisionResult result) {
				if (result.object2 == ball) {
					clones.QueueRemove(clone);
					world.RemovePhysicsBody(clone);
					score += 10;
				}
			};
		}
	}
	
	float ia = 0;
	float ib = 0;
	ball.onCollision = [&ball, &ballexist, &world, &floor, &clones, &board, &ia, &ib]
	(PhysicsBodyCollisionResult result) {
		if (result.object2 == floor) {
			ballexist = false;
			world.RemovePhysicsBody(ball);
		}
		if (result.object2 == board) {
			ball.setVelocity(Vector2f(0, 0.3));
			float a = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 0.2 - 0.1;
			float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 0.1;
			ball.applyImpulse(Vector2f(a, b));
		}
	};

	Clock clock;
	Time lastTime(clock.getElapsedTime());
	Time currentTime(lastTime);

	while (ballexist == true) {

		// calculate MS since last frame
		currentTime = clock.getElapsedTime();
		Time deltaTime = currentTime - lastTime;
		long deltaMS = deltaTime.asMilliseconds();

		if (deltaMS > 0) {

			world.UpdatePhysics(deltaMS);
			lastTime = currentTime;

			MoveBoard(board, deltaMS);

		}

		window.clear(Color(0, 0, 0));

		for (PhysicsShape& clone : clones) {
			window.draw((PhysicsRectangle&)clone);
		}

		if (score >= 1000) {
			Text winText;
			winText.setFont(font);
			winText.setString("Win!");
			FloatRect textBounds = winText.getGlobalBounds();
			winText.setPosition(Vector2f(
				400 - (textBounds.width / 2),
				300 - (textBounds.height / 2)
			));
			window.draw(winText);
		}
		else {
			scoreText.setString(to_string(score));
			FloatRect textBounds = scoreText.getGlobalBounds();
			scoreText.setPosition(
				Vector2f(760 - textBounds.width, 560 - textBounds.height));
			window.draw(scoreText);
		}

		window.draw(ball);
		window.draw(floor);
		window.draw(leftside);
		window.draw(rightside);
		window.draw(ceiling);
		window.draw(board);
		window.display();
		clones.DoRemovals();
	}

	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setString("GAME OVER");
	FloatRect textBounds = gameOverText.getGlobalBounds();
	gameOverText.setPosition(Vector2f(
		400 - (textBounds.width / 2),
		300 - (textBounds.height / 2)
	));
	window.draw(gameOverText);
	window.display();
	while (true);

}
