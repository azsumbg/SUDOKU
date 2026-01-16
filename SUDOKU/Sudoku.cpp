#include "pch.h"
#include "sudoku.h"

// RANDIT *******************************

dll::RANDIT::RANDIT()
{
	std::random_device rd{};
	std::seed_seq sq{ rd(),rd(),rd(),rd(),rd(),rd(),rd(),rd() };

	twister = new std::mt19937(sq);
}

dll::RANDIT::~RANDIT()
{
	if (twister)delete twister;
}

int dll::RANDIT::operator()(int min, int max)
{
	std::uniform_int_distribution gen(min, max);

	return gen(*twister);
}

float dll::RANDIT::operator()(float min, float max)
{
	std::uniform_real_distribution<float> gen(min, max);

	return gen(*twister);
}

/////////////////////////////////////////

// PROTON ******************************

dll::PROTON::PROTON()
{
	_width = 1.0f;
	_height = 1.0f;

	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + _width / 2.0f;
	center.y = start.y + _height / 2.0f;
}
dll::PROTON::PROTON(float _sx, float _sy) :start{ _sx, _sy }
{
	_width = 1.0f;
	_height = 1.0f;

	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + _width / 2.0f;
	center.y = start.y + _height / 2.0f;
}
dll::PROTON::PROTON(float _sx, float _sy, float _s_width, float _s_height) :start{ _sx,_sy }, _width{ _s_width },
_height{ _s_height }
{
	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + _width / 2.0f;
	center.y = start.y + _height / 2.0f;
}

void dll::PROTON::Release()
{
	if (_in_heap)delete this;
}

void dll::PROTON::set_edges()
{
	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + _width / 2.0f;
	center.y = start.y + _height / 2.0f;
}
void dll::PROTON::new_dims(float new_width, float new_height)
{
	_width = new_width;
	_height = new_height;

	end.x = start.x + _width;
	end.y = start.y + _height;

	center.x = start.x + _width / 2.0f;
	center.y = start.y + _height / 2.0f;
}
void dll::PROTON::new_width(float new_width)
{
	_width = new_width;
	
	end.x = start.x + _width;

	center.x = start.x + _width / 2.0f;
}
void dll::PROTON::new_height(float new_height)
{
	_height = new_height;

	end.y = start.y + _height;

	center.y = start.y + _height / 2.0f;
}

float dll::PROTON::get_width()const
{
	return _width;
}
float dll::PROTON::get_height()const
{
	return _height;
}

bool dll::PROTON::in_heap()const
{
	return _in_heap;
}

dll::PROTON* dll::PROTON::create(float sx, float sy, float s_width, float s_height)
{
	PROTON* ret = new PROTON(sx, sy, s_width, s_height);
	ret->_in_heap = true;
	return ret;
}

////////////////////////////////////////

// GRID *******************************

dll::GRID::GRID()
{
	float next_x = 5.0f;
	float next_y = sky;

	for (int rows = 0; rows < MAX_ROWS; ++rows)
	{
		for (int cols = 0; cols < MAX_COLS; ++cols)
		{
			int quadrant_row = 0;
			int quadrant_col = 0;

			if (rows < 3)quadrant_row = 0;
			else if (rows < 6)quadrant_row = 3;
			else quadrant_row = 6;

			if (cols < 3)quadrant_col = 0;
			else if (cols < 6)quadrant_col = 1;
			else quadrant_col = 2;


			game_grid[rows][cols].col = cols;
			game_grid[rows][cols].row = rows;

			game_grid[rows][cols].quadrant = quadrant_row + quadrant_col;

			game_grid[rows][cols].dims.start.x = next_x;
			game_grid[rows][cols].dims.start.y = next_y;
			game_grid[rows][cols].dims.new_dims(50.0f, 50.0f);

			next_x += 49.0f;
		}

		next_x = 5.0f;
		next_y += 49.0f;
	}
}

int dll::GRID::get_value(int row, int col)const
{
	return(game_grid[row][col].value);
}
int dll::GRID::get_quadrant(int row, int col)const
{
	return game_grid[row][col].quadrant;
}

