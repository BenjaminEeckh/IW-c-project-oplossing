#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "graph.h"

/***************************************************************************/
void edge_to_string(const edge_t *edge, char *str, unsigned size)
{
  assert(str != NULL);
  assert(size > 0);
  assert(edge != NULL);
  snprintf(str, size, " %d -> %d (%d)", edge->tail, edge->head, edge->weight);
}

/***************************************************************************/
bool list_is_empty(const adjacency_list_t *list)
{
  assert(list != NULL);

  return (bool)list;
}

/***************************************************************************/
unsigned list_size(const adjacency_list_t *list)
{
  assert(list != NULL);
  edge_t *element = list->first;
  int size = 0;
  while (element != NULL)
  {
    size++;
    element = element->next;
  }
  return size;
}

/***************************************************************************/
void list_prepend(adjacency_list_t *list, edge_t *edge)
{
  assert(list != NULL);
  assert(edge != NULL);
  edge_t *new = edge;
  new->next = list->first;
  list->first = new;
}

/***************************************************************************/
bool list_contains(const adjacency_list_t *list, unsigned tail, unsigned head)
{
  assert(list != NULL);
  edge_t *copy = list->first;
  while (copy != NULL)
  {
    if (tail == copy->tail && head == copy->head)
      return true;
    copy = copy->next;
  }
  return false;
}

/***************************************************************************/
bool graph_initialise(graph_t *graph, unsigned vertex_count)
{
  assert(graph != NULL);
  assert(vertex_count > 0);
  graph->adjacency_lists = malloc((sizeof(adjacency_list_t *[vertex_count])));
  // graph->edge_count = malloc(sizeof(unsigned));
  // graph->vertex_count = malloc(sizeof(unsigned));
  graph->vertex_count = vertex_count;
  if (graph->adjacency_lists != NULL)
    return true;
  return false;
}

/***************************************************************************/
void graph_print(const graph_t *graph)
{
  assert(graph != NULL);
  printf("Graph with %d vertices and %d edges:\n", graph->vertex_count, graph->edge_count);
  for (int i = 0; i < graph->vertex_count; i++)
  {
    printf("vertex %d:\n", i);
    edge_t *to_print = graph->adjacency_lists[i].first;
    // char print = "";
    while (to_print != NULL)
    {
      if (to_print->tail == i)
      {
      char *p = "";
        edge_to_string(to_print, p, 11);
        printf("%s\n", p);
      }

      to_print = to_print->next;
    }
  }
}

/***************************************************************************/
void graph_release(graph_t *graph)
{
  assert(graph != NULL);
  for (int i = 0; i < graph->vertex_count; i++)
  {
    edge_t *to_free = graph->adjacency_lists[i].first;
    while (to_free != NULL)
    {
      edge_t *next = to_free->next;
      free(to_free);
      to_free = next;
    }
    free(graph->adjacency_lists);
    graph->edge_count = 0;
    graph->vertex_count = 0;
  }
}

/***************************************************************************/
bool graph_connect(graph_t *graph, unsigned tail, unsigned head, unsigned weight)
{
  assert(graph != NULL);
  edge_t *new_edge = malloc(sizeof(edge_t));
  // if(tail >= graph->vertex_count || head >= graph->vertex_count) return false;
  if (new_edge != NULL)
  {
    new_edge->tail = tail;
    new_edge->head = head;
    new_edge->weight = weight;
    new_edge->next = graph->adjacency_lists[tail].first;
    graph->adjacency_lists[tail].first = new_edge;
    graph->edge_count++;
    return true;
  }
  return false;
}

/***************************************************************************/
void graph_disconnect(graph_t *graph, unsigned tail, unsigned head)
{
  assert(graph != NULL);
  edge_t *to_delete = graph->adjacency_lists[tail].first;
  while (to_delete != NULL)
  {
    edge_t *next = to_delete->next;
    if (to_delete->tail == tail && to_delete->head == head)
    {
      free(to_delete);
      graph->edge_count--;
    }
    to_delete = next;
  }
}

/***************************************************************************/
unsigned graph_indegree(const graph_t *graph, unsigned id)
{
  assert(graph != NULL);

  unsigned result = 0;

  if (id < graph->vertex_count)
  {
    for (size_t i = 0; i < graph->vertex_count; i++)
    {
      const adjacency_list_t *list = &graph->adjacency_lists[i];

      for (edge_t *edge = list->first; edge != NULL; edge = edge->next)
      {
        if (edge->head == id)
        {
          result++;
        }
      }
    }
  }

  return result;
}

/***************************************************************************/
unsigned graph_outdegree(const graph_t *graph, unsigned id)
{
  assert(graph != NULL);
  int total = 0;
  if (id < graph->vertex_count)
  {
    edge_t *copy = graph->adjacency_lists[id].first;
    while (copy != NULL)
    {
      total++;
      copy = copy->next;
    }
  }
  return total;
}

/***************************************************************************/
void graph_build_from_file(graph_t *graph, const char *pathname)
{
  assert(graph != NULL);
  assert(pathname != NULL);

  FILE *fp = fopen(pathname, "r");
  if (fp != NULL)
  {
    unsigned vertex_count;

    if (fscanf(fp, "%u", &vertex_count) == 1)
    {
      if (graph_initialise(graph, vertex_count))
      {
        while (!feof(fp))
        {
          unsigned tail;
          unsigned head;
          unsigned weight;

          int n = fscanf(fp, "%u %u %u", &tail, &head, &weight);

          if (!feof(fp))
          {
            if (n == 3)
            {
              if (graph_connect(graph, tail, head, weight))
              {
                fprintf(stderr, "Failed to connect: %d->%d\n", tail, head);
              }
            }
            else
            {
              /* Bad format */
            }
          }
        }
      }
    }
    else
    {
      /* Bad format */
    }

    (void)fclose(fp);
  }
}

/***************************************************************************/
void graph_to_dot(const graph_t *graph, const char *pathname)
{
  assert(graph != NULL);
  assert(pathname != NULL);

  FILE *fp = fopen(pathname, "w");
  if (fp != NULL)
  {
    fprintf(fp, "digraph {\n");

    for (size_t i = 0; i < graph->vertex_count; i++)
    {
      const adjacency_list_t *list = &graph->adjacency_lists[i];

      for (edge_t *edge = list->first; edge != NULL; edge = edge->next)
      {
        fprintf(fp, "%d -> %d\n", edge->tail, edge->head);
      }
    }

    fprintf(fp, "}\n");
  }
}
