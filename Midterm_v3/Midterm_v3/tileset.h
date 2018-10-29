#pragma once

namespace Tileset
{
	struct Tileset
	{
		int tile_width;
		int tile_height;
		int n_cols;
	};

	int get_Col( int tile_id, Tileset *t )
	{
		return t->tile_width * (tile_id % t->n_cols);
	}

	int get_Row( int tile_id, Tileset *t )
	{
		return t->tile_width * (tile_id / t->n_cols);
	}
}
