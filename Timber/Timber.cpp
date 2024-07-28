#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>


struct Point2D {
	int x;
	int y;
};


void updateBranches(int seed);
const int NUM_BRANCHES = 6;

sf::Sprite branches[NUM_BRANCHES];

enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];


int main()
{
	int playerScore{ 0 };
	const Point2D TREE_POSITION{ 810,  0 };

	//create window
	sf::VideoMode vm(1920, 1080);
	sf::RenderWindow window(vm, "Timber", sf::Style::Default);

	//background texture
	sf::Texture textureBackground;
	textureBackground.loadFromFile("assets/graphics/background.png");
	sf::Sprite spriteBackground;
	spriteBackground.setTexture(textureBackground);
	spriteBackground.setPosition(0, 0);


	//create tree
	sf::Texture textureTree;
	textureTree.loadFromFile("assets/graphics/tree.png");
	sf::Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(TREE_POSITION.x, TREE_POSITION.y);


	//create Bee
	sf::Texture textureBee;
	textureBee.loadFromFile("assets/graphics/bee.png");
	sf::Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);
	bool beeActive = false;
	float beeSpeed = 0.0f;


	//create Clouds
	sf::Texture textureCloud;
	textureCloud.loadFromFile("assets/graphics/cloud.png");

	sf::Sprite spriteCloud1;
	sf::Sprite spriteCloud2;
	sf::Sprite spriteCloud3;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);

	spriteCloud1.setPosition(0, 0);
	spriteCloud2.setPosition(0, 250);
	spriteCloud3.setPosition(0, 500);

	float cloud1Speed = 0.0f;
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;

	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;



	// CLOCK
	sf::Clock clock;
	sf::RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(sf::Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(sf::Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);
	sf::Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;


	// Pause mode
	bool paused = true;

	// load font
	sf::Font font;
	font.loadFromFile("assets/fonts/KOMIKAP_.ttf");
	if (!font.loadFromFile("assets/fonts/KOMIKAP_.ttf")) {
		return -1; // Handle font loading error
	}

	// Start Screen
	sf::Text pauseText;
	pauseText.setString("Press Enter to start!");
	pauseText.setCharacterSize(75);
	pauseText.setFillColor(sf::Color::White);
	pauseText.setFont(font);

	//center origin of pause text
	sf::FloatRect textRect = pauseText.getLocalBounds();
	pauseText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	pauseText.setPosition(1920 / 2.0f, 1080 / 2.0f);


	// Score
	int score = 0;

	sf::Text scoreText;
	scoreText.setString("Score = 0");
	scoreText.setCharacterSize(100);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(20, 20);
	scoreText.setFont(font);


	// Branches
	sf::Texture textureBranch;
	textureBranch.loadFromFile("assets/graphics/branch.png");

	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		branches[i].setOrigin(220, 20);
	}


	//PLAYER
	// Prepare the player
	sf::Texture texturePlayer;
	texturePlayer.loadFromFile("assets/graphics/player.png");
	sf::Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);


	// The player starts on the left
	side playerSide = side::LEFT;


	// Prepare the gravestone
	sf::Texture textureRIP;
	textureRIP.loadFromFile("assets/graphics/rip.png");
	sf::Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);


	// Prepare the axe
	sf::Texture textureAxe;
	textureAxe.loadFromFile("assets/graphics/axe.png");
	sf::Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);


	// Line the axe up with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;


	// Prepare the flying log
	sf::Texture textureLog;
	textureLog.loadFromFile("assets/graphics/log.png");
	sf::Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	// CONTROLS
	bool acceptInput = false;


	// SOUNDS
	sf::SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("assets/sounds/chop.wav");
	sf::Sound chop;
	chop.setBuffer(chopBuffer);

	sf::SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("assets/sounds/death.wav");
	sf::Sound death;
	death.setBuffer(deathBuffer);

	sf::SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("assets/sounds/out_of_time.wav");
	sf::Sound oot;
	oot.setBuffer(ootBuffer);





	// LOOP
	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::KeyReleased && !paused) {
				acceptInput = true;
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
			paused = false;
			timeRemaining = 6.0f;
			score = 0;

			for (int i = 1; i < NUM_BRANCHES; i++) {
				branchPositions[i] = side::NONE;
			}

			spriteRIP.setPosition(675, 2000);
			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}


		// PLAYER ACTION
		if (acceptInput) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				playerSide = side::RIGHT;

				score++;
				timeRemaining += (2 / score) + .15f;
				spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 720);

				updateBranches(score);

				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;
				acceptInput = false;
				chop.play();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				playerSide = side::LEFT;

				score++;
				timeRemaining += (2 / score) + .15;
				spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);

				updateBranches(score);


				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;
				acceptInput = false;
				chop.play();
			}

			//SCORE
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

		}

		if (paused) {
			window.clear();
			window.draw(pauseText);
			window.display();
			continue;
		}


		//UPDATE SCENE

		sf::Time delta = clock.restart();
		timeRemaining -= delta.asSeconds();
		timeBar.setSize(sf::Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

		if (timeRemaining <= 0.0f) {
			paused = true;
			pauseText.setString("GAME OVER!");
			textRect = pauseText.getLocalBounds();
			pauseText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
			pauseText.setPosition(1920 / 2.0f, 1080 / 2.0f);
			oot.play();

		}

		


		// UPDATE BRANCHES
		for (int i = 0; i < NUM_BRANCHES; i++)
		{
			float height = i * 150;
			if (branchPositions[i] == side::LEFT) {
				branches[i].setPosition(610, height);
				branches[i].setRotation(180);
			}
			else if (branchPositions[i] == side::RIGHT)
			{
				branches[i].setPosition(1330, height);
				branches[i].setRotation(0);
			}
			else {
				branches[i].setPosition(3000, height);
			}
		}


		// FLYING LOG
		if (logActive) {
			spriteLog.setPosition(spriteLog.getPosition().x + (logSpeedX * delta.asSeconds()), spriteLog.getPosition().y + (logSpeedY * delta.asSeconds()));

			if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000) {
				logActive = false;
				spriteLog.setPosition(810, 720);
			}
		}

		if (branchPositions[5] == playerSide) {
			paused = true;
			acceptInput = false;
			spriteRIP.setPosition(525, 760);
			spritePlayer.setPosition(2000, 660);
			pauseText.setString("SQUISH!");

			textRect = pauseText.getLocalBounds();
			pauseText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
			pauseText.setPosition(1920 / 2.0f, 1080 / 2.0f);
			death.play();
		}

		//BEE MOVEMENT
		if (!beeActive) {
			//Bee speed
			srand((int)time(0));
			beeSpeed = (rand() % 200) + 200;

			//Bee height
			srand((int)time(0) * 10);
			float height = (rand() % 500) + 500;
			spriteBee.setPosition(2000, height);
			beeActive = true;
		}
		else {
			//Bee movement
			spriteBee.setPosition(spriteBee.getPosition().x - (beeSpeed * delta.asSeconds()), spriteBee.getPosition().y);

			//Bee exit
			if (spriteBee.getPosition().x < -100) {
				beeActive = false;
			}
		}

		//CLOUD MOVEMENT

		//Cloud 1
		if (!cloud1Active) {
			srand((int)time(0) * 10);
			cloud1Speed = (rand() % 200);

			srand((int)time(0) * 10);
			float height = (rand() % 150);
			spriteCloud1.setPosition(-200, height);

			cloud1Active = true;
		}
		else {
			//cloud movement
			spriteCloud1.setPosition(spriteCloud1.getPosition().x + (cloud1Speed * delta.asSeconds()), spriteCloud1.getPosition().y);

			//cloud exit
			if (spriteCloud1.getPosition().x > 1920) {
				cloud1Active = false;
			}
		}

		// Cloud 2
		if (!cloud2Active)
		{

			// How fast is the cloud
			srand((int)time(0) * 20);
			cloud2Speed = (rand() % 200);

			// How high is the cloud
			srand((int)time(0) * 20);
			float height = (rand() % 300) - 150;
			spriteCloud2.setPosition(-200, height);
			cloud2Active = true;


		}
		else
		{

			spriteCloud2.setPosition(
				spriteCloud2.getPosition().x +
				(cloud2Speed * delta.asSeconds()),
				spriteCloud2.getPosition().y);

			// Has the cloud reached the right hand edge of the screen?
			if (spriteCloud2.getPosition().x > 1920)
			{
				// Set it up ready to be a whole new cloud next frame
				cloud2Active = false;
			}
		}

		//Cloud 3
		if (!cloud3Active)
		{

			// How fast is the cloud
			srand((int)time(0) * 30);
			cloud3Speed = (rand() % 200);

			// How high is the cloud
			srand((int)time(0) * 30);
			float height = (rand() % 450) - 150;
			spriteCloud3.setPosition(-200, height);
			cloud3Active = true;


		}
		else
		{

			spriteCloud3.setPosition(
				spriteCloud3.getPosition().x +
				(cloud3Speed * delta.asSeconds()),
				spriteCloud3.getPosition().y);

			// Has the cloud reached the right hand edge of the screen?
			if (spriteCloud3.getPosition().x > 1920)
			{
				// Set it up ready to be a whole new cloud next frame
				cloud3Active = false;
			}
		}



		//DRAW SCENE
		window.clear();
		window.draw(spriteBackground);
		window.draw(spriteTree);
		window.draw(spritePlayer);
		window.draw(spriteAxe);
		window.draw(spriteLog);
		window.draw(spriteRIP);
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);
		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}
		window.draw(spriteBee);
		window.draw(scoreText);
		window.draw(timeBar);
		window.display();
	}
	return 0;
}

void updateBranches(int seed) {
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}
	srand((int)time(0) + seed);
	int r = (rand() % 5);
	switch (r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	default:
		branchPositions[0] = side::NONE;
		break;
	}
}