void dll::GRID::set_value(int row, int col, int new_value)
{
	if (new_value == CLEAR_VALUE)
	{
		game_grid[row][col].value = new_value;
		game_grid[row][col].valid_number = true;
		return;
	}
	
	int my_quadrant = game_grid[row][col].quadrant;
	
	game_grid[row][col].value = new_value;
	game_grid[row][col].valid_number = true;

	for (int rows = 0; rows < MAX_ROWS; ++rows)
	{
		for (int cols = 0; cols < MAX_ROWS; ++rows)
		{
			if (rows == row && cols == col)continue;

			if (game_grid[rows][cols].quadrant == my_quadrant && game_grid[rows][cols].value == new_value)
			{
				game_grid[row][col].valid_number = false;
				break;
			}
		}
	}

	for (int rows = 0; rows < MAX_ROWS; ++rows)
	{
		if (rows == row)continue;

		if (game_grid[rows][col].value == new_value)
		{
			game_grid[row][col].valid_number = false;
			break;
		}
	}

	for (int cols = 0; cols < MAX_ROWS; ++cols)
	{
		if (cols == col)continue;

		if (game_grid[row][cols].value == new_value)
		{
			game_grid[row][col].valid_number = false;
			break;
		}
	}
}
bool dll::GRID::value_ok(int row, int col)const
{
	return game_grid[row][col].valid_number;
}

void dll::GRID::set_level(int level)
{
	RANDIT Randerer{};

	if (level == 1)
	{
		for (int i = 0; i <= 20; ++i)
		{
			bool is_ok = false;
			while (!is_ok)
			{
				is_ok = true;

				int new_value = Randerer(1, 9);
				int a_row = Randerer(0, 8);
				int a_col = Randerer(0, 8);

				if (game_grid[a_row][a_col].value > 0)is_ok = false;
				else
				{
					set_value(a_row, a_col, new_value);
					if (!value_ok(a_row, a_col))
					{
						game_grid[a_row][a_col].value = -1;
						is_ok = false;
					}
				}
			}
		}
	}
	else if (level == 2)
	{
		for (int i = 0; i <= 15; ++i)
		{
			bool is_ok = false;
			while (!is_ok)
			{
				is_ok = true;

				int new_value = Randerer(1, 9);
				int a_row = Randerer(0, 8);
				int a_col = Randerer(0, 8);

				if (game_grid[a_row][a_col].value > 0)is_ok = false;
				else
				{
					set_value(a_row, a_col, new_value);
					if (!value_ok(a_row, a_col))
					{
						game_grid[a_row][a_col].value = -1;
						is_ok = false;
					}
				}
			}
		}
	}
	else if (level == 3)
	{
		for (int i = 0; i <= 10; ++i)
		{
			bool is_ok = false;
			while (!is_ok)
			{
				is_ok = true;

				int new_value = Randerer(1, 9);
				int a_row = Randerer(0, 8);
				int a_col = Randerer(0, 8);

				if (game_grid[a_row][a_col].value > 0)is_ok = false;
				else
				{
					set_value(a_row, a_col, new_value);
					if (!value_ok(a_row, a_col))
					{
						game_grid[a_row][a_col].value = -1;
						is_ok = false;
					}
				}
			}
		}
	}
	else if (level == 4)
	{
		for (int i = 0; i <= 5; ++i)
		{
			bool is_ok = false;
			while (!is_ok)
			{
				is_ok = true;

				int new_value = Randerer(1, 9);
				int a_row = Randerer(0, 8);
				int a_col = Randerer(0, 8);

				if (game_grid[a_row][a_col].value > 0)is_ok = false;
				else
				{
					set_value(a_row, a_col, new_value);
					if (!value_ok(a_row, a_col))
					{
						game_grid[a_row][a_col].value = -1;
						is_ok = false;
					}
				}
			}
		}
	}
}

FRECT dll::GRID::get_dims(int row, int col) const
{
	FRECT ret{ game_grid[row][col].dims.start.x,game_grid[row][col].dims.start.y,
	game_grid[row][col].dims.end.x,game_grid[row][col].dims.end.y };

	return ret;
}

////////////////////////////////////////