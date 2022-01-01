#pragma once

#include <utility>

#include "color.h"
#include "hittable_list.h"
#include "vec3.h"

struct scene
{
  scene(color background, point3 lookfrom_, point3 lookto_, hittable_list world)
    : background_{ background }
    , lookfrom_{ lookfrom_ }
    , lookto_{ lookto_ }
    , world_{ std::move(world) }
  {}

  color background_;
  point3 lookfrom_;
  point3 lookto_;
  hittable_list world_;
};