#ifndef KEY_HPP
# define KEY_HPP

# include <GLFW/glfw3.h>

/**
 * @brief Class for save key or mouse button state.
 */
class Key
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of Key class.
	 *
	 * @return The default Key.
	 */
	Key(void);
	/**
	 * @brief Copy constructor of Key class.
	 *
	 * @param keycode The keycode of the key.
	 *
	 * @return The Key copied from parameter.
	 */
	Key(int keycode);
	/**
	 * @brief Copy constructor of Key class.
	 *
	 * @param obj The Key to copy.
	 *
	 * @return The Key copied from parameter.
	 */
	Key(const Key &obj);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Shader class.
	 */
	~Key();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief To know if the key is pressed/down for the first frame.
	 *
	 * @return A bool to know if the key is pressed for the first frame.
	 */
	bool	isPressed(void);
	/**
	 * @brief To know if the key is release/up for the first frame.
	 *
	 * @return A bool to know if the key is release for the first frame.
	 */
	bool	isReleased(void);
	/**
	 * @brief To know if the key is pressed/down.
	 *
	 * @return A bool to know if the key is down.
	 */
	bool	isDown(void);

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of Key class.
	 *
	 * @param obj The Key to copy.
	 *
	 * @return The Key copied from parameter.
	 */
	Key	&operator=(const Key &obj);

//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Update method for check if the key is pressed or release.
	 *
	 * @param window Pointer on glfw window.
	 */
	void	update(GLFWwindow *window);
	/**
	 * @brief Update method for check if the mouse button is pressed or release.
	 *
	 * @param window Pointer on glfw window.
	 */
	void	updateForMouse(GLFWwindow *window);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	int		keycode;
	bool	pressed, released, down;

//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
