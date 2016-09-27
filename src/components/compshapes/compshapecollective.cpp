#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <base/utils/permit.h>
#include <base/utils/threadpool.h>

#include <entities/entityids.h>

#include <components/compshapes/compshape.h>
#include <components/compshapes/compshapecollective.h>
#include <components/compshapes/dotnodeshape.h>

#include <iostream>
#include <thread>

namespace ngs {

CompShapeCollective::CompShapeCollective(Entity* entity)
    : Component(entity, kIID(), kDID()) {
}

CompShapeCollective::~CompShapeCollective() {
}

void CompShapeCollective::update_wires() {
  internal();
  // Clear any comp shapes that have been destroyed.
  DepUSet<CompShape>::iterator iter = _comp_shapes.begin();
  while (iter != _comp_shapes.end()) {
    if (!*iter) {
      iter = _comp_shapes.erase(iter);
    } else {
      ++iter;
    }
  }
  // Collect any new comp shapes.
  collect_comp_shapes();
}

void CompShapeCollective::update_state() {
  internal();

  // Collect our shape instances.
  _quads.clear();
  collect_quad_instances(_quads);

  _tris.clear();
  collect_tri_instances(_tris);

  _circles.clear();
  collect_circle_instances(_circles);

  // Collect our text.
  _chars.clear();
  collect_char_instances(_chars);
}

void CompShapeCollective::add(const Dep<CompShape>& c) {
  internal();
  assert(!_comp_shapes.count(c));
  _comp_shapes.insert(c);
}

void CompShapeCollective::remove(const Dep<CompShape>& c) {
  internal();
  assert(_comp_shapes.count(c));
  _comp_shapes.erase(c);
}

void CompShapeCollective::collect_comp_shapes() {
  internal();
  for (auto &iter: our_entity()->get_children()) {
    // Skip this group's inputs and outputs.
    if (iter.first == "inputs") {
      continue;
    }
    if (iter.first == "outputs") {
      continue;
    }
    collect_comp_shapes(iter.second);
  }
}

void CompShapeCollective::collect_comp_shapes(Entity* entity) {
  internal();
  // Look for any new comp shapes.
  if (entity->has<CompShape>()) {
    Dep<CompShape> cs = get_dep<CompShape>(entity);
    if (!_comp_shapes.count(cs)) {
      add(cs);
    }
  }

  // If it's a group node we don't dive into it.
  // We show its inputs and outputs.
  if (entity->get_did() == kGroupNodeEntity) {
    collect_comp_shapes(entity->get_entity("./inputs"));
    collect_comp_shapes(entity->get_entity("./outputs"));
    return;
  }

  // Look for child comp shapes. (ie links)
  for (const auto& iter2 : entity->get_children()) {
    collect_comp_shapes(iter2.second);
  }
}

void CompShapeCollective::collect_quad_instances(std::vector<ShapeInstance>& quads) {
  internal();
  for (auto &dep : _comp_shapes) {
    if (!dep) {
      continue;
    }
    const std::vector<ShapeInstance>* shapes = dep->get_quad_instances();
    if (!shapes) {
      continue;
    }
    quads.insert(quads.end(), shapes->begin(), shapes->end());
  }
}

void CompShapeCollective::collect_tri_instances(std::vector<ShapeInstance>& quads) {
  internal();
  for (auto &dep : _comp_shapes) {
    if (!dep) {
      continue;
    }
    const std::vector<ShapeInstance>* shapes = dep->get_tri_instances();
    if (!shapes) {
      continue;
    }
    quads.insert(quads.end(), shapes->begin(), shapes->end());
  }
}

void CompShapeCollective::collect_circle_instances(std::vector<ShapeInstance>& circles) {
  internal();
  for (auto &dep : _comp_shapes) {
    if (!dep) {
      continue;
    }
    const std::vector<ShapeInstance>* shapes = dep->get_circle_instances();
    if (!shapes) {
      continue;
    }
    circles.insert(circles.end(), shapes->begin(), shapes->end());
  }
}

void CompShapeCollective::collect_char_instances(std::vector<CharInstance>& chars) {
  internal();
  for (auto &dep : _comp_shapes) {
    if (!dep) {
      continue;
    }
    const std::vector<CharInstance>* instances = dep->get_char_instances();
    if (!instances) {
      continue;
    }
    chars.insert(chars.end(), instances->begin(), instances->end());
  }
}

Entity* CompShapeCollective::hit_test(const glm::vec2& point, HitRegion& type) const {
  external();
  for (auto &dep: _comp_shapes) {
    if (dep) {
      type = dep->hit_test(point);
      if (type) {
        return dep->our_entity();
      }
    }
  }

  type = kMissed;
  return NULL;
}

void CompShapeCollective::get_aa_bounds(glm::vec2& min, glm::vec2& max) const {
  external();
  // Get the bounds.
  get_aa_bounds(_comp_shapes, min, max);
}

void CompShapeCollective::coalesce_bounds(const std::vector<Polygon>& bounds, glm::vec2& min, glm::vec2& max) {
  // Static Method.
  min=glm::vec2(0,0);
  max=glm::vec2(0,0);

  bool first = true;
  for (size_t i=0; i<bounds.size(); ++i) {
    // Handle the nodes.
    glm::vec2 low;
    glm::vec2 high;
    bounds[i].get_aa_bounds(low,high);

    if (low != high) {
      if (first) {
        // If this is the first one, we initialize min and max with it.
        min = low;
        max = high;
        first = false;
      } else {
        if (low.x < min.x) {
          min.x = low.x;
        }
        if (low.y < min.y) {
          min.y = low.y;
        }
        if (high.x > max.x) {
          max.x = high.x;
        }
        if (high.y > max.y) {
          max.y = high.y;
        }
      }
    }
  }
}

void CompShapeCollective::get_aa_bounds(const DepUSet<CompShape>& comp_shapes, glm::vec2& min, glm::vec2& max) {
  // Static Method.
  std::vector<Polygon> bounds;
  for(Dep<CompShape> c: comp_shapes){
    bounds.push_back(c->get_bounds());
  }
  coalesce_bounds(bounds, min, max);
}

void CompShapeCollective::get_aa_bounds(const DepUSet<NodeShape>& comp_shapes, glm::vec2& min, glm::vec2& max) {
  // Static Method.
  std::vector<Polygon> bounds;
  for(Dep<NodeShape> c: comp_shapes){
    bounds.push_back(c->get_bounds());
  }
  coalesce_bounds(bounds, min, max);
}

//void tessellator(const std::vector<DepLinkPtr>& dependencies, std::pair<size_t, size_t>& range) {
//  for (size_t i = range.first; i < range.second; ++i) {
//    dependencies[i]->dependency->clean_self();
//  }
//}

//void CompShapeCollective::clean_dependencies() {
//  // If we're already clean there is nothing to do.
//  if (!is_state_dirty()) {
//    return;
//  }
//
//  // Collect our dependencies.
//  const IIDToDepLinks& dependencies = get_dependencies();
//  std::vector<DepLinkPtr> deps;
//  for (auto & iid_iter: dependencies) {
//    for (auto & dep_iter : iid_iter.second) {
//      DepLinkPtr link = dep_iter.second.lock();
//      if (link && link->dependency) {
//        deps.push_back(link);
//      }
//    }
//  }
//
//  std::function<void(std::pair<size_t,size_t>&)> func = std::bind(tessellator, deps, std::placeholders::_1);
//  ThreadPool p(deps.size(),32, func);
//}

}

