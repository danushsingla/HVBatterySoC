<template>
  <div>
    <div class="demo-control-panel">
      <label>Node count:</label>
      <el-input-number v-model="nodeCount" :min="3" :max="200" />
      <label>(&lt;= 200)</label>
    </div>

    <v-network-graph
      :zoom-level="0.5"
      :nodes="nodes"
      :edges="edges"
      :configs="configs"
      class="network-graph"
    />
  </div>
</template>

<script>
import { reactive, ref, watch } from "vue";
import * as vNG from "v-network-graph";
import { ForceLayout } from "v-network-graph/lib/force-layout";

export default {
  setup() {
    const nodeCount = ref(20);
    const nodes = reactive({});
    const edges = reactive({});

    // initialize network
    buildNetwork(nodeCount.value, nodes, edges);

    watch(nodeCount, () => {
      buildNetwork(nodeCount.value, nodes, edges);
    });

    const configs = reactive(
      vNG.defineConfigs({
        view: {
          layoutHandler: new ForceLayout({
            positionFixedByDrag: false,
            positionFixedByClickWithAltKey: true,
            createSimulation: (d3, nodes, edges) => {
              // d3-force parameters
              const forceLink = d3
                .forceLink(edges)
                .id(d => d.id);
              return d3
                .forceSimulation(nodes)
                .force("edge", forceLink.distance(40).strength(0.5))
                .force("charge", d3.forceManyBody().strength(-800))
                .force("center", d3.forceCenter().strength(0.05))
                .alphaMin(0.001);
            },
          }),
        },
        node: {
          label: {
            visible: false,
          },
        },
      })
    );

    function buildNetwork(count, nodes, edges) {
      const idNums = [...Array(count)].map((_, i) => i);

      // nodes
      const newNodes = Object.fromEntries(idNums.map(id => [`node${id}`, {}]));
      Object.keys(nodes).forEach(id => delete nodes[id]);
      Object.assign(nodes, newNodes);

      // edges
      const makeEdgeEntry = (id1, id2) => {
        return [`edge${id1}-${id2}`, { source: `node${id1}`, target: `node${id2}` }];
      };
      const newEdges = Object.fromEntries(
        [...idNums]
          .map(n => [n, Math.floor(n / 4) * 4 % count])
          .map(([n, m]) => (n === m ? [n, (n + 4) % count] : [n, m]))
          .map(([n, m]) => makeEdgeEntry(n, m))
      );
      Object.keys(edges).forEach(id => delete edges[id]);
      Object.assign(edges, newEdges);
    }

    return {
      nodeCount,
      nodes,
      edges,
      configs,
    };
  },
};
</script>

<style>
.demo-control-panel {
  padding: 10px;
}

.network-graph {
  width: 100%;
  height: 600px;
}
</style>
