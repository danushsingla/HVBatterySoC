<template>
  <div>
    <v-network-graph
    :nodes="filteredData.nodes"
    :edges="filteredData.edges"
    :layouts="filteredData.layouts"
    :configs="configs"
    :event-handlers="click_event"
    class="network-graph"
  />
  <div class="control-panel">
      <input v-model="rangeInput" placeholder="Enter range (e.g., 0-2)" />
      <button @click="filterNodesByRange">Filter Nodes</button>
      <button @click="resetNodes">Reset</button>
    </div>

    <!-- Node Info Box -->
    <div v-if="selectedNode" class="node-info-box">
      <h3>Node Information</h3>
      <p><strong>Timestamp:</strong> {{ selectedNode.name/1000 }}</p>
      <p><strong>Color:</strong> {{ selectedNodeColor }}</p>
      <p><strong>Current:</strong> {{ selectedNodeCurrent }}</p>

      <div v-if="selectedNodeVoltages">
        <h4>Voltage Array</h4>
        <ul>
          <li v-for="(voltage, index) in selectedNodeVoltages" :key="index">
            <strong>Voltage {{ index + 1 }}:</strong> {{ voltage }}
          </li>
        </ul>
      </div>
    </div>

  </div>
</template>

<script>
// MOST CODE IS FROM V-NETWORK-GRAPH AND VUE DOCUMENTATION. SOME STACK OVERFLOW AND GITHUB WAS USED FOR HELP WITH DAGRE
//https://github.com/dash14/v-network-graph
//https://dash14.github.io/v-network-graph/
//https://stackoverflow.com/questions/72558793/design-network-topology-graph-with-vuejs
//https://www.npmjs.com/
//https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/fromEntries
import { ref, reactive } from "vue";
import * as vNG from "v-network-graph";
import dagre from "dagre/dist/dagre.min.js"

