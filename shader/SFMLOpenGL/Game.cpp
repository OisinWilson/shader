#include <Game.h>

static bool flip;

Game::Game() : window(VideoMode(800, 600), "OpenGL Cube Vertex and Fragment Shaders")
{
	vertexPoints[0] = { -0.25f, 0.25f, 0.5f };
	vertexPoints[1] = { -0.25f, -0.25f, 0.5f };
	vertexPoints[2] = { 0.25f, -0.25f, 0.5f };
	vertexPoints[3] = { 0.25f, 0.25f, 0.5f };
	vertexPoints[4] = { -0.25f, 0.25f, 0.0f };
	vertexPoints[5] = { -0.25f, -0.25f, 0.0f };
	vertexPoints[6] = { 0.25f, -0.25f, 0.0f };
	vertexPoints[7] = { 0.25f, 0.25f, 0.0f };
}

Game::~Game() {}

void Game::run()
{

	initialize();

	Event event;

	while (isRunning) {

#if (DEBUG >= 2)
		DEBUG_MSG("Game running...");
#endif

		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				isRunning = false;
			}
		}
		update();
		render();
	}

}

typedef struct
{
	float coordinate[3];
	float color[4];
} Vertex;

Vertex vertex[36];
GLubyte triangles[36];

/* Variable to hold the VBO identifier and shader data */
GLuint	index, //Index to draw
		vsid, //Vertex Shader ID
		fsid, //Fragment Shader ID
		progID, //Program ID
		vao = 0, //Vertex Array ID
		vbo[1], // Vertex Buffer ID
		positionID, //Position ID
		colorID; // Color ID


void Game::initialize()
{
	isRunning = true;
	GLint isCompiled = 0;
	GLint isLinked = 0;

	glewInit();

	/* Vertices counter-clockwise winding */
	setPoints();

	/*Index of Poly / Triangle to Draw */
	for (int index = 0; index < 36; index++)
	{
		triangles[index] = index;
	}

	/* Create a new VBO using VBO id */
	glGenBuffers(1, vbo);

	/* Bind the VBO */
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	/* Upload vertex data to GPU */
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 36, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 36, triangles, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/* Vertex Shader which would normally be loaded from an external file */
	const char* vs_src = "#version 400\n\r"
		"in vec4 sv_position;"
		"in vec4 sv_color;"
		"out vec4 color;"
		"void main() {"
		"	color = sv_color;"
		"	gl_Position = sv_position;"
		"}"; //Vertex Shader Src

	DEBUG_MSG("Setting Up Vertex Shader");

	vsid = glCreateShader(GL_VERTEX_SHADER); //Create Shader and set ID
	glShaderSource(vsid, 1, (const GLchar**)&vs_src, NULL); // Set the shaders source
	glCompileShader(vsid); //Check that the shader compiles

	//Check is Shader Compiled
	glGetShaderiv(vsid, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_TRUE) {
		DEBUG_MSG("Vertex Shader Compiled");
		isCompiled = GL_FALSE;
	}
	else
	{
		DEBUG_MSG("ERROR: Vertex Shader Compilation Error");
	}

	/* Fragment Shader which would normally be loaded from an external file */
	const char* fs_src = "#version 400\n\r"
		"in vec4 color;"
		"out vec4 fColor;"
		"void main() {"
		"	fColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);"
		"}"; //Fragment Shader Src

	DEBUG_MSG("Setting Up Fragment Shader");

	fsid = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsid, 1, (const GLchar**)&fs_src, NULL);
	glCompileShader(fsid);
	//Check is Shader Compiled
	glGetShaderiv(fsid, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_TRUE) {
		DEBUG_MSG("Fragment Shader Compiled");
		isCompiled = GL_FALSE;
	}
	else
	{
		DEBUG_MSG("ERROR: Fragment Shader Compilation Error");
	}

	DEBUG_MSG("Setting Up and Linking Shader");
	progID = glCreateProgram();	//Create program in GPU
	glAttachShader(progID, vsid); //Attach Vertex Shader to Program
	glAttachShader(progID, fsid); //Attach Fragment Shader to Program
	glLinkProgram(progID);

	//Check is Shader Linked
	glGetProgramiv(progID, GL_LINK_STATUS, &isLinked);

	if (isLinked == 1) {
		DEBUG_MSG("Shader Linked");
	}
	else
	{
		DEBUG_MSG("ERROR: Shader Link Error");
	}

	// Use Progam on GPU
	// https://www.opengl.org/sdk/docs/man/html/glUseProgram.xhtml
	glUseProgram(progID);

	// Find variables in the shader
	// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGetAttribLocation.xml
	positionID = glGetAttribLocation(progID, "sv_position");
	colorID = glGetAttribLocation(progID, "sv_color");
}

