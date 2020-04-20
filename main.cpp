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
    }

    int compute_heuristic(Node *node)
    {
        return abs(node->x - goal->x) + abs(node->y - goal->y);
    }
};

int main()
{

    std::vector< std::vector<int> > mat1 = {
        {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
        {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
        {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 2, 0, 0, 0, 0, 0, 2, 0, 0},
        {1, 2, 0, 0, 0, 0, 0, 2, 0, 3},
        {0, 2, 0, 0, 0, 0, 0, 2, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
    };


//    std::vector< std::vector<int> > mat1 = {
//            {1, 0},
//            {0, 3},
//    };

    std::vector< std::vector<int> > mat2 = {
        {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
        {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
        {0, 0, 2, 0, 0, 0, 0, 2, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 2, 0, 0, 0, 0, 0, 2, 0, 2},
        {1, 2, 0, 0, 0, 0, 0, 2, 2, 2},
        {0, 2, 0, 0, 0, 0, 0, 2, 0, 3},
        {0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
    };

    Map *map1 = new Map(mat1);

    // Node* start_node;

    std::priority_queue<Node *> open_list;

    std::unordered_set<int> open_id_list;
    std::unordered_set<int> close_id_list;

    open_list.push(map1->start);
    open_id_list.insert(map1->start->node_id);

    while (!open_list.empty()) {
        std::cout << "iteration" << std::endl;
        Node *current_node = open_list.top();
        open_list.pop();
        open_id_list.erase(current_node->node_id);
        close_id_list.insert(current_node->node_id);

        if (current_node->is_goal) {
            // print path

            Node *reverse_path = current_node;
            while (reverse_path->path_parent) {
                std::cout << reverse_path->path_parent->x << " " << reverse_path->path_parent->y << std::endl;
                reverse_path = reverse_path->path_parent;
            }

            return 0;
        }

        for (auto edge : current_node->adjacent_list)
        {
            Node *node = edge.first;
            int weight = edge.second;

//            if (current_node->path_parent) {
//                if (node->node_id == current_node->path_parent->node_id) {
//                    continue;
//                }
//            }


            int update_g_value = weight + current_node->g_value;

            if (close_id_list.find(node->node_id) != close_id_list.end())
            {
                if (update_g_value < node->g_value)
                {
                    close_id_list.erase(node->node_id);
                    open_list.push(node);
                    open_id_list.insert(node->node_id);
                }
                else
                {
                    continue;
                }
            }
            else
            {
                if (open_id_list.find(node->node_id) == open_id_list.end())
                {
                    open_list.push(node);
                    open_id_list.insert(node->node_id);
                }
                else if (update_g_value > node->g_value)
                {
                    continue;
                }
            }

            node->g_value = update_g_value;
            node->f_value = update_g_value + map1->compute_heuristic(node);

            node->path_parent = current_node;
        }
    }

    return 0;
}