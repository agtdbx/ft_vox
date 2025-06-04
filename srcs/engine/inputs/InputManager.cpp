#include <engine/inputs/InputManager.hpp>

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

InputManager::InputManager(void)
{
	this->mouse = Mouse();
	this->initKeys();
}


InputManager::InputManager(GLFWwindow* window)
{
	this->mouse = Mouse(window);
	this->initKeys();
}



InputManager::InputManager(const InputManager &obj)
{
	this->mouse = obj.mouse;
	this->copyKeys(obj);
}

//---- Destructor --------------------------------------------------------------

InputManager::~InputManager()
{

}


//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

//---- Setters -----------------------------------------------------------------

//---- Operators ---------------------------------------------------------------

InputManager	&InputManager::operator=(const InputManager &obj)
{
	if (this == &obj)
		return (*this);

	this->mouse = obj.mouse;
	this->copyKeys(obj);

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	InputManager::update(GLFWwindow *window)
{
	// Init mouse
	this->mouse.update(window);

	// Init first line of keyboard
	this->escape.update(window);
	this->del.update(window);
	// Init second line of keyboard
	this->_0.update(window);
	this->_1.update(window);
	this->_2.update(window);
	this->_3.update(window);
	this->_4.update(window);
	this->_5.update(window);
	this->_6.update(window);
	this->_7.update(window);
	this->_8.update(window);
	this->_9.update(window);
	this->minus.update(window);
	this->equal.update(window);
	this->backspace.update(window);
	// Init third line of keyboard
	this->tab.update(window);
	this->q.update(window);
	this->w.update(window);
	this->e.update(window);
	this->r.update(window);
	this->t.update(window);
	this->y.update(window);
	this->u.update(window);
	this->i.update(window);
	this->o.update(window);
	this->p.update(window);
	this->enter.update(window);
	// Init fourth line of keyboard
	this->a.update(window);
	this->s.update(window);
	this->d.update(window);
	this->f.update(window);
	this->g.update(window);
	this->h.update(window);
	this->j.update(window);
	this->k.update(window);
	this->l.update(window);
	// Init fifth line of keyboard
	this->lshift.update(window);
	this->z.update(window);
	this->x.update(window);
	this->c.update(window);
	this->v.update(window);
	this->b.update(window);
	this->n.update(window);
	this->m.update(window);
	this->rshift.update(window);
	// Init sixth line of keyboard
	this->lcontrol.update(window);
	this->lalt.update(window);
	this->space.update(window);
	this->ralt.update(window);
	this->rcontrol.update(window);
	// Init arrow keys
	this->up.update(window);
	this->left.update(window);
	this->down.update(window);
	this->right.update(window);
}

//**** PRIVATE METHODS *********************************************************

void	InputManager::initKeys(void)
{
	// Init first line of keyboard
	this->escape = Key(GLFW_KEY_ESCAPE);
	this->del = Key(GLFW_KEY_BACKSPACE);
	// Init second line of keyboard
	this->_0 = Key(GLFW_KEY_0);
	this->_1 = Key(GLFW_KEY_1);
	this->_2 = Key(GLFW_KEY_2);
	this->_3 = Key(GLFW_KEY_3);
	this->_4 = Key(GLFW_KEY_4);
	this->_5 = Key(GLFW_KEY_5);
	this->_6 = Key(GLFW_KEY_6);
	this->_7 = Key(GLFW_KEY_7);
	this->_8 = Key(GLFW_KEY_8);
	this->_9 = Key(GLFW_KEY_9);
	this->minus = Key(GLFW_KEY_MINUS);
	this->equal = Key(GLFW_KEY_EQUAL);
	this->backspace = Key(GLFW_KEY_BACKSPACE);
	// Init third line of keyboard
	this->tab = Key(GLFW_KEY_TAB);
	this->q = Key(GLFW_KEY_Q);
	this->w = Key(GLFW_KEY_W);
	this->e = Key(GLFW_KEY_E);
	this->r = Key(GLFW_KEY_R);
	this->t = Key(GLFW_KEY_T);
	this->y = Key(GLFW_KEY_Y);
	this->u = Key(GLFW_KEY_U);
	this->i = Key(GLFW_KEY_I);
	this->o = Key(GLFW_KEY_O);
	this->p = Key(GLFW_KEY_P);
	this->enter = Key(GLFW_KEY_ENTER);
	// Init fourth line of keyboard
	this->a = Key(GLFW_KEY_A);
	this->s = Key(GLFW_KEY_S);
	this->d = Key(GLFW_KEY_D);
	this->f = Key(GLFW_KEY_F);
	this->g = Key(GLFW_KEY_G);
	this->h = Key(GLFW_KEY_H);
	this->j = Key(GLFW_KEY_J);
	this->k = Key(GLFW_KEY_K);
	this->l = Key(GLFW_KEY_L);
	// Init fifth line of keyboard
	this->lshift = Key(GLFW_KEY_LEFT_SHIFT);
	this->z = Key(GLFW_KEY_Z);
	this->x = Key(GLFW_KEY_X);
	this->c = Key(GLFW_KEY_C);
	this->v = Key(GLFW_KEY_V);
	this->b = Key(GLFW_KEY_B);
	this->n = Key(GLFW_KEY_N);
	this->m = Key(GLFW_KEY_M);
	this->rshift = Key(GLFW_KEY_RIGHT_SHIFT);
	// Init sixth line of keyboard
	this->lcontrol = Key(GLFW_KEY_LEFT_CONTROL);
	this->lalt = Key(GLFW_KEY_LEFT_ALT);
	this->space = Key(GLFW_KEY_SPACE);
	this->ralt = Key(GLFW_KEY_RIGHT_ALT);
	this->rcontrol = Key(GLFW_KEY_RIGHT_CONTROL);
	// Init arrow keys
	this->up = Key(GLFW_KEY_UP);
	this->left = Key(GLFW_KEY_LEFT);
	this->down = Key(GLFW_KEY_DOWN);
	this->right = Key(GLFW_KEY_RIGHT);
}


void	InputManager::copyKeys(const InputManager &obj)
{
	// Init first line of keyboard
	this->escape = obj.escape;
	this->del = obj.del;
	// Init second line of keyboard
	this->_0 = obj._0;
	this->_1 = obj._1;
	this->_2 = obj._2;
	this->_3 = obj._3;
	this->_4 = obj._4;
	this->_5 = obj._5;
	this->_6 = obj._6;
	this->_7 = obj._7;
	this->_8 = obj._8;
	this->_9 = obj._9;
	this->minus = obj.minus;
	this->equal = obj.equal;
	this->backspace = obj.backspace;
	// Init third line of keyboard
	this->tab = obj.tab;
	this->q = obj.q;
	this->w = obj.w;
	this->e = obj.e;
	this->r = obj.r;
	this->t = obj.t;
	this->y = obj.y;
	this->u = obj.u;
	this->i = obj.i;
	this->o = obj.o;
	this->p = obj.p;
	this->enter = obj.enter;
	// Init fourth line of keyboard
	this->a = obj.a;
	this->s = obj.s;
	this->d = obj.d;
	this->f = obj.f;
	this->g = obj.g;
	this->h = obj.h;
	this->j = obj.j;
	this->k = obj.k;
	this->l = obj.l;
	// Init fifth line of keyboard
	this->lshift = obj.lshift;
	this->z = obj.z;
	this->x = obj.x;
	this->c = obj.c;
	this->v = obj.v;
	this->b = obj.b;
	this->n = obj.n;
	this->m = obj.m;
	this->rshift = obj.rshift;
	// Init sixth line of keyboard
	this->lcontrol = obj.lcontrol;
	this->lalt = obj.lalt;
	this->space = obj.space;
	this->ralt = obj.ralt;
	this->rcontrol = obj.rcontrol;
	// Init arrow keys
	this->up = obj.up;
	this->left = obj.left;
	this->down = obj.down;
	this->right = obj.right;
}