void Game::update()
{
	elapsed = clock.getElapsedTime();

	center = { vertexPoints[0] + vertexPoints[1] + vertexPoints[2] + vertexPoints[3] + vertexPoints[4] + vertexPoints[5] + vertexPoints[6] + vertexPoints[7] };
	center = { center.ReturnX() / 8, center.ReturnY() / 8, center.ReturnZ() / 8 };

	//translate
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		for (auto i = 0; i < 8; i++)
		{
			vertexPoints[i] = Matrix3::Translate(0.001f, 0) * vertexPoints[i];
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		for (auto i = 0; i < 8; i++)
		{
			vertexPoints[i] = Matrix3::Translate(-0.001f, 0) * vertexPoints[i];
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		for (auto i = 0; i < 8; i++)
		{
			vertexPoints[i] = Matrix3::Translate(0, 0.001f) * vertexPoints[i];
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		for (auto i = 0; i < 8; i++)
		{
			vertexPoints[i] = Matrix3::Translate(0, -0.001f) * vertexPoints[i];
		}
	}

	//scaling
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		for (auto i = 0; i < 8; i++)
		{
			vertexPoints[i] = (matrix.Scale3D(101) * (vertexPoints[i] - center)) + center;
		}
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		for (auto i = 0; i < 8; i++)
		{
			vertexPoints[i] = (matrix.Scale3D(99) * (vertexPoints[i] - center)) + center;
		}
	}

	//Rotation
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		for (auto i = 0; i < 8; i++)
		{
			vertexPoints[i] = (matrix.RotationX(0.1) * (vertexPoints[i] - center)) + center;
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
	{
		for (auto i = 0; i < 8; i++)
		{
			vertexPoints[i] = (matrix.RotationY(0.1) * (vertexPoints[i] - center)) + center;
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		for (auto i = 0; i < 8; i++)
		{
			vertexPoints[i] = (matrix.RotationZ(0.1) * (vertexPoints[i] - center)) + center;
		}
	}

	setPoints();

#if (DEBUG >= 2)
	DEBUG_MSG("Update up...");
#endif

}

void Game::render()
{

#if (DEBUG >= 2)
	DEBUG_MSG("Drawing...");
#endif

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);

	/*	As the data positions will be updated by the this program on the
		CPU bind the updated data to the GPU for drawing	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 36, vertex, GL_STATIC_DRAW);

	/*	Draw Triangle from VBO	(set where to start from as VBO can contain
		model components that 'are' and 'are not' to be drawn )	*/

	// Set pointers for each parameter
	// https://www.opengl.org/sdk/docs/man4/html/glVertexAttribPointer.xhtml
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	//Enable Arrays
	glEnableVertexAttribArray(positionID);
	glEnableVertexAttribArray(colorID);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (char*)NULL + 0);

	window.display();
}

void Game::unload()
{
#if (DEBUG >= 2)
	DEBUG_MSG("Cleaning up...");
#endif
	glDeleteProgram(progID);
	glDeleteBuffers(1, vbo);
}

void Game::setPoints()
{
	vertex[0].coordinate[0] = vertexPoints[0].ReturnX();	vertex[1].coordinate[0] = vertexPoints[1].ReturnX();	vertex[2].coordinate[0] = vertexPoints[2].ReturnX();
	vertex[0].coordinate[1] = vertexPoints[0].ReturnY();	vertex[1].coordinate[1] = vertexPoints[1].ReturnY();	vertex[2].coordinate[1] = vertexPoints[2].ReturnY();
	vertex[0].coordinate[2] = vertexPoints[0].ReturnZ();	vertex[1].coordinate[2] = vertexPoints[1].ReturnZ();	vertex[2].coordinate[2] = vertexPoints[2].ReturnZ();

	vertex[3].coordinate[0] = vertexPoints[3].ReturnX();	vertex[4].coordinate[0] = vertexPoints[0].ReturnX();	vertex[5].coordinate[0] = vertexPoints[2].ReturnX();
	vertex[3].coordinate[1] = vertexPoints[3].ReturnY();	vertex[4].coordinate[1] = vertexPoints[0].ReturnY();	vertex[5].coordinate[1] = vertexPoints[2].ReturnY();
	vertex[3].coordinate[2] = vertexPoints[3].ReturnZ();	vertex[4].coordinate[2] = vertexPoints[0].ReturnZ();	vertex[5].coordinate[2] = vertexPoints[2].ReturnZ();


	vertex[6].coordinate[0] = vertexPoints[4].ReturnX();	vertex[7].coordinate[0] = vertexPoints[5].ReturnX();	vertex[8].coordinate[0] = vertexPoints[6].ReturnX();
	vertex[6].coordinate[1] = vertexPoints[4].ReturnY();	vertex[7].coordinate[1] = vertexPoints[5].ReturnY();	vertex[8].coordinate[1] = vertexPoints[6].ReturnY();
	vertex[6].coordinate[2] = vertexPoints[4].ReturnZ();	vertex[7].coordinate[2] = vertexPoints[5].ReturnZ();	vertex[8].coordinate[2] = vertexPoints[6].ReturnZ();

	vertex[9].coordinate[0] = vertexPoints[7].ReturnX();	vertex[10].coordinate[0] = vertexPoints[4].ReturnX();	vertex[11].coordinate[0] = vertexPoints[6].ReturnX();
	vertex[9].coordinate[1] = vertexPoints[7].ReturnY();	vertex[10].coordinate[1] = vertexPoints[4].ReturnY();	vertex[11].coordinate[1] = vertexPoints[6].ReturnY();
	vertex[9].coordinate[2] = vertexPoints[7].ReturnZ();	vertex[10].coordinate[2] = vertexPoints[4].ReturnZ();	vertex[11].coordinate[2] = vertexPoints[6].ReturnZ();


	vertex[12].coordinate[0] = vertexPoints[4].ReturnX();	vertex[13].coordinate[0] = vertexPoints[5].ReturnX();	vertex[14].coordinate[0] = vertexPoints[1].ReturnX();
	vertex[12].coordinate[1] = vertexPoints[4].ReturnY();	vertex[13].coordinate[1] = vertexPoints[5].ReturnY();	vertex[14].coordinate[1] = vertexPoints[1].ReturnY();
	vertex[12].coordinate[2] = vertexPoints[4].ReturnZ();	vertex[13].coordinate[2] = vertexPoints[5].ReturnZ();	vertex[14].coordinate[2] = vertexPoints[1].ReturnZ();

	vertex[15].coordinate[0] = vertexPoints[0].ReturnX();	vertex[16].coordinate[0] = vertexPoints[4].ReturnX();	vertex[17].coordinate[0] = vertexPoints[1].ReturnX();
	vertex[15].coordinate[1] = vertexPoints[0].ReturnY();	vertex[16].coordinate[1] = vertexPoints[4].ReturnY();	vertex[17].coordinate[1] = vertexPoints[1].ReturnY();
	vertex[15].coordinate[2] = vertexPoints[0].ReturnZ();	vertex[16].coordinate[2] = vertexPoints[4].ReturnZ();	vertex[17].coordinate[2] = vertexPoints[1].ReturnZ();


	vertex[18].coordinate[0] = vertexPoints[7].ReturnX();	vertex[19].coordinate[0] = vertexPoints[6].ReturnX();	vertex[20].coordinate[0] = vertexPoints[2].ReturnX();
	vertex[18].coordinate[1] = vertexPoints[7].ReturnY();	vertex[19].coordinate[1] = vertexPoints[6].ReturnY();	vertex[20].coordinate[1] = vertexPoints[2].ReturnY();
	vertex[18].coordinate[2] = vertexPoints[7].ReturnZ();	vertex[19].coordinate[2] = vertexPoints[6].ReturnZ();	vertex[20].coordinate[2] = vertexPoints[2].ReturnZ();

	vertex[21].coordinate[0] = vertexPoints[3].ReturnX();	vertex[22].coordinate[0] = vertexPoints[7].ReturnX();	vertex[23].coordinate[0] = vertexPoints[2].ReturnX();
	vertex[21].coordinate[1] = vertexPoints[3].ReturnY();	vertex[22].coordinate[1] = vertexPoints[7].ReturnY();	vertex[23].coordinate[1] = vertexPoints[2].ReturnY();
	vertex[21].coordinate[2] = vertexPoints[3].ReturnZ();	vertex[22].coordinate[2] = vertexPoints[7].ReturnZ();	vertex[23].coordinate[2] = vertexPoints[2].ReturnZ();


	vertex[24].coordinate[0] = vertexPoints[1].ReturnX();	vertex[25].coordinate[0] = vertexPoints[5].ReturnX();	vertex[26].coordinate[0] = vertexPoints[6].ReturnX();
	vertex[24].coordinate[1] = vertexPoints[1].ReturnY();	vertex[25].coordinate[1] = vertexPoints[5].ReturnY();	vertex[26].coordinate[1] = vertexPoints[6].ReturnY();
	vertex[24].coordinate[2] = vertexPoints[1].ReturnZ();	vertex[25].coordinate[2] = vertexPoints[5].ReturnZ();	vertex[26].coordinate[2] = vertexPoints[6].ReturnZ();

	vertex[27].coordinate[0] = vertexPoints[2].ReturnX();	vertex[28].coordinate[0] = vertexPoints[1].ReturnX();	vertex[29].coordinate[0] = vertexPoints[6].ReturnX();
	vertex[27].coordinate[1] = vertexPoints[2].ReturnY();	vertex[28].coordinate[1] = vertexPoints[1].ReturnY();	vertex[29].coordinate[1] = vertexPoints[6].ReturnY();
	vertex[27].coordinate[2] = vertexPoints[2].ReturnZ();	vertex[28].coordinate[2] = vertexPoints[1].ReturnZ();	vertex[29].coordinate[2] = vertexPoints[6].ReturnZ();

	vertex[30].coordinate[0] = vertexPoints[0].ReturnX();	vertex[31].coordinate[0] = vertexPoints[4].ReturnX();	vertex[32].coordinate[0] = vertexPoints[7].ReturnX();
	vertex[30].coordinate[1] = vertexPoints[0].ReturnY();	vertex[31].coordinate[1] = vertexPoints[4].ReturnY();	vertex[32].coordinate[1] = vertexPoints[7].ReturnY();
	vertex[30].coordinate[2] = vertexPoints[0].ReturnZ();	vertex[31].coordinate[2] = vertexPoints[4].ReturnZ();	vertex[32].coordinate[2] = vertexPoints[7].ReturnZ();

	vertex[33].coordinate[0] = vertexPoints[3].ReturnX();	vertex[34].coordinate[0] = vertexPoints[0].ReturnX();	vertex[35].coordinate[0] = vertexPoints[7].ReturnX();
	vertex[33].coordinate[1] = vertexPoints[3].ReturnY();	vertex[34].coordinate[1] = vertexPoints[0].ReturnY();	vertex[35].coordinate[1] = vertexPoints[7].ReturnY();
	vertex[33].coordinate[2] = vertexPoints[3].ReturnZ();	vertex[34].coordinate[2] = vertexPoints[0].ReturnZ();	vertex[35].coordinate[2] = vertexPoints[7].ReturnZ();
}