export default {
  setup() {
    //Actual data
    let data = reactive({
      nodes: {},
      edges: {},
      colors: {},
      layouts: { nodes: {} },
    });

    //Filtered data for ranges
    let filteredData = reactive({
      nodes: {},
      edges: {},
      colors: {},
      layouts: { nodes: {} },
    });

    //Color each node red or black
    const COLOR_RED = "#ff0000";
    const COLOR_BLACK = "#000000";

    let rangeInput = ref("");

    //For the info box
    const selectedNode = ref(null);
    const selectedNodeColor = ref('');
    const selectedNodeCurrent = ref('');
    const selectedNodeVoltages = ref([]);

    // initialize network
    buildNetwork();

    const configs = reactive(
      vNG.defineConfigs({
        //Node, divide text by 1000 for decimal point correction
        node: {
          label: {
            visible: false,
            text: (n) => n.name/1000,
          },
          normal: {
            color: (n) => {
              return filteredData.colors[n.name] ? COLOR_RED : COLOR_BLACK;
            },
            radius: 10,
          },
        },
      })
    );
    configs.node.selectable = true
    configs.node.label.visible = true

    //Filters each node by range, allows for float ranges
    function filterNodesByRange() {
      const range = rangeInput.value
      
      const [start, end] = range.split("-").map(parseFloat);
      if (isNaN(start) || isNaN(end)) {
        return [];
      }
      const minRange = Math.min(start, end);
      const maxRange = Math.max(start, end);

      //Filters nodes based on their name (timestamp) based on the range
      filteredData.nodes = Object.fromEntries(
        Object.entries(data.nodes).filter(([, node]) =>{
          //Divide by 1000 since they are by a factor of 1000 off
            if (node.name/1000 >= minRange && node.name/1000 <= maxRange) {
              console.log(node.name/1000)
              console.log(minRange)
              console.log(maxRange)
              return true;
            }
          }
        )
      );

      //Filters edges based on their source and target nodes that match those in the range
      filteredData.edges = Object.fromEntries(
        Object.entries(data.edges).filter(([, edge]) =>{
          //range.includes(data.nodes[edge.source]?.name) && range.includes(data.nodes[edge.target]?.name)
          if (data.nodes[edge.source]?.name/1000 >= minRange && data.nodes[edge.source]?.name/1000 <= maxRange
            && data.nodes[edge.target]?.name/1000 >= minRange && data.nodes[edge.target]?.name/1000 <= maxRange
          ) {
            return true;
          }
        }
        )
      );

      //Uses filteredData to get the colors because I honestly can't be bothered anymore
      filteredData.colors = Object.fromEntries(
      Object.entries(filteredData.nodes).map(([, node]) => {
        const nodeName = node.name;  // Get the name of the node
        const color = data.colors[nodeName];  // Get the color from data.colors using the node's name
        return [nodeName, color];  // Return an array of [nodeName, color]
      })
    );
    }

    //Reset the nodes after filtering, basically empty range input and set filteredData back to the original Data. Also call layout to make sure the data is in the tree format.
    function resetNodes() {
      rangeInput.value = "";
      filteredData.nodes = data.nodes;
      filteredData.edges = data.edges;
      filteredData.colors = data.colors;
      layout();
      filteredData.layouts = data.layouts;
    }

    //Layout, uses dagre for auto-creating the positioning
    function layout() {
      if (Object.keys(data.nodes).length <= 1 || Object.keys(data.edges).length == 0) {
        return
      }

      // Create a new graph instance for Dagre
      const g = new dagre.graphlib.Graph()

      //Big spacing
      const nodeSize = 50

      g.setGraph({
        rankdir: 'TB',  // Top to Bottom layout
        nodesep: nodeSize * 2,
        edgesep: nodeSize,
        ranksep: nodeSize * 2,
      })

      // Set default edge label (empty)
      g.setDefaultEdgeLabel(() => ({}))

      // Add nodes to the graph with their labels and size
      Object.entries(data.nodes).forEach(([nodeId, node]) => {
        g.setNode(nodeId, { label: node.name, width: nodeSize, height: nodeSize })
      })

      // Add edges to the graph
      Object.values(data.edges).forEach(edge => {
        g.setEdge(edge.source, edge.target)
      })

      // Apply Dagre's layout to position the nodes
      dagre.layout(g)

      // Update node positions with calculated coordinates
      g.nodes().forEach((nodeId) => {
        const node = g._nodes[nodeId];

        if (node) {
          const x = node.x;
          const y = node.y;

          //Checks for undefined positions (don't add them)
          if (x !== undefined && y !== undefined) {
            data.layouts.nodes[nodeId] = { x, y };
          } else {
            console.warn(`Node ${nodeId} does not have x or y after layout.`);
          }
        } else {
          console.warn(`Node ${nodeId} not found after layout.`);
        }
      })
    }

    //For everytime you click a node, pop up its info box with its information
    const click_event = {
      "node:click": ({ node }) => {
        selectedNode.value = filteredData.nodes[node];

        selectedNodeColor.value = filteredData.colors[filteredData.nodes[node].name] ? "Red" : "Black";
        selectedNodeCurrent.value = filteredData.nodes[node].current;
        selectedNodeVoltages.value = filteredData.nodes[node].voltages;
      },
    };

    //Build the tree, only happens at the start
    function buildNetwork() {
      // Read from file, using the dataset of 200 nodes/20,000 data points
      fetch("data_smaller.txt")
        .then(response => response.text())
        .then(fileContent => {
          const fileData = parseFileData(fileContent);
          data.nodes = fileData.nodes;
          data.edges = fileData.edges;
          data.colors = fileData.colors;
        })
        .catch(error => {
          console.error("Error reading file:", error);
        });
    }

    //Parses the file data to read the information from it
    function parseFileData(fileContent) {
      const lines = fileContent.trim().split("\n");
      //Format for the data
      const nodes = {};
      const edges = {};
      const colors = {};

      lines.forEach((line, ) => {
        const parts = line.split(' ');

        //Start ripping apart the data
        const timestamp = (parseFloat(parts[0]) * 1000).toString(); // Multiply by 1000
        const color = parts[1] === '1'; // Convert from string to boolean
        const leftNodeTime = parseFloat(parts[2]) * 1000; // Multiply by 1000
        const rightNodeTime = parseFloat(parts[3]) * 1000; // Multiply by 1000
        const current = parseFloat(parts[4]);
        const voltages = parts.slice(5).map(parseFloat);

        nodes[timestamp] = {
          name: timestamp,
          color: color,
          leftNodeTime: leftNodeTime,
          rightNodeTime: rightNodeTime,
          current: current,
          voltages: voltages,
        }

        colors[timestamp] = color;

        //Creating edges
        if (leftNodeTime !== "-1") {
          edges[`${timestamp}-left`] = {
            source: timestamp.toString(),
            target: leftNodeTime.toString(),
          };
        }
        if (rightNodeTime !== "-1") {
          edges[`${timestamp}-right`] = {
            source: timestamp.toString(),
            target: rightNodeTime.toString(),
          };
        }
      })

      return { nodes, edges, colors };
    }

    return {
      data,
      configs,
      rangeInput,
      filteredData,
      resetNodes,
      filterNodesByRange,
      click_event,
      selectedNode,
      selectedNodeColor,
      selectedNodeCurrent,
      selectedNodeVoltages
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

.node-info-box {
  position: fixed;
  top: 10px;
  right: 10px;
  max-height: 300px;
  overflow-y: auto;
  background-color: rgba(0, 0, 0, 0.7);
  color: white;
  padding: 10px;
  border-radius: 5px;
  font-size: 14px;
  z-index: 1000;
}
</style>
