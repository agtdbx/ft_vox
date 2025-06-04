#ifndef INPUTMANAGER_HPP
# define INPUTMANAGER_HPP

# include <engine/inputs/Key.hpp>
# include <engine/inputs/Mouse.hpp>

/**
 * @brief Class for save keyboard and mouse state.
 */
class InputManager
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
	Mouse	mouse;
	Key		escape, del,
			_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, minus, equal, backspace,
			tab, q, w, e, r, t, y, u, i, o, p, enter,
			a, s, d, f, g, h, j, k, l,
			lshift, z, x, c, v, b, n, m, rshift,
			lcontrol, lalt, space, ralt, rcontrol,
			up, down, left, right;

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of InputManager class.
	 *
	 * @return The default InputManager.
	 */
	InputManager(void);
	/**
	 * @brief Copy constructor of InputManager class.
	 *
	 * @param window Pointer on glfw window.
	 *
	 * @return The InputManager copied from parameter.
	 */
	InputManager(GLFWwindow* window);
	/**
	 * @brief Copy constructor of InputManager class.
	 *
	 * @param obj The InputManager to copy.
	 *
	 * @return The InputManager copied from parameter.
	 */
	InputManager(const InputManager &obj);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Shader class.
	 */
	~InputManager();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of InputManager class.
	 *
	 * @param obj The InputManager to copy.
	 *
	 * @return The InputManager copied from parameter.
	 */
	InputManager	&operator=(const InputManager &obj);

//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Update method for check if the keyboard and mouse states.
	 *
	 * @param window Pointer on glfw window.
	 */
	void	update(GLFWwindow *window);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
//**** PRIVATE METHODS *********************************************************
	/**
	 * @brief Method for init all keys.
	 */
	void	initKeys(void);
	/**
	 * @brief Method for copy all keys.
	 */
	void	copyKeys(const InputManager &obj);
};

//**** FUNCTIONS ***************************************************************

#endif
