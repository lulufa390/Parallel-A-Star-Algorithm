//
// Created by Jikai Lu on 4/24/20.
//

#ifndef FINAL_MAP_H
#define FINAL_MAP_H

#include <iostream>
#include <queue>
#include <vector>
#include <unordered_set>
#include <cmath>
#include <utility>

class Node
{
public:
    int node_id;

    int f_value;

    int g_value;

    // for bidirectional
    int g2_value[2];
    int f2_value[2];

    std::vector< std::pair<Node *, int> > adjacent_list;

    Node *path_parent;

    bool is_goal;
    bool is_start;


    // extra information
public:
    int x;
    int y;

public:
    Node(int id)
    {
        node_id = id;
        f_value = 0;
        g_value = 0;
        path_parent = nullptr;
        is_goal = false;
        is_start = false;
        x = y = -1;
    }

    bool operator<(const Node &n)
    {
        return f_value > n.f_value;
    }
};

class Map
{
public:
    Node *start;
    Node *goal;

    std::unordered_set<Node*> node_set;

    bool is_reverse;

public:
    // initialize from a 2-dimension map
    Map(std::vector< std::vector<int> > matrix)
    {

        if (matrix.empty())
        {
            return;
        }

        int height = matrix.size();
        int width = matrix[0].size();

        std::vector< std::vector<Node *> > node_matrix(height, std::vector<Node *>(width, nullptr));

        // new nodes
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                Node *node = new Node(width * i + j);
                node_set.insert(node);
                node_matrix[i][j] = node;

                node->x = i;
                node->y = j;

                if (matrix[i][j] == 1)
                {
                    node->is_start = true;
                    start = node;
                }
                else if (matrix[i][j] == 3)
                {
                    node->is_goal = true;
                    goal = node;
                }
            }
        }

        // calculate adjacent list
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {

                if (i > 0)
                {
                    if (matrix[i][j] != 2 && matrix[i - 1][j] != 2)
                    {
                        node_matrix[i][j]->adjacent_list.push_back({node_matrix[i - 1][j], 1});
                    }
                }

                if (i < height - 1)
                {
                    if (matrix[i][j] != 2 && matrix[i + 1][j] != 2)
                    {
                        node_matrix[i][j]->adjacent_list.push_back({node_matrix[i + 1][j], 1});
                    }
                }

                if (j > 0)
                {
                    if (matrix[i][j] != 2 && matrix[i][j - 1] != 2)
                    {
                        node_matrix[i][j]->adjacent_list.push_back({node_matrix[i][j - 1], 1});
                    }
                }

                if (j < width - 1)
                {
                    if (matrix[i][j] != 2 && matrix[i][j + 1] != 2)
                    {
                        node_matrix[i][j]->adjacent_list.push_back({node_matrix[i][j + 1], 1});
                    }
                }
            }
        }

        is_reverse = false;
    }

    Map (const Map& map) {
        start = map.start;
        goal = map.goal;

        for (Node* node : map.node_set) {
            node_set.insert(node);
        }
    }

    void reverse() {
        Node* start_tmp = start;
        start = goal;
        goal = start_tmp;
        is_reverse = true;
    }

    int compute_heuristic(Node *node)
    {
        return abs(node->x - goal->x) + abs(node->y - goal->y);
    }

    int compute_heuristic_reverse(Node *node)
    {
        return abs(node->x - start->x) + abs(node->y - start->y);
    }
};

#endif //FINAL_MAP_H
