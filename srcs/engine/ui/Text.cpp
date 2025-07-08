#include <engine/ui/Text.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Text::Text(void)
{
	this->pos = gm::Vec2f(0, 0);
	this->posOffset = gm::Vec2f(0, 0);
	this->drawPos = TEXT_MID_CENTER;
	this->textSize = gm::Vec2f(0, 0);
	this->fontSize = 1.0f;
	this->text = "";
	this->needUpdateBuffers = true;
}


Text::Text(const Text &obj)
{
	this->pos = obj.pos;
	this->posOffset = obj.posOffset;
	this->drawPos = obj.drawPos;
	this->textSize = obj.textSize;
	this->fontSize = obj.fontSize;
	this->text = obj.text;
	this->needUpdateBuffers = true;
}

//---- Destructor --------------------------------------------------------------

Text::~Text()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------

void	Text::setPos(const gm::Vec2f &pos)
{
	this->pos = pos;
	this->uboPos.pos = gm::Vec4f(pos);

	this->uboPos.pos = this->pos + this->posOffset;
}


void	Text::setDrawPos(const TextDrawPos &drawPos)
{
	this->drawPos = drawPos;

	if ((this->drawPos & 0b1000) == 0b1000) // top
		this->posOffset.y = 0.0f;
	else if ((this->drawPos & 0b0100) == 0b0100) // bot
		this->posOffset.y = -this->textSize.y;
	else // mid
		this->posOffset.y = -this->textSize.y / 2.0f;

	if ((this->drawPos & 0b0010) == 0b0010) // left
		this->posOffset.x = 0.0f;
	else if ((this->drawPos & 0b0001) == 0b0001) // right
		this->posOffset.x = -this->textSize.x;
	else // center
		this->posOffset.x = -this->textSize.x / 2.0f;
}


void	Text::setFontSize(float fontSize)
{
	this->fontSize = gm::max(0.1f, fontSize);
}


void	Text::setText(const std::string &text)
{
	if (this->text == text)
		return ;
	this->text = text;
	this->needUpdateBuffers = true;
}


void	Text::setTextColor(const gm::Vec4f &color)
{
	this->uboColor.textColor = color;
}


void	Text::setBackgroundColor(const gm::Vec4f &color)
{
	this->uboColor.backgroundColor = color;
}

//---- Operators ---------------------------------------------------------------

Text	&Text::operator=(const Text &obj)
{
	if (this == &obj)
		return (*this);

	this->pos = obj.pos;
	this->posOffset = obj.posOffset;
	this->drawPos = obj.drawPos;
	this->fontSize = obj.fontSize;
	this->text = obj.text;
	this->needUpdateBuffers = true;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Text::init(Engine &engine, Shader &shader)
{
	shader.initShaderParam(engine, this->shaderParam, {"font"});

	this->uboPos.pos = gm::Vec4f(this->pos);
}


void	Text::draw(Engine &engine, Shader &shader)
{
	if (this->needUpdateBuffers)
	{
		this->updateBuffers(engine);
		this->needUpdateBuffers = false;
	}

	if (!this->mesh.isBuffersCreated())
		return ;

	this->shaderParam.updateBuffer(engine.window, &this->uboPos, 0);
	this->shaderParam.updateBuffer(engine.window, &this->uboColor, 1);
	engine.window.drawMesh(this->mesh, shader, this->shaderParam);
}


void	Text::destroy(Engine &engine)
{
	this->mesh.destroy();
	this->shaderParam.destroy(engine);
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

const float	charTexW = 32.0f / 512.0f;
const float	charTexH = 32.0f / 256.0f;

void	Text::updateBuffers(Engine &engine)
{
	if (this->text == "")
		return ;

	this->mesh.destroy();

	float	charW = 32.0f / engine.window.getSize().x * this->fontSize;
	float	charH = 32.0f / engine.window.getSize().y * this->fontSize;

	float	x = 0.0f;
	float	y = 0.0f;
	this->textSize = gm::Vec2f(0.0f, 0.0f);

	int	nbVertex = 0;
	std::vector<VertexPosTex>	&vertices = this->mesh.getVertices();
	std::vector<uint32_t>		&indices = this->mesh.getIndices();

	for (const char &c : this->text)
	{
		int		ascii = c;
		float	texX = (float)(ascii % 16) * 32.0f / 512.0f;
		float	texY = (float)(ascii / 16) * 32.0f / 256.0f;

		// Create vertex
		vertices.push_back({{x        , y        , 0.0f}, {texX           , texY           }});
		vertices.push_back({{x        , y + charH, 0.0f}, {texX           , texY + charTexH}});
		vertices.push_back({{x + charW, y + charH, 0.0f}, {texX + charTexW, texY + charTexH}});
		vertices.push_back({{x + charW, y        , 0.0f}, {texX + charTexW, texY           }});

		// Create index
		indices.push_back(nbVertex + 0);indices.push_back(nbVertex + 1);indices.push_back(nbVertex + 2);
		indices.push_back(nbVertex + 0);indices.push_back(nbVertex + 2);indices.push_back(nbVertex + 3);

		if (c == '\n')
		{
			y += charH;
			x = 0.0f;
		}
		else
			x += charW;
		this->textSize.x = gm::max(this->textSize.x, x);
		this->textSize.y = gm::max(this->textSize.y, y);
		nbVertex += 4;
	}

	if ((this->drawPos & 0b1000) == 0b1000) // top
		this->posOffset.y = 0.0f;
	else if ((this->drawPos & 0b0100) == 0b0100) // bot
		this->posOffset.y = -this->textSize.y;
	else // mid
		this->posOffset.y = -this->textSize.y / 2.0f;

	if ((this->drawPos & 0b0010) == 0b0010) // left
		this->posOffset.x = 0.0f;
	else if ((this->drawPos & 0b0001) == 0b0001) // right
		this->posOffset.x = -this->textSize.x;
	else // center
		this->posOffset.x = -this->textSize.x / 2.0f;

	this->uboPos.pos = this->pos + this->posOffset;

	this->mesh.updateMeshInfo();
	this->mesh.createBuffers(engine.commandPool);
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
