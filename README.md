
### BFS (Breadth-First Search) - PSEUDOCODE & COMPLEXITY

Purpose:
  Visit nodes in order of increasing distance (in number of edges) from a start node.
  Produces an order of visited node ids.

Pseudocode:
  INPUT: graph G, start node s
  if not G.has_node(s): return empty_list

  create empty queue Q
  create map visited[id] := false for all nodes
  create empty list order

  visited[s] := true
  enqueue(Q, s)

  while Q not empty:
    u := dequeue(Q)
    append order, u
    for each (v, edge_prop) in G.neighbors(u):
      if not visited[v]:
        visited[v] := true
        enqueue(Q, v)

  return order

Notes:
  - For an unweighted graph, BFS computes shortest paths (in edges) from s to all reachable nodes.
  - For directed graphs, neighbors(u) yields outgoing neighbors (BFS follows edge direction).
  - Memory stores the visited map (O(n)) and the queue (O(n) worst-case).

Complexity:
  Let n = |V|, m = |E| (counting directed edges).
  - Time (worst-case): O(n + m)
      - Each node is enqueued/dequeued at most once.
      - Each adjacency list is scanned once across the run -> total edge visits = O(m).
  - Space: O(n) for visited map + up to O(n) for queue + O(n) for output order => O(n).



### DFS (Depth-First Search) - PSEUDOCODE & COMPLEXITY

Purpose:
  Visit nodes by exploring as far as possible before backtracking. The iterative
  stack version mimics recursive DFS. Returns visit order (preorder).

Pseudocode:
  INPUT: graph G, start node s
  if not G.has_node(s): return empty_list

  create empty stack S
  create map visited[id] := false for all nodes
  create empty list order

  push S, s
  while S not empty:
    u := pop(S)
    if visited[u]: continue
    visited[u] := true
    append order, u
    // Push neighbors in reverse so traversal order resembles recursive DFS
    neigh := G.neighbors(u) // neighbors returns vector of (v, prop)
    for each neighbor v in reverse(neigh):
      if not visited[v]:
        push S, v

  return order

Notes:
  - For directed graphs, DFS follows outgoing edges.
  - Order depends on neighbor ordering; reversing neighbors approximates the
    typical recursive-order behavior.
  - Recursive version has same complexity, but iterative avoids stack recursion limits.

Complexity:
  Let n = |V|, m = |E|.
  - Time (worst-case): O(n + m)
      - Each node is visited once; each edge examined once when scanning adjacency lists.
  - Space: O(n) for visited map + O(n) for stack (worst-case) + O(n) for order => O(n).



### Dijkstra (with edge weight extractor) - PSEUDOCODE & COMPLEXITY

Purpose:
  Single-source shortest paths for graphs with non-negative edge weights.
  Edge weights are extracted from arbitrary edge properties via an extractor:
     w = extractor(edge_prop)  // must return arithmetic (numeric) type
  Returns (distances map, predecessor map).

Constraints:
  - extractor(edge_prop) must return a non-negative numeric weight (>= 0).
  - If extractor returns +infinity for some edges you effectively ignore them.
  - If start node doesn't exist, the implementation throws an exception.

Pseudocode:
  INPUT: graph G, start s, extractor function extractor(edge_prop) -> Weight
  assert extractor returns arithmetic type
  if not G.has_node(s): throw exception

  Weight INF := +infinity
  for each node u in G.list_nodes():
    dist[u] := INF
    prev[u] := nullopt
  dist[s] := 0

  // Min-priority queue keyed by distance (binary heap)
  PQ := empty min-heap of pairs (dist, node)
  push PQ (0, s)

  while PQ not empty:
    (d, u) := pop_min(PQ)
    if d > dist[u]: continue  // stale entry
    for each (v, edge_prop) in G.neighbors(u):
      w := extractor(edge_prop)            // numeric
      if dist[u] + w < dist[v]:
        dist[v] := dist[u] + w
        prev[v] := u
        push PQ (dist[v], v)

  return (dist, prev)

Notes:
  - Using a binary heap (std::priority_queue), decrease-key is simulated by pushing
    duplicates and skipping stale entries when popped.
  - Dijkstra requires non-negative weights; negative weights will break correctness.
  - For unreachable nodes dist remains +infinity, and prev remains nullopt.

Complexity (binary heap / std::priority_queue):
  Let n = |V|, m = |E|.
  - Time (worst-case): O((n + m) log n) which is commonly simplified to O(m log n)
      - Each node insertion into heap: up to O(m) pushes in worst-case (one push per relaxed edge).
      - Each pop is O(log n). Total pushes/pops ∈ O(m + n) so complexity O((m + n) log n).
  - Space:
      - dist map: O(n)
      - prev map: O(n)
      - priority queue may hold up to O(m) entries across the run (with duplicates) => O(m)
      - overall: O(n + m)

Alternative heaps:
  - Fibonacci heap yields O(m + n log n) time. But std::priority_queue is binary-heap-based.

Practical notes:
  - If m is large and you require best asymptotic guarantees, consider a heap with decrease-key.
  - If many edges are effectively ignored by extractor (returning INF), runtime reduces accordingly.



### Reconstruct path (using predecessor map) - PSEUDOCODE & COMPLEXITY

Purpose:
  Given prev map produced by Dijkstra (or any predecessor-producing shortest-path),
  reconstruct path from the (implicit) start node to a target node.

Pseudocode:
  INPUT: prev: map id -> optional<id>, target t
  path := empty list
  if prev does not contain t: return empty list
  if prev[t] is nullopt: return empty list  // either t is unreachable or is start

  cur := t
  while true:
    append path, cur
    if prev[cur] is nullopt: break
    cur := prev[cur].value()
  reverse(path)  // we built it backwards
  return path

Notes:
  - Caller should check distances[target] to decide between "start node" vs "unreachable".
  - If prev[target] == nullopt but target == start, you may want to return [start];
    current implementation returns empty to avoid ambiguity — caller can inspect dist map.

Complexity:
  - Time: O(L), where L is the number of nodes in the reconstructed path (<= n).
  - Space: O(L) for the returned path.



### Topological Sort (Kahn's algorithm) - PSEUDOCODE & COMPLEXITY

Purpose:
  Produce a topological ordering of nodes in a DAG (directed acyclic graph).
  Throws / signals error if the graph contains a cycle.

Pseudocode:
  INPUT: directed graph G
  if not G.directed(): throw logic_error

  // compute in-degree for every node
  for each u in G.list_nodes():
    indeg[u] := 0
  for each (from, to, prop) in G.list_edges():
    indeg[to] := indeg[to] + 1

  create empty queue Q
  for each u in nodes:
    if indeg[u] == 0: enqueue(Q, u)

  order := empty list
  while Q not empty:
    u := dequeue(Q)
    append order, u
    for each (v, prop) in G.neighbors(u):  // outgoing edges from u
      indeg[v] := indeg[v] - 1
      if indeg[v] == 0: enqueue(Q, v)

  if length(order) != number_of_nodes:
    // some nodes were not removed -> cycle exists
    throw runtime_error("cycle detected")

  return order

Notes:
  - Works only for directed graphs.
  - Kahn's algorithm yields a valid topo ordering if and only if graph has no cycles.
  - Uses list_edges() to compute initial indegrees, and neighbors(u) when removing u.

Complexity:
  Let n = |V|, m = |E|.
  - Time (worst-case): O(n + m)
      - Building indegree from edges: O(m)
      - Visiting each node once and scanning its outgoing adjacency lists: total O(m)
  - Space:
      - indeg map: O(n)
      - queue: O(n) worst-case
      - output order: O(n)
      => O(n)

