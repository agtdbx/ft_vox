#ifndef TEXT_HPP
# define TEXT_HPP

# include <engine/mesh/Mesh.hpp>
# include <engine/mesh/VertexPosTex.hpp>
# include <engine/shader/ShaderParam.hpp>
# include <engine/camera/Camera.hpp>
# include <engine/shader/Shader.hpp>
# include <engine/engine.hpp>

# include <gmath.hpp>
# include <string>

using TextMesh = Mesh<VertexPosTex>;

struct UBOText {
	gm::Vec4f	pos = gm::Vec4f(0.0f);
};

struct UBOTextColor {
	gm::Vec4f	textColor = gm::Vec4f(1.0f);
	gm::Vec4f	backgroundColor = gm::Vec4f(0.0f);
};

/**
 * @brief Struct for where is the postion of the text.
 *
 * bit yyxx
 *
 * top 10
 * mid 00
 * bot 01
 * left 10
 * center 00
 * right 01
 */
enum TextDrawPos
{
	TEXT_TOP_LEFT	= 0b1010,
	TEXT_TOP_CENTER	= 0b1000,
	TEXT_TOP_RIGHT	= 0b1001,
	TEXT_MID_LEFT	= 0b0010,
	TEXT_MID_CENTER	= 0b0000,
	TEXT_MID_RIGHT	= 0b0001,
	TEXT_BOT_LEFT	= 0b0110,
	TEXT_BOT_CENTER	= 0b0100,
	TEXT_BOT_RIGHT	= 0b0101,
};


/**
 * @brief Text class.
 */
class Text
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of Text class.
	 *
	 * @return The default Text.
	 */
	Text(void);
	/**
	 * @brief Copy constructor of Text class.
	 *
	 * @param obj The Text to copy.
	 *
	 * @return The Text copied from parameter.
	 */
	Text(const Text &obj);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Text class.
	 */
	~Text();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
	/**
	 * @brief Setter for position.
	 *
	 * @param pos New position.
	 */
	void	setPos(const gm::Vec2f &pos);

	/**
	 * @brief Setter for text draw position.
	 *
	 * @param drawPos New drawPos.
	 */
	void	setDrawPos(const TextDrawPos &drawPos);

	/**
	 * @brief Setter for font size.
	 *
	 * @param fontSize New font size, 1.0f is the basic size. Size must be higher than 0.1f.
	 */
	void	setFontSize(float fontSize);

	/**
	 * @brief Setter for text.
	 *
	 * @param text New text.
	 */
	void	setText(const std::string &text);

	/**
	 * @brief Setter for text color.
	 *
	 * @param text New text color.
	 */
	void	setTextColor(const gm::Vec4f &color);

	/**
	 * @brief Setter for background color.
	 *
	 * @param text New background color.
	 */
	void	setBackgroundColor(const gm::Vec4f &color);

//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of Text class.
	 *
	 * @param obj The Text to copy.
	 *
	 * @return The Text copied from parameter.
	 */
	Text	&operator=(const Text &obj);

//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Init the text.
	 *
	 * @param engine The engine struct.
	 * @param shader Shader used to draw text.
	 */
	void	init(Engine &engine, Shader &shader);
	/**
	 * @brief Draw text mesh, and update buffer if needed.
	 *
	 * @param engine Engine struct.
	 * @param shader Shader used to draw text.
	 */
	void	draw(Engine &engine, Shader &shader);
	/**
	 * @brief Destroy the text.
	 *
	 * @param engine The engine struct.
	 */
	void	destroy(Engine &engine);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	gm::Vec2f		pos;
	gm::Vec2f		posOffset;
	TextDrawPos		drawPos;
	gm::Vec2f		textSize;
	float			fontSize;
	bool			needUpdateBuffers;
	std::string		text;
	TextMesh		mesh;
	UBOText			uboPos;
	UBOTextColor	uboColor;
	ShaderParam		shaderParam;

//**** PRIVATE METHODS *********************************************************
	/**
	 * @brief Update buffer, or create them if they not exist.
	 *
	 * @param engine The engine struct.
	 */
	void	updateBuffers(Engine &engine);
};

//**** FUNCTIONS ***************************************************************

#endif
