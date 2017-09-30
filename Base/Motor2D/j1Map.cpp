#include "p2Defs.h"
#include "p2Log.h"
#include "p2List.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include <math.h>
#include <string.h>

struct SDL_Texture;

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	// Add Maps to the list
	hello = new Map_info;
	Hello2 = new Map_info;

	//AddMap(hello);
	//AddMap(Hello2);

	return ret;
}

void j1Map::Draw()
{
	if (map_loaded == false)
		return;

	//SDL_SetRenderDrawBlendMode
	//SDL_SetRenderDrawColor

	// TODO 3.6: Iterate all tilesets and draw all their 
	// images in 0,0 (you should have only one tileset for now)
	p2List_item<Map_info*>* item_map = Maps.start;
	
	iPoint pos = { 0,0 };

	while (item_map != nullptr) {

		p2List_item<tileset_info*>* item_tileset = Maps.start->data->tilesets.start;

		while (item_tileset != nullptr) {

			App->render->Blit(item_tileset->data->image.tex, pos.x, pos.y);
			item_tileset = item_tileset->next;

			pos.y += 100;
		}

		item_map = item_map->next;
	}
	
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// TODO 3.2: Make sure you clean up any memory allocated
	// from tilesets / map
	Maps.clear();

	//map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);
	
	pugi::xml_document check_doc;

	pugi::xml_parse_result result =  check_doc.load_file(tmp.GetString());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	if (ret == true)
	{
		// TODO 3.3: Create and call a private function to load and fill
		// all your map data
		pugi::xml_node root_node = check_doc.child("map");
		Map_info* item = new Map_info();

		LoadMapData(item, &root_node);
		Maps.add(item);

	}

	// TODO 3.4: Create and call a private function to load a tileset
	// remember to support more any number of tilesets!


	if (ret == true)
	{
		// TODO 3.5: LOG all the data loaded
		// iterate all tilesets and LOG everything
	}
	
	ret = map_loaded;

	return ret;
}

void j1Map::AddMap(Map_info* name) {
	//Maps.add(name);
}

bool j1Map::LoadMapData(Map_info* item_map, pugi::xml_node* map_node) {
	bool ret = true;

	// Orientation
	const pugi::char_t* cmp = map_node->attribute("orientation").as_string();

	if (strcmp(cmp, "orthogonal")) item_map->map_type = orthogonal;
	if (strcmp(cmp, "isometric")) item_map->map_type = isometric;
	if (strcmp(cmp, "staggered")) item_map->map_type = staggered;
	if (strcmp(cmp, "hexagonal")) item_map->map_type = hexagonal;
	else item_map->map_type = unknown_;

	// Renderorder
	cmp = map_node->attribute("renderorder").as_string();

	if (strcmp(cmp, "right-down")) item_map->renderorder = right_down;
	if (strcmp(cmp, "right-up")) item_map->renderorder = right_up;
	if (strcmp(cmp, "left-down")) item_map->renderorder = left_down;
	if (strcmp(cmp, "left-up")) item_map->renderorder = left_up;
	else item_map->renderorder = unknown;

	item_map->width = map_node->attribute("width").as_uint();
	item_map->height = map_node->attribute("height").as_uint();
	item_map->tilewidth = map_node->attribute("tilewidth").as_uint();
	item_map->tileheight = map_node->attribute("tileheight").as_uint();
	item_map->nextobjectid = map_node->attribute("nextobjectid").as_uint();

	// Load Tilesets
	if (map_node->child("tileset").attribute("firstgid").as_uint() == map_node->attribute("nextobjectid").as_uint()) {
	
		pugi::xml_node tileset_node = map_node->child("tileset");

		while (tileset_node.attribute("firstgid").as_int() > 0) {

			tileset_info* item_tileset = new tileset_info;
			LoadTilesetData(&tileset_node, item_tileset);
			tileset_node = tileset_node.parent().next_sibling("tileset");
			item_map->tilesets.add(item_tileset);
		}

		map_loaded = true;
	}
	else {
		map_loaded = false;
		LOG("There is no tileset!/n");
	}

	// Load Layers
	if (map_loaded) {
		pugi::xml_node layer_node = map_node->child("layer");

		while (layer_node.attribute("name").as_string() != "") {
			layer_info* item_layer = new layer_info;
			LoadLayerData(&layer_node, item_layer);
			layer_node = layer_node.next_sibling("layer");
			item_map->layers.add(item_layer);
		}
	}

	return ret;
}

