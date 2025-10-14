#### beecrowd:
```rm -rf bee1442.out output.txt &&  g++ src/beecrowd/bee1442.cpp -o bee1442.out && (./bee1442.out < ./input.txt) > ./output.txt```

obs: https://www.udebug.com/ (several user-submitted test cases)

## PAA (Algortihms Project and Analysis) 2025.2

### BFS (Breadth-First Search) - PSEUDOCODE & COMPLEXITY

Purpose:
  Visit nodes in order of increasing distance (in number of edges) from a start node.
  Produces an order of visited node ids.

Pseudocode:
```
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
```
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
```
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
```
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
```
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
```
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
```
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
```
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
```
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
```
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

### BIPARTITE CHECK - PSEUDOCODE, NOTES & COMPLEXITY

Purpose:
  Determine whether an undirected graph is bipartite (2-colorable), i.e.
  vertices can be partitioned into two sets such that no intra-set edge exists.

Pseudocode (high-level):
```
  INPUT: graph G
  Build an undirected adjacency view (u -> neighbors) from G.list_edges() and G.list_nodes()
  color[node] := -1 for all nodes  // -1 = uncolored, 0/1 = two colors

  for each node s in nodes:
    if color[s] != -1: continue
    color[s] := 0
    enqueue(Q, s)
    while Q not empty:
      u := dequeue(Q)
      for each v in adj[u]:
        if color[v] == -1:
          color[v] := color[u] XOR 1
          enqueue(Q, v)
        else if color[v] == color[u]:
          return false  // same color on adjacent vertices -> not bipartite

  return true
```

Notes:
  - If your graph is directed, treat edges as undirected for bipartiteness testing.
  - Works on disconnected graphs by initiating BFS from every uncolored node.

Complexity:
  - Let n = |V|, m = |E|.
  - Time: O(n + m) (build view O(n+m), BFS visits each edge once)
  - Space: O(n + m) for adjacency view, O(n) for color map.

### ACYCLIC CHECK - PSEUDOCODE, NOTES & COMPLEXITY

Purpose:
  Detect whether G contains any cycle.
  - Directed graphs -> use DFS and detect back-edges (coloring method).
  - Undirected graphs -> use DFS with parent tracking (ignore the parent edge).

Pseudocode (directed):
```
  INPUT: directed graph G
  color[u] := 0 for all u  // 0=white, 1=gray, 2=black
  for each node u:
    if color[u] == 0:
      if not dfs_visit(u): return false
  return true

  dfs_visit(u):
    color[u] := 1
    for each v in neighbors(u):
      if color[v] == 0:
        if not dfs_visit(v): return false
      else if color[v] == 1:
        return false  // back-edge to gray node -> cycle
    color[u] := 2
    return true
```

Pseudocode (undirected):
```
  visited[u] := false for all u
  for each node u:
    if not visited[u]:
      if not dfs_undirected(u, parent = u): return false
  return true

  dfs_undirected(u, parent):
    visited[u] := true
    for each v in neighbors(u):
      if not visited[v]:
        if not dfs_undirected(v, u): return false
      else if v != parent:
        return false  // found a non-parent visited neighbor -> cycle
    return true
```
Complexity:
  - Time: O(n + m)
  - Space: O(n) for recursion/stack, color/visited arrays

### KOSARAJU'S SCC - PSEUDOCODE, NOTES & COMPLEXITY

Purpose:
  Compute strongly connected components (SCCs) of a directed graph.

Pseudocode:
```
  INPUT: directed graph G
  // Build adjacency and reversed adjacency lists from G.list_edges()
  visited[u] := false for all u
  // ordering
  order := empty list
     for each u:
       if not visited[u]: dfs1(u)
     dfs1(u):
       visited[u] = true
       for v in adj[u]:
         if not visited[v]: dfs1(v)
       order.push_back(u) // finish order

  // visiting
  visited[u] := false for all u
     components := []
     for u in reverse(order):
       if not visited[u]:
         comp := []
         dfs2(u, comp) // on reversed graph
         components.push_back(comp)
     dfs2(u, comp):
       visited[u] = true
       comp.push_back(u)
       for v in rev_adj[u]:
         if not visited[v]: dfs2(v, comp)

  return components
```

Notes:
  - Each component is a set of nodes mutually reachable.
  - For undirected graphs, Kosaraju returns connected components (since rev = adj).
  - Requires two DFS passes and reversed graph.

Complexity:
  - Time: O(n + m)
  - Space: O(n + m) for adj + rev_adj and recursion/storage for components

### EDMONDS-KARP (MAX FLOW) - PSEUDOCODE, NOTES & COMPLEXITY

Purpose:
  Compute maximum s->t flow in a directed network using BFS to find shortest
  augmenting paths in the residual graph (Edmonds–Karp variant of Ford–Fulkerson).

Pseudocode:
```
  INPUT: directed graph G, source s, sink t, cap_extractor(edge_prop) -> capacity
  Build residual capacity map capacity[u,v] from all edges (sum capacities for parallel edges).
  Initialize reverse capacities capacity[v,u] = 0 if absent.
  flow := 0

  While there exists an s->t path in residual graph (found by BFS that respects capacity > 0):
    parent[v] := previous node found by BFS
    // compute bottleneck
    bottleneck := +infinity
    v := t
    while v != s:
      u := parent[v]
      bottleneck := min(bottleneck, capacity[u,v])
      v := u
    // apply bottleneck
    v := t
    while v != s:
      u := parent[v]
      capacity[u,v] -= bottleneck
      capacity[v,u] += bottleneck
      v := u
    flow += bottleneck

  return flow
```

Notes:
  - Cap_extractor must return non-negative numeric capacity.
  - If G is undirected, treat each undirected edge as two directed edges (u->v and v->u).
  - Edmonds-Karp uses BFS so each augmenting path is shortest in edges, and the
    algorithm runs in O(V * E^2) worst-case (but simpler to think: O(E * maxflow) when capacities are integers).

Complexity:
  - Time (worst-case): O(V * E^2) for Edmonds-Karp, or O(E * max_flow_value) in another view.
  - Space: O(n + m) for residual capacities and adjacency.

###  BELLMAN-FORD - PSEUDOCODE, NOTES & COMPLEXITY

Purpose:
  Single-source shortest paths allowing negative edge weights. Detects negative-weight cycles reachable from the source.

Pseudocode:
```
  INPUT: graph G, start s, extractor(edge_prop) -> weight (numeric)
  Initialize dist[u] := +infinity for all u; prev[u] := null
  dist[s] := 0

  // relax edges (n - 1) times
  n := number_of_nodes
  for i in 1 .. n-1:
    updated := false
    for each edge (u, v, prop) in G.list_edges():
      w := extractor(prop)
      if dist[u] != +inf and dist[u] + w < dist[v]:
        dist[v] := dist[u] + w
        prev[v] := u
        updated := true
    if not updated: break

  // check for negative cycles
  has_negative := false
  for each edge (u, v, prop) in G.list_edges():
    w := extractor(prop)
    if dist[u] != +inf and dist[u] + w < dist[v]:
      has_negative := true
      break

  return (dist, prev, has_negative)
```
Notes:
  - If has_negative is true, shortest path distances are undefined (negative cycle affects them).
  - For negative-cycle detection anywhere in graph, run BF from every node (or add a super-source with 0-weight edges to all nodes).

Complexity:
  - Time: O(n * m)
  - Space: O(n) for dist/prev