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
#include <fstream>

class Node
{
public:
    int node_id;

    // int f_value;

    // int g_value;

    // for bidirectional
    int g2_value[2];
    int f2_value[2];

    std::vector<std::pair<Node *, int>> adjacent_list;

    Node *path_parent;

    // bool is_goal;
    // bool is_start;

    // extra information
public:
    int x;
    int y;

public:
    Node(int id)
    {
        node_id = id;
        // f_value = 0;
        // g_value = 0;
        path_parent = nullptr;
        // is_goal = false;
        // is_start = false;
        x = y = -1;
    }

    // bool operator<(const Node &n)
    // {
    //     return f_value > n.f_value;
    // }

    int compute_heuristic(Node *node)
    {
        return abs(node->x - this->x) + abs(node->y - this->y);
    }
};

class Map
{
public:
    Node *start;
    Node *goal;

    std::unordered_set<Node *> node_set;

    int width, height;

public:

    Map(std::string fileName)
    {
        std::ifstream input(fileName, std::ifstream::in);
        // int height, width;
        input >> width >> height;

        int startX, startY, destX, destY;
        input >> startX >> startY >> destX >> destY;

        std::vector<std::vector<Node *>> node_matrix(height, std::vector<Node *>(width, nullptr));

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                Node *node = new Node(width * i + j);
                node_set.insert(node);
                node_matrix[i][j] = node;

                node->x = i;
                node->y = j;
            }
        }

        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                int x, y;
                input >> x >> y;
                int count;
                input >> count;
                while (count--)
                {
                    char dir;
                    input >> dir;
                    int newX, newY;
                    switch (dir)
                    {
                    case 'N':
                        newX = x;
                        newY = y - 1;
                        break;
                    case 'S':
                        newX = x;
                        newY = y + 1;
                        break;
                    case 'E':
                        newX = x + 1;
                        newY = y;
                        break;
                    case 'W':
                        newX = x - 1;
                        newY = y;
                        break;
                    default:
                        throw std::string("unknow direction");
                    }
                    node_matrix[y][x]->adjacent_list.push_back({node_matrix[newY][newX], 1});
                }
            }
        }

        start = node_matrix[startY][startX];

        goal = node_matrix[destY][destX];

    }

    Map(const Map &map)
    {
        start = map.start;
        goal = map.goal;

        for (Node *node : map.node_set)
        {
            node_set.insert(node);
        }
    }

    // void reverse()
    // {
    //     Node *start_tmp = start;
    //     start = goal;
    //     goal = start_tmp;
    //     is_reverse = true;
    // }

    // int compute_heuristic(Node *node)
    // {
    //     return abs(node->x - goal->x) + abs(node->y - goal->y);
    // }

    // int compute_heuristic_reverse(Node *node)
    // {
    //     return abs(node->x - start->x) + abs(node->y - start->y);
    // }
};

#endif //FINAL_MAP_H