bool j1Map::LoadTilesetData(pugi::xml_node* tileset_node, tileset_info* item_tileset) {
	bool ret = true;

	// Tileset info
	item_tileset->firstgid = tileset_node->attribute("firstgid").as_uint();
	item_tileset->name = tileset_node->attribute("name").as_uint();
	item_tileset->tilewidth = tileset_node->attribute("tilewidth").as_uint();
	item_tileset->tileheight = tileset_node->attribute("tileheight").as_uint();
	item_tileset->spacing = tileset_node->attribute("spacing").as_uint();
	item_tileset->margin = tileset_node->attribute("margin").as_uint();

	item_tileset->tilecount = tileset_node->attribute("tilecount").as_uint();

	item_tileset->columns = tileset_node->attribute("columns").as_uint();

	// Load terrains
	pugi::xml_node terrain_node = tileset_node->child("terraintypes").child("terrain");

	if (terrain_node.attribute("tile").as_int() != -1) LOG("There are no terrains, RETARD ARTIST!!!!\n");

	while (terrain_node.attribute("tile").as_int() == -1) {

		terrain_info* item_terrain = new terrain_info;

		LoadTerrainData(&terrain_node, item_terrain);
		
		terrain_node = terrain_node.next_sibling("terrain");

		item_tileset->terrains.add(item_terrain);

	}

	// Image info
	*tileset_node = tileset_node->child("image");

	item_tileset->image.image_source = tileset_node->attribute("source").as_string();
	item_tileset->image.image_width = tileset_node->attribute("width").as_uint();
	item_tileset->image.image_height = tileset_node->attribute("height").as_uint();
	
	item_tileset->image.tex = App->tex->Load(item_tileset->image.image_source);

	return ret;
}

bool j1Map::LoadTerrainData(pugi::xml_node* terrain_node, terrain_info* item_terrain) {

	const pugi::char_t* cmp = terrain_node->attribute("name").as_string();

	if (strcmp(cmp, "Blocks")) item_terrain->type = Blocks;
	if (strcmp(cmp, "Cobblestone")) item_terrain->type = Cobblestone;
	if (strcmp(cmp, "Cracking Sand")) item_terrain->type = Cracking_Sand;
	if (strcmp(cmp, "Sand")) item_terrain->type = Sand;
	else item_terrain->type = unknown__;

	item_terrain->tile_check = terrain_node->attribute("tile").as_int();

	return true;
}

bool j1Map::LoadLayerData(pugi::xml_node* layer_node, layer_info* item_layer) {
	
	item_layer->name = layer_node->attribute("name").as_string();
	item_layer->width = layer_node->attribute("width").as_uint();
	item_layer->height = layer_node->attribute("height").as_uint();

	//Load all tiles in layer data
	pugi::xml_node tile_node = layer_node->child("data").child("tile");
	pugi::xml_node gid_check = layer_node->previous_sibling("tileset").previous_sibling("tileset").last_child();
	int gid_check_test = gid_check.attribute("id").as_int();
	int node_gid = tile_node.attribute("gid").as_int();

	if (tile_node.attribute("gid").as_int() <= -1) LOG("There are no valid tiles, RETARD ARTIST!!!!\n");

	while (item_layer->tiles.count() < item_layer->width * item_layer->height) {

		map_tile_info* item_tile = new map_tile_info;
	
		LoadTileData(&tile_node, item_tile);
	
		tile_node = tile_node.next_sibling("tile");

		item_layer->tiles.add(item_tile);

	}

	return true;
}

bool j1Map::LoadTileData(pugi::xml_node* tile_node, map_tile_info* item_tile) {

	if (tile_node->attribute("gid").as_int() > -1)
		item_tile->id = tile_node->attribute("gid").as_int();

	if (tile_node->attribute("walk_type").as_int() > -1)
		item_tile->type = (walk_types)tile_node->attribute("walk_type").as_int();
	
	return true;
}