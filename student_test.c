#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "test.h"
#include "graph.h"

#define TEST(expr) test(expr, __FILE__, __LINE__, #expr)
#define TESTQ(expr) testq(expr, __FILE__, __LINE__)

static struct 
{
  int pass;
  int fail;
} stats;

/****************************************************************************/
static void test(bool pass, char *file, int line, char *expr)
{
  if (pass)
  {
    stats.pass++;
  }
  else
  {
    stats.fail++;
    fprintf(stderr, "%s:%3d: Fail '%s'\n", file, line, expr);
  }
}

/****************************************************************************/
static void testq(bool pass, char *file, int line)
{
  if (pass)
  {
    stats.pass++;
  }
  else
  {
    stats.fail++;
    fprintf(stderr, "%s:%d: Fail\n", file, line);
  }
}

/****************************************************************************/
static int redirect_stdout_to_buf(char buf[], size_t size)
{
  int state;

  fflush(stdout);
  state = dup(STDOUT_FILENO);         /* save stdout state */
  freopen("/dev/null", "a", stdout);  /* redirect stdout to /dev/null */
  setvbuf(stdout, buf, _IOFBF, size); /* set buf to stdout */

  return state;
}

/****************************************************************************/
static void restore_stdout(int state)
{
  freopen("/dev/null", "a", stdout);     /* redirect stdout to /dev/null */
  dup2(state, STDOUT_FILENO);            /* restore stdout state */
  setvbuf(stdout, NULL, _IONBF, BUFSIZ); /* disable buffer */
}

/****************************************************************************/
static void test_edge_to_string(void)
{
  char str[20];

  edge_t edge;
  edge.next   = NULL;
  edge.tail   = 10;
  edge.head   = 20;
  edge.weight = 30;

  edge_to_string(&edge, str, sizeof(str));
  TEST(strncmp(str, "10 -> 20 (30)", sizeof(str)) == 0);

  /* Add more tests here */
}

/****************************************************************************/
static void test_list_is_empty(void)
{
  adjacency_list_t list;
  list.first = NULL;

  TEST(list_is_empty(&list));

  /* Add more tests here */
}

/****************************************************************************/
static void test_list_size(void)
{
  edge_t edge;
  edge.next   = NULL;
  edge.tail   = 1;
  edge.head   = 2;
  edge.weight = 1;

  adjacency_list_t list;
  list.first = &edge;

  TEST(list_size(&list) == 1);

  /* Add more tests here */
}

/****************************************************************************/
static void test_list_prepend(void)
{
  edge_t edge1;
  edge1.next   = NULL;
  edge1.tail   = 0;
  edge1.head   = 0;
  edge1.weight = 0;

  edge_t edge2;
  edge2.next   = NULL;
  edge2.tail   = 0;
  edge2.head   = 0;
  edge2.weight = 0;

  adjacency_list_t list;
  list.first = &edge1;

  list_prepend(&list, &edge2);
  TEST(list.first == &edge2);
  TEST(list.first->next == &edge1);

  /* Add more tests here */
}

/****************************************************************************/
static void test_list_contains(void)
{
  edge_t edge1;
  edge1.next   = NULL;
  edge1.tail   = 0;
  edge1.head   = 1;
  edge1.weight = 0;

  adjacency_list_t list;
  list.first = &edge1;

  TEST(list_contains(&list, 0, 1));

  /* Add more tests here */
}

/****************************************************************************/
static void test_graph_initialise(void)
{
  graph_t graph;

  graph_initialise(&graph, 5);

  TEST(graph.vertex_count == 5);

  /* Add more tests here */
}

/****************************************************************************/
static void test_graph_print(void)
{
  char buf[10*1024];
  int  save;

  edge_t edge;
  edge.next = NULL; 
  edge.tail = 0;
  edge.head = 1;
  edge.weight = 10;

  adjacency_list_t adjacency_lists[2];
  adjacency_lists[0].first = &edge;
  adjacency_lists[1].first = NULL;

  graph_t graph;
  graph.vertex_count = 2;
  graph.edge_count = 1;
  graph.adjacency_lists = adjacency_lists;

  save = redirect_stdout_to_buf(buf, sizeof(buf));
  graph_print(&graph);
  restore_stdout(save);
  TESTQ(strncmp(buf, 
                "Graph with 2 vertices and 1 edges:\n"
                "vertex 0:\n"
                "   0 ->  1 (10)\n"
                "vertex 1:\n",
               sizeof(buf)) == 0);

  /* Add more tests here */
}

/****************************************************************************/
static void test_graph_release(void)
{
  /* Add tests here */
}

/****************************************************************************/
static void test_graph_connect(void)
{
  bool success;

  adjacency_list_t adjacency_lists[2];
  adjacency_lists[0].first = NULL;
  adjacency_lists[1].first = NULL;

  graph_t graph;
  graph.vertex_count = 2;
  graph.edge_count = 0;
  graph.adjacency_lists = adjacency_lists;

  success = graph_connect(&graph, 0, 1, 1);
  TEST(success);
  if (success)
  {
    TEST(graph.adjacency_lists[0].first != NULL);
    if (graph.adjacency_lists[0].first != NULL)
    {
      TEST(graph.adjacency_lists[0].first->tail == 0);
      TEST(graph.adjacency_lists[0].first->head == 1);
    }
  }

  /* Add more tests here */
}

/****************************************************************************/
static void test_graph_disconnect(void)
{
  /* Add tests here */
}

/****************************************************************************/
static void test_graph_outdegree(void)
{
  edge_t edge;
  edge.next = NULL; 
  edge.tail = 0;
  edge.head = 1;
  edge.weight = 1;

  adjacency_list_t adjacency_lists[2];
  adjacency_lists[0].first = &edge;
  adjacency_lists[1].first = NULL;

  graph_t graph;
  graph.vertex_count = 2;
  graph.edge_count = 1;
  graph.adjacency_lists = adjacency_lists;

  TEST(graph_outdegree(&graph, 0) == 1);

  /* Add more tests here */
}

/****************************************************************************/
void student_test(void)
{
  memset(&stats, 0, sizeof(stats));

  test_edge_to_string();
  test_list_is_empty();
  test_list_size();
  test_list_prepend();
  test_list_contains();
  test_graph_initialise();
  test_graph_print();
  test_graph_release();
  test_graph_connect();
  test_graph_disconnect();
  test_graph_outdegree();

  fprintf(stdout, "%d tests passed\n", stats.pass);
  fprintf(stdout, "%d tests failed\n", stats.fail);
}
