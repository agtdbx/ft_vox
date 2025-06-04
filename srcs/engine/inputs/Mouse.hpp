#ifndef MOUSE_HPP
# define MOUSE_HPP

# include <engine/inputs/Key.hpp>

# include <gmath.hpp>

/**
 * @brief Class for save mouse state.
 */
class Mouse
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
	Key	left, middle, right;

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of Mouse class.
	 *
	 * @return The default Mouse.
	 */
	Mouse(void);
	/**
	 * @brief Copy constructor of Mouse class.
	 *
	 * @param window Pointer on glfw window.
	 *
	 * @return The Mouse copied from parameter.
	 */
	Mouse(GLFWwindow* window);
	/**
	 * @brief Copy constructor of Mouse class.
	 *
	 * @param obj The Mouse to copy.
	 *
	 * @return The Mouse copied from parameter.
	 */
	Mouse(const Mouse &obj);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Shader class.
	 */
	~Mouse();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Getter of mouse position.
	 *
	 * @return Mouse position as Vec2d.
	 */
	gm::Vec2d	getPos(void);
	/**
	 * @brief Getter of mouse scroll.
	 *
	 * @return A double that indicate the scroll state.
	 * @return Negative for down, positive for up and 0 for none.
	 */
	double		getScroll(void);
	/**
	 * @brief Getter of mouse visible state.
	 *
	 * @return A boolean for mouse visibility.
	 */
	bool		getVisible(void);

//---- Setters -----------------------------------------------------------------
	/**
	 * @brief Setter of mouse visibility.
	 *
	 * @param window Pointer on glfw window.
	 * @param visible New mouse visibility.
	 */
	void		setVisible(GLFWwindow *window, bool visible);

//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of Mouse class.
	 *
	 * @param obj The Mouse to copy.
	 *
	 * @return The Mouse copied from parameter.
	 */
	Mouse		&operator=(const Mouse &obj);

//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Update method for check if mouse buttons state and mouse position.
	 *
	 * @param window Pointer on glfw window.
	 */
	void		update(GLFWwindow *window);
	/**
	 * @brief Methode for set mouse at parameter position.
	 *
	 * @param window Pointer on glfw window.
	 * @param x x for new position.
	 * @param y y for new position.
	 */
	void		goTo(GLFWwindow *window, float x, float y);
	/**
	 * @brief Methode for set mouse at parameter position.
	 *
	 * @param window Pointer on glfw window.
	 * @param pos New position.
	 */
	void		goTo(GLFWwindow *window, const gm::Vec2d &pos);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	gm::Vec2d	pos;
	double		scroll;
	bool		visible;

//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
