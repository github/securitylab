#pragma once

#include <string>
#include <vector>

#include <stdint.h>

class Node{

    friend class RandomTree;

  private:

    int32_t id = -1;
    int32_t parent_id = -1;
    std::vector<int32_t> prv_children = {};
    int32_t depth = -1;

    std::string label;

    
  public:

    Node(uint32_t in_id, int32_t in_parent_id, uint32_t in_depth);

    const std::vector<int32_t>& children() const;

    std::string get_label() const;

    uint32_t get_id() const;

    void set_label(const std::string &in_label);

};


class RandomTree{

    friend class Labeler;

  private:

    std::vector<Node> nodes;

    std::vector<std::vector<uint32_t>> levels;

    uint32_t num_nodes = 0;

    uint32_t tree_depth = 0;

    uint32_t new_node(int32_t parent_id, uint32_t depth);
    
  public:

    RandomTree(uint32_t total_nodes);


    Node & get_node(uint32_t node_id);

    size_t size() const;

    std::string dot_format() const;

};